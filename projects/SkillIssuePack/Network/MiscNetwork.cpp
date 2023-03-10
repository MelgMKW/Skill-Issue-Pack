#include <kamek.hpp>
#include <game/Network/RKNetController.hpp>
#include <game/Network/RKNetSelect.hpp>
#include <game/System/random.hpp>
#include <SkillIssuePack.hpp>
#include <UserData/SIPData.hpp>
#include <SlotExpansion/SlotExpansion.hpp>
#include <UI/TeamsSelect/TeamSelect.hpp>

//No disconnect for being idle (Bully)
kmWrite32(0x80521408, 0x38000000);
kmWrite32(0x8053EF6C, 0x38000000);
kmWrite32(0x8053F0B4, 0x38000000);
kmWrite32(0x8053F124, 0x38000000);

//Overwrites CC rules -> 10% 200, 65% 150, 25% mirror and/or in frooms, overwritten by host setting
void DecideCC(RKNetSELECTHandler *handler){
    u8 ccSetting = SIPDataHolder::GetInstance()->GetSettings()->pages[SIP_HOST_SETTINGS].radioSetting[SIP_RADIO_CC];
    u8 ccClass = 1; //1 200, 2 150, 3 mirror
    if(ccSetting == SIP_RADIO_CC_150) ccClass = 2;
    if(handler->mode == ONLINEMODE_PUBLIC_VS || handler->mode == ONLINEMODE_PUBLIC_BATTLE || ccSetting == SIP_RADIO_CC_NORMAL){
        Random random;
        u32 result = random.NextLimited(100);
        if(result > 10) ccClass = 2;
        if(result > 75) ccClass = 3;
    }
    handler->toSendPacket.engineClass = ccClass;
}
kmCall(0x80661404, DecideCC);

//Sets Team using the TeamSelectPage if it has been enabled by the host; verifies the validity of the teams
void SetTeams(RKNetSELECTHandler *select, u32 &teams){
    RKNetController *controller = RKNetController::sInstance; //check if this only called for the host
    RKNetControllerSub *sub = &controller->subs[controller->currentSub];
    SIPUI::TeamSelectPage *teamPage = MenuData::sInstance->curScene->Get<SIPUI::TeamSelectPage>(MII_SELECT);
    bool isValid = false;
    u8 firstTeam = teamPage->teams[0];
    if(select->mode == ONLINEMODE_PRIVATE_VS && sub->localAid == sub->hostAid && SIPUI::TeamSelectPage::isEnabled){
        for(int i = 0; i<12; i++){
            u8 curSlotTeam = teamPage->teams[i];
            if(curSlotTeam != firstTeam) isValid = true;
            teams = teams | (curSlotTeam & 1) << i;
        }
    }
    if(!isValid) select->DecideTeams(teams);
};
kmCall(0x806619d8, SetTeams);