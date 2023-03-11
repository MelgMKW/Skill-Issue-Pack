#include <SkillIssuePack.hpp>
#include <core/rvl/ipc/ipc.hpp>

//Create Mod Folders


namespace SIP{

using IOS::Open2ndInst;
asm s32 Open(char *path, IOS::Mode mode){ //wiimmfi patch...{ 
    ASM(
        nofralloc;
        stwu sp, -0x0020 (sp);
        b Open2ndInst;
    )
}
u32 winningCourse = -1;
TTMode ttMode = SIPTTMODE_150;
const char *ttModeFolders[4] = {"150", "200", "150Fea", "200Fea"};
}//namespace SIP

void CreateFolders(){
    SIP::FolderHandler *folder = SIP::FolderHandler::Create();
    folder->CreateFolder(SIP::modFolder);
    folder->CreateFolder(SIP::ghostFolder);
    delete(folder);
}
BootHook Folders(CreateFolders, HIGH);

void SIP::ChangeImage(LayoutUIControl *control, const char *paneName, const char *tplName){
   void *tplRes = control->layout.resources->multiArcResourceAccessor.GetResource(res::RESOURCETYPE_TEXTURE, tplName);
   control->layout.GetPaneByName(paneName)->GetMaterial()->GetTexMapAry()->ReplaceImage((TPLPalettePtr) tplRes);
};

int SIP::GetCRC32(CourseId id){
    return SIP::CRC32Array[id];
}

//Unlock Everything Without Save (_tZ)
kmWrite32(0x80549974, 0x38600001);

//Skip ESRB page
kmWriteRegionInstruction(0x80604094, 0x4800001c, 'E');