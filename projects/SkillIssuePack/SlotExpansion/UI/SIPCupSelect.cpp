#include <game/UI/MenuData/MenuData.hpp>
#include <SlotExpansion/UI/SIPCupSelect.hpp>
#include <SlotExpansion/SlotExpansion.hpp>
#include <UI/BMG.hpp>
#include <UserData/SIPData.hpp>

//Create
namespace SIPUI{
kmWrite32(0x80623d98, 0x60000000);
ExpandedCupSelect *CreateCupSelect(){
   return new ExpandedCupSelect;
};
kmCall(0x80623da4, CreateCupSelect);

ExpandedCupSelect::ExpandedCupSelect(){
   internControlCount += 2;
   onRightArrowSelectHandler.subject = this;
   onRightArrowSelectHandler.ptmf = &ExpandedCupSelect::OnRightArrowSelect;
   onLeftArrowSelectHandler.subject = this;
   onLeftArrowSelectHandler.ptmf = &ExpandedCupSelect::OnLeftArrowSelect;
   onStartPressHandler.subject = this;
   onStartPressHandler.ptmf = static_cast<void (Pages::Menu::*)(u32)>(&ExpandedCupSelect::OnStartPress);
   onBackPressHandler.subject = this;
   onBackPressHandler.ptmf = static_cast<void (Pages::Menu::*)(u32)>(&ExpandedCupSelect::OnBackPress);
   onSwitchPressHandler.subject = this;
   onSwitchPressHandler.ptmf = &ExpandedCupSelect::OnSwitchPress;
   randomizedId = (CourseId) -1;
   this->controlsManipulatorManager.SetGlobalHandler(START_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onStartPressHandler, false, false);
   this->controlsManipulatorManager.SetGlobalHandler(SWITCH_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onSwitchPressHandler, false, false);
}

//Patch distance func to remove horizontal wrapping
kmWrite32(0x80841244, 0x38800001);

//SheetSelect
UIControl *ExpandedCupSelect::CreateControl(u32 controlId){
   if(controlId == 2){
      this->AddControl(2, &this->arrows, 0);
      arrows.SetRightArrowHandler((PtmfHolder_2A<Page, void, SheetSelectControl*, u32>*)&this->onRightArrowSelectHandler);
      arrows.SetLeftArrowHandler((PtmfHolder_2A<Page, void, SheetSelectControl*, u32>*)&this->onLeftArrowSelectHandler);
      arrows.Load(SIP::buttonFolder, "SIPCupsArrowRight", "ButtonArrowRight", 
      "SIPCupsArrowLeft", "ButtonArrowLeft", 1, 0, false);
      this->controlCount++;
      return &this->arrows;
   }else if(controlId == 3){
      this->AddControl(3, &this->randomControls, 0);
      ControlLoader loader(&this->randomControls);
      loader.Load(SIP::controlFolder, "RandomTrackCup", "RandomControls", NULL);
      return &this->randomControls;
   }
   else return CupSelect::CreateControl(controlId);
}

void ExpandedCupSelect::OnRightArrowSelect(SheetSelectControl::SheetSelectButton *button, u32 hudSlotId){
   this->OnArrowSelect(2);
}

void ExpandedCupSelect::OnLeftArrowSelect(SheetSelectControl::SheetSelectButton *button, u32 hudSlotId){
  this->OnArrowSelect(-2);
}

void ExpandedCupSelect::OnArrowSelect(s32 direction){
   CtrlMenuCupSelectCup *cups = &this->ctrlMenuCupSelectCup;
   cups->curCupID = (cups->curCupID + direction + CUPCOUNT) % CUPCOUNT;
   PushButton **buttons = (PushButton**) cups->childrenGroup.controlArray;
   for(int i = 0; i<8; i++){
      PushButton *curButton = buttons[i];
      u32 nextId = (curButton->buttonId + CUPCOUNT + direction) % CUPCOUNT; 
      curButton->buttonId = nextId;
      ChangeCupTPL(curButton, nextId);
      curButton->SetMsgId(nextId + BMG_CUPS, NULL);
   }
   this->ctrlMenuCupSelectCourse.UpdateTrackList(cups->curCupID);
}

//Disable movies
void ExpandedCupSelect::OnMoviesActivate(u32 r4){}
kmWrite32(0x808404f8, 0x60000000);

//brlyt TPL stuff
void ExpandedCupSelect::ChangeCupTPL(LayoutUIControl *control, u32 id){
   char tplName[0x20];
   snprintf(tplName, 0x20, "icon_%02d.tpl", id);
   SIP::ChangeImage(control, "icon",          tplName);
   SIP::ChangeImage(control, "icon_light_01", tplName);
   SIP::ChangeImage(control, "icon_light_02", tplName);
}

void ExpandedCupSelect::OnBackPress(u32 hudSlotId){
   this->randomizedId = (CourseId) -1;
   CupSelect::OnBackPress(hudSlotId);
}

void ExpandedCupSelect::OnStartPress(u32 hudSlotId){
   if(RaceData::sInstance->menusScenario.settings.gamemode == MODE_TIME_TRIAL && this->randomizedId == -1){
      SIPDataHolder *data = SIPDataHolder::GetInstance();
      CourseId validIds[TRACKCOUNT];
      int counter = 0;
      for(int i = 0; i<TRACKCOUNT; i++){
         if(!data->HasTrophy((CourseId) i, SIP::ttMode)){
            validIds[counter] = (CourseId) i;
            counter++;
         }
      }
      Random random;
      this->randomizedId = validIds[random.NextLimited(counter)];
   }
}

void ExpandedCupSelect::OnSwitchPress(u32 hudSlotId){
   if(RaceData::sInstance->menusScenario.settings.gamemode == MODE_TIME_TRIAL && this->randomizedId == -1){
      Random random;
      this->randomizedId = (CourseId) random.NextLimited(TRACKCOUNT);
   }
}

void ExpandedCupSelect::AfterControlUpdate(){
   CupSelect::AfterControlUpdate();
   this->randomControls.isHidden = true;
   if(RaceData::sInstance->menusScenario.settings.gamemode == MODE_TIME_TRIAL){
      
      u32 trophyCount = SIPDataHolder::GetInstance()->GetTrophyCount(SIP::ttMode);
      ControllerType controllerType = ControllerType(MenuData::sInstance->pad.padInfos[0].controllerSlotAndType & 0xFF);
      u32 randomBmg = BMG_RANDOM_TROPHIES_MISSING;
      if(controllerType == GCN){
         randomBmg = BMG_RANDOM_TROPHIES_MISSING_GCN;
         if(trophyCount == TRACKCOUNT) randomBmg = BMG_RANDOM_ALL_TROPHIES_GCN;
      }else if(trophyCount == TRACKCOUNT) randomBmg = BMG_RANDOM_ALL_TROPHIES;
      this->randomControls.isHidden = false;
      this->randomControls.SetMsgId(randomBmg);
   
      bool isInaccessible = true;
      PushButton **buttons = (PushButton**) this->ctrlMenuCupSelectCup.childrenGroup.controlArray;
      if(this->randomizedId != -1){
         SheetSelectControl::SheetSelectButton *arrow;
         u32 selectedCupId = this->randomizedId / 4;
         u32 low = abs(selectedCupId - buttons[0]->buttonId);
         low = Min(low, CUPCOUNT - low);

         u32 high = abs(selectedCupId - buttons[7]->buttonId);
         high = Min(high, CUPCOUNT - high);
         if(low >= 7 || high >= 7){
            if(high <= low) arrow = &this->arrows.rightArrow;
            else arrow = &this->arrows.leftArrow;
            arrow->Select(0);
         }else{
            u32 finalButtonIdx = abs(selectedCupId - buttons[7]->buttonId);
            finalButtonIdx = 7 - Min(finalButtonIdx, CUPCOUNT - finalButtonIdx);
            buttons[finalButtonIdx]->Select(0);
            buttons[finalButtonIdx]->HandleClick(0, 0);
            Pages::CourseSelect *coursePage = MenuData::sInstance->curScene->Get<Pages::CourseSelect>(COURSE_SELECT);
            SaveSelectedCourse(&coursePage->CtrlMenuCourseSelectCourse.courseButtons[this->randomizedId % 4]);
            coursePage->CtrlMenuCourseSelectCourse.courseButtons[this->randomizedId % 4].Select(0);
            //coursePage->CtrlMenuCourseSelectCourse.courseButtons[this->randomizedId % 4].HandleClick(0, -1);
            this->randomizedId = (CourseId) -1;
            isInaccessible = false;
         }
         for(int i = 0; i<8; i++) buttons[i]->manipulator.inaccessible = isInaccessible;
         this->arrows.leftArrow.manipulator.inaccessible = isInaccessible;
         this->arrows.rightArrow.manipulator.inaccessible = isInaccessible;
      }
   }
   

}

}//namespace SIPUI