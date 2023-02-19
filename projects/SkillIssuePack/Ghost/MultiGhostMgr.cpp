#include <Ghost/MultiGhostMgr.hpp>
#include <game/UI/Page/Other/TTSplits.hpp>
#include <UI/BMG.hpp>

MultiGhostMgr* MultiGhostMgr::sInstance = NULL;
char MultiGhostMgr::folderPath[IPCMAXPATH] = "";

MultiGhostMgr::MultiGhostMgr() : courseId((CourseId)-1) {
    this->folder = SIP::FolderHandler::Create();
    this->files = NULL;
}

MultiGhostMgr::~MultiGhostMgr(){
    delete(this->folder);
}

MultiGhostMgr *MultiGhostMgr::CreateInstance(){
    MultiGhostMgr *holder = MultiGhostMgr::sInstance;
    if(holder == NULL){
        holder = new(RKSystem::mInstance.EGGSystem) MultiGhostMgr;
        MultiGhostMgr::sInstance = holder;
    } 
    holder->Reset();
    return holder;
}

void MultiGhostMgr::DestroyInstance(){
    if(MultiGhostMgr::sInstance != NULL) delete(MultiGhostMgr::sInstance);
    MultiGhostMgr::sInstance = NULL;
}

/*
Cornerstone function; Creates the folders if they have been deleted somehow, 
reads them, fetches the leaderboard, creates GhostDatas based on the rkgs, sets the expert time
*/
void MultiGhostMgr::Init(CourseId id){
    this->Reset();
    this->courseId = id;
    SIP::FolderHandler *folderLoader = this->folder;
    
    u32 crc32 = SIP::GetCRC32(id);
    snprintf(folderPath, IPCMAXPATH, "%s/%08x", SIP::ghostFolder, crc32);
    folderLoader->CreateFolder(folderPath); //Create "Ghosts" folder
    char folderModePath[IPCMAXPATH];
    snprintf(folderModePath, IPCMAXPATH, "%s/%s", folderPath, SIP::ttModeFolders[SIP::ttMode]); //Creates Mode folder
    folderLoader->CreateFolder(folderModePath);

    folderLoader->ReadFolder(folderModePath); //Reads all files contained in the folder

    
    new (this->GetLeaderboard()) SIPLeaderboard(folderPath, crc32);

    this->files = new (RKSystem::mInstance.EGGSystem) GhostData[folderLoader->GetFileCount()];
    RKG *rkg = &this->rkg;
    u32 counter = 0;
    for(int i = 0; i<folderLoader->GetFileCount(); i++){
        rkg->ClearBuffer();
        GhostData *curHeader = &this->files[counter];
        curHeader->isValid = false;
        if(folderLoader->ReadFile(rkg, i, SIP::FILE_MODE_READ) > 0 && rkg->CheckValidity()){
            curHeader->Init(rkg);
            curHeader->courseId = id;
            if(curHeader->type == EXPERT_STAFF_GHOST){ //very easy to fake/manipulate, but SIP has 0 security so it doesn't matter
                this->expertGhost.minutes = rkg->header.minutes;
                this->expertGhost.seconds = rkg->header.seconds;
                this->expertGhost.milliseconds = rkg->header.milliseconds;
                this->expertGhost.isActive = true;
            }
            curHeader->padding = i;
            u32 crc32;
            if(rkg->header.compressed) crc32 = *(u32*)((u32)rkg + ((CompressedRKG*) rkg)->dataLength + 0x8C);
            else crc32 = rkg->uncompressedCRC32;
            counter++;
        } 
        folderLoader->CloseFile();   
    }
    
    this->rkgCount = counter;
}

void MultiGhostMgr::Reset(){
    this->courseId = (CourseId) -1;
    this->lastUsedSlot = 0;
    mainGhostIndex = 0xFF;
    for(int i = 0; i < 3; i++) selGhostsIndex[i] = 0xFF;
    new(&this->expertGhost) Timer;
    this->folder->CloseFolder();
    delete[] this->files;
    this->files = NULL;
    RaceData *racedata = RaceData::sInstance;
    racedata->menusScenario.players[1].playerType = PLAYER_NONE;
    racedata->menusScenario.players[2].playerType = PLAYER_NONE;
    racedata->menusScenario.players[3].playerType = PLAYER_NONE;
}

//Enables ghost for loading when GhostSelect's ToggleButton is pressed to true and when Challenge/Watch is pressed
bool MultiGhostMgr::EnableGhost(GhostListEntry *entry, bool isMain){
    bool exists = false;
    u32 *index = &mainGhostIndex;
    if(!isMain) index = &selGhostsIndex[this->lastUsedSlot];
    for(int i = 0; i<this->rkgCount; i++){
        if(&this->files[i] == entry->data){
            *index = i;    
            exists = true;
            break;
        }
    }
    if(exists && !isMain) this->lastUsedSlot = (this->lastUsedSlot + 1) %3;
    return exists;
}

//Used when GhostSelect' ToggleButton is pressed to false
void MultiGhostMgr::DisableGhost(GhostListEntry *entry){
    u32 index = 0xFF;
    for(int i = 0; i<this->rkgCount; i++){
        if(&this->files[i] == entry->data){
            index = i;
            break;
        }
    }
    for(int i = 0; i < 3; i++){
        if(this->selGhostsIndex[i] == index){
            this->lastUsedSlot = i;
            this->selGhostsIndex[i] = 0xFF;
        }
    }
}

//Loads and checks validity of a RKG
bool MultiGhostMgr::LoadGhost(RKG *rkg, u32 index){
    rkg->ClearBuffer();
    this->folder->ReadFile(rkg, index, SIP::FILE_MODE_READ);
    return rkg->CheckValidity();
}

//Copies ghost from src to racedata's RKG buffers and adds mii if ghost race
void MultiGhostMgr::LoadAllGhost(RKG *buffer, u32 maxGhosts, bool isGhostRace){
        u8 position = 1;
        for(int i = 0; i < maxGhosts; i++){
            if(this->selGhostsIndex[i] != 0xFF){
                if(this->LoadGhost(buffer, this->GetGhostData(this->selGhostsIndex[i])->padding)){
                    RaceData *racedata = RaceData::sInstance;
                    RKG *dest = &racedata->ghosts[position];
                    if(buffer->header.compressed) buffer->DecompressTo(dest); //0x2800
                    else memcpy(dest, buffer, sizeof(RKG));
                    racedata->menusScenario.players[position + isGhostRace].playerType = PLAYER_GHOST;
                    GhostData data(dest);
                    MenuData::sInstance->menudata98->playerMiis.AddMii(position + isGhostRace, &data.miiData);
                    position++;
                }
            } 
    }
}

//When a request has been sent, creates and writes to a RKG, updates SIPData and updates the leaderboard
void MultiGhostMgr::CreateAndSaveFiles(void *managerPtr){
        char path[IPCMAXPATH];
        MultiGhostMgr *manager = (MultiGhostMgr*) managerPtr;
        RKG *rkg = &manager->rkg;
        snprintf(path, IPCMAXPATH, "%s/%01dm%02ds%03d.rkg", manager->folder->GetName(), rkg->header.minutes, rkg->header.seconds, rkg->header.milliseconds);
        SIP::FileHandler *loader = SIP::FileHandler::sInstance;
        loader->CreateAndOpen(path, SIP::FILE_MODE_WRITE);
        u32 length = sizeof(RKG);
        if(rkg->header.compressed) length = ((CompressedRKG*)rkg)->dataLength + sizeof(RKGHeader) + 0x4 + 0x4; //size of header + yaz1 header + crc32
        loader->Overwrite(length, rkg);
        loader->Close();

        char folderPath[IPCMAXPATH];
        snprintf(folderPath, IPCMAXPATH, "%s/%08x", SIP::ghostFolder, SIP::GetCRC32(manager->courseId));
        manager->GetLeaderboard()->Save(folderPath);
        SIPDataHolder::GetInstance()->Save();
        manager->Init(SIP::GetCourseId());
        MenuData::sInstance->menudata98->isNewTime = true;
}

//Inits MultiGhostMgr and uses it to fill the GhostList
void InsertCustomGroupToList(GhostList *list, CourseId id){ //check id here
    MultiGhostMgr *manager = MultiGhostMgr::GetInstance();
    manager->Init(SIP::GetCourseId());
    u32 index = 0;
    for(int i = 0; i < manager->folder->GetFileCount(); i++){
        if(index == 38) break;
        const GhostData *data = manager->GetGhostData(i);
        if(data->isValid){
            list->entries[index].data = data;
            index++;
        }
    }
    list->count = index;
    qsort(list, list->count, sizeof(GhostListEntry), (int (*)(const void *, const void *))*&GhostList::CompareEntries);
};
kmCall(0x806394f0, InsertCustomGroupToList);

//80856fec Race get ldr position for animation almost certainly
//80855c6c save ghost (get ldr position) r4 timer r5 savedatamanagerlicence
//8051ca80 write ghost file
s32 PlayCorrectFinishAnim(LicenseManager *license, Timer *timer, CourseId courseId){
    return MultiGhostMgr::GetInstance()->GetLeaderboard()->GetPosition(timer);
}
kmCall(0x80856fec, PlayCorrectFinishAnim);

//Complete rewrite TTSplits BeforeEntranceAnimations; this will request a RKG if needed (flap or top 10 time)
void BeforeEntranceAnimations(Pages::TTSplits *page){
    //Init Variables
    MenuData *menu = MenuData::sInstance;
    MenuData98 *m98 = menu->menudata98;
    m98->isNewTime = false;
    m98->fastestLapId = -1;
    m98->unknown_0x3D8 = false;
    TimeEntry entry;
    entry.character = RaceData::sInstance->racesScenario.players[0].characterId;
    entry.kart = RaceData::sInstance->racesScenario.players[0].kartId;
    entry.controllerType = menu->pad.GetType(MenuData::sInstance->pad.GetCurrentSlotAndType(0));
    Mii *mii = m98->playerMiis.GetMii(0);
    mii->ComputeRawMii(&entry.mii, &mii->texMap);
    
    //Find which lap is the best
    page->timers[0] = *RaceInfo::sInstance->players[0]->raceFinishTime;
    page->ctrlRaceTimeArray[0]->SetTimer(&page->timers[0]);
    page->ctrlRaceTimeArray[0]->OnFocus();
    Timer *bestLap = &page->timers[0];
    u32 bestLapId = 1;
    for(int i = 1; i < page->splitsRowCount; i++){
        RaceInfo::sInstance->players[0]->FillTimerWithSplits(i, &page->timers[i]);
        if((*bestLap)>page->timers[i]){
            bestLap = &page->timers[i];
            bestLapId = i;
        }     
        CtrlRaceTime *curRaceTime = page->ctrlRaceTimeArray[i];
        curRaceTime->SetTimer(&page->timers[i]);
        curRaceTime->OnFocus();
    }
    //enhanced replay
    if(menu->curScene->menuId >= WATCH_GHOST_FROM_CHANNEL && menu->curScene->menuId <= WATCH_GHOST_FROM_MENU){
        page->ctrlRaceTimeArray[0]->EnableFlashingAnimation();
        page->ctrlRaceTimeArray[bestLapId]->EnableFlashingAnimation();
        page->ctrlRaceCount.isHidden = false;
        page->ctrlRaceCount.Animate();
        page->PlaySound(0xDD, -1);
        page->savedGhostMessage.SetMsgId(SIPUI::BMG_SAVED_GHOST, NULL);
    }

    //Finish Time Leaderboard check and request
    else{
        //Compare against leaderboard and save
        MultiGhostMgr *manager = MultiGhostMgr::GetInstance();
        bool request = false;
        manager->GetLeaderboard()->EntryToTimer(manager->entry.timer, ENTRY_FLAP);
        if(manager->entry.timer>(*bestLap)){
            entry.timer = *bestLap;
            request = true;
            manager->GetLeaderboard()->Update(ENTRY_FLAP, &entry, -1);
            m98->fastestLapId = bestLapId;
            page->ctrlRaceTimeArray[bestLapId]->EnableFlashingAnimation();
        }
        entry.timer = page->timers[0];
        s32 position = manager->GetLeaderboard()->GetPosition(&page->timers[0]);
        m98->leaderboardPosition = position;
        if(position >= 0){
            request = true;
            if(position == 0){
                page->ctrlRaceCount.isHidden = false;
                page->ctrlRaceCount.Animate();
                m98->unknown_0x3D8 = true;
                if(InputData::sInstance->realControllerHolders[0].ghostWriter->state != 3 && page->timers[0].minutes < 6) m98->isNewTime = true;
                page->ctrlRaceTimeArray[0]->EnableFlashingAnimation();
                page->PlaySound(0xDD, -1);
                page->savedGhostMessage.SetMsgId(SIPUI::BMG_SAVED_GHOST, NULL);
            }
        }
        else page->ctrlRaceCount.isHidden = true;

        if(request){
            GhostData data;
            data.Fill(0);
            RKG buffer;
            buffer.ClearBuffer();
            RKG *rkg = &manager->rkg;
            if(data.CreateRKG(&buffer) && buffer.CompressTo(rkg)){
                u32 crc32;
                if(rkg->header.compressed) crc32 = *(u32*)((u32)rkg + ((CompressedRKG*) rkg)->dataLength + 0x8C);
                else crc32 = rkg->uncompressedCRC32;
                if(position >= 0) manager->GetLeaderboard()->Update(position, &entry, crc32); //in this order as save opens files too
                const Timer &expert = manager->GetExpert();
                if(expert.isActive && expert>entry.timer){
                    SIPDataHolder::GetInstance()->AddTrophy(SIP::GetCRC32(manager->GetCourseId()), SIP::ttMode);
                    manager->leaderboard.AddTrophy();
                    page->savedGhostMessage.SetMsgId(SIPUI::BMG_TROPHY_EARNED);
                }
                SIP::FileHandler::sInstance->taskThread->Request(&MultiGhostMgr::CreateAndSaveFiles, manager, NULL); 
            }
            //delete(buffer);
        }
    }
}
kmWritePointer(0x808DA614, BeforeEntranceAnimations);

//make racedata bigger to have 2 more rkgs in main
int IncreaseRacedataSize(){
    return 0xC3F0;
}
kmCall(0x8052fe78, IncreaseRacedataSize); 
kmWrite32(0x80531f44, 0x4800001c); //make it so the game will only use the first rkg buffer for normal ghost usage

//Patch needed since we now have 4 rkgs which are used in order
void RacedataCheckCorrectRKG(u8 id){
    u8 offset = 0;
    if (RaceData::sInstance->menusScenario.players[0].playerType != PLAYER_GHOST) offset = 1;
    RaceData::sInstance->ghosts[id - offset].CheckValidity();
}
kmWrite32(0x8052f5c0, 0x5763063E);
kmCall(0x8052f5c8, RacedataCheckCorrectRKG);

//Same as above
void GhostHeaderGetCorrectRKG(GhostData *header, u8 id){
    u8 offset = 0;
    RaceData *racedata = RaceData::sInstance;
    RacedataScenario *scenario = &racedata->menusScenario;
    if (scenario->players[0].playerType != PLAYER_GHOST) offset = 1;
    scenario->players[id].localPlayerNum = id;
    scenario->settings.hudPlayerIds[id] = id;
    header->Init(&racedata->ghosts[id - offset]);
    header->courseId = scenario->settings.courseId;
}   
kmWrite32(0x8052f5dc, 0x5764063E);
kmCall(0x8052f5e4, GhostHeaderGetCorrectRKG);

//Function name says it all
void LoadCorrectMainGhost(Pages::GhostManager *ghostManager, u8 r4){
    MultiGhostMgr *manager = MultiGhostMgr::GetInstance();
    manager->LoadGhost(ghostManager->rkgPointer, manager->GetGhostData(manager->mainGhostIndex)->padding);
    if(ghostManager->state == SAVED_GHOST_RACE_FROM_MENU) ghostManager->state = STAFF_GHOST_RACE_FROM_MENU;
    //faking that it's a staff so it copies from the buffer and not savadatemanager
}
kmCall(0x805e158c, LoadCorrectMainGhost);

//Loads all the ghosts while setting up the ghost race/replay
void ExtendSetupGhostRace(Pages::GhostManager *ghostManager, bool isStaffGhost, bool replaceGhostMiiByPlayer, bool disablePlayerMii){
    ghostManager->SetupGhostRace(true, replaceGhostMiiByPlayer, disablePlayerMii);
    MultiGhostMgr::GetInstance()->LoadAllGhost(ghostManager->rkgPointer, 2, true);
   
}
kmCall(0x805e13ac, ExtendSetupGhostRace);
kmCall(0x805e13e4, ExtendSetupGhostRace);
kmCall(0x805e141c, ExtendSetupGhostRace);
kmCall(0x805e149c, ExtendSetupGhostRace);
kmCall(0x805e14c8, ExtendSetupGhostRace);
kmCall(0x805e14f4, ExtendSetupGhostRace);

void ExtendSetupGhostReplay(Pages::GhostManager *ghostManager, bool isStaffGhosts){
    ghostManager->SetupGhostReplay(true);
    MultiGhostMgr::GetInstance()->LoadAllGhost(ghostManager->rkgPointer, 3, false);
}
kmCall(0x805e144c, ExtendSetupGhostReplay);
kmCall(0x805e1518, ExtendSetupGhostReplay);






