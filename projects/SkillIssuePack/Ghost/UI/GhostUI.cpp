#include <Ghost/UI/GhostUI.hpp>
#include <Ghost/MultiGhostMgr.hpp>
#include <UI/BMG.hpp>

namespace  SIPUI{
//When the player goes back to the main menu, MultiGhostMgr is destroyed
void DestroyMultiGhostManager(Scene *scene, PageId pageId){
    scene->CreatePage(pageId);
    MultiGhostMgr::DestroyInstance();
}
kmCall(0x8062cf98, DestroyMultiGhostManager);

//GhostInfoControl BRCTR
void LoadCustomGhostInfoBRCTR(ControlLoader *loader, const char*folderName, const char*ctrName, const char*variantName, const char** anims){
    loader->Load(folderName, "SIPGhostInfo", variantName, anims);
}
kmCall(0x805e28c0, LoadCustomGhostInfoBRCTR);

SIPGhostSelect *CreateExpandedSelectGhostPage(){
    MultiGhostMgr::CreateInstance();
    return new(SIPGhostSelect);
};
kmCall(0x80623dec, CreateExpandedSelectGhostPage);
kmWrite32(0x80623de0, 0x60000000); //nop the new
kmWrite32(0x80639958, 0x3880000B); //Add one control

SIPGhostSelect::SIPGhostSelect(){
    this->onChallengeGhostClickHandler.ptmf = static_cast<void(Page::*)(PushButton*, u32)>(&SIPGhostSelect::OnChallengeGhostPress);
    this->onWatchReplayClickHandler.ptmf = static_cast<void(Page::*)(PushButton*, u32)>(&SIPGhostSelect::OnWatchReplayPress);
    this->onRightArrowPressHandler.ptmf = static_cast<void(Page::*)(SheetSelectControl*, u32)>(&SIPGhostSelect::OnRightArrowPress);
    this->onLeftArrowPressHandler.ptmf = static_cast<void(Page::*)(SheetSelectControl*, u32)>(&SIPGhostSelect::OnLeftArrowPress);
    this->onSelectGhostChangeHandler.subject = this;
    this->onSelectGhostChangeHandler.ptmf = static_cast<void (Page::*)(ToggleButton*)>(&SIPGhostSelect::OnSelectGhostChange);
    //this->onBackPress.ptmf = static_cast<void(Page::*)(u32)>(&ExtendedGhostSelectPage::OnBackPress);
}

void SIPGhostSelect::OnInit(){
    GhostSelect::OnInit();
    this->AddControl(0x9, &this->bottomText, 0);
    this->bottomText.Load();
    this->selectGhostButton.buttonId = 0xA;
    this->AddControl(0xA, &this->selectGhostButton, 0);
    this->selectGhostButton.Load(SIP::buttonFolder, "SIPSelGhostButton", "SelectGhost", 1, 0, false); //check multighost
    this->selectGhostButton.SetOnChangeHandler(&this->onSelectGhostChangeHandler);
    this->Reset();
}

//BottomText will display the current TTmode as well as if the player has a trophy on the track
void SIPGhostSelect::OnActivate(){
    GhostSelect::OnActivate();
    const TextInfo &text = GetCourseBottomText(SIP::GetCourseId());
    this->bottomText.SetMsgId(BMG_TT_BOTTOM_COURSE, &text);
    this->ctrlMenuPageTitleText.SetMsgId(BMG_CHOOSE_GHOST_DATA);
    this->Reset();
    this->selectGhostButton.SetMsgId(BMG_SELECT_GHOST);
}

//Creates space by making the usual 3 buttons smaller, could be done without a BRCTR but this is easier to maintain
const char ghostSelectButtonCtr[23] = "SIPTimeAttackGhostList";
void LoadButtonWithCustBRCTR(PushButton *button, const char*folderName, const char*ctrName, const char*variant, u32 localPlayerBitfield, u32 r8, bool inaccessible){
    button->Load(folderName, ghostSelectButtonCtr, variant, localPlayerBitfield, r8, inaccessible);
}
kmCall(0x80639ab8, LoadButtonWithCustBRCTR);
kmCall(0x80639ad8, LoadButtonWithCustBRCTR);
kmCall(0x80639af8, LoadButtonWithCustBRCTR);


void SIPGhostSelect::OnChallengeGhostPress(PushButton *button, u32 hudSlotId){
    GhostSelect::OnChallengeGhostPress(button, hudSlotId);
    GhostListEntry *entry = &this->ghostList->entries[this->page];
    MultiGhostMgr::GetInstance()->EnableGhost(entry, true);
}

void SIPGhostSelect::OnWatchReplayPress(PushButton *button, u32 hudSlotId){
    GhostSelect::OnWatchReplayPress(button, hudSlotId);
    GhostListEntry *entry = &this->ghostList->entries[this->page];
    MultiGhostMgr::GetInstance()->EnableGhost(entry, true);
}

void SIPGhostSelect::OnSelectGhostChange(ToggleButton *button){
    MultiGhostMgr *holder = MultiGhostMgr::GetInstance();
    GhostListEntry *entry = &this->ghostList->entries[this->page];
    
    if(button->state == true){
        u32 index = holder->lastUsedSlot;
        if(holder->EnableGhost(entry, false)){
            this->selectedGhostsPages[index] = this->page;
            this->selectedGhostsCount = (this->selectedGhostsCount + 1) > 3 ? 3 : (this->selectedGhostsCount + 1);
        } 
        else button->ToggleState(false);
    }else{
        holder->DisableGhost(entry);
        this->selectedGhostsPages[holder->lastUsedSlot] = -1;
        this->selectedGhostsCount -= 1;
    }
    TextInfo text;
    text.intToPass[0] = this->selectedGhostsCount;
    this->ctrlMenuPageTitleText.SetMsgId(BMG_GHOST_SELECTED_COUNTER, &text);
    this->SetToggleBMG();
}

void SIPGhostSelect::OnRightArrowPress(SheetSelectControl *control, u32 hudSlotId){
    GhostSelect::OnRightArrowPress(control, hudSlotId);
    this->OnNewPage();
}

void SIPGhostSelect::OnLeftArrowPress(SheetSelectControl *control, u32 hudSlotId){
    GhostSelect::OnLeftArrowPress(control, hudSlotId);
    this->OnNewPage();
}

void SIPGhostSelect::OnNewPage(){
    ToggleButton *button = &this->selectGhostButton;
    if (this->page == this->selectedGhostsPages[0] || this->page == this->selectedGhostsPages[1] || this->page == this->selectedGhostsPages[2]){
        if(button->state == false){
            button->ToggleState(true);
        }
    }
    else if(button->state == true) button->ToggleState(false);
    this->SetToggleBMG();
}

void SIPGhostSelect::SetToggleBMG(){
    ToggleButton *button = &this->selectGhostButton;
    u32 bmgId = button->state == false ? BMG_SELECT_GHOST : BMG_GHOST_SELECTED;
    button->SetMsgId(bmgId);
}

void SIPGhostSelect::Reset(){
    this->selectedGhostsPages[0] = -1;
    this->selectedGhostsPages[1] = -1;
    this->selectedGhostsPages[2] = -1;
    this->selectedGhostsCount = 0;
    this->selectGhostButton.ToggleState(false);
}

void TrophyBMG(CtrlMenuInstructionText *bottomText, u32 bmgId){

    TextInfo text;
    text.intToPass[0] = SIPDataHolder::GetInstance()->GetTrophyCount(SIP::ttMode);
    text.intToPass[1] = TRACKCOUNT;
    text.bmgToPass[0] = BMG_TT_MODE_BOTTOM_CUP + SIP::ttMode;
    bottomText->SetMsgId(BMG_TT_BOTTOM_CUP, &text);
}
kmCall(0x8084144c, TrophyBMG);

void IndividualTrophyBMG(Pages::CourseSelect *courseSelect, CtrlMenuCourseSelectCourse *course, PushButton *button, u32 hudSlotId){
    if(RaceData::sInstance->menusScenario.settings.gamemode != MODE_TIME_TRIAL){
        courseSelect->UpdateBottomText(course, button, hudSlotId);
        return;
    }
    const TextInfo &text = GetCourseBottomText((CourseId) button->buttonId);
    courseSelect->bottomText->SetMsgId(BMG_TT_BOTTOM_COURSE, &text);
}
//kmCall(0x807e5294, IndividualTrophyBMG);
kmCall(0x807e54ec, IndividualTrophyBMG);

//Global function as it is also used by CourseSelect
const TextInfo GetCourseBottomText(CourseId id){
    bool hasTrophy = SIPDataHolder::GetInstance()->HasTrophy(id, SIP::ttMode);
    TextInfo text;
    text.bmgToPass[0] = BMG_TT_MODE_BOTTOM_CUP + SIP::ttMode;
    u32 bmgId = BMG_NO_TROPHY;
    if(hasTrophy) bmgId = BMG_TROPHY; 
    text.bmgToPass[1] = bmgId;
    return text;
}
}//namespace  SIPUI