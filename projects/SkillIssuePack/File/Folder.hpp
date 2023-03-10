#ifndef _FOLDERLOADER_
#define _FOLDERLOADER_
#include <kamek.hpp>
#include <File/File.hpp>

namespace SIP{
class FolderHandler{ //Wraps around FileHandler and is also able to Create/Read folders
public:
    FolderHandler() : fileCount(0), fileNames(NULL) {}
    ~FolderHandler();
    static FolderHandler *Create();
    virtual s32 CreateFolder(const char *path);
    virtual void GetFilePath(char *dest, u32 index) const;
    virtual s32 ReadFolder(const char *path);
    s32 ReadFile(void *buffer, u32 index, u32 mode/*, u32 maxLength = 0xFFFFFFFF*/);
    //void CreateFile();
    //void DeleteFile(u32 index);
    void CloseFile(){this->curFile->Close();}
    const char *GetName() const {return this->folderName;};
    const int GetFileCount() const {return this->fileCount;}
    //s32 Move(char *newName, u32 index);
    void CloseFolder();
protected:
    char folderName[64];
    u32 fileCount;
    IOS::IPCPath *fileNames;
    FileHandler *curFile;

};

class RiivoFolderHandler : public FolderHandler{
public:
    RiivoFolderHandler(){}
    s32 CreateFolder(const char *path) override;
    //virtual void GetFilePath(char *path, u32 index);
    s32 ReadFolder(const char *path) override;
};
}//namespace SIP
#endif