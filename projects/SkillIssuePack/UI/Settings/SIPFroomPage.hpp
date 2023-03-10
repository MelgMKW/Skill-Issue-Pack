#ifndef _SIPFROOMPAGE_
#define _SIPFROOMPAGE_
#include <kamek.hpp>
#include <game/UI/Page/Other/FriendRoom.hpp>
#include <game/UI/Page/Other/Message.hpp>
#include <UI/Settings/SettingsPanel.hpp>

/*Extends FriendRoom to add a settings and a change teams button; contrary to the other two pages where the settings are present,
here they are added as a layer. That is because the FriendRoom still needs to run in the background as it is what checks when players join/leave,
when the host starts a gamemode etc...
Consequently, most controls present on the FriendRoom (whether its own or other Pages') are hidden when these buttons are pressed 
to prevent ugly Z-fighting
*/
namespace SIPUI{
class ExpandedFroom : public Pages::FriendRoom{
public:
    ExpandedFroom(){
        this->onSettingsClickHandler.subject = this;
        this->onSettingsClickHandler.ptmf = &ExpandedFroom::OnSettingsButtonClick;
        this->onTeamsClickHandler.subject = this;
        this->onTeamsClickHandler.ptmf = &ExpandedFroom::OnTeamsButtonClick;
        this->onButtonSelectHandler.ptmf = static_cast<void (FriendRoom::*)(PushButton*, u32)> (&ExpandedFroom::ExtOnButtonSelect);
    }
    void OnInit() override;
    void AfterControlUpdate() override;
    void OnResume() override;
    void OnMessageBoxClick(Pages::MessageBoxTransparent *msgPage);
    
private:
    void EnableControls(bool isVisible);
    void ExtOnButtonSelect(PushButton *button, u32 hudSlotId);
    void OnSettingsButtonClick(PushButton *button, u32 hudSlotId);
    void OnTeamsButtonClick(PushButton *button, u32 hudSlotId);
    PtmfHolder_2A<ExpandedFroom, void, PushButton*, u32> onSettingsClickHandler;
    PtmfHolder_2A<ExpandedFroom, void, PushButton*, u32> onTeamsClickHandler;
    PushButton settingsButton;
    PushButton teamsButton;
public:
    PageId topSettingsPage;
};
}//namespace SIPUI
#endif