#include <kamek.hpp>
#include <game/UI/Page/Menu/SinglePlayer.hpp>
#include <game/Item/ItemPlayer.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceItemWindow.hpp>
#include <game/GlobalFunctions.hpp>
#include <SkillIssuePack.hpp>
#include <Ghost/MultiGhostMgr.hpp>
#include <UI/BMG.hpp>

//Implements 4 TT modes by splitting the "Time Trials" button
void SetCC();

namespace SIPUI{
const char *buttonBrctr = "SIPTT";
kmWrite32(0x806266c0, 0x38000007);
kmWrite32(0x8084f080, 0x7F89E378);
void LoadCorrectBRCTR(PushButton *button, const char*folderName, const char*ctrName, const char*variant, u32 localPlayerBitfield, u32 r8, u32 idx){
    if(idx == 1 || idx > 3){
        char ttVariant[0x15];
        ctrName = buttonBrctr;
        u32 varId;
        if(idx == 1) varId = 0;
        else varId = idx - 3;
        snprintf(ttVariant, 0x15, "%s_%d", buttonBrctr, varId);
        variant = ttVariant;
    }
    button->Load(folderName, ctrName, variant, localPlayerBitfield, r8, false);
    ((Pages::SinglePlayer*)(button->parentGroup->parentPage))->curMovieCount = 0;
    //if(idx > 3) idx = 1;
    //return idx ^ 0x80000000; //so that correct movie crop is done
}
kmCall(0x8084f084, LoadCorrectBRCTR);
//kmWrite32(0x8084f08c, 0x60000000);
//kmWrite32(0x8084f094, 0x38030001);
//kmWrite32(0x8084f098, 0x60000000);

//Hacky custom CalcDistance so that the navigating the single player menu is intuitive
int FixCalcDistance(ControlManipulator *subject, ControlManipulator *other, Directions direction, 
                    bool hasHorizontalWrapping, bool hasVerticalWrapping){
    u32 subId = ((CtrlMenuSingleTopMovieButton*)(subject->actionHandlers[0]->subject))->buttonId;
    u32 destId = ((CtrlMenuSingleTopMovieButton*)(other->actionHandlers[0]->subject))->buttonId;
    switch(subId){
        case(0):
            if(direction == DIRECTION_DOWN && destId == 1) return 1;
            break;
        case(2):
            if(direction == DIRECTION_UP && destId == 1) return 1;
            break;
        case(1):
        case(4):
        case(5):
        case(6):
            if(direction == DIRECTION_UP && destId == 0 || direction == DIRECTION_DOWN && destId == 2) return 1;
    }
    return subject->CalcDistanceBothWrapping(other, direction);
}

void SetDistanceFunc(ControlsManipulatorManager *mgr){
    mgr->calcDistanceFunc = &FixCalcDistance;
}
kmCall(0x8084ef68, SetDistanceFunc);


void OnButtonSelect(Pages::SinglePlayer *page, PushButton *button, u32 hudSlotId){
    u32 id = button->buttonId;
    if(id > 3) page->bottomText->SetMsgId(BMG_TT_MODE_BOTTOM_SINGLE + id - 3);
    else page->Pages::SinglePlayer::OnExternalButtonSelect(button, hudSlotId);
}
kmWritePointer(0x808D9F64, &OnButtonSelect);



//Sets the ttMode based on which button was clicked
void OnButtonClick(Pages::SinglePlayer *page, PushButton *button, u32 hudSlotId){
    u32 id = button->buttonId;
    if(id == 1 || id > 3) button->buttonId = 1;

    page->Pages::SinglePlayer::OnButtonClick(button, hudSlotId);
    button->buttonId = id;
    if(id == 1 || id > 3){
        if(id == 1) SIP::ttMode = SIP::SIPTTMODE_150;
        else SIP::ttMode = (SIP::TTMode)(id -3);
        SetCC();
    }
}
kmWritePointer(0x808BBED0, OnButtonClick);

//Since the item only exists within TT, it is much easier to do it this way
const char *ChangeFeatherTPL(ItemId id, u32 itemCount){
    if(id == BLOOPER){
        if(itemCount == 2) return "gesso_2";
        else if(itemCount == 3) return "gesso_3";
    }
    return GetItemIconPaneName(id, itemCount);
}
kmCall(0x807ef168, ChangeFeatherTPL);
}//namespace SIPUI

//Sets the CC (based on the mode) when retrying after setting a time, as racedata's CC is overwritten usually
void SetCC(){
    EngineClass cc = CC_150;
    if(SIP::ttMode == SIP::SIPTTMODE_200 || SIP::ttMode == SIP::SIPTTMODE_200_FEATHER) cc = CC_100;
    RaceData::sInstance->menusScenario.settings.engineClass = cc;
}
kmBranch(0x805e1ef4, SetCC);
kmBranch(0x805e1d58, SetCC);

//3 feathers in TTs, the game has already checked the gamemode so no need to do it again
void SetStartingItem(PlayerInventory *inventory, ItemId id, u8 r5){
    SIP::TTMode mode = SIP::ttMode;
    if(mode == SIP::SIPTTMODE_150_FEATHER || mode == SIP::SIPTTMODE_200_FEATHER){
        inventory->SetItem(BLOOPER, r5);
        inventory->currentItemCount = 3;
    } 
    else inventory->SetItem(id, r5);
}
kmCall(0x80799808, SetStartingItem);
