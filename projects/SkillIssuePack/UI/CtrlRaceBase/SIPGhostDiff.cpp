#include <UI/CtrlRaceBase/SIPGhostDiff.hpp>

namespace SIPUI{
//brctr change SIPtTime
kmWrite16(0x808a9af9, 0x5349);
kmWrite8(0x808a9afb, 0x50);

SIPGhostDiff *SIPGhostDiff::Create(Pages::RaceHUD *page, u8 idx){
    SIPGhostDiff *ghostDiff = new(SIPGhostDiff);
    page->AddControl(idx, ghostDiff, 0);
    ghostDiff->Load();
    return ghostDiff;
}
kmWrite32(0x8085844c, 0x48000060);
kmCall(0x808584c0, SIPGhostDiff::Create); //skip inlined ctor of usual ghostdiff
kmWrite32(0x808584d4, 0x60000000); //nop load of usual ghostdiff

SIPGhostDiff::SIPGhostDiff(){
    RacedataScenario *scenario = &RaceData::sInstance->menusScenario;
    if(scenario->players[0].playerType == PLAYER_GHOST) this->isGhostReplay = true;
    u32 count = 0;
    for(int i = 1; i < 4; i++) if(scenario->players[i].playerType == PLAYER_GHOST) count++;
    this->diffTimeCount = count;
}
void SIPGhostDiff::Load(){
    if(this->diffTimeCount > 0){
        this->diffTimes = new CtrlRaceGhostDiffTime[this->diffTimeCount];
        this->InitControlGroup(this->diffTimeCount);
        char variantName[0x80];
        for(int i = 0; i < this->diffTimeCount; i++){
            this->AddControl(i, &this->diffTimes[i]);
            snprintf(variantName, 0x80, "TimeDiffGhost_%d", i);
            this->diffTimes[i].Load(variantName);
        }
    }
   
}

void SIPGhostDiff::Init(){
    RaceData *racedata = RaceData::sInstance;
    for(int i = 0; i < this->diffTimeCount; i++) this->diffTimes[i].ghostData.Init(&racedata->ghosts[i + this->isGhostReplay]);
    UIControl::Init();
}
kmWrite32(0x807ee500, 0x48000010);
}//namespace SIPUI