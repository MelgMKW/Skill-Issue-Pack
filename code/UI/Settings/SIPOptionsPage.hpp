#ifndef _SIPOPTIONS_
#define _SIPOPTIONS_
#include <kamek.hpp>
#include <game/UI/Page/Other/Options.hpp>
#include <SkillIssuePack.hpp>

//Extends Options to add a settings button
namespace SIPUI{
class ExpandedOptions : public Pages::Options {
public:
    ExpandedOptions(){
        this->onButtonClickHandler.ptmf = static_cast<void (Options::*)(PushButton*, u32)> (&ExpandedOptions::ExpandedOnButtonClick);
    }
    void OnInit() override;
private:
    void ExpandedOnButtonClick(PushButton *pushButton, u32 hudSlotId);
    PushButton settingsButton;
public:
    PageId topSettingsPage;
};
}//namespace SIPUI
#endif