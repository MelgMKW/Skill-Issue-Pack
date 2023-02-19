#include <SkillIssuePack.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Page/Other/GhostManager.hpp>
#include <game/System/Archive.hpp>
#include <SlotExpansion/SlotExpansion.hpp>


u32 lastSelectedCup = 0;
u32 lastSelectedButtonIdx = 0;
u32 selectedCourse = 0;

//TrackSlot
//Patches course buttons IDs so that it goes vertical than horizontal rather than the opposite
kmWrite32(0x807e525c, 0x937403CC); //Course buttons ids

//CourseId of 1st button is 4*CupID due to reordering
kmWrite32(0x8084182c, 0x5400103A); 
kmWrite32(0x80841830, 0x60000000);

//Gets ID; removes 44 if needed; in this current form, this will break regs but SIP does not support them
CourseId SIP::GetCourseId(){
   if(SIP::winningCourse >= 0x44) return (CourseId) (SIP::winningCourse - 0x44);
   else return (CourseId) SIP::winningCourse;
}

//When a course button is pressed, CourseSelect::LoadNextPage calls this
void SaveSelectedCourse(PushButton *button){
   selectedCourse = lastSelectedCup * 4 + button->buttonId;
   SIP::winningCourse = selectedCourse + 0x44; //+0x44 for online stuff, since slot 0x43 is used by the game for "hasn't voted yet"
}

//Converts trackID to track slot using table
CourseId GetCorrectTrackSlot(){
   CourseId id = SIP::GetCourseId();
   return (CourseId) SIP::extSlotToTrackId[id / 4][id % 4];
}

//CourseSelect::LoadNextPage patch mentioned above
asm void UpdateSlot(){
   ASM(
      nofralloc;
      mflr r31;
      mr r3, r5;
      bl SaveSelectedCourse;
      bl GetCorrectTrackSlot;
      mtlr r31;
      mr r31, r3;
      blr;
   )
}
kmCall(0x80840858, UpdateSlot);

//m98 slot patch, used for ghost stuff (ghosts check that the rkg has the same trackID as the current for example)
void SetCorrectGhostRaceSlot(GhostList *list, s32 entryIdx){
   list->InitMenudata98Params(entryIdx);
   if(entryIdx >= 0 && entryIdx < list->count){
      CourseId slot = GetCorrectTrackSlot();
      MenuData::sInstance->menudata98->courseId = slot;
   }
}
kmCall(0x805c7b6c, SetCorrectGhostRaceSlot);
kmCall(0x805c7d2c, SetCorrectGhostRaceSlot);
kmCall(0x80639e54, SetCorrectGhostRaceSlot);
kmCall(0x80639fb0, SetCorrectGhostRaceSlot);


//+0x44 for reasons explained above
void SetVotedTrack(Pages::Vote *vote){
   vote->SetVotedCourseId((CourseId) (selectedCourse + 0x44));
}
kmCall(0x8084099c, SetVotedTrack);

//CtrlMenuCupSelectCup::OnCupButtonClick patch that updates lastSelectCup so that the game remembers it in btw races
void UpdateLastSelCup(Pages::CupSelect *page, CtrlMenuCupSelectCup *cups, PushButton *button, u32 hudSlotId){
   if(button->buttonId != lastSelectedCup){
      lastSelectedCup = button->buttonId;
      selectedCourse = lastSelectedCup * 4;
   }
   PushButton **buttons = (PushButton**) cups->childrenGroup.controlArray;
   for(int i = 0; i<8; i++) if(buttons[i] == button) lastSelectedButtonIdx = i;
   page->LoadNextPage(cups, button, hudSlotId);
   RaceData::sInstance->menusScenario.settings.cupId = lastSelectedCup % 8;
}
kmCall(0x807e5da8, UpdateLastSelCup);

//MusicSlot
int GetCorrectMusicSlot(){
   CourseId id = RaceAudioMgr::sInstance->courseId;
   if(id <= 0x1F){
      id = SIP::GetCourseId();
      id = (CourseId) SIP::extSlotToMusicSlot[id/4][id % 4];   
   }
   return RaceAudioMgr::trackToMusicIDTable[id];
}
kmCall(0x80711fd8, GetCorrectMusicSlot);
kmCall(0x8071206c, GetCorrectMusicSlot);

//Loads correct file, in this form it breaks regs but SIP does not support them
void FormatTrackPath(char *path, u32 length, const char *format, const char *fileName){
   GameMode gamemode = RaceData::sInstance->menusScenario.settings.gamemode;
   if(gamemode != MODE_BATTLE && gamemode != MODE_PUBLIC_BATTLE && gamemode != MODE_PRIVATE_BATTLE && gamemode != MODE_AWARD){
      snprintf(path, 0x80, "Race/Course/%d", SIP::GetCourseId());
   } 
   else snprintf(path, 0x80, format, fileName);
}
kmCall(0x80540820, FormatTrackPath);
kmCall(0x80540870, FormatTrackPath);
kmCall(0x80541bc4, FormatTrackPath);
kmWrite32(0x80531fbc, 0x38800000); //fix incorrect courseId array read


void FixGrandPrix(Pages::CupSelect *page, MenuId nextMenu, PushButton *button){
   SIP::winningCourse = button->buttonId * 4 + 0x44;
   page->ChangeMenuById(VS_RACE_PANORAMA, button);
};
//kmCall(0x80841854, FixGrandPrix);

/*
using SIP::extSlotToTrackId;
using SIP::winningCourse;
asm void GetCorrectSlotArray(){
   ASM(
      lis r5, winningCourse@ha;
      lwz r6, winningCourse@l(r5);
      add r6, r6, r4;
      stw r6, winningCourse@l(r5);
      lis r5, extSlotToTrackId@h;
      ori r5, r5, extSlotToTrackId@l;
   )
};
kmBranch(0x8052f210, GetCorrectSlotArray);
kmPatchExitPoint(GetCorrectSlotArray, 0x8052f214);
*/

//Fixes GP since it usually uses racedata's courseId which only holds the slot
RacedataScenario *UseCorrectCourse(RacedataScenario *scenario){
   SIP::winningCourse = lastSelectedCup * 4 + scenario->settings.raceNumber + 0x44;
   scenario->settings.courseId = GetCorrectTrackSlot();
   return scenario;
};
kmWrite32(0x8052f220, 0x60000000);

//Wrapper around function above
asm void UseCorrectCourseWrapper(){
   ASM(
      nofralloc;
      mflr r0;
      stw r0, 0x8(sp);
      bl UseCorrectCourse;
      lwz r0, 0x8(sp);
      mtlr r0;
      blr;
   )
}
kmBranch(0x8052f224, UseCorrectCourseWrapper);
kmPatchExitPoint(UseCorrectCourseWrapper, 0x8052f228);

//Badly written, but does the job even though it can in theory hang forever, as unlikely as it is
void VSRaceRandomFix(MenuData98 *m98){ //properly randomizes tracks and sets the first one
   m98->vsRaceLimit = 32;
   Random random;
   CourseId id;
   bool isRepeat;
   for(int i = 0; i<32; i++){
      do{
         id = (CourseId) random.NextLimited(TRACKCOUNT);
         isRepeat = false;
         for(int j = 0; j<i; j++){
            if(m98->vsTracks[j] == id){
               isRepeat = true;
               break;
            } 
         } 
      }while(isRepeat);
      m98->vsTracks[i] = id;
   }
   SIP::winningCourse = m98->vsTracks[0] + 0x44;
   RaceData::sInstance->menusScenario.settings.courseId = GetCorrectTrackSlot();
};
kmBranch(0x805e32ec, VSRaceRandomFix);
kmWrite32(0x8084e5e4, 0x60000000); //nop racedata courseId store since it's done in the function

//Same as GP, racedata only ever has courseId
void VSRaceOrderedFix(MenuData98 *m98){
   m98->vsRaceLimit = 32;
   CourseId initial = SIP::GetCourseId();
   for(int i = 0; i<32; i++) m98->vsTracks[i] = (CourseId) ((initial + i)%TRACKCOUNT);
};
kmCall(0x80840a24, VSRaceOrderedFix);

CourseId VSNextTrackFix(CourseId id){//properly sets the next track
   SIP::winningCourse = id + 0x44;
   return GetCorrectTrackSlot();
} 
kmBranch(0x808606cc, VSNextTrackFix);
