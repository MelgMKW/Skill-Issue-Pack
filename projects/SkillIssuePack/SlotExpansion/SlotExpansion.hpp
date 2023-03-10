#ifndef _SIPSLOTEXPANSION_
#define _SIPSLOTEXPANSION_
#include <kamek.hpp>
#include <game/UI/Page/Menu/CupSelect.hpp>
#include <game/UI/Page/Menu/CourseSelect.hpp>
#include <game/Sound/RaceAudioManager.hpp>
#include <game/UI/Page/Other/Votes.hpp>
#include <game/Network/network.hpp>

extern u32 lastSelectedCup;
extern u32 lastSelectedButtonIdx;
extern u32 selectedCourse;
CourseId GetCorrectTrackSlot();
void SaveSelectedCourse(PushButton *button);


#endif