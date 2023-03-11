#ifndef _SIPINFODISPLAY_
#define _SIPINFODISPLAY_
#include <kamek.hpp>
#include <UI/CtrlRaceBase/SIPCtrlRaceBase.hpp>
#include <UI/Ctrl/CtrlRace/CtrlRaceWifi.hpp>
#include <SlotExpansion/UI/ExpansionUIMisc.hpp>

//Displays the track name and its author when a race starts, but only once in TTs (until leaving the current track)
namespace SIPUI{
class CtrlRaceTrackInfoDisplay : public CtrlRaceWifiStartMessage {
public:
    static u32 Count();
    static void Create(Page *page, u32 index);
private:
    void Load();
    static u32 lastCourse;
};
}//namespace SIPUI

#endif