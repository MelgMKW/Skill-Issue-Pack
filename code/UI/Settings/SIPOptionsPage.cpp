#include <UI/Settings/SIPOptionsPage.hpp>


namespace  SIPUI{
ExpandedOptions *CreateSIPOptionsPage(){
    return new(ExpandedOptions);
};
kmCall(0x8062453c, CreateSIPOptionsPage);
kmWrite32(0x80624530,0x60000000); //nop the original new
kmWrite32(0x805fd754,0x60000000); //nop the InitControl call in the init func

void ExpandedOptions::OnInit(){
    this->InitControlGroup(5 + 1);
    Options::OnInit();
    this->AddControl(this->controlGroup.controlCount - 1, &settingsButton, 0);

    this->settingsButton.Load(SIP::buttonFolder, "OptionSettingsButton", "Settings", 1, 0, false);

    this->settingsButton.buttonId = 5;
    this->settingsButton.SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*)&this->onButtonClickHandler, 0);
    this->settingsButton.SelectInitialButton(0);
    this->topSettingsPage = VS_SETTINGS;
}

void ExpandedOptions::ExpandedOnButtonClick(PushButton *pushButton, u32 hudSlotId){
    if (pushButton->buttonId == 5){
        this->nextPageId = this->topSettingsPage;
        this->EndStateAnimate(pushButton->GetAnimationFrameSize(), 0);
    }
    else{
        this->OnButtonClick(pushButton, hudSlotId);
    }
}

void PatchOptionsBRCTR(PushButton *button, const char*folderName, const char*ctrName, const char*variant, u32 playerCount, u32 r8, bool inaccessible){
    button->Load(folderName, "OptionSettingsButton", variant, playerCount, r8, inaccessible);
}
kmCall(0x805fd7bc, PatchOptionsBRCTR); //so that the positions are correct
kmCall(0x805fd80c, PatchOptionsBRCTR);
kmCall(0x805fd858, PatchOptionsBRCTR);
}//namespace SIPUI