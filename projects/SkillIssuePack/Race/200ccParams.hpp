#include <kamek.hpp>
#include <game/Race/RaceData.hpp>

#define speedFactor 1.5f //a value of 1 is normal 150cc
#define GLOBAL 0 //set this to 1 if you want 200cc to replace all CC or 0 if you want it to only replace 100cc

#if GLOBAL == 0
  #define CHECK_200 RaceData::sInstance->racesScenario.settings.engineClass == CC_100
#else
  #define CHECK_200 true
#endif