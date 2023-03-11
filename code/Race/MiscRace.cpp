#include <kamek.hpp>
#include <game/Race/racedata.hpp>
#include <game/Network/RKNetController.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceBalloon.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceResult.hpp>
#include <game/Item/ItemManager.hpp>
#include <SkillIssuePack.hpp>
#include <UserData/SIPData.hpp>


//From JoshuaMK, ported to C++ by Brawlbox and adapted as a setting
int MiiHeads(RaceData *racedata, int r4, int r5, u8 id){
    u32 charId = racedata->racesScenario.players[id].characterId;
    if (SIPDataHolder::GetInstance()->GetSettings()->pages[SIP_RACE_SETTINGS].radioSetting[SIP_RADIO_MII] == SIP_RADIO_MII_ENABLED){
        if (charId < 0x2a){
            if (id == 0) charId = 0x2a;
            else if(RKNetController::sInstance->connectionState != 0) charId = 0x2a;
        }
    }
    return charId;
}
kmCall(0x807eb154, MiiHeads);
kmWrite32(0x807eb15c, 0x60000000);
kmWrite32(0x807eb160, 0x88de01b4);

//credit to XeR for finding the float address
void BattleGlitchEnable(){ 
    float maxDistance = 7500.0;
    if(SIPDataHolder::GetInstance()->GetSettings()->pages[SIP_RACE_SETTINGS].radioSetting[SIP_RADIO_BATTLE] == SIP_RADIO_BATTLE_GLITCH_ENABLED) maxDistance = 75000.0;
    RaceBalloons::maxDistanceNames = maxDistance;
}
static SettingsHook BattleGlitch(BattleGlitchEnable);

//Mega TC
void MegaTC(KartMovement *movement){
    movement->ActivateMega();
}
kmCall(0x80580630, MegaTC);

kmWrite32(0x8085C914 ,0x38000000); //times at the end of races in VS
void DisplayTimesInsteadOfNames(CtrlRaceResult *result, u8 id){
    result->DisplayFinishTime(id);
}
kmCall(0x8085d460, DisplayTimesInsteadOfNames); //for WWs

//Halve TC duration
kmWrite32(0x80580618, 0x38800132);

//Draggable blue shells
void DraggableBlueShells(ItemPlayerSub *sub){
    if(SIPDataHolder::GetInstance()->GetSettings()->pages[SIP_RACE_SETTINGS].radioSetting[SIP_RADIO_BLUES] == SIP_RADIO_DRAGGABLE_BLUES_DISABLED) sub->isNotDragged = true;
}
kmBranch(0x807ae8ac , DraggableBlueShells);

// No Team Invincibility
kmWrite32(0x8056fd24, 0x38000000); //KartCollision::CheckKartCollision()
kmWrite32(0x80572618, 0x38000000); //KartCollision::CheckItemCollision()
kmWrite32(0x80573290, 0x38000000); //KartCollision::HandleFIBCollision()
kmWrite32(0x8068e2d0, 0x38000000); //PlayerEffects ctor
kmWrite32(0x8068e314, 0x38000000); //PlayerEffects ctor
kmWrite32(0x807a7f6c, 0x38c00000); //FIB are always red
kmWrite32(0x807b0bd4, 0x38000000); //pass TC to teammate
kmWrite32(0x807bd2bc, 0x38000000); //RaceGlobals
kmWrite32(0x807f18c8, 0x38000000); //TC alert

////don't hide position tracker (MrBean35000vr)
kmWrite32(0x807F4DB8, 0x38000001); 
