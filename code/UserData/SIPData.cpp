#include <UserData/SIPData.hpp>

//SETTINGS HOLDER, IMPLEMENT INIT AND SAVE BASED ON YOUR SPECIFIC SETTINGS
SettingsHolder *SettingsHolder::sInstance = NULL;
SettingsHook *SettingsHook::sHooks = NULL;

SettingsHolder::SettingsHolder(){ 
    settings = NULL;
}

void SettingsHolder::Update(){
    SettingsHook::exec();
    this->RequestSave();
}



void SIPDataHolder::UpdateTrackList(u32 *crc32Array){
    SIPData *data = this->GetSettings();

    u32 oldTrackCount = data->trackCount;
    u16 missingCRCIndex[TRACKCOUNT];
    u16 *toberemovedCRCIndex = new (RKSystem::mInstance.EGGSystem) u16[oldTrackCount];
    memset(missingCRCIndex, 0xFFFF, sizeof(u16) * TRACKCOUNT);
    memset(toberemovedCRCIndex, 0xFFFF, sizeof(u16) * oldTrackCount);

    TrackTrophy *trophies = data->sipTrophies.trophies;
    for(int i = 0; i<TRACKCOUNT; i++)
        for(int j = 0; j<oldTrackCount; j++)
            if(crc32Array[i] == trophies[j].crc32){
                missingCRCIndex[i] = j;
                break;
            } 
    
    for(int j = 0; j<oldTrackCount; j++)
        for(int i = 0; i<TRACKCOUNT; i++)
            if(trophies[j].crc32 == crc32Array[i]){
                toberemovedCRCIndex[j] = i;
                break;
            } 
    
    for(int i = 0; i<TRACKCOUNT; i++){
        if(missingCRCIndex[i] == 0xFFFF){
            for(int j = 0; j<oldTrackCount; j++){
                if(toberemovedCRCIndex[j] == 0xFFFF){
                    toberemovedCRCIndex[j] = 0;
                    trophies[j].crc32 = crc32Array[i];
                    for(int mode = 0; mode < 4; mode++) trophies[j].hastrophy[mode] = false;
                    break;
                }
            }
        }
    }
    if(data->trackCount < TRACKCOUNT){
        for(int i = 0; i<TRACKCOUNT; i++){
            if(missingCRCIndex[i] == 0xFFFF){
                trophies[oldTrackCount].crc32 = crc32Array[i];
                for(int mode = 0; mode < 4; mode++) trophies[oldTrackCount].hastrophy[mode] = false;
                oldTrackCount++;
            }
        }
        data->trackCount = TRACKCOUNT;
    }
    delete[](toberemovedCRCIndex);

}

TrackTrophy *SIPDataHolder::FindTrackTrophy(u32 crc32, SIP::TTMode mode){
    SIPData *data = (SIPData*) this->GetSettings();
    for(int i = 0; i<data->trackCount; i++) if(data->sipTrophies.trophies[i].crc32 == crc32) return &data->sipTrophies.trophies[i];
    return NULL;
}

void SIPDataHolder::AddTrophy(u32 crc32, SIP::TTMode mode){
    TrackTrophy *trophy = this->FindTrackTrophy(crc32, mode);
    if(trophy != NULL && !trophy->hastrophy[mode]){
        this->GetSettings()->sipTrophies.trophyCount[mode]++;
        trophy->hastrophy[mode] = true;
    }
}

bool SIPDataHolder::HasTrophy(u32 crc32, SIP::TTMode mode){
    TrackTrophy *trophy = this->FindTrackTrophy(crc32, mode);
    if(trophy != NULL && trophy->hastrophy[mode]) return true;
    return false;
}

bool SIPDataHolder::HasTrophy(CourseId id, SIP::TTMode mode){
    return this->HasTrophy(SIP::GetCRC32(id), mode);
}

void SIPDataHolder::Init(char *path, const char*curMagic, u32 curVersion){
    strncpy(this->filePath, path, IPCMAXPATH);
    SIPData * buffer = new (RKSystem::mInstance.EGGSystem, 0x20) SIPData;
    
    SIP::FileHandler *loader = SIP::FileHandler::sInstance;
    loader->CreateAndOpen(this->filePath, SIP::FILE_MODE_READ_WRITE);
    loader->Read(buffer, sizeof(SIPData));
    if(strcmp(buffer->magic, curMagic) != 0 || buffer->version != curVersion){
        memset(buffer, 0, sizeof(SIPData));
        strncpy(buffer->magic, curMagic, 4);
        strncpy(buffer->sipTrophies.magic, "TROP", 4);
        buffer->version = curVersion;
    }
    this->settings = buffer;
    this->UpdateTrackList((u32*)SIP::CRC32Array);
    loader->Overwrite(sizeof(SIPData), buffer);
    
    loader->Close();
}

void SIPDataHolder::RequestSave(){
    SIP::FileHandler::sInstance->taskThread->Request(&SIPDataHolder::SaveTask, NULL, 0);
}

void SIPDataHolder::SaveTask(void*){
    SIPDataHolder::GetInstance()->Save();
}

void SIPDataHolder::Save(){
    SIP::FileHandler *loader = SIP::FileHandler::sInstance;
    loader->Open(this->filePath, SIP::FILE_MODE_WRITE);
    loader->Overwrite(sizeof(SIPData), this->settings);
    loader->Close();
};


void SettingsHolder::Create(){
    SettingsHolder *settings = new(RKSystem::mInstance.EGGSystem) SIPDataHolder();
    char path[IPCMAXPATH];
    snprintf(path, IPCMAXPATH, "%s/%s", SIP::modFolder, "SIPData.bin");
    settings->Init(path, "SIPD", 1);
    SettingsHolder::sInstance = settings;
}
BootHook TestSettings(SettingsHolder::Create, MEDIUM);