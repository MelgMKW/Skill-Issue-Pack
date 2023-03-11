#include <UI/TeamsSelect/TeamSelect.hpp>
#include <UI/BMG.hpp>

namespace SIPUI{
u8 TeamSelectPage::teams[12] = {0};
bool TeamSelectPage::isEnabled = true;
const char *TeamSelectPage::border = "border";
const char *TeamSelectPage::miiBg = "bg";


TeamSelectPage::TeamSelectPage(){
    hasBackButton = true;
    externControlCount = 0;
    internControlCount = 26;
    movieStartFrame = -1;
    extraControlNumber = 0;
    controlSources = 2;
    titleBmg = 0x2511;
    nextPageId = PAGE_NONE;
    prevPageId = FRIEND_ROOM;
    nextMenu = MENU_NONE;
    isLocked = false;
    controlCount = 0;
    onArrowClickHandler.subject = this;
    onArrowClickHandler.ptmf = &TeamSelectPage::OnArrowClick;
    onArrowSelectHandler.subject = this;
    onArrowSelectHandler.ptmf = &TeamSelectPage::OnArrowSelect;
    onArrowDeselectHandler.subject = this;
    onArrowDeselectHandler.ptmf = &TeamSelectPage::OnArrowDeselect;
    onButtonClickHandler.subject = this;
    onButtonClickHandler.ptmf = static_cast<void (Menu::*)(PushButton *, u32)>(&TeamSelectPage::OnButtonClick);
    onButtonSelectHandler.subject = this;
    onButtonSelectHandler.ptmf = static_cast<void (Menu::*)(PushButton *, u32)>(&TeamSelectPage::OnButtonSelect);
    onButtonDeselectHandler.subject = this;
    onButtonDeselectHandler.ptmf = static_cast<void (Menu::*)(PushButton *, u32)>(&TeamSelectPage::OnButtonDeselect);
    onBackPressHandler.subject = this;
    onBackPressHandler.ptmf = static_cast<void (Menu::*)(u32)>(&TeamSelectPage::OnBackPress);
    onToggleButtonClick.subject = this;
    onToggleButtonClick.ptmf = &TeamSelectPage::OnToggleButtonClick;
    isEnabled = false;
    
    this->controlsManipulatorManager.Init(1, false);
    this->SetManipulatorManager(&controlsManipulatorManager);
    this->controlsManipulatorManager.SetGlobalHandler(START_PRESS, (PtmfHolder_1A<Page, void, u32>*) &onStartPressHandler, false, false);
    this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, (PtmfHolder_1A<Page, void, u32>*) &onBackPressHandler, false, false);   
    activePlayerBitfield = 1;
    
}

void TeamSelectPage::OnInit(){
    this->miiGroup = &MenuData::sInstance->curScene->Get<Pages::FriendRoomManager>(FRIEND_ROOM_MANAGER)->miiGroup;
    Menu::OnInit();
    for(int idx = 0; idx < 12; idx++){
        if(this->teams[idx] == 1) this->RotateArrow(&this->arrows[idx]);
    }
}

void TeamSelectPage::BeforeEntranceAnimations(){
    Pages::Menu::BeforeEntranceAnimations();
    if(this->toggle.state != this->isEnabled) this->toggle.ToggleState(this->isEnabled);
    this->toggle.SelectInitial(0);
    u32 bmgId = this->toggle.state == false ? BMG_TEAMS_DISABLED : BMG_TEAMS_ENABLED;
    this->toggle.SetMsgId(bmgId);
    this->isLocked = false;
    for(int idx = 0; idx<12; idx++){
        u8 curTeam = this->teams[idx];
        if(curTeam == 1) this->RotateArrow(&this->arrows[idx]);
        this->SetColours(idx, curTeam);
        this->miis[idx].animator.GetAnimationGroupById(0)->PlayAnimationAtFrameAndDisable(!curTeam, 0.0f);
    }
}

void TeamSelectPage::BeforeControlUpdate(){
    RKNetController *controller =RKNetController::sInstance;
    RKNetControllerSub *sub = &controller->subs[0]; 
    if(sub->connectionUserDatas[0].playersAtConsole == 0) sub = &controller->subs[1];
    //u32 playerCount = MenuData::sInstance->curScene->Get<Pages::FriendRoomManager>(FRIEND_ROOM_MANAGER)->playerCount;
    int idx = 0;
    for(int aid = 0; aid<12; aid++){
        if(sub->availableAids & (1<<aid)){
            for(int player = 0; player < sub->connectionUserDatas[aid].playersAtConsole; player++){
                    this->miis[idx].SetMiiPane("chara", this->miiGroup, aid * 2 + player, 2);
                    this->miis[idx].SetMiiPane("chara_shadow", this->miiGroup, aid * 2 + player, 2);
                    this->miis[idx].isHidden = false;
                    this->arrows[idx].isHidden = false;
                    this->arrows[idx].manipulator.inaccessible = false;
                    this->arrowMiiIdx[idx] = aid * 2 + player;
                    idx++;
            }
        }
    }
    for(int remIdx = idx; remIdx < 12; remIdx++){
        this->miis[remIdx].isHidden = true;
        this->arrows[remIdx].isHidden = true;
        this->arrows[remIdx].manipulator.inaccessible = true; 
    }
}

UIControl *TeamSelectPage::CreateControl(u32 id){
    if(id > 26) return NULL;
    u32 count = this->controlCount;
    this->controlCount++;
    char variant[0x40];
    if(id < 12){
        this->AddControl(count, &this->arrows[id], 0);
        snprintf(variant, 0x40, "Arrow%d", id);
        this->arrows[id].Load(SIP::buttonFolder, "SIPTeamArrow", variant, 1, 0, false);
        this->arrows[id].buttonId = id;
        this->arrows[id].SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32> *) &this->onArrowClickHandler, 0);
        this->arrows[id].SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton *, u32> *) &this->onArrowSelectHandler);
        return &this->arrows[id];
    }else if(id < 24){
        id = id - 12;
        this->AddControl(count, &this->miis[id], 0);
        ControlLoader loader(&this->miis[id]);
        snprintf(variant, 0x40, "Mii%d", id);
        const char *brctr = "SIPTeamMii";
        static const char *miiAnim[5] = {"Translate", "TranslateRight", "TranslateLeft", NULL, NULL};
        //if(SystemManager::sInstance->isWidePage == 0) brctr = "SIPTeamMii43";
        loader.Load(SIP::controlFolder, brctr, variant, miiAnim);
        return &this->miis[id];
    }else if(id == 24){
        this->AddControl(count, &this->name, 0);
        ControlLoader loader(&this->name);
        const char *brctr = "SIPTeamName";
        loader.Load(SIP::controlFolder, brctr, brctr, NULL);
        return &this->name;
    
    }else /*if(id == 25)*/{
        this->AddControl(count, &this->toggle, 0);
        this->toggle.Load(SIP::buttonFolder, "SIPTeamSelectEnable", "Enable", 1, 0, false);
        this->toggle.SetOnChangeHandler((PtmfHolder_1A<Page, void, ToggleButton *>*)&this->onToggleButtonClick);
        return &this->toggle;
    }
    /*
    else{
        this->AddControl(count, &this->bg, 0);
        ControlLoader loader(&this->bg);
        const char *brctr = "SIPTeamsBG";
        loader.Load(SIP::bgFolder, brctr, brctr, NULL);
        return &this->bg;
    }
    */
}



void TeamSelectPage::OnArrowClick(PushButton *button, u32 hudSlotId){
    this->RotateArrow(button);
    u32 id = button->buttonId;
    u8 newTeam = TeamSelectPage::teams[id]^1;
    TeamSelectPage::teams[id] = newTeam;
    this->SetColours(id, newTeam);
    this->miis[button->buttonId].animator.GetAnimationGroupById(0)->PlayAnimationAtFrameAndDisable(!TeamSelectPage::teams[button->buttonId], 0.0f);
}

void TeamSelectPage::OnArrowSelect(PushButton *button, u32 hudSlotId){
    this->text.miis[0] = this->miiGroup->GetMii(this->arrowMiiIdx[button->buttonId]);
    this->name.SetMsgId(BMG_MII_NAME, &this->text);
}

void TeamSelectPage::OnToggleButtonClick(ToggleButton *button){
    u32 bmgId = button->state == false ? BMG_TEAMS_DISABLED : BMG_TEAMS_ENABLED;
    this->isEnabled = button->state;
    button->SetMsgId(bmgId);
}

int TeamSelectPage::GetActivePlayerBitfield() const{
    return this->activePlayerBitfield;
}

int TeamSelectPage::GetPlayerBitfield() const{
    return this->playerBitfield;
}

ManipulatorManager *TeamSelectPage::GetManipulatorManager(){
    return &this->controlsManipulatorManager;
}

UIControl *TeamSelectPage::CreateExternalControl(u32 id){
    return NULL;
}

void TeamSelectPage::SetButtonHandlers(PushButton *button){
    button->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*) &this->onButtonClickHandler, 0);
    button->SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*) &this->onButtonSelectHandler);
    button->SetOnDeselectHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*) &this->onButtonDeselectHandler);
}

void TeamSelectPage::OnBackPress(u32 hudSlotId){
    this->EndStateAnimate(0.0f, 1);
}

void TeamSelectPage::OnButtonClick(PushButton *button, u32 hudSlotId){
    this->EndStateAnimate(button->GetAnimationFrameSize(), 1);
}

void TeamSelectPage::RotateArrowPane(PushButton *button, const char *name){
    Picture *pane = (Picture*) button->layout.GetPaneByName(name);
    nw4r::math::VEC2 *vec = (nw4r::math::VEC2*) pane->texCoords.data;
    vec[0].x = 1.0f - vec[0].x;
    vec[1].x = 1.0f - vec[1].x;
    vec[2].x = 1.0f - vec[2].x;
    vec[3].x = 1.0f - vec[3].x;
}

void TeamSelectPage::SetColours(u32 idx, u8 team){
    s16 r = 33;
    s16 b = 0xFF;
    s16 g = 33;
    if(team == 1){
        r = 0xDC;
        b = 0;
        g = 33;
    }
    nw4r::lyt::Material *arrowBorder = this->arrows[idx].layout.GetPaneByName(this->border)->GetMaterial();
    arrowBorder->tevColours[0].r = r;
    arrowBorder->tevColours[0].g = 0;
    arrowBorder->tevColours[0].b = b;
    arrowBorder->tevColours[0].a = 255;
    arrowBorder->tevColours[1].r = r;
    arrowBorder->tevColours[1].g = 0;
    arrowBorder->tevColours[1].b = b;
    arrowBorder->tevColours[1].a = 255;


    nw4r::lyt::Material *miiBg = this->miis[idx].layout.GetPaneByName(this->miiBg)->GetMaterial();
    //miiBg->tevColours[0].r = r;
    //miiBg->tevColours[0].g = 0;
    //miiBg->tevColours[0].b = b;
    //miiBg->tevColours[0].a = 255;
    
    miiBg->tevColours[1].r = r;
    miiBg->tevColours[1].g = 0;
    miiBg->tevColours[1].b = b;
    miiBg->tevColours[1].a = 255;

    
    nw4r::lyt::Material *miiBorder = this->miis[idx].layout.GetPaneByName(this->border)->GetMaterial();
    if(r == 0xDC) r = 0xb3;
    else{
        r = 0;
        g = 0;
    }
    miiBorder->tevColours[0].r = r;
    miiBorder->tevColours[0].g = 0;
    miiBorder->tevColours[0].b = b;
    miiBorder->tevColours[0].a = 255;
    miiBorder->tevColours[1].r = r;
    miiBorder->tevColours[1].g = 0;
    miiBorder->tevColours[1].b = b;
    miiBorder->tevColours[1].a = 255;
}
}//namespace SIPUI