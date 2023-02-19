#include <kamek.hpp>

//Region Patch (Leseratte)
kmWrite32(0x80384fd0, 0x31303033); //login
kmWrite16(0x80384fd4, 0x3938); 
kmWrite32(0x80653644, 0x38A0018E); //search
kmWrite32(0x806536B0, 0x38C0018E);
kmWrite32(0x8065920C, 0x38E0018E);
kmWrite32(0x80659260, 0x38E0018E);
kmWrite32(0x80659724, 0x38E0018E);
kmWrite32(0x80659778, 0x38E0018E);
kmWrite32(0x8065A034, 0x3880008E);
kmWrite32(0x8065A080, 0x3880008E);