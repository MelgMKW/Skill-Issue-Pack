#include <game/UI/Page/Other/Globe.hpp>
#include <UI/Settings/SIPFroomPage.hpp>
#include <UI/TeamsSelect/TeamSelect.hpp>
#include <UI/BMG.hpp>

namespace SIPUI{
kmWrite32(0x80624200, 0x60000000); //nop the new
kmWrite32(0x805d8260, 0x60000000); //nop initcontrolgroup

ExpandedFroom *CreateSIPFroomPage(){
    TeamSelectPage *teamSelect = new(TeamSelectPage);
    MenuData::sInstance->curScene->Set(teamSelect, MII_SELECT);
    teamSelect->Setup(MII_SELECT);
    return new(ExpandedFroom);
}
kmCall(0x8062420c, CreateSIPFroomPage);

void ExpandedFroom::OnInit(){
    this->InitControlGroup(7); //5 usually + settings button
    FriendRoom::OnInit();

    this->AddControl(5, &settingsButton, 0);
    this->settingsButton.Load(SIP::buttonFolder, "FroomSettingsButton", "Settings", 1, 0, false);
    this->settingsButton.buttonId = 5;
    this->settingsButton.SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*) &this->onSettingsClickHandler, 0);
    this->settingsButton.SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*)&this->onButtonSelectHandler);
    this->topSettingsPage = VS_SETTINGS;

    this->AddControl(6, &teamsButton, 0);
    this->teamsButton.Load(SIP::buttonFolder, "FroomSettingsButton", "Teams", 1, 0, false);
    this->teamsButton.buttonId = 6;
    this->teamsButton.SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*) &this->onTeamsClickHandler, 0);
    this->teamsButton.SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*)&this->onButtonSelectHandler);

    this->manipulatorManager.SetDistanceFunc(3);

}

void ExpandedFroom::OnResume(){
    this->EnableControls(true);
    FriendRoom::OnResume();
}

void ExpandedFroom::ExtOnButtonSelect(PushButton *button, u32 hudSlotId){
    if (button->buttonId == 5){
        u32 bmgId = BMG_SETTINGS_BOTTOM + 1;
        if(this->topSettingsPage == VS_TEAMS_VIEW) bmgId += 1;
        else if(this->topSettingsPage == BATTLE_MODE_SELECT) bmgId += 2;
        this->bottomText.SetMsgId(bmgId, 0);
    }
    else if(button->buttonId == 6) this->bottomText.SetMsgId(BMG_TEAMS_BOTTOM, 0);
    else this->OnButtonSelect(button, hudSlotId);
}

void ExpandedFroom::OnSettingsButtonClick(PushButton *button, u32 hudSlotId){
    this->EnableControls(false);
    Scene *scene = MenuData::sInstance->curScene;
    scene->Get<SettingsPanel>(VS_SETTINGS)->prevPageId = FRIEND_ROOM;
    scene->Get<SettingsPanel>(VS_TEAMS_VIEW)->prevPageId = FRIEND_ROOM;
    scene->Get<SettingsPanel>(BATTLE_MODE_SELECT)->prevPageId = FRIEND_ROOM;
    this->AddPageLayer(this->topSettingsPage, 0);
}

void ExpandedFroom::OnTeamsButtonClick(PushButton *button, u32 hudSlotId){
    this->EnableControls(false);
    this->AddPageLayer(MII_SELECT, 0);
}

void ExpandedFroom::AfterControlUpdate(){
    FriendRoom::AfterControlUpdate();
    RKNetController *controller = RKNetController::sInstance;
    RKNetControllerSub *sub = &controller->subs[controller->currentSub];
    bool hidden = true;

    if(sub->hostAid == sub->localAid && sub->playerCount >= 2){
        hidden = false;
    } 
    //this->teamsButton.isHidden = hidden;
    //this->teamsButton.manipulator.inaccessible = hidden;
}

void ExpandedFroom::OnMessageBoxClick(Pages::MessageBoxTransparent *msgBoxPage){
    Scene *scene = MenuData::sInstance->curScene;
    scene->activePages[scene->layerCount - 1] = msgBoxPage;
    scene->layerCount--;
}

void FixLayerCountOnMsgBoxClick(Pages::MessageBoxTransparent *msgBoxPage){
    Scene *scene = MenuData::sInstance->curScene;
    if(scene->layerCount == 9){
        scene->activePages[scene->layerCount] = NULL;
        scene->layerCount--;
    }
    msgBoxPage->Reset();
}
kmCall(0x805d860c, FixLayerCountOnMsgBoxClick);

void ExpandedFroom::EnableControls(bool isEnabled){
    bool hidden = !isEnabled;
    this->messagesButton.isHidden = hidden;
    this->startButton.isHidden = hidden;
    this->addFriendsButton.isHidden = hidden;
    this->backButton.isHidden = hidden;
    this->bottomText.isHidden = hidden;
    this->settingsButton.isHidden = hidden;
    this->teamsButton.isHidden = hidden;


    Scene *scene = MenuData::sInstance->curScene;

    Pages::Globe *globe = scene->Get<Pages::Globe>(GLOBE);
    globe->message.isHidden = hidden;

    Pages::FriendRoomWaiting *waiting = scene->Get<Pages::FriendRoomWaiting>(FRIEND_ROOM_WAITING_TEXT);
    waiting->messageWindow.isHidden = hidden;

    Pages::FriendRoomManager *mgr = scene->Get<Pages::FriendRoomManager>(FRIEND_ROOM_MANAGER);
    mgr->titleText.isHidden = hidden;
    mgr->busySymbol.isHidden = hidden;
    for(FriendMatchingPlayer *player = &mgr->miiIcons[0]; player<&mgr->miiIcons[24]; player++) player->isHidden = hidden;
}

}//namespace SIPUI