#include <UI/Settings/SIPWFCMainPage.hpp>
#include <UI/BMG.hpp>

namespace SIPUI{
//EXPANDED WFC, keeping WW button and just hiding it in case it is ever needed...
ExpandedWFCMain *CreateExpandedWFCMainPagen(){
    return new(ExpandedWFCMain);
};
kmCall(0x8062405c, CreateExpandedWFCMainPagen);
kmWrite32(0x80624050,0x60000000); //nop the original new
kmWrite32(0x8064b984,0x60000000); //nop the InitControl call in the init func

kmWrite16(0x80899a36, 0x5349); //8064ba38
kmWrite8(0x80899a38, 0x50);
kmWrite16(0x80899a5b, 0x5349); //8064ba90
kmWrite8(0x80899a5d, 0x50);
kmWrite16(0x80899a87, 0x5349); //8064bb44
kmWrite8(0x80899a89, 0x50);


void ExpandedWFCMain::OnInit(){
    this->InitControlGroup(6); //5 controls usually + settings button
    WFCMain::OnInit();
    this->AddControl(5, &settingsButton, 0);

    this->settingsButton.Load(SIP::buttonFolder, "SIPiMenuSingleTop", "Settings", 1, 0, false);
    this->settingsButton.buttonId = 5;
    this->settingsButton.SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*) &this->onSettingsClick, 0);
    this->settingsButton.SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*)&this->onButtonSelectHandler);

    this->worldwideButton.isHidden = true;
    this->worldwideButton.manipulator.inaccessible = true;
    this->topSettingsPage = VS_SETTINGS;
}

void ExpandedWFCMain::OnSettingsButtonClick(PushButton *pushButton, u32 r5){
    Scene *scene = MenuData::sInstance->curScene;
    scene->Get<SettingsPanel>(VS_SETTINGS)->prevPageId = WFC_MAIN_PAGE;
    scene->Get<SettingsPanel>(VS_TEAMS_VIEW)->prevPageId = WFC_MAIN_PAGE;
    scene->Get<SettingsPanel>(BATTLE_MODE_SELECT)->prevPageId = WFC_MAIN_PAGE;
    this->nextPageId = this->topSettingsPage;
    this->EndStateAnimate(pushButton->GetAnimationFrameSize(), 0);
}

void ExpandedWFCMain::ExtOnButtonSelect(PushButton *button, u32 hudSlotId){
    if (button->buttonId == 5){
        u32 bmgId = BMG_SETTINGS_BOTTOM + 1;
        if(this->topSettingsPage == VS_TEAMS_VIEW) bmgId += 1;
        else if(this->topSettingsPage == BATTLE_MODE_SELECT) bmgId += 2;
        this->bottomText.SetMsgId(bmgId, 0);
    } 
    else this->OnButtonSelect(button, hudSlotId);
}

//change initial button and instruction
kmWrite32(0x8064bcb4, 0x386306d8);
kmWrite32(0x8064bcc0, 0x388010d8);
}//namespace SIPUI