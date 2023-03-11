#include <kamek.hpp>
#include <game/System/Archive.hpp>
#include <SkillIssuePack.hpp>
//Adds SIPUI.szs and SIPCommon.szs to the relevant archive holders; SIP assets will be contained inside
kmWrite32(0x8052a108, 0x38800003); //Add one archive to CommonArchiveHolder
kmWrite32(0x8052a188, 0x38800003); //Add one archive to UIArchiveHolder
void LoadUIFile(ArchiveFile *file, const char *path, EGG::Heap *decompressedHeap, bool isCompressed, s32 allocDirection, 
                EGG::Heap *archiveHeap, EGG::Archive::FileInfo *info){
    if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_UI]->archives[2] == file) path = SIP::UIAssets;
    else if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_COMMON]->archives[2] == file) path = SIP::CommonAssets;
    file->Load(path, decompressedHeap, isCompressed, allocDirection, archiveHeap, info);
}
kmCall(0x8052aa2c, LoadUIFile);


/* for when I eventually add a Heap to speed up loading; this doesn't work as obviously the MEM2 heap holding archives gets destroyed
kmWrite32(0x8052a108, 0x38800003); //Add one archive to CommonArchiveHolder
kmWrite32(0x8052a188, 0x38800003); //Add one archive to UIArchiveHolder
void LoadSIPFile(ArchiveFile *file, const char *path, EGG::Heap *decompressedHeap, bool isCompressed, s32 allocDirection, 
                EGG::Heap *archiveHeap, EGG::Archive::FileInfo *info){
    if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_UI]->archives[2] == file){
        if(file->status == 6){
            file->status = 4;
            return;
        } 
        path = SIP::UIAssets;
    } 
    else if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_COMMON]->archives[2] == file){
        if(file->status == 6){
            file->status = 4;
            return;
        }
        path = SIP::CommonAssets;
    }
    file->Load(path, decompressedHeap, isCompressed, allocDirection, archiveHeap, info);
}
kmCall(0x8052aa2c, LoadSIPFile);

void PreventSIPArchiveUnmount(ArchiveFile *file){
    ArchiveRoot *root = ArchiveRoot::sInstance;
    if(file == &root->archivesHolders[ARCHIVE_HOLDER_UI]->archives[2] || file == &root->archivesHolders[ARCHIVE_HOLDER_COMMON]->archives[2]){
        if(file->status == 4){
            file->status = 6; //custom status so that "HasArchives" returns false, leading to game UI/common files getting loaded properly
            return;
        } 
    }
    file->Unmount();
}
kmCall(0x8052aab8, PreventSIPArchiveUnmount);
*/