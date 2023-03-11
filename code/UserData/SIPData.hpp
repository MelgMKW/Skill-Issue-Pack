#ifndef _SIPDATA_
#define _SIPDATA_
#include <kamek.hpp>
#include <File/File.hpp>
#include <SkillIssuePack.hpp>

//Contains trophy count and all the settings. 
class SettingsHook {
private:
    typedef void (Func)();
    Func *mFunc;
    SettingsHook * mNext;

    static SettingsHook * sHooks;

public:
    SettingsHook(Func * f) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
    }

    static void exec() {
        for (SettingsHook * p = sHooks; p; p = p->mNext)
            p->mFunc();
    }
};

struct SettingsPage{
    u8 radioSetting[6];
    u8 scrollSetting[6];
};

class SettingsDummy{
    //empty just exists for future version proofing
};


class Settings : public SettingsDummy{ 
public:
    Settings(){};
    
    /*Settings(SettingsDummy *oldSettings) //in case a new file needs to get built
    {
       magic[0] = 'T';
       magic[1] = 'E';
       magic[2] = 'S';
       magic[3] = 'T';
       version = 1;
       settingsRadioCount = 3;
    }*/
public:
    char magic[4];
    u32 version; //just in case more than 255 versions
    SettingsPage pages[3];
};

struct TrackTrophy{
    u32 crc32;
    bool hastrophy[4];
};

struct SIPTrophies{
    SIPTrophies(){
        magic[0] = 'T';
        magic[1] = 'R';
        magic[2] = 'O';
        magic[3] = 'P';
    }
    char magic[4];
    u32 trophyCount[4];
    TrackTrophy trophies[TRACKCOUNT];
};


class SIPData : public Settings{
public:
    u32 reserved[20];
    u32 trackCount;
    SIPTrophies sipTrophies;
}__attribute((aligned (0x20)));

class SettingsHolder {
public:
    SettingsHolder();
    static void Create();
    static SettingsHolder *GetInstance(){return sInstance;}
    Settings *GetSettings() const {return this->settings;}
    void Update();
protected:
    static SettingsHolder *sInstance;
    virtual void Init(char *path, const char*curMagic, u32 version) = 0;
    virtual void RequestSave() = 0;
    
    char filePath[IPCMAXPATH];
    Settings *settings;
    //any global var can go here I guess
};

class SIPDataHolder : public SettingsHolder{
public:
    static SIPDataHolder *GetInstance(){return (SIPDataHolder*)sInstance;}
    SIPData *GetSettings() const {return (SIPData*) this->settings;} //hides parent
private:
    void Init(char *path, const char*curMagic, u32 curVersion) override;
    void RequestSave() override;
public:
    TrackTrophy *FindTrackTrophy(u32 crc32, SIP::TTMode mode);
    void UpdateTrackList(u32 *crc32Array);
    void AddTrophy(u32 crc32, SIP::TTMode mode);
    bool HasTrophy(u32 crc32, SIP::TTMode mode);
    bool HasTrophy(CourseId id, SIP::TTMode mode);
    int GetTrophyCount(SIP::TTMode mode){return this->GetSettings()->sipTrophies.trophyCount[mode];}
    static void SaveTask(void *);
    void Save();
};




enum SIPSettingsPages{
    SIP_MENU_SETTINGS,
    SIP_RACE_SETTINGS,
    SIP_HOST_SETTINGS
};

//MENU SETTINGS
enum SIPRadioMenuSettings{
   SIP_RADIO_FASTMENUS
};

enum SIPRadioMenuFastMenus {
    SIP_RADIO_FASTMENUS_DISABLED = 0x0,
    SIP_RADIO_FASTMENUS_ENABLED = 0x1
};

enum SIPScrollMenuSettings{
    SIP_SCROLL_BOOT
};

enum SIPScrollMenuBoot{
    SIP_SCROLL_BOOT_DISABLED,
    SIP_SCROLL_BOOT_L1,
    SIP_SCROLL_BOOT_L2,
    SIP_SCROLL_BOOT_L3,
    SIP_SCROLL_BOOT_L4
};

//RACE SETTINGS
enum SIPRadioRaceSettings{
   SIP_RADIO_MII,
   SIP_RADIO_SPEEDUP,
   SIP_RADIO_BATTLE,
   SIP_RADIO_BLUES
};

enum SIPRadioRaceMII {
    SIP_RADIO_MII_DISABLED = 0x0,
    SIP_RADIO_MII_ENABLED = 0x1
};

enum SIPRadioRaceSPEEDUP {
    SIP_RADIO_SPEEDUP_DISABLED = 0x0,
    SIP_RADIO_SPEEDUP_ENABLED = 0x1
};

enum SIPRadioRaceBATTLE {
    SIP_RADIO_BATTLE_GLITCH_DISABLED = 0x0,
    SIP_RADIO_BATTLE_GLITCH_ENABLED = 0x1
};

enum SIPRadioRaceBLUES {
    SIP_RADIO_DRAGGABLE_BLUES_DISABLED = 0x0,
    SIP_RADIO_DRAGGABLE_BLUES_ENABLED = 0x1
};

//HOST SETTINGS
enum SIPRadioHostSettings{
   SIP_RADIO_HOSTWINS,
   SIP_RADIO_CC
};

enum SIPRadioHostHostWins {
    SIP_RADIO_HOSTWINS_DISABLED,
    SIP_RADIO_HOSTWINS_ENABLED
};

enum SIPRadioHostCC{
    SIP_RADIO_CC_NORMAL,
    SIP_RADIO_CC_150,
    SIP_RADIO_CC_200    
};

enum SIPScrollHostSettings{
    SIP_SCROLL_GP_RACES
};

enum SIPScrollHostGPRACES{
    SIP_SCROLL_GP_RACES_4,
    SIP_SCROLL_GP_RACES_8,
    SIP_SCROLL_GP_RACES_12,
    SIP_SCROLL_GP_RACES_24,
    SIP_SCROLL_GP_RACES_32,
    SIP_SCROLL_GP_RACES_64,
    SIP_SCROLL_GP_RACES_2
};

#endif