#include <kamek.hpp>
#include <Debug/SIPDebug.hpp>

namespace SIPDebug{
//const OSContext *crashContext = NULL;
//u16 crashError = 0;


void LaunchSoftware(){ //If dolphin, restarts game, else launches Riivo->HBC->OHBC->WiiMenu
    s32 result = SIP::Open("/dev/dolphin", IOS::MODE_NONE);
    if(result >= 0){
        IOS::Close(result);
        SystemManager::RestartGame();
        return;
    }
    result = SIP::Open("/title/00010001/4c554c5a/content/title.tmd\0", IOS::MODE_NONE);
    if (result >= 0){
        ISFS::Close(result);
        OSLaunchTitle(0x00010001, 0x4c554c5a);
        return;
    }
    result = SIP::Open("/title/00010001/48424330/content/title.tmd\0", IOS::MODE_NONE); // If HBC can't be found try OHBC
    if (result >= 0){
        ISFS::Close(result);
        OSLaunchTitle(0x00010001, 0x48424330);
        return;
    }
    OSLaunchTitle(0x1, 0x2); // Launch Wii Menu if channel isn't found
}

//Credit Star and Riidefi
//Data Shown
kmWrite16(0x802A7410, 0x00000023);
//Show Handler
kmWrite32(0x802A7404, 0x00000000);
//Show StackTrace
kmWrite32(0x80023948, 0x281e0007);
//Max number of lines
kmWrite32(0x80009324, 0x38800068);
//Lines on the screen and x-pos
void SetConsoleParams(){
    nw4r::db::detail::ConsoleHead *console = EGG::Exception::console;
    console->viewLines = 0x16;
    console->viewPosX = 0x10;
}
BootHook ConsoleParams(SetConsoleParams, LOW);

/*
SIPExceptionFile::SIPExceptionFile(const OSContext *context){
    magic = 0x53495044;
    this->srr0.name = 0x73727230;
    this->srr0.gpr = context->srr0;
    this->srr1.name = 0x73727231;
    this->srr1.gpr = context->srr1;
    this->msr.name = 0x6D73723A;
    this->msr.gpr = PPCMfmsr();
    this->cr.name = 0x63723A20;
    this->cr.gpr = context->cr;
    this->lr.name = 0x6C723A20;
    this->lr.gpr = context->lr;
    for(int i = 0; i<32; i++){
        this->gprs[i].Set(context, i);
        this->fprs[i].Set(context, i);
    }
    this->fpscr.name = 0x66736372;
    this->fpscr.fpr = context->fpscr;
    u32 *sp = (u32*) context->gpr[1];
    for(int i = 0; i<10; i++){
        if(sp == NULL || (u32) sp == 0xFFFFFFFF) break;
        this->frames[i].sp = (u32) sp;
        this->frames[i].lr = sp[1];
        sp = (u32*) *sp;
    }
}

 Can't get the file to be created on all threads therefore shelving this for now
void DumpContextToFile(u16 error, const OSContext *context, u32 dsisr, u32 dar){
    crashError = error;
    crashContext = context;
    nw4r::db::PrintContext_(error, context, dsisr, dar);
}
kmCall(0x80023484, DumpContextToFile);


void CreateCrashFile(){
    FileHandler *fileHandler = FileHandler::sInstance;
    if(fileHandler != NULL){ //should always exist if the crash is after strap 
        SIPExceptionFile *file = new(RKSystem::sInstance.EGGSystem, 0x20) SIPExceptionFile(crashContext);
        file->error = crashError;
        char path[IPCMAXPATH];
        snprintf(path, IPCMAXPATH, "%s/SIPCrash.bin", SIP::modFolder);
        fileHandler->CreateAndOpen(path, IOS::MODE_READ_WRITE);
        fileHandler->Overwrite(sizeof(SIPExceptionFile), file);
        fileHandler->Close();
        delete(file);
    }
    LaunchSoftware();
}
*/

kmBranch(0x802265f0, LaunchSoftware);
}//namespace SIPDebug



