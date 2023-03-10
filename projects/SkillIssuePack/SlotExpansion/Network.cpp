#include <kamek.hpp>
#include <game/Network/RKNetController.hpp>
#include <game/Network/RKNetSelect.hpp>
#include <game/Network/RKNetPlayerInfo.hpp>
#include <game/System/random.hpp>
#include <SkillIssuePack.hpp>
#include <UserData/SIPData.hpp>
#include <SlotExpansion/SlotExpansion.hpp>
#include <UI/TeamsSelect/TeamSelect.hpp>




//Implements Host Always Wins, TrackBlocking (16) and fixes random
u8 lastTracks[TRACKBLOCKING] = {-1};
u8 curArrayIdx = 0;
void DecideTrack(RKNetSELECTHandler *select){
    Random random;
    RKNetController *controller = RKNetController::sInstance;
    if(select->mode == ONLINEMODE_PUBLIC_VS){     
        
        u32 availableAids = controller->subs[controller->currentSub].availableAids;
        u8 aids[12];
        u8 newVotesAids[12];
        int playerCount = 0;
        int newVoters = 0;
        for(u8 aid = 0; aid<12; aid++){
            if((1<<aid & availableAids) == 0) continue;
            aids[playerCount] = aid;
            playerCount++;
            bool isRepeat = false;
            SELECTPacket *aidPacket = &select->receivedPackets[aid];
            for(int i = 0; i < TRACKBLOCKING; i++) if(lastTracks[i] == aidPacket->playersData[0].courseVote) isRepeat = true;
            if(!isRepeat){
                newVotesAids[newVoters] = aid;
                newVoters++;
            }
        }
        u8 winner;
        if(newVoters > 0) winner = newVotesAids[random.NextLimited(newVoters)];
        else winner = aids[random.NextLimited(playerCount)];
        u8 vote = select->receivedPackets[winner].playersData[0].courseVote;
        if(vote == 0xFF) vote = random.NextLimited(TRACKCOUNT) + 0x44;
        select->toSendPacket.winningCourse = vote;
        select->toSendPacket.winningVoterAid = winner;
        lastTracks[curArrayIdx] = vote;
        curArrayIdx++;
    }
    if(select->mode == ONLINEMODE_PRIVATE_VS && SIPDataHolder::GetInstance()->GetSettings()->pages[SIP_HOST_SETTINGS].radioSetting[SIP_RADIO_HOSTWINS] == SIP_RADIO_HOSTWINS_ENABLED){
            u8 hostAid = controller->subs[controller->currentSub].hostAid;
            select->toSendPacket.winningVoterAid = hostAid;
            u8 hostVote = select->toSendPacket.playersData[0].courseVote;
            if(hostVote == 0xFF) hostVote = random.NextLimited(TRACKCOUNT) + 0x44;
            select->toSendPacket.winningCourse = hostVote;
    }
    else select->DecideTrack();
}
kmCall(0x80661490, DecideTrack);

u32 FixRandom(Random *random){
   return random->NextLimited(CUPCOUNT * 4) + 0x44;
}
kmCall(0x80661f34, FixRandom);

//Patches GetWinningCOURSE call so that non-hosts prepare the correct track
CourseId SetCorrectTrack(RKNetSELECTHandler *select){
    CourseId id = select->GetWinningCourse();
    if(select->toSendPacket.engineClass !=0){
        SIP::winningCourse = id;
        lastTracks[curArrayIdx] = id;
        curArrayIdx = (curArrayIdx + 1) % 16;
        id = GetCorrectTrackSlot();
    }
    return id;
}
kmCall(0x80650ea8, SetCorrectTrack);

//Fixes for spectating
void CorrectRH1(PacketHolder *packetHolder, RACEHEADER1Packet *rh1Packet, u32 len){
    rh1Packet->trackId = (u8) SIP::winningCourse;
    packetHolder->Copy(rh1Packet, len);
}
kmCall(0x80655458, CorrectRH1);
kmCall(0x806550e4, CorrectRH1);

CourseId ReturnCorrectId(RKNetRH1Handler *rh1Handler){
    for(int i = 0; i<12; i++){
        RH1Data *curRH1 = &rh1Handler->rh1Data[i];
        if(curRH1->trackId != 0x42 && curRH1->trackId != 0x43 && curRH1->trackId != 0xff && curRH1->timer != 0){
            SIP::winningCourse = curRH1->trackId;
            return GetCorrectTrackSlot();
        }
    }
    return (CourseId) -1;
}
kmBranch(0x80664560, ReturnCorrectId);
kmWrite32(0x80663f2c, 0x280500fe);
kmWrite32(0x80663f54, 0x280500fe);
kmWrite32(0x80663f80, 0x280500fe);
kmWrite32(0x80663fa8, 0x280500fe);
kmWrite32(0x80663fd4, 0x280500fe);
kmWrite32(0x80663ffc, 0x280500fe);
kmWrite32(0x80664028, 0x280500fe);
kmWrite32(0x80664050, 0x280500fe);
kmWrite32(0x80664250, 0x280500fe);
kmWrite32(0x80664278, 0x280500fe);
kmWrite32(0x806642a4, 0x280500fe);
kmWrite32(0x806642cc, 0x280500fe);
kmWrite32(0x806642f8, 0x280500fe);
kmWrite32(0x80664320, 0x280500fe);
kmWrite32(0x8066434c, 0x280500fe);
kmWrite32(0x80664374, 0x280500fe);

kmWrite32(0x80664b50, 0x280400fe);
kmWrite32(0x80664b88, 0x280400fe);
kmWrite32(0x80664bc0, 0x280400fe);
kmWrite32(0x80664bf8, 0x280400fe);


//Bunch of patches related to "usual" IDs not going over 0x43
kmWrite32(0x80644338, 0x2C03FFFF);
kmWrite32(0x8064433c, 0x418200dc);
kmWrite32(0x806440cc, 0x2C0300FF);
kmWrite32(0x806440d4, 0x41820008);
kmWrite32(0x80660460, 0x60000000);
kmWrite32(0x806605a0, 0x60000000);
kmWrite32(0x806605bc, 0x60000000);


