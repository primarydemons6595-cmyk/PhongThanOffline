#ifndef PT_P1_BLUE_WORLD_TEST_H
#define PT_P1_BLUE_WORLD_TEST_H
#ifdef _SERVER
#include "LegacyDropItemAdapter.h"
#include "KObjSet.h"
#include "KObj.h"
#include "KSubWorld.h"
#include "P1BlueMagicTrace.h"
inline void P1BlueWorldTick(KPlayer& player)
{
    // m_bFinishLoading is never set TRUE by the current native login path.
    // Test commands require the connected test role to have a valid world NPC.
    if(strcmp(player.AccountName,"p1blueonly")||player.m_nNetConnectIdx<0||player.m_nIndex<=0||player.m_nIndex>=MAX_NPC)return;
    if(Npc[player.m_nIndex].m_SubWorldIndex<0||Npc[player.m_nIndex].m_RegionIndex<0)return;
    char root[MAX_PATH];DWORD n=GetEnvironmentVariableA("PHONGTHAN_P1_BLUE_ROOT",root,sizeof(root));
    if(!n||n>=sizeof(root)||!strstr(root,"\\P0-BlueMagic-Isolated\\p1_"))return;
    char exe[MAX_PATH];GetModuleFileNameA(0,exe,sizeof(exe));if(strncmp(exe,root,n)||exe[n]!='\\')return;
    FILE* f=fopen("p1_world.request","r");if(!f)return;char command[32],nonce[64];int got=fscanf(f,"%31s %63s",command,nonce);fclose(f);if(got!=2)return;
    if(!MoveFileA("p1_world.request","p1_world.running"))return;
    bool ok=true;
    if(!strcmp(command,"ground")){
        LegacyDropItemAdapter adapter;ok=adapter.Init(ItemGen);
        for(int b=0;b<2&&ok;++b){
            PhongThanQuestItemTuple tuple;ok=adapter.Map(0,6,0,tuple);tuple.level=1;tuple.series=0;int index=0;std::string error;
            if(!ok||!adapter.GenerateMapped(tuple,index,error)){ok=false;break;}
            if(b==0){KItemNormalAttrib attrs[MAX_ITEM_MAGICATTRIB];ZeroMemory(attrs,sizeof(attrs));attrs[0].nAttribType=85;attrs[0].nValue[0]=17;attrs[0].nValue[1]=-1;attrs[0].nValue[2]=6;attrs[1].nAttribType=86;attrs[1].nValue[0]=23;attrs[1].nValue[1]=2;attrs[1].nValue[2]=9;ok=Item[index].SetMagicAttrib(attrs)!=0;}
            KNpc& npc=Npc[player.m_nIndex];int x,y;npc.GetMpsPos(&x,&y);POINT pos;pos.x=x+32*b;pos.y=y;int world=npc.m_SubWorldIndex;
            SubWorld[world].GetFreeObjPos(pos);KMapPos map;map.nSubWorld=world;SubWorld[world].Mps2Map(pos.x,pos.y,&map.nRegion,&map.nMapX,&map.nMapY,&map.nOffX,&map.nOffY);
            KObjItemInfo info;ZeroMemory(&info,sizeof(info));info.m_nItemID=index;info.m_nItemWidth=Item[index].GetWidth();info.m_nItemHeight=Item[index].GetHeight();info.m_nColorID=Item[index].GetQuality();info.m_nGenre=Item[index].GetGenre();info.m_nDetailType=Item[index].GetDetailType();info.m_nMovieFlag=info.m_nSoundFlag=1;g_StrCpyLen(info.m_szName,Item[index].GetName(),sizeof(info.m_szName));
            int object=ok?ObjSet.Add(Item[index].GetObjIdx(),map,info):0;
            if(object<=0){ItemSet.Remove(index);ok=false;}else{Object[object].SetItemBelong(player.GetPlayerIndex());P1BlueTrace("SERVER_TEST_GROUND",Item[index]);}
        }
    }else if(!strcmp(command,"resync")||!strcmp(command,"trace")||!strcmp(command,"badtype")){
        PlayerItem* entry=player.m_ItemList.GetFirstItem();while(entry){int index=entry->nIdx;
            if(index>0&&index<MAX_ITEM){
                P1BlueTrace("SERVER_TEST_INVENTORY",Item[index]);
                if(!strcmp(command,"resync"))player.m_ItemList.SyncItem(index,TRUE,entry->nPlace,entry->nX,entry->nY,player.GetPlayerIndex());
                if(!strcmp(command,"badtype")&&Item[index].GetQuality()==equip_magic){PHONGTHAN_ITEM_PROPERTIES_MESSAGE packet;ZeroMemory(&packet,sizeof(packet));PhongThanInitializeWireHeader(&packet.Header,PHONGTHAN_MSG_INVENTORY_ITEM_PROPERTIES,sizeof(packet),PHONGTHAN_WIRE_FLAG_NONE,0);packet.ItemId=Item[index].GetID();packet.Attributes[0].Type=magic_normal_end;g_pServer->PackDataToClient(player.m_nNetConnectIdx,&packet,sizeof(packet));P1BlueTrace("SERVER_BAD_TYPE_SENT",Item[index]);}
            }entry=player.m_ItemList.GetNextItem();
        }
    }else if(!strcmp(command,"level4")){player.SetLevel(4);player.m_uMustSave=SAVE_REQUEST;}
    else if(!strcmp(command,"save")){player.m_uMustSave=SAVE_REQUEST;}
    else ok=false;
    f=fopen("p1_world.result","w");if(f){fprintf(f,"%s %s %s pid=%lu\n",nonce,command,ok?"OK":"FAIL",GetCurrentProcessId());fclose(f);}DeleteFileA("p1_world.running");
}
#endif
#endif
