#ifndef _SIPSOM_
#define _SIPSOM_
#include <kamek.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <UI/CtrlRaceBase/SIPCtrlRaceBase.hpp>

//Simple speedometer that uses TPLs instead of text because the default font numbers have a very ugly black outline
namespace SIPUI{
class CtrlRaceSpeedo : public CtrlRaceBase {
public:
    void Init() override;
    void OnUpdate() override;
    static u32 Count();
    static void Create(Page *page, u32 index);
private:
    void Load(const char* variant, u8 id);
};
}//namespace SIPUI

#endif