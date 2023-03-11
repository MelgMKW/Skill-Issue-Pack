#ifndef _TEAMSELECT_
#define _TEAMSELECT_
#include <kamek.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Page/Menu/Menu.hpp>
#include <game/UI/Page/Menu/MiiSelect.hpp>
#include <game/UI/Page/Other/FriendRoom.hpp>
#include <game/Network/RKNetController.hpp>
#include <core/System/SystemManager.hpp>
#include <SkillIssuePack.hpp>
#include <UI/ToggleButton.hpp>

//Custom Page that allows the host (and only the host) to arrange teams as they see fit. This also has a ToggleButton to disable it if wanted
namespace SIPUI{
class TeamSelectPage : public Pages::Menu {
public:
    TeamSelectPage();
    void OnInit() override; //To be defined
    void BeforeEntranceAnimations() override;
    void BeforeControlUpdate() override;
    int GetActivePlayerBitfield() const override;
    int GetPlayerBitfield() const override;
    ManipulatorManager *GetManipulatorManager() override;; //offset 0x70
    UIControl *CreateExternalControl(u32 id) override; //0x84
    UIControl *CreateControl(u32 id) override; //0x88
    void SetButtonHandlers(PushButton *pushButton) override; //80853aac 0x8C
    
private:
 
    void OnArrowClick(PushButton *button, u32 hudSlotId);
    void OnArrowSelect(PushButton *button, u32 hudSlotId);
    void OnArrowDeselect(PushButton *button, u32 hudSlotId){};
    void OnButtonClick(PushButton *button, u32 hudSlotId);
    void OnButtonSelect(PushButton *button, u32 hudSlotId){};
    void OnButtonDeselect(PushButton *button, u32 hudSlotId){};
    void OnBackPress(u32 hudSlotId);
    void OnToggleButtonClick(ToggleButton *button);
    void SetColours(u32 idx, u8 team);
    static void RotateArrow(PushButton *button){
        RotateArrowPane(button, "chara_shadow");
        RotateArrowPane(button, "color_yajirushi");
        RotateArrowPane(button, "chara_c_down");
        RotateArrowPane(button, "chara_light_01");
    }

    static void RotateArrowPane(PushButton *button, const char *pane);
    PtmfHolder_2A<TeamSelectPage, void, PushButton*, u32> onArrowClickHandler;
    PtmfHolder_2A<TeamSelectPage, void, PushButton*, u32> onArrowSelectHandler;
    PtmfHolder_2A<TeamSelectPage, void, PushButton*, u32> onArrowDeselectHandler;
    PtmfHolder_1A<TeamSelectPage, void, ToggleButton*> onToggleButtonClick;
    PushButton arrows[12];
    LayoutUIControl miis[12];
    MiiName name;
    MiiGroup *miiGroup; //take friendroom's
    //LayoutUIControlScaleFade bg;
    u8 arrowMiiIdx[12];
    

public:
    ToggleButton toggle;
    static u8 teams[12];
    static bool isEnabled;
    static const char *miiBg;
    static const char *border;

 
      
};
}//namespace SIPUI
#endif