#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <UserData/SIPData.hpp>

//Adapted from east_'s code
void FasterMenusOnTransition(MenuData *menuData, u32 delay, u32 r5){
    if (SIPDataHolder::GetInstance()->GetSettings()->pages[SIP_MENU_SETTINGS].radioSetting[SIP_RADIO_FASTMENUS] == SIP_RADIO_FASTMENUS_ENABLED) delay = 0;
    menuData->StartTransition(delay, r5);
}
kmCall(0x80602510, FasterMenusOnTransition);

//Adapted from east_'s code
void FasterMenuPatchTransitionDelay(){
    float transitionDelay = 176.0f;
    if (SIPDataHolder::GetInstance()->GetSettings()->pages[SIP_MENU_SETTINGS].radioSetting[SIP_RADIO_FASTMENUS] == SIP_RADIO_FASTMENUS_ENABLED) transitionDelay -= transitionDelay;
    Page::transitionDelay = transitionDelay;
};
static SettingsHook FasterMenus(FasterMenuPatchTransitionDelay);

//Adapted from east_'s code
void FasterMenusOnBoot(){
    float transitionDelay = 176.0f;
    if (SIPDataHolder::GetInstance()->GetSettings()->pages[SIP_MENU_SETTINGS].radioSetting[SIP_RADIO_FASTMENUS] == SIP_RADIO_FASTMENUS_ENABLED){
        transitionDelay -= transitionDelay;
    }
    Page::transitionDelay = transitionDelay;
};
static BootHook FasterMenusBoot(FasterMenusOnBoot, LOW);