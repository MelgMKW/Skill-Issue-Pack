#include <game/Item/ItemManager.hpp>
#include <game/Item/Obj/Gesso.hpp>
#include <game/Visual/Model/CourseModel.hpp>
#include <game/Race/globalraceparams.hpp>
#include <Network/network.hpp>

//Credit CLF78 and Stebler, this is mostly a port of their version with slightly different hooks and proper arguments naming since this is C++
void UseFeather(ItemPlayer *itemPlayer){
    KartPointers *pointers = itemPlayer->kartPointers;
    pointers->kartMovement->specialFloor |= 0x4; //JumpPad

    KartStatus *status = pointers->kartStatus; //Hijacking bitfield1 14th bit to create a feather state
    u32 type = 0x7;
    if ((status->bitfield1 & 0x4000) != 0) type = 0x2; //if already in a feather, lower vertical velocity (30.0f instead of 50.0 for type 7)
    status->jumpPadType = type;
    status->trickableTimer = 0x4;

    itemPlayer->inventory.RemoveItems(1);

    if(GlobalRaceParams::isOnlineRace && itemPlayer->isRemote) AddEVENTEntry(0xA, itemPlayer->id);
};
kmBranch(0x807a81b4, UseFeather); //replaces UseBlooper



void ReplaceBlooperUseOtherPlayers(GessoMgr *gessoMgr, u8 id){
    UseFeather(&ItemManager::sInstance->players[id]);
}
kmCall(0x80796d8c, ReplaceBlooperUseOtherPlayers); //replaces the small blooper model when someone uses a blooper with a feather use

kmWrite32(0x808b5c24, 0x42AA0000); //increases min, max speed of jump pad type 0x7 as well as its vertical velocity
kmWrite32(0x808b5c28, 0x42AA0000);
kmWrite32(0x808b5c2C, 0x42960000);


kmWrite32(0x805b68d8, 0x7DE97B78); //mr r9, r15 to get playercollision
bool ConditionalIgnoreInvisibleWalls(float radius, CourseModel *model, Vec3 *position, Vec3 *lastPosition, KCLTypesBIT acceptedFlags, UnkType *normalsInfo, KCLTypeHolder &kclFlags, KartCollision *collision){
    KartStatus *status = collision->base.pointers->kartStatus;
    if(status->bitfield0 & 0x40000000 && status->jumpPadType == 0x7) acceptedFlags = (KCLTypesBIT) (acceptedFlags & ~(1<<KCL_INVISIBLE_WALL));
     //to remove invisible walls from the list of flags checked, these walls at flag 0xD and 2^0xD = 0x2000
    return model->ProcessCollision(radius, position, lastPosition, acceptedFlags, normalsInfo, kclFlags, 0);
}
kmCall(0x805b68dc, ConditionalIgnoreInvisibleWalls);

u8 ConditionalFastFallingBody(KartSub *sub){
    KartPhysicsHolder *physics = sub->base.GetKartPhysicsHolder();
    KartStatus *status = sub->base.pointers->kartStatus;
    if(status->bitfield0 & 0x40000000 && status->jumpPadType == 0x7 && status->airtime >= 2 && (!status->bool_0x97||status->airtime > 19)){
        ControllerHolder *controllerHolder = sub->base.GetControllerHolder();
        float input = controllerHolder->inputStates[0].stickY <= 0.0f ? 0.0f : (controllerHolder->inputStates[0].stickY + controllerHolder->inputStates[0].stickY);
        physics->kartPhysics->gravity -= input * 0.39f;
    }
    
    return sub->base.GetPlayerIdx();
}
kmCall(0x805967ac, ConditionalFastFallingBody);


void ConditionalFastFallingWheels(float unk_float, WheelPhysicsHolder *wheelPhysicsHolder, Vec3 *gravityVector, Mtx34 *wheelMat){
    KartStatus *status = wheelPhysicsHolder->base.pointers->kartStatus;
    if(status->bitfield0 & 0x40000000 && status->jumpPadType == 0x7){
        if(status->airtime == 0) status->bool_0x97 = ((status->bitfield0 & 0x80) !=0) ? true : false;
        else if(status->airtime >= 2 && (!status->bool_0x97||status->airtime > 19)){
            ControllerHolder *controllerHolder = wheelPhysicsHolder->base.GetControllerHolder();
            float input = controllerHolder->inputStates[0].stickY <= 0.0f ? 0.0f : (controllerHolder->inputStates[0].stickY + controllerHolder->inputStates[0].stickY);
            gravityVector->y -=input * 0.39f;
        }      
    }
    wheelPhysicsHolder->Update(unk_float, gravityVector, wheelMat);
}
kmCall(0x805973b4, ConditionalFastFallingWheels);


s32 HandleGroundFeatherCollision(KartCollision *collision){
    ItemPlayer *itemPlayer = &ItemManager::sInstance->players[collision->base.GetPlayerIdx()];
    itemPlayer->inventory.currentItemCount += 1;
    itemPlayer->UseBlooper();
    return -1;
}
kmWritePointer(0x808b54e8, HandleGroundFeatherCollision);

kmWrite32(0x808D1dd4, 0x00000004); //blooper capacity
kmWrite16(0x808D1E28, 0x0101);     //allows blooper to fall on the ground
kmWrite32(0x807BBA64, 0x38000000); //removes timer
kmWrite32(0x807A8514, 0x4800009C); //skip animations
kmWrite32(0x808A9C16, 0x53495000); //changes brctr name to item_window_SIP
