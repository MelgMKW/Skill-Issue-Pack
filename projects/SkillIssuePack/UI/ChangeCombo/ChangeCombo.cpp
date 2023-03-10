#include <UI/ChangeCombo/ChangeCombo.hpp>

u8 kartsSortedByWeight[3][12] = {
    {STANDARD_KART_S, BABY_BOOSTER, MINI_BEAST, CHEEP_CHARGER, RALLY_ROMPER, BLUE_FALCON, STANDARD_BIKE_S, BULLET_BIKE, BIT_BIKE, QUACKER, MAGIKRUISER, JET_BUBBLE},
    {STANDARD_KART_M, CLASSIC_DRAGSTER, WILD_WING, SUPER_BLOOPER, ROYAL_RACER, SPRINTER, STANDARD_BIKE_M, MACH_BIKE, BON_BON, RAPIDE, NITROCYCLE, DOLPHIN_DASHER},
    {STANDARD_KART_L,  OFFROADER, FLAME_FLYER, PIRANHA_PROWLER, JETSETTER, HONEYCOUPE, STANDARD_BIKE_L, BOWSER_BIKE, WARIO_BIKE, SHOOTING_STAR, SPEAR, PHANTOM},
};

namespace SIPUI{
kmWrite32(0x806508d4, 0x60000000); //Add VR screen outside of 1st race in frooms
kmWrite32(0x806240e0, 0x60000000); //nop the new
Pages::VR *AddComboPages(){ 
    MenuId menuid = MenuData::sInstance->curScene->menuId;
    
    if(menuid == P1_WIFI_VS_VOTING || menuid == P1_WIFI_FRIEND_ROOM_VS_VOTING || menuid == P1_WIFI_FRIEND_ROOM_TEAM_VS_VOTING
     || menuid == P1_WIFI_BATTLE_VOTING || menuid == P1_WIFI_FRIEND_ROOM_BALLOON_VOTING || menuid == P1_WIFI_FRIEND_ROOM_COIN_VOTING){
        return new(ExpandedVR); 
    }
    return new(Pages::VR);
}
kmCall(0x806240ec, AddComboPages);


ExpandedVR::ExpandedVR() : comboButtonState(0){
    this->onRandomComboClick.subject = this;
    this->onRandomComboClick.ptmf = &ExpandedVR::RandomizeCombo;
    this->onChangeComboClick.subject = this;
    this->onChangeComboClick.ptmf = &ExpandedVR::ChangeCombo;
}

void TempKillInitControl(Pages::VR *page, u32 controlCount){
    MenuId menuid = MenuData::sInstance->curScene->menuId;
    if(menuid == P1_WIFI_VS_VOTING || menuid == P1_WIFI_FRIEND_ROOM_VS_VOTING || menuid == P1_WIFI_FRIEND_ROOM_TEAM_VS_VOTING
    || menuid == P1_WIFI_BATTLE_VOTING || menuid == P1_WIFI_FRIEND_ROOM_BALLOON_VOTING || menuid == P1_WIFI_FRIEND_ROOM_COIN_VOTING) return;
    page->InitControlGroup(controlCount);
}
kmCall(0x8064a61c, TempKillInitControl);
//kmWrite32(0x8064a61c, 0x60000000); //nop initControlGroup

void ExpandedVR::OnInit(){
    this->InitControlGroup(0x11);
    VR::OnInit();

    PushButton *button = &this->randomComboButton;
    this->AddControl(0xF, button, 0);
    button->Load(SIP::buttonFolder, "WifiMemberButtons", "ButtonRandom", 1, 0, false);
    button->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*)&this->onRandomComboClick, 0);

    button = &this->changeComboButton;
    this->AddControl(0x10, button, 0);
    button->Load(SIP::buttonFolder, "WifiMemberButtons", "ButtonChange", 1, 0, false);
    button->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*)&this->onChangeComboClick, 0);
    button->manipulator.SetAction(START_PRESS, &button->onClickHandlerObj, 0);

    Scene *scene = MenuData::sInstance->curScene;
    
    Pages::CountDownTimer *countdownPage = scene->Get<Pages::CountDownTimer>(TIMER);
    CountDown *timer = &countdownPage->countdown;

    Pages::CharacterSelect *charPage = scene->Get<Pages::CharacterSelect>(CHARACTER_SELECT);
    charPage->timer = timer;
    charPage->ctrlMenuCharSelect.timer = timer;

    Pages::KartSelect *kartPage = scene->Get<Pages::KartSelect>(KART_SELECT);
    if (kartPage != NULL) kartPage->timer = timer;

    Pages::BattleKartSelect *kartBattlePage = scene->Get<Pages::BattleKartSelect>(BATTLE_KART_SELECT);
    if (kartBattlePage != NULL) kartBattlePage->timer = timer;

    Pages::MultiKartSelect *multiKartPage = scene->Get<Pages::MultiKartSelect>(MULTIPLAYER_KART_SELECT);
    if (multiKartPage != NULL) multiKartPage->timer = timer;

    Pages::DriftSelect *driftPage = scene->Get<Pages::DriftSelect>(DRIFT_SELECT);
    if(driftPage != NULL) driftPage->timer = timer;

    Pages::MultiDriftSelect *multiDriftPage = scene->Get<Pages::MultiDriftSelect>(MULTIPLAYER_DRIFT_SELECT);
    if(multiDriftPage != NULL) multiDriftPage->timer = timer;
}

void UseCorrectOKButtonBRCTR(PushButton *okButton, const char*folderName, const char*ctrName, const char*variant, u32 localPlayerCount, u32 r8, bool inaccessible){
    MenuId menuid = MenuData::sInstance->curScene->menuId;
    if(menuid == P1_WIFI_VS_VOTING || menuid == P1_WIFI_FRIEND_ROOM_VS_VOTING || menuid == P1_WIFI_FRIEND_ROOM_TEAM_VS_VOTING
     || menuid == P1_WIFI_BATTLE_VOTING || menuid == P1_WIFI_FRIEND_ROOM_BALLOON_VOTING || menuid == P1_WIFI_FRIEND_ROOM_COIN_VOTING){
        ctrName = "WifiMemberButtons";
    }
    okButton->Load(folderName, ctrName, variant, localPlayerCount, r8, inaccessible);
}
kmCall(0x8064a6e8, UseCorrectOKButtonBRCTR);

void ExpandedVR::RandomizeCombo(PushButton *randomComboButton, u32 hudSlotId){
    Random random;
    

    MenuData *menu = MenuData::sInstance;
    MenuData98 *menuData98 = menu->menudata98;
    for(int hudId = 0; hudId < menuData98->localPlayerCount; hudId++){
        CharacterId character = (CharacterId) random.NextLimited(24);
        u32 randomizedKartPos = random.NextLimited(12);
        KartId kart = (KartId)(kartsSortedByWeight[CharacterIDToWeightClass(character)][randomizedKartPos]);
    
        menuData98->characters[hudId] = character;
        menuData98->karts[hudId] = kart;
        menuData98->combos[hudId].selCharacter = character;
        menuData98->combos[hudId].selKart = kart;
        
        Scene *scene = menu->curScene;
        ExpandedCharacterSelect *charSelect = scene->Get<ExpandedCharacterSelect>(CHARACTER_SELECT); //guaranteed to exist on this page
        for(int i = 0; i<24; i++){
            if(buttonIdToCharacterId[i] == character){
                charSelect->randomizedCharId = i;
                charSelect->rolledCharId = i;
                charSelect->rouletteCounter = 0x50;
                charSelect->ctrlMenuCharSelect.selectedCharacter = character;
                break;
            }
        }

        ExpandedBattleKartSelect *battleKartSelect = scene->Get<ExpandedBattleKartSelect>(BATTLE_KART_SELECT);
        if(battleKartSelect != NULL) battleKartSelect->selectedKart = random.NextLimited(2);

        ExpandedKartSelect *kartSelect = scene->Get<ExpandedKartSelect>(KART_SELECT);
        if(kartSelect != NULL){
            kartSelect->rouletteCounter = 0x50;
            kartSelect->randomizedKartPos = randomizedKartPos;
            kartSelect->rolledKartPos = randomizedKartPos;
        }

        ExpandedMultiKartSelect *multiKartSelect = scene->Get<ExpandedMultiKartSelect>(MULTIPLAYER_KART_SELECT);
        if(multiKartSelect != NULL){
            multiKartSelect->rouletteCounter = 0x50;
            multiKartSelect->rolledKartPos[0] = randomizedKartPos;
            u32 options = 12;
            MenuId menuId = scene->menuId;
            if(menuId == P2_WIFI_BATTLE_VOTING || menuId == P2_WIFI_FRIEND_ROOM_BALLOON_VOTING || menuId == P2_WIFI_FRIEND_ROOM_COIN_VOTING){
                options = 2;
            } 
            multiKartSelect->rolledKartPos[1] = random.NextLimited(options);
        }
    }
    this->comboButtonState = 1;
    this->EndStateAnimate(randomComboButton->GetAnimationFrameSize(), 0);
}

void ExpandedVR::ChangeCombo(PushButton *changeComboButton, u32 hudSlotId){
    this->comboButtonState = 2;
    this->EndStateAnimate(changeComboButton->GetAnimationFrameSize(), 0);
}

void AddChangeComboPages(Scene *scene, PageId id){
    scene->CreatePage(id);
    scene->CreatePage(CHARACTER_SELECT);
    switch(scene->menuId){
        case(P1_WIFI_VS_VOTING):
        case(P1_WIFI_FRIEND_ROOM_VS_VOTING):
        case(P1_WIFI_FRIEND_ROOM_TEAM_VS_VOTING):
            scene->CreatePage(KART_SELECT);
            scene->CreatePage(DRIFT_SELECT);
            break;
        case(P1_WIFI_BATTLE_VOTING):
        case(P1_WIFI_FRIEND_ROOM_BALLOON_VOTING):
        case(P1_WIFI_FRIEND_ROOM_COIN_VOTING):
            scene->CreatePage(BATTLE_KART_SELECT);
            scene->CreatePage(DRIFT_SELECT);
            break;
        case(P2_WIFI_VS_VOTING):
        case(P2_WIFI_FRIEND_ROOM_VS_VOTING):
        case(P2_WIFI_FRIEND_ROOM_TEAM_VS_VOTING):
        case(P2_WIFI_BATTLE_VOTING):
        case(P2_WIFI_FRIEND_ROOM_BALLOON_VOTING):
        case(P2_WIFI_FRIEND_ROOM_COIN_VOTING):
            scene->CreatePage(MULTIPLAYER_KART_SELECT);
            scene->CreatePage(MULTIPLAYER_DRIFT_SELECT);
            break;
    }
          
}  
//P1
kmCall(0x8062e09c, AddChangeComboPages); //0x58 can't do this more efficiently because supporting page 0x7F breaks kart images
kmCall(0x8062e7e0, AddChangeComboPages); //0x60
kmCall(0x8062e870, AddChangeComboPages); //0x61
kmCall(0x8062e0e4, AddChangeComboPages); //0x59
kmCall(0x8062e900, AddChangeComboPages); //0x62
kmCall(0x8062e990, AddChangeComboPages); //0x63
//P2
kmCall(0x8062e708, AddChangeComboPages); //0x5e
kmCall(0x8062e798, AddChangeComboPages); //0x5f
kmCall(0x8062ea68, AddChangeComboPages); //0x64
kmCall(0x8062eaf8, AddChangeComboPages); //0x65
kmCall(0x8062eb88, AddChangeComboPages); //0x66
kmCall(0x8062ec18, AddChangeComboPages); //0x67


kmWrite32(0x80623d50, 0x60000000);
ExpandedCharacterSelect *AddCharSelect(){
    return new(ExpandedCharacterSelect);
}
kmCall(0x80623d5c, AddCharSelect);

void ExpandedCharacterSelect::BeforeControlUpdate(){
    CtrlMenuCharacterSelect::ButtonDriver *array = this->ctrlMenuCharSelect.driverButtonsArray;
    s32 roulette = this->rouletteCounter;
    if(roulette == 0x50){
        for(CtrlMenuCharacterSelect::ButtonDriver *button = &array[0]; button<&array[0x1A]; button++){
            button->manipulator.inaccessible = true;
            button->HandleDeselect(0,0);
        }
    }
    if(roulette > 0){
        u32 prevChar = this->rolledCharId;
        Random random;
        if(roulette == 1) this->rolledCharId = this->randomizedCharId;
        else while(this->rolledCharId == prevChar) this->rolledCharId = random.NextLimited(24);
        array[prevChar].HandleDeselect(0, -1);
        array[this->rolledCharId].HandleSelect(0, -1);
        array[this->rolledCharId].Select(0);
        this->rouletteCounter--;
    }
    else if(roulette == 0){
        this->rouletteCounter = -1;

        array[this->randomizedCharId].HandleClick(0, -1);
    }
}
//store correct buttons in menudata98

kmWrite32(0x80623e58, 0x60000000);
ExpandedBattleKartSelect *AddBattleKartSelect(){
    return new(ExpandedBattleKartSelect);
}
kmCall(0x80623e64, AddBattleKartSelect);

void ExpandedBattleKartSelect::BeforeControlUpdate(){
    s32 kart = this->selectedKart;
    if(kart >= 0 && this->currentState == 0x4){
        PushButton *kartButton = (PushButton*) this->controlGroup.controlArray[kart];
        kartButton->HandleSelect(0, -1);
        kartButton->Select(0);
        kartButton->HandleClick(0, -1);
        PushButton *otherButton = (PushButton*) this->controlGroup.controlArray[kart^1];
        otherButton->HandleDeselect(0, -1);
        this->selectedKart = -1;
    }
}


kmWrite32(0x80623d68, 0x60000000);
ExpandedKartSelect *AddKartSelect(){
    return new(ExpandedKartSelect);
}
kmCall(0x80623d74, AddKartSelect);

void ExpandedKartSelect::BeforeControlUpdate(){
    s32 roulette = this->rouletteCounter;
    if(roulette >= 0){
        LayoutUIControl *globalButtonHolder = (LayoutUIControl*) this->controlGroup.controlArray[2]; //holds the 6 controls that each hold a pair of buttons
        ButtonMachine *randomizedButton = (ButtonMachine*) globalButtonHolder->childrenGroup.controlArray[this->randomizedKartPos/2]->childrenGroup.controlArray[this->randomizedKartPos%2];
        if(roulette == 0x50){

            LayoutUIControl *buttonHolder;
            for(buttonHolder = (LayoutUIControl*) globalButtonHolder->childrenGroup.controlArray[0];
                buttonHolder <= (LayoutUIControl*) globalButtonHolder->childrenGroup.controlArray[6]; buttonHolder++){

                ButtonMachine *leftButton = (ButtonMachine*) buttonHolder->childrenGroup.controlArray[0];
                leftButton->manipulator.inaccessible = true;
                leftButton->HandleDeselect(0,0);

                ButtonMachine *rightButton = (ButtonMachine*) buttonHolder->childrenGroup.controlArray[1];
                rightButton->manipulator.inaccessible = true;
                rightButton->HandleDeselect(0,0);
            }
            this->curButtonId = randomizedButton->buttonId;
        }
        if(roulette > 0){
            Random random;
            int prevKart = this->rolledKartPos;
            ButtonMachine *prevButton = (ButtonMachine*) globalButtonHolder->childrenGroup.controlArray[prevKart/2]->childrenGroup.controlArray[prevKart%2];
            prevButton->HandleDeselect(0, -1);
            
            int nextKart = prevKart;
            if(roulette == 1) nextKart = this->randomizedKartPos;
            else while(nextKart == prevKart) nextKart = random.NextLimited(12);
            ButtonMachine *nextButton = (ButtonMachine*) globalButtonHolder->childrenGroup.controlArray[nextKart/2]->childrenGroup.controlArray[nextKart%2];
            nextButton->HandleSelect(0, -1);
            nextButton->Select(0);
            this->rolledKartPos = nextKart;
            this->rouletteCounter--;
        }
        else if(roulette == 0){
            this->rouletteCounter = -1;
            randomizedButton->HandleClick(0, -1);
        }
    }
}

kmWrite32(0x80623f60, 0x60000000);
ExpandedMultiKartSelect *AddMultiKartSelect(){
    return new(ExpandedMultiKartSelect);
}
kmCall(0x80623f6c, AddMultiKartSelect);
void ExpandedMultiKartSelect::BeforeControlUpdate(){
    s32 roulette = this->rouletteCounter;
    Random random;
    MenuData *menudata = MenuData::sInstance;
    for(int i = 0; i < menudata->menudata98->localPlayerCount; i++){ //in all likelihood always 2
        if(roulette >= 0){

            if(roulette == 0x50){
                this->arrows[i].manipulator.inaccessible = true;
                MenuId menuId = menudata->curScene->menuId;
                u32 options = 12;
                if(menuId == P2_WIFI_BATTLE_VOTING || menuId == P2_WIFI_FRIEND_ROOM_BALLOON_VOTING || menuId == P2_WIFI_FRIEND_ROOM_COIN_VOTING){
                    options = 2;
                } 
                this->arrows[i].SelectInitial(options, this->rolledKartPos[i]);
            } 
            if(roulette > 0){
                if(random.NextLimited(2) == 0) this->arrows[i].HandleRightPress(i, -1);
                else this->arrows[i].HandleLeftPress(i, 0);  
            }
            else if(roulette == 0) this->arrows[i].HandleClick(i, -1);
        }
    }
    this->rouletteCounter--;
}



void DriftSelectBeforeControlUpdate(Pages::DriftSelect *driftSelect){
    ExpandedCharacterSelect *charSelect = MenuData::sInstance->curScene->Get<ExpandedCharacterSelect>(CHARACTER_SELECT);
    if(charSelect->rouletteCounter != -1 && driftSelect->currentState == 0x4){
        PushButton *autoButton = (PushButton*) driftSelect->controlGroup.controlArray[1];
        autoButton->HandleDeselect(0, -1);
        PushButton *manualButton = (PushButton*) driftSelect->controlGroup.controlArray[0];
        manualButton->HandleSelect(0, -1);
        manualButton->Select(0);
        manualButton->HandleClick(0, -1);
        charSelect->rouletteCounter = -1;
    }
}
kmWritePointer(0x808D9DF8, DriftSelectBeforeControlUpdate);

void MultiDriftSelectBeforeControlUpdate(Pages::MultiDriftSelect *multiDriftSelect){
    ExpandedCharacterSelect *charSelect = MenuData::sInstance->curScene->Get<ExpandedCharacterSelect>(CHARACTER_SELECT);;
    if(charSelect->rouletteCounter != -1 && multiDriftSelect->currentState == 0x4){
        for(int i = 0; i < MenuData::sInstance->menudata98->localPlayerCount; i++){
            PushButton *autoButton = multiDriftSelect->externControls[0 + 2 * i];
            autoButton->HandleDeselect(i, -1);
            PushButton *manualButton = multiDriftSelect->externControls[1 + 2 * i];
            manualButton->HandleSelect(i, -1);
            manualButton->Select(i);
            manualButton->HandleClick(i, -1);
        }
        charSelect->rouletteCounter = -1;
    }
}
kmWritePointer(0x808D9C10, DriftSelectBeforeControlUpdate);

void AddCharSelectLayer(Pages::CountDownTimer *page, PageId id, u32 r5){
    Scene *scene =   MenuData::sInstance->curScene;
    MenuId menuid = scene->menuId;
    if(menuid == P1_WIFI_VS_VOTING || menuid == P1_WIFI_FRIEND_ROOM_VS_VOTING || menuid == P1_WIFI_FRIEND_ROOM_TEAM_VS_VOTING
    || menuid == P1_WIFI_BATTLE_VOTING || menuid == P1_WIFI_FRIEND_ROOM_BALLOON_VOTING || menuid == P1_WIFI_FRIEND_ROOM_COIN_VOTING){
        ExpandedVR *votingPage = scene->Get<ExpandedVR>(PLAYER_LIST_VR_PAGE); //always present when 0x90 is present
        if(votingPage->comboButtonState != 0) id = CHARACTER_SELECT;
    }
    return page->AddPageLayer(id, r5);
}
kmCall(0x806509d0, AddCharSelectLayer);
}//namespace SIPUI
