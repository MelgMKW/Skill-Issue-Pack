#include <SlotExpansion/UI/SIPCupSelect.hpp>
#include <UI/BMG.hpp>

//Create
namespace SIPUI{
kmWrite32(0x80623d98, 0x60000000);
ExpandedCupSelect *CreateCupSelect(){
   return new ExpandedCupSelect;
};
kmCall(0x80623da4, CreateCupSelect);

ExpandedCupSelect::ExpandedCupSelect(){
   internControlCount += 1;
   onRightArrowSelectHandler.subject = this;
   onRightArrowSelectHandler.ptmf = &ExpandedCupSelect::OnRightArrowSelect;
   onLeftArrowSelectHandler.subject = this;
   onLeftArrowSelectHandler.ptmf = &ExpandedCupSelect::OnLeftArrowSelect;
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
   }
   else return CupSelect::CreateControl(controlId);
}

void ExpandedCupSelect::OnRightArrowSelect(SheetSelectControl::SheetSelectButton *button, u32 hudSlotId){
   this->OnArrowSelect(button, 2);
}

void ExpandedCupSelect::OnLeftArrowSelect(SheetSelectControl::SheetSelectButton *button, u32 hudSlotId){
  this->OnArrowSelect(button, -2);
}

void ExpandedCupSelect::OnArrowSelect(SheetSelectControl::SheetSelectButton *button, s32 direction){
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
}//namespace SIPUI