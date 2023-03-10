#ifndef _SIPWFC_
#define _SIPWFC_
#include <kamek.hpp>
#include <game/UI/Page/Other/WFCMain.hpp>
#include <UI/Settings/SettingsPanel.hpp>

//Extends WFCMain to add a settings button
namespace SIPUI{
class ExpandedWFCMain : public Pages::WFCMain {
public:
    ExpandedWFCMain(){
        this->onSettingsClick.subject = this;
        this->onSettingsClick.ptmf = &ExpandedWFCMain::OnSettingsButtonClick;
        this->onButtonSelectHandler.ptmf = static_cast<void (WFCMain::*)(PushButton*, u32)> (&ExpandedWFCMain::ExtOnButtonSelect);
    }
    void OnInit() override;
private:
    void OnSettingsButtonClick(PushButton *PushButton, u32 r5);
    void ExtOnButtonSelect(PushButton *pushButton, u32 hudSlotId);
    PtmfHolder_2A<ExpandedWFCMain, void, PushButton*, u32> onSettingsClick;
    PushButton settingsButton;
public:
    PageId topSettingsPage;
};
}//namespace SIPUI

#endif