#include <File/Folder.hpp>
#include <SkillIssuePack.hpp>

namespace SIP{
FolderHandler* FolderHandler::Create(){
    s32 riivo_fd = SIP::Open("file", IOS::MODE_NONE);
    EGG::Heap *heap = RKSystem::mInstance.EGGSystem;
    FolderHandler *folder;
    if(riivo_fd < 0){
        folder = new (heap) FolderHandler();
        folder->curFile = new (heap) FileHandler();
    } 
    else{ 
        IOS::Close(riivo_fd);
        folder = new (heap) RiivoFolderHandler();
        folder->curFile = new (heap) RiivoFileHandler();
    }     
    return folder;
}

s32 FolderHandler::ReadFolder(const char *path){
    char realPath[IPCMAXPATH];
    this->curFile->GetCorrectPath(realPath, path);

    u32 count = MAXFILECOUNT;
    char *tmpArray = new(RKSystem::mInstance.EGGSystem, 0x20) char[255 * (count +1)];
    void *originalPtr = tmpArray;
    s32 error = ISFS::ReadDir(realPath, tmpArray, &count);
    if(error >= 0){
        strncpy(this->folderName, path, IPCMAXPATH);
        IOS::IPCPath *namesArray = new(RKSystem::mInstance.EGGSystem, 0x20) IOS::IPCPath[count];
        u32 realCount = 0;
        char curFile[IPCMAXPATH];
        while(tmpArray[0] != '\0'){
            u32 length = strlen(tmpArray);
            if(length > 255) break;
            if(length <= IPCMAXFILENAME){
                snprintf(curFile, IPCMAXPATH, "%s/%s", realPath, tmpArray);
                s32 curFilefd = ISFS::Open(curFile, ISFS::MODE_NONE);
                if(curFilefd >= 0){
                    strcpy(namesArray[realCount], (char*) tmpArray);
                    realCount++;
                    ISFS::Close(curFilefd);
                }
            }
            tmpArray = tmpArray + length + 1; 
        }
        this->fileCount = realCount;
        this->fileNames = namesArray;
    }
    RKSystem::mInstance.EGGSystem->free(originalPtr);
    return error;
}

void FolderHandler::GetFilePath(char *path, u32 index) const{
    snprintf(path, IPCMAXPATH, "%s/%s", &this->folderName, &this->fileNames[index]);
}

s32 FolderHandler::ReadFile(void *bufferIn, u32 index, u32 mode){
    char path[IPCMAXPATH];
    this->GetFilePath(path, index);
    this->curFile->Open(path, mode);
    s32 ret = this->curFile->Read(bufferIn, this->curFile->GetFileSize()); //
    return ret;
}

s32 FolderHandler::CreateFolder(const char *path){
    char realPath[IPCMAXPATH];
    this->curFile->GetCorrectPath(realPath, path);
    return ISFS::CreateDir(realPath, 0, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE);
}

/*
void FolderHandler::DeleteFile(u32 index){
    char path[64];
    this->GetFilePath(path, index);
    this->curFile->Delete();
}
*/
void FolderHandler::CloseFolder(){
    this->curFile->Close();
    if(this->fileNames != NULL) delete[](this->fileNames);
    this->fileNames = NULL;
    this->folderName[0] = '\0';
    this->fileCount = 0;
}

FolderHandler::~FolderHandler(){
    this->CloseFolder();
    delete(this->curFile);
}

s32 RiivoFolderHandler::ReadFolder(const char *path){
    s32 riivo_fd = this->curFile->GetDevice_fd();
    strncpy(this->folderName, path, IPCMAXPATH);
    IOS::IOCtlvRequest request[3] __attribute((aligned (0x20)));
    s32 folder_fd __attribute((aligned (0x20))) = IOS::IOCtl(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_OPENDIR, (void*) path, strlen(path) + 1, NULL, 0);
    char fileName[RIIVOMAXPATH] __attribute((aligned (0x20)));
    RiivoStats stats __attribute((aligned (0x20)));
    IOS::IPCPath *tmpArray = new (RKSystem::mInstance.EGGSystem) IOS::IPCPath[MAXFILECOUNT];
    u32 count = 0;
    while(count < MAXFILECOUNT){
        request[0].address = &folder_fd;
        request[0].size = 4;
        request[1].address = &fileName;
        request[1].size = RIIVOMAXPATH;
        request[2].address = &stats;
        request[2].size = sizeof(RiivoStats);
        s32 retIOCtlv = IOS::IOCtlv(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_NEXTDIR, 1, 2, request);
        if(retIOCtlv != 0) break;
        if((stats.Mode & S_IFDIR) == S_IFDIR) continue; //if the next entry is a directory, skip

        //snprintf(tmpArray[count], IPCMAXPATH, "/mnt/identifier/%016llX", stats.Identifier);
        strncpy(tmpArray[count], fileName, IPCMAXPATH);
        count++;
    }

    IOS::IPCPath *namesArray = new (RKSystem::mInstance.EGGSystem) IOS::IPCPath[count]; //here
    memcpy(namesArray, tmpArray, sizeof(IOS::IPCPath) * count);
    this->fileCount = count;
    this->fileNames = namesArray;
    delete[](tmpArray);
    s32 result = IOS::IOCtl(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_CLOSEDIR, (void*) &folder_fd, sizeof(s32), NULL, 0);
    IOS::Close(riivo_fd);
    return result;
}

s32 RiivoFolderHandler::CreateFolder(const char *path){
    char realPath[IPCMAXPATH];
    //this->curFile->GetCorrectPath(realPath, path);
    s32 riivo_fd = this->curFile->GetDevice_fd();
    s32 error =  IOS::IOCtl(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_CREATEDIR, (void*) path, strlen(path) + 1, NULL, 0);
    IOS::Close(riivo_fd);
    return error;
}

/*void RiivoFolderHandler::GetFilePath(char *path, u32 index){
    strncpy(path, (const char*) &this->fileNames[index], IPCMAXPATH);
}*/

/*
s32 FolderLoader::Move(char *newFolder, u32 index){
    char path[64];
    this->GetFilePath(path, index);
    strcpy(this->curFile->path, path);
    snprintf(path, 64, "%s/%s", &this->folderName, newName);
    s32 error = this->curFile->ChangeName(path);
    if(error >= 0) strcpy(this->fileNames[index], newName);
    return error;
}
*/
}//namespace SIP