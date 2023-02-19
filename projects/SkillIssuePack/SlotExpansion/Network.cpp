#include <kamek.hpp>
#include <game/Network/RKNetController.hpp>
#include <game/Network/RKNetSelect.hpp>
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

//Bunch of patches related to "usual" IDs not going over 0x43
kmWrite32(0x80644338, 0x2C03FFFF);
kmWrite32(0x8064433c, 0x418200dc);
kmWrite32(0x806440cc, 0x2C0300FF);
kmWrite32(0x806440d4, 0x41820008);
kmWrite32(0x80660460, 0x60000000);
kmWrite32(0x806605a0, 0x60000000);
kmWrite32(0x806605bc, 0x60000000);


