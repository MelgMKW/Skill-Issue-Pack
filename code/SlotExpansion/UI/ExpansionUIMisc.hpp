#ifndef _SIPEXPANSIONUI_
#define _SIPEXPANSIONUI_
#include <kamek.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Page/Menu/DriftSelect.hpp>
#include <game/UI/Page/Menu/CourseSelect.hpp>
#include <game/UI/Ctrl/SheetSelect.hpp>
#include <game/UI/Page/Other/GhostSelect.hpp>
#include <game/GlobalFunctions.hpp>
#include <SkillIssuePack.hpp>
#include <UserData/SIPData.hpp>
#include <SlotExpansion/SlotExpansion.hpp>
#include <SlotExpansion/UI/SIPCupSelect.hpp>
namespace SIPUI{
int GetCorrectTrackBMGById(u8 id);
int GetCurTrackBMG();
}//namespace SIPUI

#endif