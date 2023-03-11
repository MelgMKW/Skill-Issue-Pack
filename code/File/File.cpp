#include <File/File.hpp>
#include <SkillIssuePack.hpp>


namespace SIP{
FileHandler* FileHandler::sInstance = NULL;
void FileHandler::CreateStaticInstance(){
    EGG::Heap *heap = RKSystem::mInstance.EGGSystem;
    s32 ret = SIP::Open("file", IOS::MODE_NONE);
    FileHandler *file;
    bool valid = false; //For ISO users on console, we don't want anything to be saved on a real NAND
    if(ret < 0){
        ret = SIP::Open("/dev/dolphin", IOS::MODE_NONE);
        if(ret >= 0){
            valid = true;
            IOS::Close(ret);
        } 
        file = new (heap) FileHandler();


    } 
    else{ 
        valid = true;
        IOS::Close(ret);
        file = new (heap) RiivoFileHandler();
    } 
    FileHandler::sInstance = file;
    file->isValid = valid;
    FileHandler::sInstance->taskThread = EGG::TaskThread::Create(2, 30, 0x2000, NULL);
}
BootHook SetUpFileLoader(&FileHandler::CreateStaticInstance, FIRST);


s32 FileHandler::Open(const char *path, u32 mode){

    char realPath[IPCMAXPATH];
    this->GetCorrectPath(realPath, path);
    strcpy(this->path, realPath);
    this->fd = SIP::Open(realPath, (IOS::Mode) mode);
    this->fileSize = IOS::Seek(this->fd, 0, IOS::SEEK_END);
    IOS::Seek(this->fd, 0, IOS::SEEK_START);
    return this->fd;
}

s32 FileHandler::CreateAndOpen(const char *path, u32 mode){
    char realPath[IPCMAXPATH];
    this->GetCorrectPath(realPath, path);
    ISFS::CreateFile(realPath, 0, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE);
    return this->Open(path, mode);
}  

void FileHandler::GetCorrectPath(char *realPath, const char *path) const{
    snprintf(realPath, IPCMAXPATH, "%s%s", "/title/00010004", path);
}

s32 FileHandler::GetDevice_fd() const{
    return IOS::fs_fd;
}

/*
void FileHandler::Delete(){
    IOS::IOCtl(this->GetDevice_fd(), IOS::IOCTL_FS_DELETE, &this->path, IPCMAXPATH, 0, 0);
}*/

s32 FileHandler::Read(void *buffer, u32 length){
    if(this->fd < 0) return -1;
    //if(length > this->fileSize) length = this->fileSize;
    return IOS::Read(this->fd, buffer, length);
}

s32 FileHandler::Write(u32 length, void *buffer){
    if(this->fd < 0) return -1;
    return IOS::Write(this->fd, buffer, length);
}

s32 FileHandler::Overwrite(u32 length, void *buffer){
    if(this->fd < 0) return -1;
    IOS::Seek(this->fd, 0, IOS::SEEK_START);
    return IOS::Write(this->fd, buffer, length);
}

void FileHandler::Close(){
    if(this->fd >=0) IOS::Close(this->fd);
    this->fd = -1;
    this->fileSize = -1;
}

s32 RiivoFileHandler::Open(const char *path, u32 mode){
    return FileHandler::Open(path, this->GetRiivoMode(mode)); 
}

s32 RiivoFileHandler::CreateAndOpen(const char *path, u32 mode){
    s32 riivo_fd = this->GetDevice_fd();
    IOS::IOCtl(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_CREATEFILE, (void*) path, strlen(path) + 1, NULL, 0);
    IOS::Close(riivo_fd);
    return this->Open(path, mode);
}

s32 RiivoFileHandler::GetDevice_fd() const{
    return SIP::Open("file", IOS::MODE_NONE);
}

void RiivoFileHandler::GetCorrectPath(char *realPath, const char *path) const{

   snprintf(realPath, IPCMAXPATH, "%s%s", "file", path);
}

/*
void RiivoFileHandler::Delete(){
    s32 riivo_fd = SIP::Open("file", IOS::MODE_NONE);
    IOS::IOCtl(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_DELETE, &this->path, IPCMAXPATH, 0, 0);
    IOS::Close(riivo_fd);
}*/


RiivoMode RiivoFileHandler::GetRiivoMode(u32 mode) const{
    if((mode & 0x3) != 0) mode -= 1;
    return (RiivoMode) mode;
}
}//namespace SIP










