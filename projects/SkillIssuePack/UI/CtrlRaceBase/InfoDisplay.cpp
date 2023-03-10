#include <UI/CtrlRaceBase/InfoDisplay.hpp>
#include <UI/BMG.hpp>

namespace SIPUI{
//So that it is only done once in TTs
u32 CtrlRaceTrackInfoDisplay::lastCourse = -1;

u32 CtrlRaceTrackInfoDisplay::Count(){
   u32 gamemode = RaceData::sInstance->racesScenario.settings.gamemode;
   if ((gamemode == MODE_GRAND_PRIX) || (gamemode == MODE_VS_RACE) || (gamemode == MODE_PUBLIC_VS)|| (gamemode == MODE_PRIVATE_VS)) return 1;
   if(gamemode == MODE_TIME_TRIAL && SIP::winningCourse != lastCourse){
      lastCourse = SIP::winningCourse;
      return 1;
   }
   return 0;
}
void CtrlRaceTrackInfoDisplay::Create(Page *page, u32 index){
   CtrlRaceTrackInfoDisplay *info = new(CtrlRaceTrackInfoDisplay);
   page->AddControl(index, info, 0);
   info->Load();
}
static CustomCtrlBuilder INFODISPLAYPANEL(CtrlRaceTrackInfoDisplay::Count, CtrlRaceTrackInfoDisplay::Create);


void CtrlRaceTrackInfoDisplay::Load(){
   this->hudSlotId = 0;
   ControlLoader loader(this);
   loader.Load("game_image", "CTInfo", "Info", NULL);
   this->textBox_00 = this->layout.GetPaneByName("TextBox_00");
   this->SetMsgId(GetCurTrackBMG() + TRACKINFOBMGOFFSET, 0);
}
}//namespace SIPUI
