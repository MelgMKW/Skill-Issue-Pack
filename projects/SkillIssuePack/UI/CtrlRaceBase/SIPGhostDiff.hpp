#ifndef _SIPGHOSTDIFF_
#define _SIPGHOSTDIFF_
#include <kamek.hpp>
#include <game/UI/Ctrl/UIControl.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceGhostDiffTime.hpp>
#include <game/UI/Page/RaceHUD/RaceHUD.hpp>
#include <game/Race/racedata.hpp>

//Completely replaces the usual ghost diff with a control that holds as many as there are ghosts. Also moves them to the top left corner
namespace SIPUI{
class SIPGhostDiff : public UIControl{
public:
    SIPGhostDiff();
    static SIPGhostDiff *Create(Pages::RaceHUD *page, u8 controlIdx);
    void Init() override;
private:
    void Load();
    CtrlRaceGhostDiffTime *diffTimes;
    u32 diffTimeCount;
    bool isGhostReplay;
};
}//namespace SIPUI

#endif