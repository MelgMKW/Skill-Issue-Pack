#include <SlotExpansion/UI/ExpansionUIMisc.hpp>
#include <UI/BMG.hpp>

namespace SIPUI{
//Change brctr names
kmWrite16(0x808a85ef, 0x5349); //used by 807e5754
kmWrite8(0x808a85f1, 0x50);
kmWrite16(0x808a82c6, 0x5349); //used by 807e4538
kmWrite8(0x808a82c8, 0x50);
void LoadCorrectTrackListBox(ControlLoader *loader, const char *folder, const char *normalBrctr, const char *variant, const char **anims){
   loader->Load(folder, "SIPSelectNULL", variant, anims);
}
kmCall(0x807e5f24, LoadCorrectTrackListBox);

//BMG
int GetCorrectTrackBMGById(u8 id){
   return BMG_TRACKS + 4 * MenuData::sInstance->curScene->Get<Pages::CupSelect>(CUP_SELECT)->ctrlMenuCupSelectCup.curCupID + id;
};
kmWrite32(0x807e6184, 0x7FA3EB78);
kmCall(0x807e6188, GetCorrectTrackBMGById);
kmWrite32(0x807e6088, 0x7F63DB78);
kmCall(0x807e608c, GetCorrectTrackBMGById);

int GetCurTrackBMG(){
   return SIP::GetCourseId() + BMG_TRACKS;
}

void SetVSIntroBmgId(LayoutUIControl *trackName){
   trackName->SetMsgId(GetCurTrackBMG() + TRACKINFOBMGOFFSET);
}
kmCall(0x808552cc, SetVSIntroBmgId);

void SetGhostInfoTrackBMG(GhostInfoControl *control, const char*textBoxName){
   control->SetTextBoxMsg(textBoxName, GetCurTrackBMG(), NULL);
}
kmCall(0x805e2a4c, SetGhostInfoTrackBMG);
kmWrite32(0x80644104, 0x3b5b6FBC); //BMG_TRACKS -0x44
kmWrite32(0x80644340, 0x38036FBC);

//Rewrote InitSelf to start with correct TPLs
void ExtCupSelectCupInitSelf(CtrlMenuCupSelectCup *cups){
   cups->curCupID = lastSelectedCup;
   PushButton **buttons = (PushButton**) cups->childrenGroup.controlArray;
   for(int i = 0; i<8; i++){
      PushButton *curButton = buttons[i];
      u32 id = (i + lastSelectedCup - lastSelectedButtonIdx + CUPCOUNT) % CUPCOUNT;
      curButton->buttonId = id;
      curButton->SetMsgId(id + BMG_CUPS, NULL);
      curButton->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*) &cups->onCupButtonClickHandler, 0);
      curButton->SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*) &cups->onCupButtonSelectHandler);
      curButton->SetPlayerBitfield(MenuData::sInstance->curScene->Get<Pages::CupSelect>(CUP_SELECT)->GetPlayerBitfield());
      ExpandedCupSelect::ChangeCupTPL(curButton, id);
   }
   buttons[lastSelectedButtonIdx]->SelectInitialButton(0);
};
kmWritePointer(0x808d324c, ExtCupSelectCupInitSelf); //807e5894

void ExtCourseSelectCupInitSelf(CtrlMenuCourseSelectCup *courseCups){
   for(int i = 0; i<8; i++){
      CtrlMenuCourseSelectCupSub *cur = &courseCups->cupIcons[i];
      u32 id = (i + lastSelectedCup - lastSelectedButtonIdx + CUPCOUNT) % CUPCOUNT;
      cur->SetMsgId(id + BMG_CUPS, NULL);
      ExpandedCupSelect::ChangeCupTPL(cur, id);
      cur->animator.GetAnimationGroupById(0)->PlayAnimationAtFrame(0, 0.0f);
      bool clicked = lastSelectedButtonIdx == i ? true : false;
      cur->animator.GetAnimationGroupById(1)->PlayAnimationAtFrame(!clicked, 0.0f);
      cur->animator.GetAnimationGroupById(2)->PlayAnimationAtFrame(!clicked, 0.0f);
      cur->animator.GetAnimationGroupById(3)->PlayAnimationAtFrame(clicked, 0.0f);
      cur->selected = clicked;
      cur->SetRelativePosition(&courseCups->positionAndscale[1]);    
   }
   Scene *curScene = MenuData::sInstance->curScene;
  
   

   Pages::CupSelect *cup = curScene->Get<Pages::CupSelect>(CUP_SELECT);
   NoteModelControl *positionArray = cup->modelPosition;

 
   switch(cup->extraControlNumber){
      case(2):
         positionArray[0].positionAndscale[1].position.x = -52.0f;
         positionArray[0].positionAndscale[1].position.y = -8.0f;
         positionArray[0].positionAndscale[1].scale.x = 0.875f;
         positionArray[0].positionAndscale[1].scale.z = 0.875f;
         positionArray[1].positionAndscale[1].position.x = -52.0f;
         positionArray[1].positionAndscale[1].position.y = -13.0f;
         positionArray[1].positionAndscale[1].scale.x = 0.875f;
         positionArray[1].positionAndscale[1].scale.z = 0.875f;
         break;
      case(1):
         positionArray[0].positionAndscale[1].position.x = -32.0f;
         positionArray[0].positionAndscale[1].position.y = -32.0f;
         positionArray = curScene->Get<Pages::CourseSelect>(COURSE_SELECT)->modelPosition;
         positionArray[0].positionAndscale[1].position.x = -32.0f;
         positionArray[0].positionAndscale[1].position.y = -32.0f;
         break;
      case(4):
         positionArray[3].positionAndscale[1].position.x = 64.0f;
         positionArray[3].positionAndscale[1].position.y = -55.25f;
         positionArray[3].positionAndscale[1].scale.x = 0.6875f;
         positionArray[3].positionAndscale[1].scale.z = 0.6875f;
      case(3):
         positionArray[0].positionAndscale[1].position.x = 64.0f;
         positionArray[0].positionAndscale[1].position.y = -64.0f;
         positionArray[0].positionAndscale[1].scale.x = 0.6875f;
         positionArray[0].positionAndscale[1].scale.z = 0.6875f;
         positionArray[1].positionAndscale[1].position.x = 64.0f;
         positionArray[1].positionAndscale[1].position.y = -64.0f;
         positionArray[1].positionAndscale[1].scale.x = 0.6875f;
         positionArray[1].positionAndscale[1].scale.z = 0.6875f;
         positionArray[2].positionAndscale[1].position.x = 64.0f;
         positionArray[2].positionAndscale[1].position.y = -55.25f;
         positionArray[2].positionAndscale[1].scale.x = 0.6875f;
         positionArray[2].positionAndscale[1].scale.z = 0.6875f;
         break;
   }
};
kmWritePointer(0x808d3190, ExtCourseSelectCupInitSelf); //807e45c0

void ExtCourseSelectCourseInitSelf(CtrlMenuCourseSelectCourse *course){
   Scene *curScene = MenuData::sInstance->curScene;
   Pages::CupSelect *cupPage = curScene->Get<Pages::CupSelect>(CUP_SELECT);
   Pages::CourseSelect *coursePage = curScene->Get<Pages::CourseSelect>(COURSE_SELECT);
   //channel ldb stuff ignored
   u32 cupId = cupPage->clickedCupId;
   for(int i = 0; i<4; i++){
      PushButton *curButton = &course->courseButtons[i];
      curButton->buttonId = i;
      u32 bmgId = GetCorrectTrackBMGById(i);
      curButton->SetMsgId(bmgId);
      if(lastSelectedCup * 4 + i == selectedCourse){
         coursePage->SelectButton(curButton);
         if(RaceData::sInstance->menusScenario.settings.gamemode == MODE_TIME_TRIAL){
            TextInfo text;
            text.bmgToPass[0] = BMG_TT_MODE_BOTTOM_CUP + SIP::ttMode;
            u32 bmgId = BMG_NO_TROPHY;
            TrackTrophy *trophy = SIPDataHolder::GetInstance()->FindTrackTrophy(SIP::GetCRC32((CourseId)curButton->buttonId), SIP::ttMode);
            if(trophy != NULL && trophy->hastrophy[SIP::ttMode]) bmgId = BMG_TROPHY;
            text.bmgToPass[1] = bmgId;
            coursePage->bottomText->SetMsgId(BMG_TT_BOTTOM_COURSE, &text);
         }
      }
   };
};
kmWritePointer(0x808d30d8, ExtCourseSelectCourseInitSelf); //807e5118

//Multiplayer Fix
kmWrite32(0x807e56d4, 0x60000000);
kmWrite32(0x807e5f04, 0x60000000);

//TPL
//CupSelectCup patch, disable picture panes
kmWrite32(0x807e57a4, 0x60000000); 
kmWrite32(0x807e57bc, 0x60000000);
kmWrite32(0x807e57d4, 0x60000000);

//CourseSelectCup patch, disable picture panes
kmWrite32(0x807e4550, 0x60000000);
kmWrite32(0x807e4568, 0x60000000);
kmWrite32(0x807e4580, 0x60000000);
}//namespace SIPUI