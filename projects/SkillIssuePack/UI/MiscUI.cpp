#include <kamek.hpp>
#include <game/UI/Ctrl/PushButton.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceWifi.hpp>
#include <Debug/SIPDebug.hpp>
#include <UI/BMG.hpp>

namespace SIPUI{
//BMG size patch (Diamond)
kmWrite32(0x8007B37C, 0x38000128);


//Credit to Kazuki for making the original ASM code, and Brawlbox for porting it to C++
void LaunchRiivolutionButton(MenuData *menuData){
    MenuId id = menuData->nextMenuId;
    if (id == CHANNEL_FROM_MENU || id == CHANNEL_FROM_CHECK_RANKINGS || id == CHANNEL_FROM_DOWNLOADS) SIPDebug::LaunchSoftware();
    else menuData->LoadNextMenu();
}
kmCall(0x80553a60, LaunchRiivolutionButton);

//Top left message when a race is about to start in a froom
void FixStartMessageFroom(CtrlRaceWifiStartMessage *startMsg, u32 bmgId, TextInfo *info){
    MenuData *menuData = MenuData::sInstance;
    MenuId id = menuData->curScene->menuId;
    if(id == P1_WIFI_FRIEND_VS_GAMEPLAY || id == P1_WIFI_FRIEND_TEAM_VS_GAMEPLAY 
    || id == P2_WIFI_FRIEND_VS_GAMEPLAY || id == P2_WIFI_FRIEND_TEAM_VS_GAMEPLAY){
        bmgId = BMG_GP_RACE;
        info->intToPass[0] = menuData->menudata98->currentRaceNumber + 1;
        info->intToPass[1] = SIP::racesPerGP + 1;
    }
    startMsg->SetMsgId(bmgId, info);
}
kmCall(0x807f8b7c, FixStartMessageFroom);
} //namespace SIPUI