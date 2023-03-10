#ifndef _SIPCUPSELECT_
#define _SIPCUPSELECT_
#include <kamek.hpp>
#include <game/UI/Page/Menu/CupSelect.hpp>
#include <game/UI/Ctrl/SheetSelect.hpp>
#include <SkillIssuePack.hpp>

namespace SIPUI{
class ExpandedCupSelect : public Pages::CupSelect{
public:
    ExpandedCupSelect();
    static void ChangeCupTPL(LayoutUIControl *control, u32 id);
    void AfterControlUpdate() override;
    UIControl *CreateControl(u32 controlId) override;
    void OnMoviesActivate(u32 r4) override;
    void OnStartPress(u32 hudSlotId) override;
    
private:
    void OnRightArrowSelect(SheetSelectControl::SheetSelectButton *button, u32 hudSlotId);
    void OnLeftArrowSelect(SheetSelectControl::SheetSelectButton *button, u32 hudSlotId);
    void OnArrowSelect(s32 direction);
    void OnSwitchPress(u32 hudSlotId);
    void OnBackPress(u32 hudSlotId);
    
    PtmfHolder_2A<ExpandedCupSelect, void, SheetSelectControl::SheetSelectButton*, u32> onRightArrowSelectHandler;
    PtmfHolder_2A<ExpandedCupSelect, void, SheetSelectControl::SheetSelectButton*, u32> onLeftArrowSelectHandler;
    PtmfHolder_1A<ExpandedCupSelect, void, u32> onSwitchPressHandler;
    SheetSelectControl arrows;
    LayoutUIControl randomControls;
    CourseId randomizedId;
};
}//namespace SIPUI

#endif