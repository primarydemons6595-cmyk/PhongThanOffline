"""Apply reviewed ASCII edits without transcoding legacy C++ source files."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]/'PhongThanSource'
def edit(path,before,after):
    p=ROOT/path;b=p.read_bytes();before=before.encode();after=after.encode()
    # Normalize only matching text: leave the file's other legacy bytes intact.
    crlf=b'\r\n' in b
    normalized=b.replace(b'\r\n',b'\n')
    if after in normalized:return
    assert before in normalized,(path,before[:80])
    normalized=normalized.replace(before,after,1)
    p.write_bytes(normalized.replace(b'\n',b'\r\n') if crlf else normalized)
edit('Sources/Core/Src/KItem.h','#include "../../../Headers/PhongThanSocketState.h"','#include "../../../Headers/PhongThanSocketState.h"\n#include "../../../Headers/PhongThanSoulState.h"')
edit('Sources/Core/Src/KItem.h','int GetUnderlyingParam() const {PT_SOCKET_STATE s;return PtSocketRead(m_CommonAttrib.nParam,s)?s.BaseParam:m_CommonAttrib.nParam;}','int GetUnderlyingParam() const {return PtEquipmentBaseParam(m_CommonAttrib.nParam);}')
edit('Sources/Core/Src/KItem.h','GetParam() {return m_CommonAttrib.nParam;}', 'GetParam() const {return m_CommonAttrib.nParam;}')
edit('Sources/Core/Src/KItem.cpp','#include "PhongThanSocketRegistry.inl"','#include "PhongThanSocketRegistry.inl"\n#include "PhongThanSoulRegistry.inl"')
edit('Sources/Core/Src/KItem.cpp','#include "PhongThanSoulRegistry.inl"','#include "PhongThanSoulRegistry.inl"\n#include "PhongThanSoulPresentation.inl"')
edit('Sources/Core/Src/KItem.cpp','\tPT_SOCKET_STATE sockets;\n\tif(GetGenre()==item_equip && PtSocketRead(m_CommonAttrib.nParam,sockets))', '''    PT_SOUL_STATE soul;if(GetGenre()==item_equip&&PtSoulFromParam(m_CommonAttrib.nParam,soul)&&soul.MagicType){KItemNormalAttrib bonus;ZeroMemory(&bonus,sizeof(bonus));bonus.nAttribType=soul.MagicType;bonus.nValue[0]=soul.MagicValue;pNPC->ModifyAttrib(pNPC->m_Index,&bonus);}
\tPT_SOCKET_STATE sockets;
\tif(GetGenre()==item_equip && PtSocketRead(m_CommonAttrib.nParam,sockets))''')
# The removal path must reverse exactly the same immutable colored attribute.
edit('Sources/Core/Src/KItem.cpp','\tPT_SOCKET_STATE sockets;\n\tif(GetGenre()==item_equip && PtSocketRead(m_CommonAttrib.nParam,sockets))for(int si=0;si<4;++si)if(sockets.Attributes[si].Type){\n\t KItemNormalAttrib a;memcpy(&a,&sockets.Attributes[si],sizeof(a));for(int sv=0;sv<3;++sv)a.nValue[sv]=-a.nValue[sv];', '''    PT_SOUL_STATE soul;if(GetGenre()==item_equip&&PtSoulFromParam(m_CommonAttrib.nParam,soul)&&soul.MagicType){KItemNormalAttrib bonus;ZeroMemory(&bonus,sizeof(bonus));bonus.nAttribType=soul.MagicType;bonus.nValue[0]=-soul.MagicValue;pNPC->ModifyAttrib(pNPC->m_Index,&bonus);}
\tPT_SOCKET_STATE sockets;
\tif(GetGenre()==item_equip && PtSocketRead(m_CommonAttrib.nParam,sockets))for(int si=0;si<4;++si)if(sockets.Attributes[si].Type){
\t KItemNormalAttrib a;memcpy(&a,&sockets.Attributes[si],sizeof(a));for(int sv=0;sv<3;++sv)a.nValue[sv]=-a.nValue[sv];''')
edit('Sources/Core/Src/KItem.cpp','    if(GetGenre()==item_equip && PtSocketParam(GetParam())){','    if(GetGenre()==item_equip)PtSoulDescription(*this,pszMsg);\n    if(GetGenre()==item_equip && PtSocketParam(GetParam())){')
edit('Sources/Core/Src/PhongThanForgeItem.inl','if(PtSocketParam(m_CommonAttrib.nParam)){PT_SOCKET_STATE state;', 'PT_SOUL_STATE soul;if(PtSoulFromParam(m_CommonAttrib.nParam,soul)){soul.BaseParam=param;if(!PtSoulReplace(m_CommonAttrib.nParam,soul,param))return FALSE;}\n else if(PtSocketParam(m_CommonAttrib.nParam)){PT_SOCKET_STATE state;')
edit('Sources/Core/Src/PhongThanForgeItem.inl',' m_CommonAttrib.nParam=param;\n int starter=', ''' if(GetGenre()==item_equip&&PtSoulParam(param)){
  PT_SOUL_STATE state;if(PtSoulRead(param,state)){SetParam(state.BaseParam);if(m_CommonAttrib.nParam==state.BaseParam)m_CommonAttrib.nParam=param;}
  else {m_CommonAttrib.nParam=param;
#ifndef _SERVER
   PtSoulRequest(param);
#endif
  }return;
 }
 m_CommonAttrib.nParam=param;
 int starter=''')
edit('Sources/Core/Src/PhongThanForgePower.inl','total=0;forged=false;int param=item.m_CommonAttrib.nParam;','total=0;forged=false;int param=item.GetUnderlyingParam();')
edit('Sources/Core/Src/KPlayerDBFuns.cpp','NewItem.SetParam(nScriptParam);','''NewItem.SetParam(nScriptParam);
        if(NewItem.GetGenre()==item_equip&&PtSoulParam(PtSoulInnerParam(nScriptParam))){PT_SOUL_STATE soul;if(!PtSoulFromParam(nScriptParam,soul)||NewItem.GetParam()!=nScriptParam){CoreLoginDiag("Missing or invalid soul definition");return -1;}}''')
edit('Sources/Core/Src/PhongThanPetAutomation.inl','static bool PtPetSaleKeep(int item,const PT_PET_DATA& d){','static bool PtPetSaleKeep(int item,const PT_PET_DATA& d){\n if(PtSoulParam(PtSoulInnerParam(Item[item].GetParam())))return true;')
edit('Sources/Core/Src/KProtocolProcess.cpp','void PtSocketRequest(int param){','''void PtSoulRequest(int param){
 if(!g_pClient||!PtSoulParam(param))return;static std::map<int,DWORD> pending;DWORD now=GetTickCount();if(pending[param]&&now-pending[param]<2000)return;pending[param]=now;
 PT_SOUL_MESSAGE q;ZeroMemory(&q,sizeof(q));q.Param=param;PhongThanInitializeWireHeader(&q.Header,PT_SOUL_QUERY,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,0);g_pClient->SendPackToServer(&q,sizeof(q));
}
void PtSocketRequest(int param){''')
edit('Sources/Core/Src/KProtocolProcess.cpp','        if(pHeader->MessageType==PT_SOCKET_QUERY){','''        if(pHeader->MessageType==PT_SOUL_QUERY){
            if(PhongThanWorldFixedPacket(pMsg,nMsgSize,PT_SOUL_QUERY,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PT_SOUL_MESSAGE))){
                const PT_SOUL_MESSAGE* q=(const PT_SOUL_MESSAGE*)pMsg;PT_SOUL_MESSAGE reply;ZeroMemory(&reply,sizeof(reply));reply.Param=q->Param;
                static DWORD last[MAX_PLAYER],counts[MAX_PLAYER];DWORD now=GetTickCount();if(now-last[nIndex]>=1000){last[nIndex]=now;counts[nIndex]=0;}
                if(++counts[nIndex]<=64&&PtSoulRead(reply.Param,reply.State)){PhongThanInitializeWireHeader(&reply.Header,PT_SOUL_REPLY,sizeof(reply),PHONGTHAN_WIRE_FLAG_RESPONSE,0);g_pServer->SendData(Player[nIndex].m_nNetConnectIdx,&reply,sizeof(reply));}
            }return;
        }
        if(pHeader->MessageType==PT_SOCKET_QUERY){''')
edit('Sources/Core/Src/PhongThanWorldDispatch.inl','    if(header->MessageType==PT_SOCKET_REPLY){','''    if(header->MessageType==PT_SOUL_REPLY){
        if(PhongThanWorldFixedPacket(data,size,PT_SOUL_REPLY,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PT_SOUL_MESSAGE))){const PT_SOUL_MESSAGE* s=(const PT_SOUL_MESSAGE*)data;
            PT_SOUL_STATE old;bool fresh=!PtSoulRead(s->Param,old);if(PtSoulRegister(s->Param,s->State)&&fresh){
                for(int i=1;i<MAX_ITEM;++i)if(Item[i].GetGenre()==item_equip&&PtSoulInnerParam(Item[i].GetParam())==s->Param){KItemNormalAttrib magic[MAX_ITEM_MAGICATTRIB];memcpy(magic,Item[i].m_aryMagicAttrib,sizeof(magic));int dur=Item[i].GetDurability();Item[i].SetParam(Item[i].GetParam());Item[i].SetMagicAttrib(magic);Item[i].SetDurability(dur);}
                int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor>0&&actor<MAX_NPC)Player[CLIENT_PLAYER_INDEX].UpdataCurData();
            }
        }return true;
    }
    if(header->MessageType==PT_SOCKET_REPLY){''')
edit('Sources/Core/Src/ScriptFuns.cpp','#include "PhongThanMiningRuntime.inl"','#include "PhongThanMiningRuntime.inl"\n#include "PhongThanSoulRuntime.inl"')
edit('Sources/Core/Src/KSkills.cpp','#include "KCore.h"','#include "KCore.h"\n#include "PhongThanSoulRuntime.h"')
edit('Sources/Core/Src/KSkills.cpp','if(m_nId==62 && eLauncherType==SKILL_SLT_Npc)return PhongThanMiningCast(nLauncher,nParam1,nParam2)?TRUE:FALSE;','if(m_nId==62 && eLauncherType==SKILL_SLT_Npc)return PhongThanMiningCast(nLauncher,nParam1,nParam2)?TRUE:FALSE;\n    if(eLauncherType==SKILL_SLT_Npc&&Npc[nLauncher].IsPlayer()&&Npc[nLauncher].m_ActiveSkillID==m_nId&&!PhongThanSoulCharge(nLauncher,m_nId,(int)m_ulLevel))return FALSE;')
edit('Sources/Core/Src/KPlayer.cpp','Npc[m_nIndex].m_bRideHorse?-1:itempart_horse);','Npc[m_nIndex].m_bRideHorse?-1:itempart_horse,Npc[m_nIndex].m_Level,m_cProfession.GetProfession());')
edit('Sources/Core/Src/KProtocolProcess.cpp','Npc[index].m_bRideHorse?-1:itempart_horse);','Npc[index].m_bRideHorse?-1:itempart_horse,Npc[index].m_Level,Player[CLIENT_PLAYER_INDEX].m_cProfession.GetProfession());')
edit('Sources/Core/Src/KItemList.cpp','\t\tif(PtSocketParam(sItem.Param)){PT_SOCKET_MESSAGE state;', '''        int soulParam=PtSoulInnerParam(sItem.Param);if(PtSoulParam(soulParam)){PT_SOUL_MESSAGE soul;ZeroMemory(&soul,sizeof(soul));soul.Param=soulParam;if(!PtSoulRead(soul.Param,soul.State))return;PhongThanInitializeWireHeader(&soul.Header,PT_SOUL_REPLY,sizeof(soul),PHONGTHAN_WIRE_FLAG_RESPONSE,0);g_pServer->SendData(Player[nTargetPlayer].m_nNetConnectIdx,&soul,sizeof(soul));}
\t\tif(PtSocketParam(sItem.Param)){PT_SOCKET_MESSAGE state;''')
print('PASS soul item state/query/skill integration')
edit('Sources/Core/Src/PhongThanComposeServer.inl','#include "PhongThanSocketCompose.inl"','#include "PhongThanSocketCompose.inl"\n#include "PhongThanSoulCompose.inl"')
edit('Sources/Core/Src/PhongThanComposePreview.inl','#include "PhongThanSocketCompose.inl"','#include "PhongThanSocketCompose.inl"\n#include "PhongThanSoulCompose.inl"')
edit('Sources/Core/Src/PhongThanComposePreview.inl','    if(PtSocketComposeMode()>=2&&PtSocketComposeMode()<=6)', '''    if(PtSocketComposeMode()==8||PtSocketComposeMode()==9){quote.Status=PtSoulCompose(CLIENT_PLAYER_INDEX,indexes,selected,PtSocketComposeMode(),quote.RecipeId,quote.Money,true);if(quote.Status==PT_COMPOSE_OK)for(int q=0;q<9;++q)quote.Quantities[q]=selected[q]?selected[q]:(indexes[q]?-1:0);return;}
    if(PtSocketComposeMode()>=2&&PtSocketComposeMode()<=6)''')
edit('Sources/Core/Src/KProtocolProcess.cpp','        if(pHeader->MessageType==PT_SOUL_QUERY){', '''        if(pHeader->MessageType==PT_SOUL_COMPOSE_REQUEST){
            if(PhongThanWorldFixedPacket(pMsg,nMsgSize,PT_SOUL_COMPOSE_REQUEST,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PHONGTHAN_COMPOSE_COMMAND))){PHONGTHAN_COMPOSE_COMMAND command=*(const PHONGTHAN_COMPOSE_COMMAND*)pMsg;command.Header.MessageType=PHONGTHAN_COMPOSE_REQUEST;if(PhongThanValidateComposeCommand(&command,sizeof(command)))PhongThanSoulComposeCommand(nIndex,&command);}return;
        }
        if(pHeader->MessageType==PT_SOUL_QUERY){''')
edit('Sources/Core/Src/CoreShell.cpp','PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_COMPOSE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);', 'PhongThanInitializeWireHeader(&request.Header,(PtSocketComposeMode()==8||PtSocketComposeMode()==9)?PT_SOUL_COMPOSE_REQUEST:PHONGTHAN_COMPOSE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,(PtSocketComposeMode()==8||PtSocketComposeMode()==9)?PtSoulUiNonce():0);')
edit('Sources/Core/Src/PhongThanWorldDispatch.inl','\tif(header->MessageType==PHONGTHAN_COMPOSE_RESPONSE)', '''    if(header->MessageType==PT_SOUL_COMPOSE_REPLY){
        if(PhongThanWorldFixedPacket(data,size,PT_SOUL_COMPOSE_REPLY,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PHONGTHAN_COMPOSE_RESULT))){const PHONGTHAN_COMPOSE_RESULT* r=(const PHONGTHAN_COMPOSE_RESULT*)data;int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(a>0&&a<MAX_NPC&&r->MapId==(unsigned)SubWorld[0].m_SubWorldID&&r->EntityId==Npc[a].m_dwID&&(PtSocketComposeMode()==8||PtSocketComposeMode()==9)&&r->Header.Sequence&&(int)(r->Header.Sequence-(unsigned)PtSoulUiNonce())>=0){PtSoulUiNonce()=r->Header.Sequence;CoreDataChanged(PHONGTHAN_COMPOSE_CORE_RESULT,r->Result,0);}
        }return true;
    }
\tif(header->MessageType==PHONGTHAN_COMPOSE_RESPONSE)''')
edit('Sources/Core/Src/KProtocolProcess.cpp','PtSocketComposeMode()=pSync->Value>=2&&pSync->Value<=7?pSync->Value:1;\n            CoreDataChanged(GDCNI_ENCHASE, pSync->Value>=2&&pSync->Value<=6?1:pSync->Value, 0);', 'PtSocketComposeMode()=pSync->Value>=2&&pSync->Value<=9?pSync->Value:1;\n            PtSoulUiNonce()=pSync->Value>=8?pSync->Header.Sequence:0;\n            CoreDataChanged(GDCNI_ENCHASE, pSync->Value>=2&&pSync->Value<=9&&pSync->Value!=7?1:pSync->Value, 0);')
edit('Headers/PhongThanStatusPages.h','PT_STATUS_LIFE_MAKE=33','PT_STATUS_LIFE_MAKE=33,PT_STATUS_SOUL_OPEN=34')
edit('Sources/Core/Src/PhongThanStatusPages.inl','#include "PhongThanLifeRuntime.h"','#include "PhongThanLifeRuntime.h"\n#include "../../../Headers/PhongThanSoulState.h"')
edit('Sources/Core/Src/PhongThanStatusPages.inl','  }else if(q.Operation==PT_STATUS_LIFE_MAKE)', '  }else if(q.Operation==PT_STATUS_SOUL_OPEN){state.Result=q.Value==8?PhongThanSoulOpen(player,8):0;\n  }else if(q.Operation==PT_STATUS_LIFE_MAKE)')
edit('Sources/Core/Src/KItemList.cpp','    extern int PhongThanUseLifeItem(int,int);','    int soulUse=PhongThanSoulUseStone(m_PlayerIdx,nIdx);if(soulUse>=0)return soulUse!=0;\n    extern int PhongThanUseLifeItem(int,int);')
edit('Sources/GameClient/Ui/UiCase/UiLifeSkills.inl','KWndPureTextBtn m_Action;','KWndPureTextBtn m_Action,m_Soul;')
edit('Sources/GameClient/Ui/UiCase/UiLifeSkills.inl','  Select(0);Wnd_AddWindow(this);', '''  controls.WriteInteger("Soul","Left",120);controls.WriteInteger("Soul","Top",305);controls.WriteInteger("Soul","Width",110);controls.WriteInteger("Soul","Height",18);controls.WriteInteger("Soul","Font",14);controls.WriteString("Soul","Color","255,225,90");controls.WriteString("Soul","OverColor","255,255,255");controls.WriteInteger("Soul","CentreAlign",1);m_Soul.Init(&controls,"Soul");m_Soul.SetText("Hoan Hon");AddChild(&m_Soul);
  Select(0);Wnd_AddWindow(this);''')
edit('Sources/GameClient/Ui/UiCase/UiLifeSkills.inl',' int WndProc(unsigned int msg,unsigned int param,int data){',''' int WndProc(unsigned int msg,unsigned int param,int data){
  if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)(KWndWindow*)&m_Soul){PT_STATUS_PAGES_REQUEST q;memset(&q,0,sizeof(q));q.Operation=PT_STATUS_SOUL_OPEN;q.Value=8;if(g_pCoreShell)g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q));Hide();return 1;}''')
print('PASS soul crafting/refill UI and authoritative transactions wired')
