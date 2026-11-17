// 场景地图（客户端版）
// Copyright : Kingsoft 2002
// Author    : Wooy (wu yue)
// CreateTime: 2002-11-11
// ---------------------------------------------------------------------------------------
// ***************************************************************************************
#include "KCore.h"
#include "../../../../Headers/PhongThanSceneViewport.h"
#include "PhongThanVisibility.h"
#include "../../../../Headers/PhongThanEscortHealth.h"
#include "../../../../Headers/PhongThanEscortRider.h"
#include "../../../Represent/iRepresent/PhongThanSpriteAnchor.h"
#include "KEngine.h"
#include "KScenePlaceC.h"
#include "../../../../Headers/PhongThanNpcStatMath.h"
#include <crtdbg.h>
#include "KIpotLeaf.h"
#include "SceneMath.h"
#include "../CoreShell.h"
#include "../KNpc.h"
#include "../KNpcSet.h"
#include "../KNpcResList.h"
#include "../KPlayer.h"
#include "../PhongThanSummon.h"
#include <map>
#include "../KSubWorld.h"
#include "../KSubWorldSet.h"
#include "../ImgRef.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../../Engine/Src/Text.h"
#include "ObstacleDef.h"
#include <math.h>
#include "../ImgRef.h"
#include "KWeather.h"
#include "KWeatherMgr.h"
#include "PhongThanNpcVitalRules.h"
#include "PhongThanOverheadDraw.h"
#include "../../../../Headers/PhongThanGatherProgress.h"
#include "../../../../Headers/PhongThanPotProgress.h"
#include "PhongThanChatBubble.h"
#include "../../../../Headers/PhongThanNationOverhead.h"
#include "../../../../Headers/PhongThanItemDisplay.h"
#include "../../../../Headers/PhongThanQuestMarkSprites.h"
#include "../../../../Headers/PhongThanTopLevelName.h"
#include "KIniFile.h"
extern int PhongThanGetNpcQuestMark(DWORD entity,int map);

#ifdef SWORDONLINE_SHOW_DBUG_INFO
	bool		g_bShowGameInfo = false;	
	int			g_nMapIndex = 0;
	int			g_bShowObstacle = false;
#endif

KScenePlaceC	g_ScenePlace;
//====是否预绘制地表层====
static bool		l_bPrerenderGround = true;

//摆放场景地图文件的目录
#define	ALL_PALCE_ROOT_FOLDER	"\\Maps"
//判断区域索引坐标是否落在以焦点区域为中心的一个范围内
#define INSIDE_AREA(h, v, range)	\
	( ((h) - m_FocusRegion.x) * ((h) - m_FocusRegion.x) <= (range * range) &&   \
		((v) - m_FocusRegion.y) * ((v) - m_FocusRegion.y) <= (range * range) )

#define GET_IN_PROCESS_AREA_REGION(h, v)		\
	( m_pInProcessAreaRegions[((v) - m_FocusRegion.y + SPWP_PROCESS_RANGE / 2) * SPWP_PROCESS_RANGE + (h) - m_FocusRegion.x + SPWP_PROCESS_RANGE / 2])

// NPC bodies are painted inside the scene object tree.  Drawing their names
// there lets later scene/weather primitives overwrite the font pixels.  Keep
// one authoritative name pass at the end of the scene frame instead.
static void ProjectPhongThanWorldAnchor(
	int nWorldX, int nWorldY, int nWorldZ,
	int nFocusX, int nFocusY,
	int nViewWidth, int nViewHeight,
	int& nScreenX, int& nScreenY)
{
	// This is the same projection as Represent2::CoordinateTransform after
	// LookAt(focus). Body SPRs, names and life bars therefore share one origin.
	PhongThanProjectWorldAnchor(nWorldX, nWorldY, nWorldZ,
		nFocusX, nFocusY, nViewWidth, nViewHeight, nScreenX, nScreenY);
}

// Original TopLevel palette and artwork, loaded once from the original INI.
struct PtTopLevelNameArt {
    DWORD color, border; char sprite[128];
    PtTopLevelNameArt():color(0xfffff37b),border(0xffde2829) {
        const char* path="\\ui\\\xCD\xB7\xB6\xA5\xCD\xBC\xB1\xEA.ini";
        strcpy(sprite,"\\spr\\ui4\\\xCD\xB7\xB6\xA5\xCD\xBC\xB1\xEA\\\xBB\xCA\xB9\xDA" "200.spr");
        KIniFile ini;
        if(ini.Load(path)) {
            char value[64]; int r,g,b;
            ini.GetString("TopLevel","NameColor","255,243,123",value,sizeof(value));
            if(sscanf(value,"%d,%d,%d",&r,&g,&b)==3 && r>=0&&r<=255&&g>=0&&g<=255&&b>=0&&b<=255)
                color=0xff000000|(r<<16)|(g<<8)|b;
            ini.GetString("TopLevel","NameBorderColor","222,40,41",value,sizeof(value));
            if(sscanf(value,"%d,%d,%d",&r,&g,&b)==3 && r>=0&&r<=255&&g>=0&&g<=255&&b>=0&&b<=255)
                border=0xff000000|(r<<16)|(g<<8)|b;
            char configured[128];ini.GetString("TopLevel","NameSpr",sprite,configured,sizeof(configured));
            strcpy(sprite,configured);
        }
    }
};
static const PtTopLevelNameArt& PtTopLevelArt() { static PtTopLevelNameArt art; return art; }
static void PtDrawTopLevelCrown(int left,int top,int font) {
    const PtTopLevelNameArt& art=PtTopLevelArt();
    KImageParam info;ZeroMemory(&info,sizeof(info));
    if(!g_pRepresent->GetImageParam(art.sprite,&info,ISI_T_SPR)||info.nNumFrames<=0)return;
    PhongThanTopLevelPosition p=PhongThanTopLevelCrownPosition(left,top,font,info.nWidth,info.nHeight);
    KRUImage image;ZeroMemory(&image,sizeof(image));
    image.nType=ISI_T_SPR;image.nISPosition=IMAGE_IS_POSITION_INIT;
    image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;image.Color.Color_b.a=255;
    strcpy(image.szImage,art.sprite);image.oPosition.nX=p.x;image.oPosition.nY=p.y;
    g_pRepresent->DrawPrimitives(1,&image,RU_T_IMAGE,TRUE);
}

static void PaintPhongThanLifeBarOverlay(KNpc& npc, int centerX, int top)
{
    if (npc.m_CurrentLifeMax <= 0 ||
        (npc.m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex && npc.m_HideState.nTime > 0)) return;
    int observer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
    int observerLevel = observer > 0 && observer < MAX_NPC ? Npc[observer].m_Level : 0;
    int type = 0;
    bool cart=npc.m_NpcSettingIdx==556;
    if (npc.m_Kind == kind_normal && npc.m_NpcSettingIdx >= 0)
        g_NpcSetting.GetInteger(npc.m_NpcSettingIdx + 2, "LifeBarType", 0, &type);
    if(cart)type=1;
    int displayLevel=cart?1:npc.m_Level;
    if(PhongThanIsClientSummon(npc.m_Index))
        displayLevel=PhongThanSummonShownLevel(npc.m_Level,PhongThanClientSummonRank(npc.m_Index));
    PhongThanDrawLife(g_pRepresent, centerX, top, displayLevel, observerLevel,
        PhongThanLifePercent(npc.m_CurrentLife, npc.m_CurrentLifeMax, MAX_PERCENT), type);
    if(cart){
        char text[24];PhongThanEscortHealthText(npc.m_CurrentLife,npc.m_CurrentLifeMax,text);
        g_pRepresent->OutputText(12,text,KRF_ZERO_END,
            centerX+11-3*(int)strlen(text),top+1,0xffffffff,0,TEXT_IN_SINGLE_PLANE_COORD,0xff000000);
    }
}
#include "../../../../Headers/PhongThanNpcNameColor.h"
// Freeze the label on a species head anchor, independent of wing animation.
// Reference art is the original petinfoex standing sprite, never the current
// cropped animation frame. Coordinates are in that sprite's authoring canvas.
static POINT PhongThanPetFixedHead(int templateId,const char* fallback){
 static std::map<int,POINT> cache;std::map<int,POINT>::iterator found=cache.find(templateId);
 if(found!=cache.end())return found->second;
 char image[MAX_PATH];g_StrCpyLen(image,fallback,sizeof(image));int type=-1;
 KTabFile pets,info;
 if(pets.Load("\\settings\\pets.txt")&&info.Load("\\settings\\petinfoex.txt")){
  for(int row=2;row<=pets.GetHeight();++row){int id=-1;pets.GetInteger(row,2,-1,&id);if(id-2==templateId){type=row-2;info.GetString(row,1,image,image,sizeof(image));break;}}
 }
 KRPosition2 offset,size;POINT head;head.x=0;head.y=0;
 if(g_pRepresent->GetImageFrameParam(image,0,&offset,&size,ISI_T_SPR)){
  head.x=offset.nX+size.nX/2;head.y=offset.nY;
  // Luc Phi Phi: original ani101_st contact sheet places the head at the
  // central body, below the moving wing tips. Fixed clearance is authored.
  if(type==1){head.x=256;head.y=225;}
  cache[templateId]=head;
 }
 return head;
}
static void PaintPhongThanNpcNameOverlay(
	const RECT* pRepresentArea,
	int nFocusX, int nFocusY,
	int nViewWidth, int nViewHeight)
{
	if (g_pRepresent == NULL || pRepresentArea == NULL ||
		nViewWidth <= 0 || nViewHeight <= 0)
		return;

    struct TeamFlag {int x,top,state;} flags[MAX_NPC];int flagCount=0;
	int nNpcIdx = 0;
	while ((nNpcIdx = NpcSet.GetNextIdx(nNpcIdx)) != 0)
	{
        KNpc& NpcObj = Npc[nNpcIdx];
        if(PhongThanClientVisualHidden(nNpcIdx))continue;
        if(PhongThanIsEscortRider(nNpcIdx))continue;
		// NPC names were added here because the legacy world-coordinate text
		// path can be covered by later scene primitives. Players need the same
		// final-pass treatment: their body is rendered, but their name otherwise
		// goes through the legacy projection and can disappear at non-origin
		// camera positions.
		if (NpcObj.m_Kind != kind_dialoger && NpcObj.m_Kind != kind_normal &&
			NpcObj.m_Kind != kind_player)
			continue;
		if (NpcObj.m_SubWorldIndex < 0 || NpcObj.m_RegionIndex < 0)
			continue;

        char plainName[128];unsigned int explicitNameColor=0;bool hasNameColor=false;
        if(NpcObj.m_Kind!=kind_player)hasNameColor=PhongThanParseNpcName(NpcObj.Name,plainName,sizeof(plainName),explicitNameColor);
        else PhongThanPlainItemName(NpcObj.Name,plainName,sizeof(plainName));
		const char* pszName = plainName;
		if (pszName[0] == '#')
			pszName++;
		if (pszName[0] == 0)
			continue;

        char ownerName[32], summonName[32];
        ownerName[0] = summonName[0] = 0;
        const char* ownerSuffix = strrchr(pszName, '(');
        int summonOwner=PhongThanClientSummonOwner(nNpcIdx);
        // Ownership survives cosmetic template changes. A template allowlist
        // alone loses summon layout/color immediately after morphing.
        bool isSummon = NpcObj.m_Kind == kind_normal && PhongThanIsClientSummon(nNpcIdx) && ownerSuffix && ownerSuffix>pszName;
        if (isSummon)
        {
            g_StrCpyLen(ownerName, ownerSuffix+1, sizeof(ownerName));
            char* close = strrchr(ownerName, ')'); if (close) *close=0;
            if(summonOwner>0&&summonOwner<MAX_NPC)g_StrCpyLen(ownerName,Npc[summonOwner].Name,sizeof(ownerName));
            int count = ownerSuffix - pszName;
            while (count > 0 && pszName[count-1] == ' ') --count;
            if (count >= sizeof(summonName)) count = sizeof(summonName)-1;
            memcpy(summonName, pszName, count); summonName[count]=0;
        }
        int driver=PhongThanClientEscortDriver(nNpcIdx);
        KNpc& anchor=Npc[driver>0?driver:nNpcIdx];
        int nMpsX = 0;
		int nMpsY = 0;
        anchor.GetMpsPos(&nMpsX, &nMpsY);
        anchor.GetNpcRes()->GetRenderPosition(&nMpsX, &nMpsY);
		if (nMpsX < pRepresentArea->left || nMpsX >= pRepresentArea->right ||
			nMpsY < pRepresentArea->top || nMpsY >= pRepresentArea->bottom)
		{
			continue;
		}

		const int nFontSize = (NpcObj.m_Kind == kind_player) ? 12 : 14;
		int nNameHeight = anchor.GetNpcVisualPate();
		// Preserve the entity-specific sprite/body/mount visual height.
		nNameHeight = PhongThanVisualAnchorHeight(nNameHeight, 96);
		nNameHeight += nFontSize + 1;

		int nScreenX = 0;
		int nScreenY = 0;
		ProjectPhongThanWorldAnchor(
			nMpsX, nMpsY, nNameHeight,
			nFocusX, nFocusY, nViewWidth, nViewHeight,
			nScreenX, nScreenY);
        if(NpcObj.m_Kind==kind_player)nScreenY-=PhongThanPlayerNameClearance(NpcObj.m_bRideHorse);
        // Summons can contain asymmetric effect art. Align the overlay with
        // the cropped body frame, using the renderer's actual reference spot.
        const bool petOverlay=anchor.GetNpcRes()->m_PetStyle!=0;
        if (petOverlay || isSummon || NpcObj.m_NpcSettingIdx==556 || (NpcObj.m_NpcSettingIdx >= 359 && NpcObj.m_NpcSettingIdx <= 362) ||
            (NpcObj.m_NpcSettingIdx >= 403 && NpcObj.m_NpcSettingIdx <= 407))
        {
            int part = 0;
            while (part < MAX_PART-1 && !anchor.GetNpcRes()->m_cNpcImage[part].CheckExist()) ++part;
            KSprControl& body = anchor.GetNpcRes()->m_cNpcImage[part];
            KImageParam info;
            KRPosition2 offset, size;
            ZeroMemory(&info, sizeof(info));
            if (g_pRepresent->GetImageParam(body.m_szName, &info, ISI_T_SPR) &&
                g_pRepresent->GetImageFrameParam(body.m_szName, body.m_nCurFrame, &offset, &size, ISI_T_SPR))
            {
                int cx = info.nReferenceSpotX + 105, cy = info.nReferenceSpotY + 105;
                int referenceX = (cx || cy) ? cx : 160;
                if (PhongThanUsesNpcBodyCanvas(body.m_szName, info.nWidth, info.nHeight))
                    referenceX = (cx || cy) ? cx : 255;
                int referenceY = (cx || cy) ? cy : 192;
                if (PhongThanUsesNpcBodyCanvas(body.m_szName, info.nWidth, info.nHeight))
                    referenceY = PhongThanActorReferenceY(body.m_szName, cx, cy);
                ProjectPhongThanWorldAnchor(nMpsX, nMpsY, anchor.m_Height,
                    nFocusX, nFocusY, nViewWidth, nViewHeight, nScreenX, nScreenY);
                POINT fixedHead;
                if(petOverlay)fixedHead=PhongThanPetFixedHead(NpcObj.m_NpcSettingIdx,body.m_szName);
                nScreenX += (petOverlay?fixedHead.x:offset.nX+size.nX/2)-referenceX;
                KImageParam bar;
                ZeroMemory(&bar, sizeof(bar));
                int barHeight = 13;
                if (g_pRepresent->GetImageParam("\\spr\\ui4\\barback.spr", &bar, ISI_T_SPR)) barHeight=bar.nHeight;
                // Pet dialogers have no world HP bar. Align their label to the
                // same current cropped body as rendering, in both X and Y.
                if(petOverlay)barHeight=0;
                // Stack from the actual cropped head: name, HP bar, 4px gap, body.
                nScreenY += (petOverlay?fixedHead.y:offset.nY) - referenceY - nFontSize - 2 - barHeight - 4;
                static DWORD loggedEntities[MAX_NPC];
                static int loggedTemplates[MAX_NPC];
                if (loggedEntities[nNpcIdx] != NpcObj.m_dwID || loggedTemplates[nNpcIdx] != NpcObj.m_NpcSettingIdx)
                {
                    loggedEntities[nNpcIdx] = NpcObj.m_dwID;loggedTemplates[nNpcIdx] = NpcObj.m_NpcSettingIdx;
                    FILE* log=fopen("summon_visual_diag.log","a");
                    if (log) { fprintf(log,"entity=%u template=%d part=%d resource=%s frame=%d canvas=%d,%d center=%d,%d offset=%d,%d crop=%d,%d nameHeight=%d\n",
                        NpcObj.m_dwID,NpcObj.m_NpcSettingIdx,part,body.m_szName,body.m_nCurFrame,
                        info.nWidth,info.nHeight,cx,cy,offset.nX,offset.nY,size.nX,size.nY,nNameHeight); fclose(log); }
                }
            }
        }
		if (nScreenX < -160 || nScreenX >= nViewWidth + 160 ||
			nScreenY < -40 || nScreenY >= nViewHeight + 40)
		{
			continue;
		}

		const BOOL bShowLife =
            NpcObj.m_NpcSettingIdx==556 ||
			NpcObj.m_Kind == kind_player ||
			(NpcObj.m_Kind == kind_normal &&
			 (NpcSet.CheckShowLife() || Player[CLIENT_PLAYER_INDEX].GetTargetNpc() == nNpcIdx));
		if (bShowLife)
			PaintPhongThanLifeBarOverlay(NpcObj, nScreenX,
				nScreenY + nFontSize + 2);

		DWORD dwColor = 0xffffffff;
		if (NpcObj.m_Kind == kind_player || isSummon)
		{
			switch (NpcObj.m_CurrentCamp)
			{
			// PT Game.exe 6F5BE2/6F7244: raw current-camp base palette.
			// Legacy enum labels do not define PT camp/PK semantics.
			// Do not infer camp from level or mutate combat/trade state here.
			case 1:
				dwColor = 0xffd1aa6e;
				break;
			case 2:
				dwColor = 0xffff92ff;
				break;
			case 3:
				dwColor = 0xff82d9ff;
				break;
			case 4:
				dwColor = 0xffffff00;
				break;
			case 7:
				dwColor = 0xff55ff91;
				break;
			case 8:
				dwColor = 0xffff0000;
				break;
			case camp_event:
				dwColor = 0xffee1289;
				break;
			default:
				dwColor = 0xffffffff;
				break;
			}
		}
		else if (NpcObj.m_Kind == kind_normal)
		{
			switch (NpcObj.m_btSpecial)
			{
			case npc_blue:
				dwColor = 0xff6569d7;
				break;
			case npc_gold:
				dwColor = 0xffffd94e;
				break;
			default:
				dwColor = 0xffffffff;
				break;
			}
		}

        if(hasNameColor)dwColor=explicitNameColor;
        DWORD nameBorder=0xff000000;
        bool topLevel=PhongThanHasTopLevelCrown(NpcObj.m_Kind==kind_player,NpcObj.m_Level);
        if(PhongThanHasTopLevelNameColor(NpcObj.m_Kind==kind_player,NpcObj.m_Level,NpcObj.m_CurrentCamp)) {
            dwColor=PtTopLevelArt().color;nameBorder=PtTopLevelArt().border;
        }
        const bool nationMember=NpcObj.m_Kind==kind_player&&NpcObj.m_bTongFlag&&NpcObj.m_dwTongNameID&&NpcObj.m_nFigure>=1&&NpcObj.m_nFigure<=5;
        const int nationOffset=nationMember?20:0;
        int highestTextY=nScreenY-(nationMember?18:0);
        if(nationMember){
            char nationName[64];PhongThanPlainItemName(NpcObj.m_szTongName,nationName,sizeof(nationName));
            g_pRepresent->OutputText(nFontSize,nationName,KRF_ZERO_END,
                nScreenX-nFontSize*g_StrLen(nationName)/4,nScreenY-18,dwColor,0,TEXT_IN_SINGLE_PLANE_COORD,nameBorder);
        }
        if(NpcObj.m_Kind==kind_player && NpcObj.m_nTeamServerID>=0 && NpcObj.m_PtTeamName[0]) {
            highestTextY=min(highestTextY,nScreenY-nationOffset-(NpcObj.m_PTrade.nTrade&&NpcObj.m_PTrade.cName[0]?42:22));
            char teamName[64];PhongThanPlainItemName(NpcObj.m_PtTeamName,teamName,sizeof(teamName));
            PhongThanDrawStallFrame(g_pRepresent,nScreenX,nScreenY-nationOffset-(NpcObj.m_PTrade.nTrade&&NpcObj.m_PTrade.cName[0]?42:22),14*g_StrLen(teamName)/2);
            g_pRepresent->OutputText(14,teamName,KRF_ZERO_END,
                nScreenX-14*g_StrLen(teamName)/4,nScreenY-nationOffset-(NpcObj.m_PTrade.nTrade&&NpcObj.m_PTrade.cName[0]?40:20),0xffffd94e,0,TEXT_IN_SINGLE_PLANE_COORD,0xff000000);
        }
        if(NpcObj.m_Kind==kind_player && NpcObj.m_PTrade.nTrade && NpcObj.m_PTrade.cName[0]) {
            highestTextY=min(highestTextY,nScreenY-nationOffset-22);
            char shop[64];PhongThanPlainItemName(NpcObj.m_PTrade.cName,shop,sizeof(shop));
            PhongThanDrawStallFrame(g_pRepresent,nScreenX,nScreenY-nationOffset-22,14*g_StrLen(shop)/2);
            g_pRepresent->OutputText(14,shop,KRF_ZERO_END,
                nScreenX-14*g_StrLen(shop)/4,nScreenY-nationOffset-20,0xffffd94e,0,TEXT_IN_SINGLE_PLANE_COORD,0xff000000);
        }
        if(NpcObj.m_Kind==kind_player&&NpcObj.GetMenuState()==PLAYER_MENU_STATE_TEAMOPEN&&flagCount<MAX_NPC){
            flags[flagCount].x=nScreenX;flags[flagCount].top=highestTextY;
            flags[flagCount++].state=PLAYER_MENU_STATE_TEAMOPEN;
        }
        if(NpcObj.m_Kind==kind_dialoger && NpcObj.m_SubWorldIndex>=0 && NpcObj.m_SubWorldIndex<MAX_SUBWORLD){
            PTPotProgress pot;
            if(PhongThanGetPotProgress(SubWorld[NpcObj.m_SubWorldIndex].m_SubWorldID,NpcObj.m_dwID,pot)){
                int top=highestTextY-24;PhongThanDrawGatherProgress(g_pRepresent,nScreenX,top,PTPotRemainingPercent(pot,pot.Now),"");
                int seconds=pot.Now<pot.Ready?pot.Ready-pot.Now:pot.Burn-pot.Now;char clock[40];sprintf(clock,"%02d:%02d",seconds/60,seconds%60);
                g_pRepresent->OutputText(12,clock,KRF_ZERO_END,nScreenX-3*(int)strlen(clock),top+3,0xffffffff,0,TEXT_IN_SINGLE_PLANE_COORD,0xff000000);
                if(pot.Text[0]){
                    char text[512];lstrcpynA(text,pot.Text,sizeof(text));int length=TEncodeText(text,strlen(text)),width=0;
                    int lines=TGetEncodedTextLineCount(text,length,1000,width,14,0,3);if(lines<1)lines=1;if(lines>3)lines=3;
                    KOutputTextParam param;memset(&param,0,sizeof(param));param.nX=nScreenX-width*7/2;param.nY=top-lines*15-3;param.nZ=TEXT_IN_SINGLE_PLANE_COORD;param.nVertAlign=1;
                    param.Color=0xffffff00;param.BorderColor=0xff000000;param.nNumLine=(short)lines;g_pRepresent->OutputRichText(14,&param,text,length);
                }
            }
        }
        if(NpcObj.m_Kind==kind_player)PhongThanDrawGatherProgress(g_pRepresent,nScreenX,highestTextY-16,NpcObj.m_nPacePercent);
        bool routeActive=Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoRunMap || Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoRunFlagMap;
        if(NpcObj.m_Kind==kind_player&&nNpcIdx==Player[CLIENT_PLAYER_INDEX].m_nIndex&&
           (routeActive || Player[CLIENT_PLAYER_INDEX].m_cAI.m_bIsActive)){
            const char* captionSpr=routeActive?"\\spr\\phongthan\\interaction\\045bce29a0458b63.spr":"\\spr\\phongthan\\interaction\\4e5822ebb45b17e1.spr";
            KImageParam param;ZeroMemory(&param,sizeof(param));
            if(g_pRepresent->GetImageParam(captionSpr,&param,ISI_T_SPR)&&param.nNumFrames>0){
                KRUImage caption;ZeroMemory(&caption,sizeof(caption));
                caption.nType=ISI_T_SPR;caption.Color.Color_b.a=255;caption.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
                caption.nISPosition=IMAGE_IS_POSITION_INIT;caption.bRenderFlag=RUIMAGE_RENDER_FLAG_REF_SPOT;
                strcpy(caption.szImage,captionSpr);
                caption.nFrame=(GetTickCount()/(param.nInterval>0?param.nInterval:100))%param.nNumFrames;
                caption.oPosition.nX=nScreenX;caption.oPosition.nY=nScreenY+6;
                g_pRepresent->DrawPrimitives(1,&caption,RU_T_IMAGE,TRUE);continue;
            }
        }
        if(NpcObj.m_Kind==kind_dialoger) {
            int variant=PhongThanGetNpcQuestMark(NpcObj.m_dwID,SubWorld[NpcObj.m_SubWorldIndex].m_SubWorldID);
            if(variant>=0 && variant<5) {
                const char* path=s_PTQuestMarkSprites[variant];KImageParam info;memset(&info,0,sizeof(info));
                if(g_pRepresent->GetImageParam(path,&info,ISI_T_SPR) && info.nNumFrames>0) {
                    KRUImage book;memset(&book,0,sizeof(book));book.nType=ISI_T_SPR;
                    book.Color.Color_b.a=255;book.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
                    book.nISPosition=IMAGE_IS_POSITION_INIT;strcpy(book.szImage,path);
                    book.nFrame=(GetTickCount()/(info.nInterval>0?info.nInterval:100))%info.nNumFrames;
                    book.oPosition.nX=nScreenX-info.nWidth/2;book.oPosition.nY=nScreenY-info.nHeight-4;
                    g_pRepresent->DrawPrimitives(1,&book,RU_T_IMAGE,TRUE);
                }
            }
        }
        if(NpcObj.m_Kind==kind_player && NpcObj.m_nChatContentLen>0 && NpcObj.m_nChatNumLine>0 &&
           (nNpcIdx==Player[CLIENT_PLAYER_INDEX].m_nIndex || NpcObj.m_HideState.nTime<=0))
            PhongThanDrawChatBubble(g_pRepresent,nScreenX,nScreenY,NpcObj.m_szChatBuffer,
                NpcObj.m_nChatContentLen,NpcObj.m_nChatFontWidth,NpcObj.m_nChatNumLine);
        if (isSummon)
        {
            g_pRepresent->OutputText(nFontSize, ownerName, KRF_ZERO_END,
                nScreenX-nFontSize*g_StrLen(ownerName)/4, nScreenY-nFontSize-2,
                dwColor,0,TEXT_IN_SINGLE_PLANE_COORD,0xff000000);
            pszName = summonName;
        }
        if(nationMember){
            const char* caption=PhongThanNationDutyCaption(NpcObj.m_nFigure);
            int prefixWidth=nFontSize*g_StrLen(caption)/2;
            int left=nScreenX-(prefixWidth+nFontSize*g_StrLen(pszName)/2)/2;
            g_pRepresent->OutputText(nFontSize,caption,KRF_ZERO_END,left,nScreenY,0xffffff00,0,TEXT_IN_SINGLE_PLANE_COORD,0xff000000);
            if(topLevel)PtDrawTopLevelCrown(left,nScreenY,nFontSize);
            g_pRepresent->OutputText(nFontSize,pszName,KRF_ZERO_END,left+prefixWidth,nScreenY,dwColor,0,TEXT_IN_SINGLE_PLANE_COORD,nameBorder);
            continue;
        }
        if(topLevel)PtDrawTopLevelCrown(nScreenX-nFontSize*g_StrLen(pszName)/4,nScreenY,nFontSize);
		g_pRepresent->OutputText(
			nFontSize, pszName, KRF_ZERO_END,
			nScreenX - nFontSize * g_StrLen(pszName) / 4, nScreenY,
			dwColor, 0, TEXT_IN_SINGLE_PLANE_COORD, nameBorder);

	}
    // Render all flags after the complete text pass so scene lights, weapon
    // effects and later player labels cannot overwrite or tint them.
    for(int f=0;f<flagCount;++f){char image[128];image[0]=0;
        g_NpcResList.m_cMenuState.GetStateSpr(flags[f].state,image);
        PhongThanDrawTeamFlag(g_pRepresent,image,flags[f].x,flags[f].top);
    }

}

//***********************************************************************************************
// EnvironmentLight类的实现
DWORD ChaZhiColor(KLColor &cLight1, KLColor &cLight2, float f2)
{
	if(f2 < 0.0f || f2 > 1.0f)
		return 0xff000000;

	float f1 = 1.0f - f2;
	unsigned int r, g, b;
	r = (unsigned int)(cLight2.r * f2 + cLight1.r * f1);
	g = (unsigned int)(cLight2.g * f2 + cLight1.g * f1);
	b = (unsigned int)(cLight2.b * f2 + cLight1.b * f1);
	return 0xff000000 | (r<<16) | (g<<8) | b;
}

EnvironmentLight::EnvironmentLight()
{
	for(int i=0; i<7; i++)
		m_cLight[i].r = m_cLight[i].g = m_cLight[i].b = 0x00000040;

	m_cLight[0].r = 0x18, m_cLight[0].g = 0x18, m_cLight[0].b = 0x18;
	m_cLight[1].r = 0x18, m_cLight[1].g = 0x18, m_cLight[1].b = 0x28;
	m_cLight[2].r = 0x38, m_cLight[2].g = 0x38, m_cLight[2].b = 0x3b;
	m_cLight[3].r = 0x40, m_cLight[3].g = 0x40, m_cLight[3].b = 0x40;
	m_cLight[4].r = 0x58, m_cLight[4].g = 0x58, m_cLight[4].b = 0x58;
	m_cLight[5].r = 0x38, m_cLight[5].g = 0x30, m_cLight[5].b = 0x28;
	m_cLight[6].r = 0x30, m_cLight[6].g = 0x2a, m_cLight[6].b = 0x28;
}

// 设置第nIdx个颜色
void EnvironmentLight::SetLight(const KLColor &cLight, int nIdx)
{
	m_cLight[nIdx] = cLight;
}

// 设置第nIdx个颜色
void EnvironmentLight::SetLight(BYTE r, BYTE g, BYTE b, int nIdx)
{
	m_cLight[nIdx].r = r;
	m_cLight[nIdx].g = g;
	m_cLight[nIdx].b = b;
}

// 设置所有7个颜色
void EnvironmentLight::SetLight(KLColor *pLight)
{
	for(int i=0; i<7; i++)
		m_cLight[i] = pLight[i];
}

// 取得距一天开始nMinutes分钟时的环境光颜色
DWORD EnvironmentLight::GetEnvironmentLight(int nMinutes)
{
	if(nMinutes < 660)
	{
		if(nMinutes < 300)
		{
			if(nMinutes >= 0 && nMinutes < 60)
				return m_cLight[0].GetColor();
			else if(nMinutes >= 60 && nMinutes < 180)
				return ChaZhiColor(m_cLight[0], m_cLight[1], (nMinutes - 60) / 120.0f);
			else if(nMinutes >= 180 && nMinutes < 300)
				return m_cLight[1].GetColor();
		}
		else
		{
			if(nMinutes >= 300 && nMinutes < 420)
				return ChaZhiColor(m_cLight[1], m_cLight[2], (nMinutes - 300) / 120.0f);
			else if(nMinutes >= 420 && nMinutes < 540)
				return ChaZhiColor(m_cLight[2], m_cLight[3], (nMinutes - 420) / 120.0f);
			else if(nMinutes >= 540 && nMinutes < 660)
				return ChaZhiColor(m_cLight[3], m_cLight[4], (nMinutes - 540) / 120.0f);
		}
	}
	else
	{
		if(nMinutes < 1020)
		{
			if(nMinutes >= 660 && nMinutes < 780)
				return m_cLight[4].GetColor();
			else if(nMinutes >= 780 && nMinutes < 900)
				return ChaZhiColor(m_cLight[4], m_cLight[3], (nMinutes - 780) / 120.0f);
			else if(nMinutes >= 900 && nMinutes < 1020)
				return ChaZhiColor(m_cLight[3], m_cLight[5], (nMinutes - 900) / 120.0f);
		}
		else
		{
			if(nMinutes >= 1020 && nMinutes < 1140)
				return ChaZhiColor(m_cLight[5], m_cLight[6], (nMinutes - 1020) / 120.0f);
			else if(nMinutes >= 1140 && nMinutes < 1260)
				return ChaZhiColor(m_cLight[6], m_cLight[0], (nMinutes - 1140) / 120.0f);
			else if(nMinutes >= 1260 && nMinutes < 1440)
				return m_cLight[0].GetColor();
		}
	}
	return 0xff000000;
}

//*************************************************************************************************

//##ModelId=3DDB39150334
POINT KScenePlaceC::m_RangePosTable[SPWP_MAX_NUM_REGIONS] =
{
	{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0},
	{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1},
	{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2},
	{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3},
	{0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4},
	{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5},
	{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6},
};

//##ModelId=3DDB39BA029B
int	KScenePlaceC::m_PRIIdxTable[SPWP_MAX_NUM_REGIONS] =
{
	24, 31, 30, 23, 16, 17, 18, 
	25, 32, 39, 38, 37, 36, 29,
	22, 15, 8,  9,  10, 11, 12,
	19, 26, 33, 40, 47, 46, 45,
	44, 43, 42, 35, 28, 21, 14,
	7,  0,  1,  2,  3,  4,  5,
	6,  13, 20, 27, 34, 41, 48,
};

const KPrevLoadPosItem KScenePlaceC::m_PrevLoadPosOffset[3][3] = 
{
    {
        { 5, {{-2,  0}, {-2, -1}, {-2, -2}, {-1, -2}, { 0, -2}} },  // 左上角(-1, -1)的对应需要预加载的Region的数目和相对坐标
        { 3, {{-1, -2}, { 0, -2}, { 1, -2}, { 0,  0}, { 0,  0}} },  // 正上方( 0, -1)的对应需要预加载的Region的数目和相对坐标
        { 5, {{ 0, -2}, { 1, -2}, { 2, -2}, { 2, -1}, { 2,  0}} },  // 右上角( 1, -1)的对应需要预加载的Region的数目和相对坐标
    },
    {
        { 3, {{-2, -1}, {-2,  0}, {-2,  1}, { 0,  0}, { 0,  0}} },  // 正左方(-1,  0)的对应需要预加载的Region的数目和相对坐标
        { 0, {{ 0,  0}, { 0,  0}, { 0,  0}, { 0,  0}, { 0,  0}} },  // 原  点( 0,  0)的对应需要预加载的Region的数目和相对坐标
        { 3, {{ 2, -1}, { 2,  0}, { 2,  1}, { 0,  0}, { 0,  0}} },  // 正右方( 1,  0)的对应需要预加载的Region的数目和相对坐标
    },
    {
        { 5, {{-2,  0}, {-2,  1}, {-2,  2}, {-1,  2}, { 0,  2}} },  // 左下角(-1,  1)的对应需要预加载的Region的数目和相对坐标
        { 3, {{-1,  2}, { 0,  2}, { 1,  2}, { 0,  0}, { 0,  0}} },  // 正下方( 0,  1)的对应需要预加载的Region的数目和相对坐标
        { 5, {{ 0,  2}, { 1,  2}, { 2,  2}, { 2,  1}, { 2,  0}} },  // 右下角( 1,  1)的对应需要预加载的Region的数目和相对坐标
    },
};


//##ModelId=3DBE3B53008C
KScenePlaceC::KScenePlaceC()
{
	m_hLoadAndPreprocessThread = NULL;
	m_bInited = false;
	m_bLoading = false;
	m_bEnableWeather = true;
	m_bFollowWithMap = false;
	m_FocusPosition.x = m_FocusPosition.y = 0;//SPWP_FARAWAY_COORD;
	m_FocusRegion.x = m_FocusRegion.y = 0;
	m_FocusMoveOffset.cx = m_FocusMoveOffset.cy = 0;
	m_bPreprocessEvent = false;
	m_hLoadRegionEvent = NULL;
	m_hSwitchLoadFinishedEvent = NULL;
	m_nFirstToLoadIndex = -1;
	m_szPlaceRootPath[0] = 0;
	m_szSceneName[0] = 0;
	m_nSceneId = SPWP_NO_SCENE;
	m_RepresentArea.left = m_RepresentArea.right = m_RepresentArea.top = m_RepresentArea.bottom = 0;
	m_RepresentExactHalfSize.cx = m_RepresentExactHalfSize.cy = 0;
	m_MapFocusOffset.x = m_MapFocusOffset.y = 0;

	for (int i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_pRegions[i] = &m_RegionObjs[i];
	memset(&m_pBGImg, 0, sizeof(m_pBGImg));
	memset(&m_pOCImg, 0, sizeof(m_pOCImg));
	memset(&m_pInProcessAreaRegions, 0, sizeof(m_pInProcessAreaRegions));
	memset(&m_RegionGroundImages, 0, sizeof(m_RegionGroundImages));
	m_nNumGroundImagesAvailable = 0;

	m_bRenderGround = false;
	m_pfunRegionLoadedCallback = NULL;
	m_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;
	
	m_pObjsAbove = NULL;
	m_nNumObjsAbove = 0;
	
	m_pWeather = NULL;

	//////////////////////////////////////////////////////////////////////////
	bPaintMode = false;
	bFlagMode = false;
	m_bBGImg = false;
	m_nBGNum = 0;
	memset(BGArea, 0, sizeof(BGArea));
	m_bOCImg = false;
	m_nOCNum = 0;
	OCArea.left = OCArea.top = OCArea.right = OCArea.bottom = 0;
	m_bBCImg = false;
	m_nBCNum = 0;
	BCArea.left = BCArea.top = BCArea.right = BCArea.bottom = 0;
	//////////////////////////////////////////////////////////////////////////
}

//##ModelId=3DD17A770383
KScenePlaceC::~KScenePlaceC()
{
	Terminate();
}

//##ModelId=3DCAAE3703A6
void KScenePlaceC::ClosePlace()
{
	if (m_szPlaceRootPath[0] == 0)
		return;

	m_nSceneId = SPWP_NO_SCENE;
	m_Map.Free();
	m_MapFocusOffset.x = m_MapFocusOffset.y = 0;
	m_bFollowWithMap = false;

	SetLoadingStatus(false);

	ResetEvent(m_hSwitchLoadFinishedEvent);
	ResetEvent(m_hLoadRegionEvent);	//这要放在修改m_FocusRegion之前，要不子线程就可能在两句代码执行之间退出结束了。

	EnterCriticalSection(&m_RegionListAdjustCritical);
	EnterCriticalSection(&m_LoadCritical);
	m_nFirstToLoadIndex = -1;
	for (int i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
		m_pInProcessAreaRegions[i] = NULL;
	LeaveCriticalSection(&m_LoadCritical);
	LeaveCriticalSection(&m_RegionListAdjustCritical);

	EnterCriticalSection(&m_ProcessCritical);
	ClearPreprocess(true);
	for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_RegionObjs[i].Clear();
	LeaveCriticalSection(&m_ProcessCritical);

	m_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;
	m_szPlaceRootPath[0] = 0;
	m_szSceneName[0] = 0;
	m_bPreprocessEvent = false;
	m_bRenderGround = false;

	if(m_pWeather)
	{
		delete m_pWeather;
		m_pWeather = NULL;
	}
}

//##ModelId=3DCAA6A703DB
bool KScenePlaceC::Initialize()
{
	if (m_bInited)
		return true;

	m_hSwitchLoadFinishedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hLoadRegionEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hSwitchLoadFinishedEvent && m_hLoadRegionEvent)
	{
		InitializeCriticalSection(&m_RegionListAdjustCritical);
		InitializeCriticalSection(&m_LoadCritical);
		InitializeCriticalSection(&m_ProcessCritical);

		DWORD	ThreadId;
		m_hLoadAndPreprocessThread = CreateThread(NULL, 0,
			LoadThreadEntrance, this, 0, &ThreadId);

		if (m_hLoadAndPreprocessThread)
		{
			m_bInited = true;
			return true;
		}

		DeleteCriticalSection(&m_ProcessCritical);
		DeleteCriticalSection(&m_LoadCritical);
		DeleteCriticalSection(&m_RegionListAdjustCritical);
	}

	CloseHandle(m_hLoadRegionEvent);
	m_hLoadRegionEvent = NULL;
	CloseHandle(m_hSwitchLoadFinishedEvent);
	m_hSwitchLoadFinishedEvent = NULL;
	return false;
}

void KScenePlaceC::LoadSymbol(int nSubWorldID)
{
	if (nSubWorldID)
		m_Map.LoadSymbol(nSubWorldID);
	else
		m_Map.LoadSymbol(SubWorld[0].m_SubWorldID);
}

//##ModelId=3DCAA64C01DA
bool KScenePlaceC::OpenPlace(int nPlaceIndex)
{
	if (m_bInited == false)
		return false;

	ClosePlace();

	KIniFile	Ini;
	char		Index[16];
	char		Buff[128];

//	g_SetFilePath(ALL_PALCE_ROOT_FOLDER);
	if (Ini.Load(MAPLIST_SETTING_FILE) == FALSE)
		return false;

	itoa(nPlaceIndex, Index, 10);
	if (Ini.GetString("List", Index, "", Buff, sizeof(Buff)) == FALSE)
		return false;

	sprintf(m_szPlaceRootPath, "%s\\%s", ALL_PALCE_ROOT_FOLDER, Buff);

	/*strcat(Index, "_name");
	if (!Ini.GetString("List", Index, "", m_szSceneName, sizeof(m_szSceneName)))
	{
		char* pName = strchr(Buff, '\\');
		if (pName)
		{
			while(strchr(pName, '\\'))
				pName = strchr(pName, '\\') + 1;
			strcpy(m_szSceneName, pName);
		}
		else
			strcpy(m_szSceneName, Buff);
	}

	Ini.Clear();*/

	strcpy(m_szSceneName, g_SubWorldSet.m_sMapListInfo[nPlaceIndex].szName);
	

	// 取得场景的环境光信息
	int nValue = sprintf(m_szPlaceRootPath, "%s\\%s.wor",ALL_PALCE_ROOT_FOLDER, Buff);
	if (Ini.Load(m_szPlaceRootPath) == FALSE)
	{
		m_szPlaceRootPath[0] = 0;
		return false;
	}
	m_szPlaceRootPath[nValue - 4] = 0;
	m_nSceneId = nPlaceIndex;
	CoreDataChanged(GDCNI_SWITCHING_MAPMODE, m_Map.Load(&Ini, m_szPlaceRootPath), 0);
	
	int nIsInDoor;
	Ini.GetInteger("MAIN", "IsInDoor", 0, &nIsInDoor);
	m_ObjectsTree.SetIsIndoor(nIsInDoor != 0);

	RECT	sRect;
	if(nIsInDoor == 0)
	{
		sRect.left = 0xff, sRect.top = 10, sRect.right = 15, sRect.bottom = 35;
		Ini.GetRect("LIGHT", "MidNight", &sRect);
		SetEnvironmentLight(ENVLIGHT_MIDNIGHT, (BYTE)sRect.top, (BYTE)sRect.right, (BYTE)sRect.bottom);
		sRect.left = 0xff, sRect.top = 22, sRect.right = 28, sRect.bottom = 55;
		Ini.GetRect("LIGHT", "Dawn", &sRect);
		SetEnvironmentLight(ENVLIGHT_DAWN, (BYTE)sRect.top, (BYTE)sRect.right, (BYTE)sRect.bottom);
		sRect.left = 0xff, sRect.top = 55, sRect.right = 49, sRect.bottom = 57;
		Ini.GetRect("LIGHT", "Morning", &sRect);
		SetEnvironmentLight(ENVLIGHT_MORNING, (BYTE)sRect.top, (BYTE)sRect.right, (BYTE)sRect.bottom);
		sRect.left = 0xff, sRect.top = 68, sRect.right = 64, sRect.bottom = 60;
		Ini.GetRect("LIGHT", "Forenoon", &sRect);
		SetEnvironmentLight(ENVLIGHT_FORENOON, (BYTE)sRect.top, (BYTE)sRect.right, (BYTE)sRect.bottom);
		sRect.left = 0xff, sRect.top = 93, sRect.right = 88, sRect.bottom = 85;
		Ini.GetRect("LIGHT", "Noon", &sRect);
		SetEnvironmentLight(ENVLIGHT_NOON, (BYTE)sRect.top, (BYTE)sRect.right, (BYTE)sRect.bottom);
		sRect.left = 0xff, sRect.top = 100, sRect.right = 74, sRect.bottom = 35;
		Ini.GetRect("LIGHT", "Dusk", &sRect);
		SetEnvironmentLight(ENVLIGHT_DUSK, (BYTE)sRect.top, (BYTE)sRect.right, (BYTE)sRect.bottom);
		sRect.left = 0xff, sRect.top = 35, sRect.right = 47, sRect.bottom = 60;
		Ini.GetRect("LIGHT", "Evening", &sRect);
		SetEnvironmentLight(ENVLIGHT_EVENING, (BYTE)sRect.top, (BYTE)sRect.right, (BYTE)sRect.bottom);
	}
	else
	{
		sRect.left = 0xff, sRect.top = 0x40, sRect.right = 0x40, sRect.bottom = 0x40;
		Ini.GetRect("LIGHT", "InDoor", &sRect);
		for(int i=0; i<7; i++)
			SetEnvironmentLight(i, (BYTE)sRect.top, (BYTE)sRect.right, (BYTE)sRect.bottom);
	}

	Ini.Clear();

#ifdef SWORDONLINE_SHOW_DBUG_INFO
	g_nMapIndex = nPlaceIndex;
#endif

	m_FocusRegion.x = m_FocusRegion.y = -SPWP_LOAD_EXTEND_RANGE;
	m_FocusMoveOffset.cx = m_FocusMoveOffset.cy = 0;
	int	nImageIndex = 0;
	for (int i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_RegionObjs[i].ToLoad(-m_RangePosTable[i].x, -m_RangePosTable[i].y);

	if (g_pRepresent && m_nNumGroundImagesAvailable == 0 && l_bPrerenderGround)
	{
		for (int i = 0; m_nNumGroundImagesAvailable < SPWP_NUM_REGIONS_IN_PROCESS_AREA && i < 2147483647; i++)
		{
			KRUImage* pImage = &m_RegionGroundImages[m_nNumGroundImagesAvailable];
			pImage->bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
			pImage->nType = ISI_T_BITMAP16;
			sprintf(pImage->szImage, "_*PlaceGround*_#~%d~#_", i);
			pImage->uImage = g_pRepresent->CreateImage(
				pImage->szImage, KScenePlaceRegionC::RWPP_AREGION_WIDTH,
				KScenePlaceRegionC::RWPP_AREGION_HEIGHT / 2, ISI_T_BITMAP16);
			if (pImage->uImage)
				m_nNumGroundImagesAvailable ++;
		}
	}

	SetLoadingStatus(true);
	//测试用
//	ChangeWeather(0);
	return true;
}

//##ModelId=3DBCE7B70358
void KScenePlaceC::SetFocusPosition(int nX, int nY, int nZ)
{
	if (m_bInited == false || m_szPlaceRootPath[0] == 0 ||
		(m_FocusPosition.x == nX &&	m_FocusPosition.y == nY))
		return;

	if (m_bFollowWithMap)
	{
		m_OrigFocusPosition.x = nX;
		m_OrigFocusPosition.y = nY;
		return;
	}

	if(m_pWeather)
		m_pWeather->SetFocusPos(nX, nY);

	//忽略z轴坐标
	m_FocusPosition.x = nX;
	m_FocusPosition.y = nY;

	POINT	pos;
	pos.x = m_FocusPosition.x / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	pos.y = m_FocusPosition.y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	if (g_pRepresent)
		PTAlignSceneViewport(g_pRepresent,nX,nY,
			(m_RepresentExactHalfSize.cx-SPWP_REPRESENT_RECT_WINDAGE_X)*2,
			m_RepresentExactHalfSize.cy-SPWP_REPRESENT_RECT_WINDAGE_T*2);

	m_RepresentArea.right  -= m_RepresentArea.left;
	m_RepresentArea.bottom -= m_RepresentArea.top;
	m_RepresentArea.left = m_FocusPosition.x - m_RepresentExactHalfSize.cx;
	m_RepresentArea.top  = m_FocusPosition.y - m_RepresentExactHalfSize.cy;
	m_RepresentArea.right  += m_RepresentArea.left;
	m_RepresentArea.bottom += m_RepresentArea.top;

	if (pos.x == m_FocusRegion.x && pos.y == m_FocusRegion.y)
	{
		m_Map.SetFocusPosition(m_FocusPosition.x + m_MapFocusOffset.x,
			m_FocusPosition.y + m_MapFocusOffset.y, false);
		return;
	}

    #ifdef _DEBUG
    char szDebugString[128];
    sprintf(
        szDebugString, 
        "Change From %d, %d To %d, %d Region!\n", 
        m_FocusRegion.x, 
        m_FocusRegion.y, 
        pos.x,
        pos.y
    );
    OutputDebugString(szDebugString);
    #endif

    POINT OffsetPos;
    // 如果根据移动后Region为相对位移进行的预读
    OffsetPos.x = pos.x - m_FocusRegion.x + 1;
    OffsetPos.y = pos.y - m_FocusRegion.y + 1; 
    if (
        (OffsetPos.x >= 0) &&
        (OffsetPos.x <  3) &&
        (OffsetPos.y >= 0) &&
        (OffsetPos.y <  3)
    )
    {
        const KPrevLoadPosItem *pcPosOffsetItem = NULL;
        pcPosOffsetItem = &m_PrevLoadPosOffset[OffsetPos.y][OffsetPos.x];

        m_PreLoadPosItem.m_nNum = 0;

        int i = 0;

        EnterCriticalSection(&m_RegionListAdjustCritical);
        for (i = 0; i < (pcPosOffsetItem->m_nNum); i++)
        {
            // 如果根据移动后Region为相对位移进行的预读
            m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].x = 
                pos.x + pcPosOffsetItem->m_Pos[i].x;   
            
            m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].y = 
                pos.y + pcPosOffsetItem->m_Pos[i].y;

            // 如果根据移动前Region为相对位移进行的预读
            //m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].x = 
            //    m_FocusRegion.x + pcPosOffsetItem->m_Pos[i].x;   
            //
            //m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].y = 
            //    m_FocusRegion.y + pcPosOffsetItem->m_Pos[i].y;
                
            m_PreLoadPosItem.m_nNum++;   
        }
        LeaveCriticalSection(&m_RegionListAdjustCritical);
        
        #ifdef _DEBUG
        for (i = 0; i < m_PreLoadPosItem.m_nNum; i++)
        {
            char szDebugString[128];
            sprintf(
                szDebugString, 
                "Preload %d, %d Region!\n", 
                m_PreLoadPosItem.m_Pos[i].x, 
                m_PreLoadPosItem.m_Pos[i].y 
            );
            OutputDebugString(szDebugString);
        }
        #endif

    }
    else
    {
        // 如果是超过预加载的范围，清空，不需要加载
        m_PreLoadPosItem.m_nNum = 0;
    }

	m_Map.SetFocusPosition(m_FocusPosition.x + m_MapFocusOffset.x,
		m_FocusPosition.y + m_MapFocusOffset.y, true);

	m_FocusMoveOffset.cx += pos.x - m_FocusRegion.x;
	m_FocusMoveOffset.cy += pos.y - m_FocusRegion.y;
	m_FocusRegion.x   = pos.x;
	m_FocusRegion.y = pos.y;

	m_ObjectsTree.SetLightenAreaLeftTopPos(
		(m_FocusRegion.x  - 1) * KScenePlaceRegionC::RWPP_AREGION_WIDTH,
		(m_FocusRegion.y  - 1) * KScenePlaceRegionC::RWPP_AREGION_HEIGHT);

	ClearPreprocess(false);
	if (m_FocusMoveOffset.cx >= SPWP_TRIGGER_RANGE  ||
		m_FocusMoveOffset.cx <= -SPWP_TRIGGER_RANGE ||
		m_FocusMoveOffset.cy >= SPWP_TRIGGER_RANGE  ||
		m_FocusMoveOffset.cy <= -SPWP_TRIGGER_RANGE)
	{

		if (m_FocusMoveOffset.cx >= SPWP_TRIGGER_LOADING_RANGE ||
			m_FocusMoveOffset.cx <= -SPWP_TRIGGER_LOADING_RANGE ||
			m_FocusMoveOffset.cy >= SPWP_TRIGGER_LOADING_RANGE  ||
			m_FocusMoveOffset.cy <= -SPWP_TRIGGER_LOADING_RANGE)
		{
			SetLoadingStatus(true);
		}

		ChangeLoadArea();

		m_FocusMoveOffset.cx = 0;
		m_FocusMoveOffset.cy = 0;
	}
	ChangeProcessArea();
	// Even while neighbouring regions load, rebuild the currently loaded
	// area before the next paint instead of exposing a cleared object tree.
	m_bPreprocessEvent = true;
}

void KScenePlaceC::SetRepresentAreaSize(int nWidth, int nHeight)
{
	m_RepresentExactHalfSize.cx = nWidth / 2 + SPWP_REPRESENT_RECT_WINDAGE_X;
	m_RepresentExactHalfSize.cy = nHeight + SPWP_REPRESENT_RECT_WINDAGE_T * 2;
	m_RepresentArea.left=m_FocusPosition.x-m_RepresentExactHalfSize.cx;
	m_RepresentArea.top=m_FocusPosition.y-m_RepresentExactHalfSize.cy;
	m_RepresentArea.right = m_RepresentArea.left + nWidth +
		SPWP_REPRESENT_RECT_WINDAGE_X + SPWP_REPRESENT_RECT_WINDAGE_X;
	m_RepresentArea.bottom = m_RepresentArea.top + 
		(nHeight + SPWP_REPRESENT_RECT_WINDAGE_T + SPWP_REPRESENT_RECT_WINDAGE_B) * 2;
	PTAlignSceneViewport(g_pRepresent,m_FocusPosition.x,m_FocusPosition.y,nWidth,nHeight);
	m_bPreprocessEvent=true;
}

void KScenePlaceC::GetFocusPosition(int& nX, int& nY, int& nZ)
{
	nX = m_FocusPosition.x;
	nY = m_FocusPosition.y;
	nZ = 0;
}

//##ModelId=3DCD58AC00BC
void KScenePlaceC::Terminate()
{
	if (m_bInited == false)
		return ;
	ClosePlace();

	m_Map.Terminate();

	//触发子线程退出执行
	m_FocusRegion.x = SPWP_FARAWAY_COORD;
	SetEvent(m_hLoadRegionEvent);
	//等待子线程关闭
	DWORD	dwExitCode;
	if (GetExitCodeThread(m_hLoadAndPreprocessThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
		WaitForSingleObject(m_hLoadAndPreprocessThread, INFINITE);
	CloseHandle(m_hLoadAndPreprocessThread);
	m_hLoadAndPreprocessThread = NULL;
	
	DeleteCriticalSection(&m_ProcessCritical);
	DeleteCriticalSection(&m_LoadCritical);
	DeleteCriticalSection(&m_RegionListAdjustCritical);

	CloseHandle(m_hSwitchLoadFinishedEvent);
	m_hSwitchLoadFinishedEvent = NULL;
	CloseHandle(m_hLoadRegionEvent);
	m_hLoadRegionEvent = NULL;

	m_bPreprocessEvent = false;

	for (int i = 0; i < m_nNumGroundImagesAvailable; i++)
	{
		if (g_pRepresent)
			g_pRepresent->FreeImage(m_RegionGroundImages[i].szImage);
		m_RegionGroundImages[i].szImage[0] = 0;
		m_RegionGroundImages[i].uImage = 0;
	}
	m_nNumGroundImagesAvailable = 0;
	m_bInited = false;
}

// 预加载地图上的图素
void KScenePlaceC::PreLoadProcess()
{
    if (m_PreLoadPosItem.m_nNum == 0)
        return;

    int i = 0;
    int j = 0;
    m_nPrevLoadFileCount = 0;

    EnterCriticalSection(&m_RegionListAdjustCritical);
    for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
    {
        int nRegionX = 0;
        int nRegionY = 0;
        
        if (!m_pRegions[i])
            continue;

        m_pRegions[i]->GetRegionIndex(nRegionX, nRegionY);

        for (j = 0; j < m_PreLoadPosItem.m_nNum; j++)
        {
            if (
                (m_PreLoadPosItem.m_Pos[j].x == nRegionX) &&
                (m_PreLoadPosItem.m_Pos[j].y == nRegionY)
            )
            {
                break;
            }
        }

        if (j >= m_PreLoadPosItem.m_nNum)
            continue;   // 如果没有找到匹配的项，则跳到下一个

        
        KBuildinObj *pObjsList = NULL;
        unsigned uNumObjs      = 0;

        m_pRegions[i]->GetBIOSBuildinObjs(pObjsList, uNumObjs); 

        for (NULL; uNumObjs > 0; uNumObjs--, pObjsList++)
        {
            memcpy(
                m_PrevLoadFileNameAndFrames[m_nPrevLoadFileCount].szName, 
                pObjsList->szImage, 
                MAX_RESOURCE_FILE_NAME_LEN
            );   
            m_PrevLoadFileNameAndFrames[m_nPrevLoadFileCount].nFrame = pObjsList->nFrame;

            m_nPrevLoadFileCount++;
            if (m_nPrevLoadFileCount >= MAX_PREV_LOAD_FILE_COUNT)
                break;
        }
        if (m_nPrevLoadFileCount >= MAX_PREV_LOAD_FILE_COUNT)
            break;
    }
    m_PreLoadPosItem.m_nNum = 0;
    LeaveCriticalSection(&m_RegionListAdjustCritical);

    if (m_nPrevLoadFileCount == 0)
        return;
    

    //return;

    for (i = 0; i < m_nPrevLoadFileCount; i++)
    {
        KRPosition2 Offset;
        KRPosition2 Size;
        

        g_pRepresent->GetImageFrameParam(
            m_PrevLoadFileNameAndFrames[i].szName,
            m_PrevLoadFileNameAndFrames[i].nFrame,
            &Offset,
            &Size,
            ISI_T_SPR
        );
        
        //KPakFile DataFile;
        //
        //if (DataFile.Open(m_PrevLoadFileNameAndFrames[i].szName))
        //{
        //    if (DataFile.Size() <= PREV_LOAD_FILE_MIN_SIZE)
        //        continue;
        //
        //    char szBuffer[16];
        //
        //    DataFile.Read(szBuffer, 16);
        //
        //    DataFile.Close();
        //}
        
        //char szDebugString[256];
        //sprintf(szDebugString, "%s\n", m_PrevLoadFileNameAndFrames[i].szName);
        //OutputDebugString(szDebugString);
    }

}


//##ModelId=3DCB6BC90345
void KScenePlaceC::LoadProcess()
{
    DWORD dwRetCode = 0;

	while(true)
	{
        dwRetCode = WaitForSingleObject(m_hLoadRegionEvent, 1000);
        if (dwRetCode == WAIT_OBJECT_0)
        {
			if (m_FocusRegion.x == SPWP_FARAWAY_COORD)
				break;
			KScenePlaceRegionC*	pRegion = NULL;
			EnterCriticalSection(&m_RegionListAdjustCritical);
			if (m_nFirstToLoadIndex >= 0)
				pRegion = m_pRegions[m_nFirstToLoadIndex];
			LeaveCriticalSection(&m_RegionListAdjustCritical);

			if (pRegion)
			{
				EnterCriticalSection(&m_LoadCritical);
				pRegion->Load(m_szPlaceRootPath);
				LeaveCriticalSection(&m_LoadCritical);
				ARegionLoaded(pRegion);
			}
		}
        else if (dwRetCode == WAIT_TIMEOUT)
        {
            if (m_nSceneId == SPWP_NO_SCENE)
                continue;

            if (m_nFirstToLoadIndex >= 0)
                continue;   // 说明还在加载过程中

            PreLoadProcess();
        }
	}
}

//##ModelId=3DCCD131018C
DWORD WINAPI KScenePlaceC::LoadThreadEntrance(void* pParam)
{
	if (pParam)
		((KScenePlaceC*)pParam)->LoadProcess();
	return 0;
}

//##ModelId=3DBDBC7200B4
void KScenePlaceC::SetRegionsToLoad()
{
	if (m_FocusMoveOffset.cx == 0 && m_FocusMoveOffset.cy == 0)
		return;

	EnterCriticalSection(&m_RegionListAdjustCritical);

	//可能正在加载中的区域
	KScenePlaceRegionC* pMayLoadingRegion = NULL;
	if (m_nFirstToLoadIndex >= 0)
		pMayLoadingRegion = m_pRegions[m_nFirstToLoadIndex];

	KScenePlaceRegionC* pTempRegions[SPWP_MAX_NUM_REGIONS];
	int nFirst = 0;
	int nLast = SPWP_MAX_NUM_REGIONS - 1;
	int i, nBourn, nX, nY;
	
	if (m_nFirstToLoadIndex < 0)
		nBourn = SPWP_MAX_NUM_REGIONS;
	else
		nBourn	= m_nFirstToLoadIndex;

	for (i = 0; i < nBourn; i++)
	{
		m_pRegions[i]->GetRegionIndex(nX, nY);
		if (INSIDE_AREA(nX, nY, SPWP_LOAD_EXTEND_RANGE))
			pTempRegions[nFirst++] = m_pRegions[i];
		else
			pTempRegions[nLast--] = m_pRegions[i];
	}

	//设置新的下个起始加载区域在区域指针列表中的索引
	m_nFirstToLoadIndex = (nFirst < SPWP_MAX_NUM_REGIONS) ? nFirst : -1;

	for (i = nBourn; i < SPWP_MAX_NUM_REGIONS; i++)
	{
		m_pRegions[i]->GetRegionIndex(nX, nY);
		if (INSIDE_AREA(nX, nY, SPWP_LOAD_EXTEND_RANGE))
			pTempRegions[nFirst++] = m_pRegions[i];
		else
			pTempRegions[nLast--] = m_pRegions[i];
	}

	int nNewLoadIdx[SPWP_MAX_NUM_REGIONS];
	int nCount = 0;

	if (m_FocusMoveOffset.cx * m_FocusMoveOffset.cx > SPWP_LOAD_EXTEND_RANGE * SPWP_LOAD_EXTEND_RANGE * 4 ||
		m_FocusMoveOffset.cy * m_FocusMoveOffset.cy > SPWP_LOAD_EXTEND_RANGE * SPWP_LOAD_EXTEND_RANGE * 4 )
	{
		memcpy(nNewLoadIdx, m_PRIIdxTable, sizeof(m_PRIIdxTable));
		nCount = SPWP_MAX_NUM_REGIONS;
	}
	else
	{
		int nBeginX, nEndX, nBeginY, nEndY;
		if (m_FocusMoveOffset.cx > 0)
		{
			nBeginX = SPWP_LOAD_EXTEND_RANGE * 2 + 1 - m_FocusMoveOffset.cx;
			nEndX = SPWP_LOAD_EXTEND_RANGE * 2;
		}
		else
		{
			nBeginX = 0;
			nEndX = -m_FocusMoveOffset.cx - 1;
		}
		
		if (m_FocusMoveOffset.cy > 0)
		{
			nBeginY = SPWP_LOAD_EXTEND_RANGE * 2 + 1 - m_FocusMoveOffset.cy;
			nEndY = SPWP_LOAD_EXTEND_RANGE * 2;
		}
		else
		{
			nBeginY = 0;
			nEndY = -m_FocusMoveOffset.cy - 1;
		}

		for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		{
			if (
				(m_RangePosTable[m_PRIIdxTable[i]].x >= nBeginX 
				&& m_RangePosTable[m_PRIIdxTable[i]].x <= nEndX)
				||
				(m_RangePosTable[m_PRIIdxTable[i]].y >= nBeginY 
				&& m_RangePosTable[m_PRIIdxTable[i]].y <= nEndY)
				)
			{
				nNewLoadIdx[nCount++] = m_PRIIdxTable[i];
			}
		}
	}

	_ASSERT((nFirst + nCount == SPWP_MAX_NUM_REGIONS));
	for (i = 0; i < nCount; i++)
	{
		if (pMayLoadingRegion == pTempRegions[nFirst + i])
		{
			//确保没有区域对象正在执行加载，如果有则等待这个区域对象加载结束
			EnterCriticalSection(&m_LoadCritical);
			LeaveCriticalSection(&m_LoadCritical);
			//运行到此处，此时可能为“一个区域刚加载完毕，但是还未及更新区域
			//指针列表的下个加载区域索引。”的情况
			//下面的处理把它当作还未加载处理。
		}

		pTempRegions[nFirst + i]->ToLoad(m_FocusRegion.x + m_RangePosTable[nNewLoadIdx[i]].x - SPWP_LOAD_EXTEND_RANGE,
			m_FocusRegion.y + m_RangePosTable[nNewLoadIdx[i]].y - SPWP_LOAD_EXTEND_RANGE);
	}
	memcpy(m_pRegions, pTempRegions, sizeof(m_pRegions));
	LeaveCriticalSection(&m_RegionListAdjustCritical);

	if (nCount)
		SetEvent(m_hLoadRegionEvent);
}

//##ModelId=3DCAA6B90196
unsigned int KScenePlaceC::AddObject(unsigned int uGenre, int nId, int x, int y, int z,
									 int eLayerParam)
{
	POINT	ri;
	KIpotRuntimeObj* pLeaf = NULL;

	ri.x = x / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	if (eLayerParam && INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_RANGE / 2))
	{
		pLeaf = (KIpotRuntimeObj*)malloc(sizeof(KIpotRuntimeObj));
		if (pLeaf)
		{
			pLeaf->eLeafType = pLeaf->IPOTL_T_RUNTIME_OBJ;
			pLeaf->uGenre = uGenre;
			pLeaf->nId = nId;
			pLeaf->oPosition.x = x;
			pLeaf->oPosition.y = y + POINT_LEAF_Y_ADJUST_VALUE;
			pLeaf->nPositionZ = z;
			pLeaf->pAheadBrother = NULL;
			pLeaf->pBrother = NULL;
			pLeaf->pLChild = NULL;
			pLeaf->pRChild = NULL;
			pLeaf->pParentLeaf = NULL;
			pLeaf->pParentBranch = NULL;
			pLeaf->eLayerParam = eLayerParam;

			EnterCriticalSection(&m_ProcessCritical);
			m_ObjectsTree.AddLeafPoint(pLeaf);
			LeaveCriticalSection(&m_ProcessCritical);
		}
	}

	return ((unsigned int)pLeaf);
}

//##ModelId=3DCAA7000085
#include "PhongThanSceneMembership.h"
unsigned int KScenePlaceC::MoveObject(unsigned int uGenre, int nId,  int x, int y, int z,
									  unsigned int& uRtoid, int eLayerParam)
{
	if (uRtoid == 0)
	{
		uRtoid = AddObject(uGenre, nId, x, y, z, eLayerParam);
		return uRtoid;
	}

	KScenePlaceRegionC* pRegion = NULL;
	KIpotRuntimeObj* pLeaf = (KIpotRuntimeObj*)uRtoid;
	POINT	ri;
	ri.x = x / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

	if (eLayerParam && INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_RANGE / 2))
	{
		EnterCriticalSection(&m_ProcessCritical);
		pLeaf->uGenre = uGenre;
		pLeaf->nId = nId;
		pLeaf->nPositionZ = z;
		pLeaf->eLayerParam = eLayerParam;
		if (PhongThanSceneNeedsAttach(pLeaf, x, y + POINT_LEAF_Y_ADJUST_VALUE))
		{
			m_ObjectsTree.PluckRto(pLeaf);
			pLeaf->oPosition.x = x;
			pLeaf->oPosition.y = y + POINT_LEAF_Y_ADJUST_VALUE;
			m_ObjectsTree.AddLeafPoint(pLeaf);
		}
		LeaveCriticalSection(&m_ProcessCritical);
	}
	else
	{
		EnterCriticalSection(&m_ProcessCritical);
		m_ObjectsTree.PluckRto(pLeaf);
		LeaveCriticalSection(&m_ProcessCritical);
		free(pLeaf);
		pLeaf = NULL;
		uRtoid = 0;
	}
	return uRtoid;
}

//##ModelId=3DCAA70603E3
void KScenePlaceC::RemoveObject(unsigned int uGenre, int nId, unsigned int& uRtoid)
{
	if (uRtoid)
	{
		EnterCriticalSection(&m_ProcessCritical);
		m_ObjectsTree.PluckRto((KIpotRuntimeObj*)uRtoid);
		LeaveCriticalSection(&m_ProcessCritical);
		free ((KIpotRuntimeObj*)uRtoid);
		uRtoid = 0;
	}
}

void KScenePlaceC::Breathe()
{
	DWORD dwLight;
	dwLight = m_EnLight.GetEnvironmentLight(m_nCurrentTime);
	if(m_pWeather)
		m_pWeather->FilterAmbient(dwLight);
	SetAmbient(dwLight);

	if (m_bLoading)
	{
		WaitForSingleObject(m_hSwitchLoadFinishedEvent, SPWP_SWITCH_SCENE_TIMEOUT);
		m_bRenderGround = true;
	}

	#include "PhongThanScenePreparePaint.inl"
	m_ObjectsTree.Breathe();

	if(m_pWeather)
	{
		if(m_pWeather->IsShutDown())
		{
			// 天气生命期已经结束，释放之
			delete m_pWeather;
			m_pWeather = NULL;
		}
		else
			m_pWeather->Breath();
	}
}

//##ModelId=3DCD7F0A0071
void KScenePlaceC::Paint()
{
	IR_UpdateTime();
	if (m_bInited == false || m_szPlaceRootPath[0] == 0)
		return;

	// Camera interpolation can cross a region after Breathe(). Rebuild the
	// scene before drawing, so no frame sees the tree cleared by SetFocus.
	#include "PhongThanScenePreparePaint.inl"

	if (m_bRenderGround)
	{
		m_bRenderGround = false;
		PrerenderGround(false);
	}

	EnterCriticalSection(&m_ProcessCritical);
	
	bool bPrerenderGroundImg = PaintBackGround();
	PaintBackGroundCloud();
	unsigned int i;
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
			m_pInProcessAreaRegions[i]->PaintGround(bPrerenderGroundImg);
	}

	m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_COVER_GROUND);
	m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_OBJECT);

	for (i = 0; i < m_nNumObjsAbove; i++)
	{
		KScenePlaceRegionC::PaintAboveHeadObj(m_pObjsAbove[i], &m_RepresentArea);
	}

	m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_INFRONTOF_ALL);

	// 绘制天气对象
	if(m_pWeather)
		m_pWeather->Render(g_pRepresent);

	//PaintOverCloud();
	LeaveCriticalSection(&m_ProcessCritical);
	//====显示游戏（场景）信息====

	/*for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
		{
			m_pInProcessAreaRegions[i]->PaintObstacle();
		}
	}*/

#ifdef SWORDONLINE_SHOW_DBUG_INFO
	if (g_bShowObstacle)
	{
		for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
		{
			if (m_pInProcessAreaRegions[i])
			{
				m_pInProcessAreaRegions[i]->PaintObstacle();
			}
		}
	}

	if (g_bShowGameInfo)
	{
		KRUShadow	Shadow;
		Shadow.oPosition.nX = 0;
		Shadow.oPosition.nY = 360;
		Shadow.oEndPos.nX = 268;
		Shadow.oEndPos.nY = 440;
		Shadow.Color.Color_dw = 0x16000000;

		g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
		char	szInfo[120];
		KOutputTextParam	Param;
		Param.BorderColor = 0;
		Param.nX = Shadow.oPosition.nX + 2;
		Param.nY = Shadow.oPosition.nY + 2;
		Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;
		Param.Color = 0xffffffff;
		Param.nNumLine = 1;
		Param.nSkipLine = 0;
		int nLen = sprintf(szInfo, "Map<color=Green>[%d]<color=White>:%s", g_nMapIndex, m_szSceneName);
		nLen = TEncodeText(szInfo, nLen);
		g_pRepresent->OutputRichText(12, &Param, szInfo, nLen);
		nLen = sprintf(szInfo, "Focus:<color=Green>%d,%d<color=White>-%d,%d",
			m_FocusRegion.x, m_FocusRegion.y, m_FocusPosition.x, m_FocusPosition.y);
		nLen = TEncodeText(szInfo, nLen);
		Param.nY += 12;
		g_pRepresent->OutputRichText(12, &Param, szInfo, nLen);
	}
#endif
}

void KScenePlaceC::PaintNpcNameOverlay()
{
	if (m_bInited == false || m_szPlaceRootPath[0] == 0 || g_pRepresent == NULL)
		return;

	const int nViewWidth = m_RepresentArea.right - m_RepresentArea.left -
		SPWP_REPRESENT_RECT_WINDAGE_X * 2;
	const int nViewHeight = (m_RepresentArea.bottom - m_RepresentArea.top) / 2 -
		SPWP_REPRESENT_RECT_WINDAGE_T - SPWP_REPRESENT_RECT_WINDAGE_B;
	PaintPhongThanNpcNameOverlay(
		&m_RepresentArea, m_FocusPosition.x, m_FocusPosition.y,
		nViewWidth, nViewHeight);
}

//##ModelId=3DCE68BB0238
void KScenePlaceC::ChangeLoadArea()
{
	SetRegionsToLoad();
}

//##ModelId=3DBF946D0053
void KScenePlaceC::ChangeProcessArea()
{
	KRUImage*	pImage = NULL;
	RECT	rc;
	rc.left = (m_FocusRegion.x - SPWP_PROCESS_RANGE / 2) * KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	rc.top  = (m_FocusRegion.y - SPWP_PROCESS_RANGE / 2) *  KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	rc.right  = rc.left + SPWP_PROCESS_RANGE * KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	rc.bottom = rc.top  + SPWP_PROCESS_RANGE * KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

	EnterCriticalSection(&m_ProcessCritical);
	int h, v;
	for (int i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
		{
			m_pInProcessAreaRegions[i]->GetRegionIndex(h, v);
			if (INSIDE_AREA(h, v, SPWP_PROCESS_RANGE / 2) == 0)
				m_pInProcessAreaRegions[i]->LeaveProcessArea();
			m_pInProcessAreaRegions[i] = NULL;
		}
	}
	EnterCriticalSection(&m_RegionListAdjustCritical);
	int nNum;
	if (m_nFirstToLoadIndex < 0)
		nNum = SPWP_MAX_NUM_REGIONS;
	else
		nNum = m_nFirstToLoadIndex;
	for (i = 0; i < nNum; i++)
	{
		m_pRegions[i]->GetRegionIndex(h, v);
		if (INSIDE_AREA(h, v, SPWP_PROCESS_RANGE / 2))
		{
			GET_IN_PROCESS_AREA_REGION(h, v) = m_pRegions[i];
			pImage = m_pRegions[i]->GetPrerenderGroundImage();
			if (pImage == NULL && l_bPrerenderGround)
			{
				pImage = GetFreeGroundImage();
//				_ASSERT(pImage);
			}
			m_pRegions[i]->EnterProcessArea(pImage);
		}
	}

	LeaveCriticalSection(&m_RegionListAdjustCritical);
	LeaveCriticalSection(&m_ProcessCritical);

	if (m_nFirstToLoadIndex < 0 || m_nFirstToLoadIndex >= SPWP_PROCESS_PRERENDER_REGION_COUNTER_TRIGGER)
	{
		m_bPreprocessEvent = true;
		m_bRenderGround = true;
	}
}

void KScenePlaceC::PrerenderGround(bool bForce)
{
	EnterCriticalSection(&m_RegionListAdjustCritical);
	for (int i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
			m_pInProcessAreaRegions[i]->PrerenderGround(bForce);
	}
	LeaveCriticalSection(&m_RegionListAdjustCritical);
}

//##ModelId=3DBFA1460230
void KScenePlaceC::Preprocess()
{
	unsigned int i, j, dx, dy, nTotalLineObj = 0;

	struct
	{
		KIpotBuildinObj*	pObjsPoint;
		KIpotBuildinObj*	pObjsLine;
		KIpotBuildinObj*	pObjsTree;
		KBuildinObj*		pObjsAbove;
		unsigned int		nNumObjsPoint;
		unsigned int		nNumObjsLine;
		unsigned int		nNumObjsTree;
		unsigned int		nNumObjsAbove;
	}RegionRtoData[SPWP_NUM_REGIONS_IN_PROCESS_AREA] = { 0 };

	EnterCriticalSection(&m_ProcessCritical);

	ClearPreprocess(false);

	m_ObjectsTree.SetPermanentBranchPos(
		m_FocusPosition.x - KScenePlaceRegionC::RWPP_AREGION_WIDTH * 2,
		m_FocusPosition.x + KScenePlaceRegionC::RWPP_AREGION_WIDTH * 2,
		m_FocusPosition.y - KScenePlaceRegionC::RWPP_AREGION_HEIGHT * 2);

	//--------获取内建对象的列表----------
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
		{
			m_pInProcessAreaRegions[i]->GetBuildinObjs(
				RegionRtoData[i].pObjsPoint, RegionRtoData[i].nNumObjsPoint,
				RegionRtoData[i].pObjsLine, RegionRtoData[i].nNumObjsLine,
				RegionRtoData[i].pObjsTree, RegionRtoData[i].nNumObjsTree);
			nTotalLineObj += RegionRtoData[i].nNumObjsLine;

			RegionRtoData[i].nNumObjsAbove = m_pInProcessAreaRegions[i]->
				GetAboveHeadLayer(RegionRtoData[i].pObjsAbove);
			m_nNumObjsAbove += RegionRtoData[i].nNumObjsAbove;
		}
	}

	//--------处理高空对象---------
	if (m_nNumObjsAbove)
	{
		m_pObjsAbove = (KBuildinObj**)malloc(sizeof(KBuildinObj*) * m_nNumObjsAbove);
		m_nNumObjsAbove = 0;
		if (m_pObjsAbove)
		{
			for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
			{
				while(RegionRtoData[i].nNumObjsAbove)
				{
					RegionRtoData[i].nNumObjsAbove--;
					KBuildinObj* pBio = &RegionRtoData[i].pObjsAbove[RegionRtoData[i].nNumObjsAbove];
					for (j = 0; j < m_nNumObjsAbove; j++)
					{
						KBuildinObj* pBio2 = m_pObjsAbove[j];
						if ((pBio->oPos1.y < pBio2->oPos1.y) ||
							(pBio->oPos1.y == pBio2->oPos1.y &&
								(pBio->oPos1.z < pBio2->oPos1.z)))
						{
							break;
						}
					}
					for (unsigned int k = m_nNumObjsAbove; k > j; k--)
						m_pObjsAbove[k] = m_pObjsAbove[k - 1];
					m_pObjsAbove[j] = &RegionRtoData[i].pObjsAbove[RegionRtoData[i].nNumObjsAbove];
					m_nNumObjsAbove ++;
				}
			}
		}			
	}

	//--------处理树方式排序的对象---------
	class TreeObjSet : public KNode
	{
	public:
		float	fAngleXY;
		float	fNodicalY;
		int		nLength2;
		POINT	oLP1, oLP2;
		KIpotBuildinObj*	pObjs;
	};

	KList		List;
	TreeObjSet	*pNode1 = NULL, *pNode2 = NULL;
	KIpotBuildinObj* pObj = NULL;

	//---把同在一条直线上的连在一起--
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		for (j = 0; j < RegionRtoData[i].nNumObjsTree; j++)
		{
			pObj = &RegionRtoData[i].pObjsTree[j];
			
			pNode1 = (TreeObjSet*)List.GetHead();
			while (pNode1)
			{
				if (SM_IsLineLinkable(pObj->pBio->fAngleXY,
					pObj->pBio->fNodicalY,
					pNode1->fAngleXY, pNode1->fNodicalY))
				{
					break;
				}
				pNode1 = (TreeObjSet*)pNode1->GetNext();
			};

			if (pNode1 == NULL)
			{
				if ((pNode1 = new TreeObjSet) == NULL)
				{
					i = SPWP_NUM_REGIONS_IN_PROCESS_AREA;
					break;
				}
				pNode1->pObjs = pObj;
				pNode1->oLP1 = pObj->oPosition;
				pNode1->oLP2 = pObj->oEndPos;
				pNode1->fAngleXY  = pObj->fAngleXY  = pObj->pBio->fAngleXY;
				pNode1->fNodicalY = pObj->fNodicalY = pObj->pBio->fNodicalY;
				List.AddHead(pNode1);
				continue;
			}
			
			pObj->fAngleXY  = pNode1->fAngleXY;
			pObj->fNodicalY = pNode1->fNodicalY;

			int x = pObj->oPosition.x;

			if (x <  pNode1->pObjs->oPosition.x)
			{
				pObj->pBrother = pNode1->pObjs;
				pNode1->pObjs = pObj;
			}
			else
			{
				KIpotBuildinObj* pSortStop = pNode1->pObjs;
				while(pSortStop->pBrother)
				{
					if (x < ((KIpotBuildinObj*)pSortStop->pBrother)->oPosition.x)
						break;
					pSortStop = (KIpotBuildinObj*)pSortStop->pBrother;
				};
				pObj->pBrother = pSortStop->pBrother;
				pSortStop->pBrother = pObj;
			}
			if (pNode1->oLP1.x > pObj->oPosition.x)
				pNode1->oLP1 = pObj->oPosition;
			if (pNode1->oLP2.x < pObj->oEndPos.x)
				pNode1->oLP2 = pObj->oEndPos;
		}
	}
	
	//----把线条组合按长度排序----
	pNode1 = (TreeObjSet*)List.GetHead();
	while(pNode1)
	{
		dx = pNode1->oLP1.x - pNode1->oLP2.x;
		dy = pNode1->oLP1.y - pNode1->oLP2.y;
		pNode1->nLength2 = dx * dx + dy * dy;
		pNode1 = (TreeObjSet*)pNode1->GetNext();
	};

	KList		List2;
	while(pNode1 = (TreeObjSet*)List.GetHead())
	{
		pNode2 = (TreeObjSet*)pNode1->GetNext();
		while(pNode2)
		{
			if (pNode1->nLength2 < pNode2->nLength2)
				pNode1 = pNode2;
			pNode2 = (TreeObjSet*)pNode2->GetNext();
		};
		pNode1->Remove();
		List2.AddTail(pNode1);
	};
	
	//----把按树方式排序的对象加入对象树-----
	while(pNode1 = (TreeObjSet*)List2.RemoveHead())
	{
		while(pObj = pNode1->pObjs)
		{
			pNode1->pObjs = (KIpotBuildinObj*)pObj->pBrother;
			pObj->pBrother = NULL;
			m_ObjectsTree.AddBranch(pObj);
		}
		delete pNode1;	
	};

	//----把线方式排序的对象进行根据线长度排序-----
	if (nTotalLineObj)
	{
		struct LineObjItem//---处理线方式排序的对象-----
		{
			int		nLength2;
			KIpotBuildinObj*	pObj;
		};
		LineObjItem* pNodeList = (LineObjItem*)malloc(sizeof(LineObjItem) * nTotalLineObj);
		nTotalLineObj = 0;
		if (pNodeList)
		{
			for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
			{
				for (j = 0; j < RegionRtoData[i].nNumObjsLine; j++)
				{
					KIpotBuildinObj* pObj = &RegionRtoData[i].pObjsLine[j];
					dx = pObj->oEndPos.x - pObj->oPosition.x;
					dy = pObj->oEndPos.y - pObj->oPosition.y;
					int nLength2 = dx * dx + dy * dy;

					for (unsigned int k = 0; k < nTotalLineObj; k++)
					{
						if (nLength2 > pNodeList[k].nLength2)
						{
							for (unsigned int j = nTotalLineObj; j > k; j--)
								pNodeList[j] = pNodeList[j - 1];
							pNodeList[k].pObj = pObj;
							pNodeList[k].nLength2 = nLength2;
							break;
						}
					}
					if (k == nTotalLineObj)
					{
						pNodeList[nTotalLineObj].pObj = pObj;
						pNodeList[nTotalLineObj].nLength2 = nLength2;
					}
					nTotalLineObj++;
				}
			}
			//----把线方式排序的对象加入对象树-----
			for (i = 0; i < nTotalLineObj; i++)
			{
				m_ObjectsTree.AddLeafLine(pNodeList[i].pObj);
			}
			free(pNodeList);
			pNodeList = NULL;
		}
	}

	//----把点方式排序的对象加入对象树-----
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		for (j = 0; j < RegionRtoData[i].nNumObjsPoint; j++)
		{
			m_ObjectsTree.AddLeafPoint(&RegionRtoData[i].pObjsPoint[j]);
		}
	}

	
	//----把场景内建光源加入树-----
	KBuildInLightInfo* pLights = NULL;
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{

		if (m_pInProcessAreaRegions[i])
		{
			j = m_pInProcessAreaRegions[i]->GetBuildinLights(pLights);
			if (j)
				m_ObjectsTree.AddBuildinLight(pLights, j);			
		}
	}

	RECT	KeepArea;
	KeepArea.left = (m_FocusRegion.x - SPWP_PROCESS_RANGE / 2) * KScenePlaceRegionC::RWPP_AREGION_WIDTH - SPWP_RTO_HALF_RANGE;
	KeepArea.right = KeepArea.left + KScenePlaceRegionC::RWPP_AREGION_WIDTH * SPWP_PROCESS_RANGE + SPWP_RTO_HALF_RANGE * 2;
	KeepArea.top = (m_FocusRegion.y - SPWP_PROCESS_RANGE / 2) * KScenePlaceRegionC::RWPP_AREGION_HEIGHT - SPWP_RTO_HALF_RANGE;
	KeepArea.bottom = KeepArea.top + KScenePlaceRegionC::RWPP_AREGION_HEIGHT * SPWP_PROCESS_RANGE + SPWP_RTO_HALF_RANGE * 2;
	m_ObjectsTree.StrewRtoLeafs(KeepArea);

	LeaveCriticalSection(&m_ProcessCritical);
}

//##ModelId=3DCCBD7B0239
void KScenePlaceC::ClearPreprocess(int bIncludeRto)
{
	if (m_pObjsAbove)
	{
		free(m_pObjsAbove);
		m_pObjsAbove = NULL;
	}
	m_nNumObjsAbove = 0;

	EnterCriticalSection(&m_ProcessCritical);
	if (bIncludeRto == false)
		m_ObjectsTree.Fell();
	else
		m_ObjectsTree.Clear();
	LeaveCriticalSection(&m_ProcessCritical);
}

void KScenePlaceC::ProjectDistToSpaceDist(int& nXDistance, int& nYDistance)
{
	nYDistance = nYDistance + nYDistance;
}

void KScenePlaceC::ViewPortCoordToSpaceCoord(int& nX, int& nY, int nZ)
{
	if (g_pRepresent)
		g_pRepresent->ViewPortCoordToSpaceCoord(nX, nY, nZ);
	else
	{
		nX = nX + m_RepresentArea.left + SPWP_REPRESENT_RECT_WINDAGE_X;
		nY = (nY + ((nZ * 887) >> 10)) * 2 + m_RepresentArea.top  + SPWP_REPRESENT_RECT_WINDAGE_T * 2;
	}
}

void KScenePlaceC::ViewPortCoordToSpaceCoordNew(int& nW, int& nX, int& nY, int nZ)
{
	nW = m_nSceneId;
	if (g_pRepresent)
		g_pRepresent->ViewPortCoordToSpaceCoord(nX, nY, nZ);
	else
	{
		nX = nX + m_RepresentArea.left + SPWP_REPRESENT_RECT_WINDAGE_X;
		nY = (nY + ((nZ * 887) >> 10)) * 2 + m_RepresentArea.top  + SPWP_REPRESENT_RECT_WINDAGE_T * 2;
	}
}

void KScenePlaceC::GetRegionLeftTopPos(int nRegionX, int nRegionY, int& nLeft, int& nTop)
{
	nLeft = nRegionX * KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	nTop  = nRegionY * KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
}

void KScenePlaceC::ARegionLoaded(KScenePlaceRegionC* pRegion)
{
	_ASSERT(pRegion);
	//如果刚加载完毕的区域属于预处理范围则触发预处理信号
	int	h, v;

	EnterCriticalSection(&m_RegionListAdjustCritical);
	int nCount;
	if (m_nFirstToLoadIndex >= 0)		
	{
		nCount = m_nFirstToLoadIndex;
		if (m_pRegions[m_nFirstToLoadIndex] == pRegion)
		{
			m_nFirstToLoadIndex++;
			if (m_nFirstToLoadIndex == SPWP_MAX_NUM_REGIONS)
				m_nFirstToLoadIndex = -1;
		}
	}
	else
	{
		nCount = SPWP_MAX_NUM_REGIONS - 1;
	}
	for (h = 0; h < nCount; h++)
	{
		pRegion->SetNestRegion(m_pRegions[h]);
		m_pRegions[h]->SetNestRegion(pRegion);
	}
	LeaveCriticalSection(&m_RegionListAdjustCritical);
	if (m_nFirstToLoadIndex < 0)
		ResetEvent(m_hLoadRegionEvent);

	if (nCount >= SPWP_PROCESS_PRERENDER_REGION_COUNTER_TRIGGER)
	{
		SetLoadingStatus(false);
		m_bRenderGround = true;
	}

	KRUImage* pImage = NULL;
	pRegion->GetRegionIndex(h, v);
	if (INSIDE_AREA(h, v, SPWP_PROCESS_RANGE / 2))
	{
		EnterCriticalSection(&m_ProcessCritical);
		if (l_bPrerenderGround)
			pImage = GetFreeGroundImage();
//		_ASSERT(pImage);
		pRegion->EnterProcessArea(pImage);
		GET_IN_PROCESS_AREA_REGION(h, v) = pRegion;

		if (nCount >= 8)
			m_bPreprocessEvent = true;
		LeaveCriticalSection(&m_ProcessCritical);
	}

	//处理highlight的special object
	if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT &&
		h == m_nHLSpecialObjectRegionX && v == m_nHLSpecialObjectRegionY)
	{
		EnterCriticalSection(&m_ProcessCritical);
		pRegion->SetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
		LeaveCriticalSection(&m_ProcessCritical);
	}

	if (m_pfunRegionLoadedCallback)
		m_pfunRegionLoadedCallback(h, v);
}

KRUImage* KScenePlaceC::GetFreeGroundImage()
{
	for (int i = 0; i < m_nNumGroundImagesAvailable; i++)
	{
		if (m_RegionGroundImages[i].GROUND_IMG_OCCUPY_FLAG == false)
			return (&m_RegionGroundImages[i]);
	}
	return NULL;
}

long KScenePlaceC::GetObstacleInfo(int nX, int nY)
{
	POINT	ri;
	ri.x = nX / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = nY / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	
	if (INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_RANGE / 2))
	{
		KScenePlaceRegionC* pRegion = GET_IN_PROCESS_AREA_REGION(ri.x, ri.y);
		if (pRegion)
			return pRegion->GetObstacleInfo(nX, nY);
	}
	return Obstacle_Normal;
}

long KScenePlaceC::GetObstacleInfoMin(int nX, int nY, int nOffX, int nOffY)
{
	POINT	ri;
	ri.x = nX / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = nY / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	
	if (INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_RANGE / 2))
	{
		KScenePlaceRegionC* pRegion = GET_IN_PROCESS_AREA_REGION(ri.x, ri.y);
		if (pRegion)
			return pRegion->GetObstacleInfoMin(nX, nY, nOffX, nOffY);
	}
	return Obstacle_Normal;
}

void KScenePlaceC::RepresentShellReset()
{
	PTAlignSceneViewport(g_pRepresent,m_FocusPosition.x,m_FocusPosition.y,
		(m_RepresentExactHalfSize.cx-SPWP_REPRESENT_RECT_WINDAGE_X)*2,
		m_RepresentExactHalfSize.cy-SPWP_REPRESENT_RECT_WINDAGE_T*2);
	m_bPreprocessEvent=true;
	m_bRenderGround = true;
	for (int i = 0; i < m_nNumGroundImagesAvailable; i++)
		m_RegionGroundImages[i].GROUND_IMG_OK_FLAG = false;
}

//设置场景中一个区域被加载完毕后的回调函数
void  KScenePlaceC::SetRegionLoadedCallback(funScenePlaceRegionLoadedCallback pfunCallback)
{
	m_pfunRegionLoadedCallback = pfunCallback;
}

void  KScenePlaceC::SetHightLightSpecialObject(int nRegionX, int nRegionY, int nBioIndex)
{
	KScenePlaceRegionC* pRegion = NULL;
	EnterCriticalSection(&m_RegionListAdjustCritical);
	{
		if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT)
		{
			pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
			if (pRegion)
			{
				EnterCriticalSection(&m_ProcessCritical);
				pRegion->UnsetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
				LeaveCriticalSection(&m_ProcessCritical);
			}
		}

		m_nHLSpecialObjectRegionX = nRegionX;
		m_nHLSpecialObjectRegionY = nRegionY;
		m_nHLSpecialObjectBioIndex = nBioIndex;

		if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT)
		{
			pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
			if (pRegion)
			{
				EnterCriticalSection(&m_ProcessCritical);
				pRegion->SetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
				LeaveCriticalSection(&m_ProcessCritical);
			}
		}
	}
	LeaveCriticalSection(&m_RegionListAdjustCritical);
}

void  KScenePlaceC::UnsetHightLightSpecialObject()
{
	if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT)
	{
		EnterCriticalSection(&m_RegionListAdjustCritical);
		KScenePlaceRegionC* pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
		if (pRegion)
		{
			EnterCriticalSection(&m_ProcessCritical);
			pRegion->UnsetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
			LeaveCriticalSection(&m_ProcessCritical);
		}
		LeaveCriticalSection(&m_RegionListAdjustCritical);
		m_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;
	}
}

KScenePlaceRegionC*	KScenePlaceC::GetLoadedRegion(int h, int v)
{
	EnterCriticalSection(&m_RegionListAdjustCritical);
	int nCount;
	KScenePlaceRegionC* pRegion = NULL;
	if (m_nFirstToLoadIndex < 0)
		nCount = SPWP_MAX_NUM_REGIONS;
	else
		nCount = m_nFirstToLoadIndex;
	for (int i = 0; i < nCount; i++)
	{
		int rh, rv;
		m_pRegions[i]->GetRegionIndex(rh, rv);
		if (rh == h && rv == v)
		{
			pRegion = m_pRegions[i];
			break;
		}
	}
	LeaveCriticalSection(&m_RegionListAdjustCritical);
	return pRegion;
}

void KScenePlaceC::GetSceneNameAndFocus(char* pszName, int& nId, int& nX, int& nY)
{
	_ASSERT(pszName);
	strcpy(pszName, m_szSceneName);
	nId = m_nSceneId;
	nX = m_FocusPosition.x;
	nY = m_FocusPosition.y;
}

void KScenePlaceC::GetSceneNameAndFocusOften(char* pszName, int& nId, int& nX, int& nY)
{
	_ASSERT(pszName);
	strcpy(pszName, m_szSceneName);
	nId = m_nSceneId;
	nX = m_Map.m_DirectPos.x / 16;
	nY = m_Map.m_DirectPos.y / 16;
}

void KScenePlaceC::SetAmbient(DWORD dwAmbient)
{
	m_ObjectsTree.SetAmbient(dwAmbient);
}

void KScenePlaceC::SetCurrentTime(DWORD dwCurrentTime)
{
	m_nCurrentTime = (dwCurrentTime / 100) % 1440;
	m_ObjectsTree.SetCurrentTime(m_nCurrentTime);
}

void KScenePlaceC::SetEnvironmentLight(int idx, BYTE r, BYTE g, BYTE b)
{
	m_EnLight.SetLight(r, g, b, idx);
}

void KScenePlaceC::EnableBioLights(bool bEnable)
{
	m_ObjectsTree.EnableBioLights(bEnable);
}

void KScenePlaceC::EnableDynamicLights(bool bEnable)
{
	m_ObjectsTree.EnableDynamicLights(bEnable);
}

void KScenePlaceC::ChangeWeather(int nWeatherID)
{
	if (m_bEnableWeather == false)
		return;
	if(m_pWeather)
	{
		if(nWeatherID == WEATHERID_NOTHING)
			m_pWeather->ShutDown();
		else
			return;
	}
	else
	{
		if(nWeatherID == WEATHERID_NOTHING)
			return;
		else
		{
			m_pWeather = new KWeatherRain;
			if(!m_pWeather->ReadInfoFromIniFile(nWeatherID))
			{
				delete m_pWeather;
				m_pWeather = NULL;
			}
		}
	}
}

void KScenePlaceC::EnableWeather(int nbEnable)
{
	if (!m_bEnableWeather != !nbEnable)
	{
		if (nbEnable == false)
			ChangeWeather(WEATHERID_NOTHING);
		m_bEnableWeather = (nbEnable != 0);
	}
}

void KScenePlaceC::SetLoadingStatus(bool bLoading)
{
	if ((!m_bLoading) != (!bLoading))
	{
		m_bLoading = bLoading;
		CoreDataChanged(GDCNI_SWITCHING_SCENEPLACE, 0, m_bLoading);
		if (m_bLoading == false)
			SetEvent(m_hSwitchLoadFinishedEvent);
	}
}

void KScenePlaceC::PaintMap(int nX, int nY)
{
	m_Map.PaintMap(nX, nY);
}

void KScenePlaceC::SetMapParam(unsigned int uShowElems, int nSize)
{
	m_Map.SetShowElemsFlag(uShowElems);
	if (uShowElems)
		m_Map.SetSize((nSize & 0xffff), (nSize >> 16));
}

//设置场景的地图的焦点(单位:场景坐标)
void KScenePlaceC::SetMapFocusPositionOffset(int nOffsetX, int nOffsetY)
{
	if (m_bFollowWithMap == false)
	{
		m_MapFocusOffset.x = nOffsetX;
		m_MapFocusOffset.y = nOffsetY;
		m_Map.SetFocusPosition(m_FocusPosition.x + m_MapFocusOffset.x,
			m_FocusPosition.y + m_MapFocusOffset.y, true);
	}
	else
	{
		m_bFollowWithMap = false;
		SetFocusPosition(m_OrigFocusPosition.x + nOffsetX, m_OrigFocusPosition.y + nOffsetY , 0);
		m_bFollowWithMap = true;
	}
}

//获取场景的小地图信息
int KScenePlaceC::GetMapInfo(KSceneMapInfo* pInfo)
{
	RECT	MapRc;
	int nRet = m_Map.GetMapRect(&MapRc);
	if (pInfo)
	{
		int nHalfShowWidth  = m_RepresentExactHalfSize.cx * 2; //- SPWP_REPRESENT_RECT_WINDAGE_X
		int nHalfShowHeight = m_RepresentExactHalfSize.cy * 2; //- SPWP_REPRESENT_RECT_WINDAGE_T
		pInfo->nFocusMinH = MapRc.left + nHalfShowWidth;
		pInfo->nFocusMinV = MapRc.top  + nHalfShowHeight;
		pInfo->nFocusMaxH = MapRc.right  - nHalfShowWidth;
		pInfo->nFocusMaxV = MapRc.bottom - nHalfShowHeight;
		if (pInfo->nFocusMaxH < pInfo->nFocusMinH)
			pInfo->nFocusMaxH = pInfo->nFocusMinH;
		if (pInfo->nFocusMaxV < pInfo->nFocusMinV)
			pInfo->nFocusMaxV = pInfo->nFocusMinV;

		if (m_bFollowWithMap == false)
		{
			pInfo->nOrigFocusH = m_FocusPosition.x;
			pInfo->nOrigFocusV = m_FocusPosition.y;
			pInfo->nFocusOffsetH = m_MapFocusOffset.x;
			pInfo->nFocusOffsetV = m_MapFocusOffset.y;
		}
		else
		{
			pInfo->nOrigFocusH = m_OrigFocusPosition.x;
			pInfo->nOrigFocusV = m_OrigFocusPosition.y;
			pInfo->nFocusOffsetH = m_FocusPosition.x - m_OrigFocusPosition.x;
			pInfo->nFocusOffsetV = m_FocusPosition.y - m_OrigFocusPosition.y;
		}
		pInfo->nScallH = KScenePlaceMapC::MAP_SCALE_H;
		pInfo->nScallV = KScenePlaceMapC::MAP_SCALE_V;
	}
	return nRet;
}

//设置是否跟随地图的移动而移动
void  KScenePlaceC::FollowMapMove(int nbEnable)
{
	if ((!m_bFollowWithMap) != (!nbEnable))
	{
		if (m_bFollowWithMap = (nbEnable != 0))
		{
			m_OrigFocusPosition.x = m_FocusPosition.x;
			m_OrigFocusPosition.y = m_FocusPosition.y;
			int	x = m_MapFocusOffset.x;
			int y = m_MapFocusOffset.y;
			m_MapFocusOffset.x = 0;
			m_MapFocusOffset.y = 0;
			SetMapFocusPositionOffset(x, y);
		}
		else
		{
			m_MapFocusOffset.x = m_FocusPosition.x - m_OrigFocusPosition.x;
			m_MapFocusOffset.y = m_FocusPosition.y - m_OrigFocusPosition.y;
			SetFocusPosition(m_OrigFocusPosition.x, m_OrigFocusPosition.y, 0);
		}
	}
}

BOOL KScenePlaceC::OnDirectMap(int nX, int nY)
{
	return m_Map.OnDirectMap(nX, nY);
}

void KScenePlaceC::DoDirectMap(int nX, int nY)
{
	m_Map.DoDirectMap(nX, nY);
}

void KScenePlaceC::DirectFindPos(int nX, int nY, BOOL bSync, BOOL bPaintLine)
{ 
	m_Map.DirectFindPos(nX, nY, bSync, bPaintLine);
}

//TamLTM check hover mouse
void KScenePlaceC::CheckHoverMouseMiniMap(BOOL hoverM)
{
	m_Map.CheckHoverMoveMouse(hoverM);
}
//end code

void KScenePlaceC::LoadIni(KIniFile *pIni)
{
	m_nBGNum = 0;
	m_nOCNum = 0;
	m_nBCNum = 0;
	memset(m_pBGImg, 0, sizeof(m_pBGImg));
	m_bBGImg = false;
	memset(&m_pOCImg, 0, sizeof(m_pOCImg));
	m_bOCImg = false;
	memset(&m_pBCImg, 0, sizeof(m_pBCImg));
	m_bBCImg = false;

	if (pIni)
	{
		int nCount = 0;
		pIni->GetInteger("Main", "Count", 0, &nCount);
		
		if (nCount)
		{
			int i, j;
			char szSection[32], szKeyName[32];
			for (i = 0; i < nCount; i ++)
			{
				int nValue = 0;
				sprintf(szSection, "%d", i);
				pIni->GetInteger(szSection, "Type", 0, &nValue);
				if (nValue)
				{
					switch (nValue)
					{
					case BACKGROUND_IMAGE:
						if (m_nBGNum < MAX_BACKGROUND_IMAGE)
						{
							pIni->GetInteger(szSection, "AreaLeft", 0, &nValue);
							BGArea[m_nBGNum].left = nValue;
							pIni->GetInteger(szSection, "AreaTop", 0, &nValue);
							BGArea[m_nBGNum].top = nValue;
							pIni->GetInteger(szSection, "AreaRight", 0, &nValue);
							BGArea[m_nBGNum].right = nValue;
							pIni->GetInteger(szSection, "AreaBottom", 0, &nValue);
							BGArea[m_nBGNum].bottom = nValue;
							m_pBGImg[m_nBGNum].nType = ISI_T_BITMAP16;
							m_pBGImg[m_nBGNum].Color.Color_b.a = 255;
							m_pBGImg[m_nBGNum].bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
							m_pBGImg[m_nBGNum].uImage = 0;
							m_pBGImg[m_nBGNum].nISPosition = IMAGE_IS_POSITION_INIT;
							m_pBGImg[m_nBGNum].bRenderFlag = 0;
							pIni->GetString(szSection, "Image", "", m_pBGImg[m_nBGNum].szImage, sizeof(m_pBGImg[m_nBGNum].szImage));
							pIni->GetInteger(szSection, "PicCenterPointX", 0, &m_pBGImg[m_nBGNum].oPosition.nX);
							pIni->GetInteger(szSection, "PicCenterPointY", 0, &m_pBGImg[m_nBGNum].oPosition.nY);
							m_pBGImg[m_nBGNum].oPosition.nZ = 0;
							m_pBGImg[m_nBGNum].nFrame = 0;
							m_nBGNum++;
						}
						break;
					case OVER_CLOUD_IMAGE:
						for (j = 0; j < MAX_CLOUD_IMAGE; j ++)
						{
							pIni->GetInteger(szSection, "AreaLeft", 0, &nValue);
							OCArea.left = nValue;
							pIni->GetInteger(szSection, "AreaTop", 0, &nValue);
							OCArea.top = nValue;
							pIni->GetInteger(szSection, "AreaRight", 0, &nValue);
							OCArea.right = nValue;
							pIni->GetInteger(szSection, "AreaBottom", 0, &nValue);
							OCArea.bottom = nValue;
							m_pOCImg[m_nOCNum].nType = ISI_T_SPR;
							m_pOCImg[m_nOCNum].Color.Color_b.a = 255;
							m_pOCImg[m_nOCNum].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
							m_pOCImg[m_nOCNum].uImage = 0;
							m_pOCImg[m_nOCNum].nISPosition = IMAGE_IS_POSITION_INIT;
							m_pOCImg[m_nOCNum].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
							sprintf(szKeyName, "Image%d", m_nOCNum);
							pIni->GetString(szSection, szKeyName, "", m_pOCImg[m_nOCNum].szImage, sizeof(m_pOCImg[m_nOCNum].szImage));
							m_pOCImg[m_nOCNum].oPosition.nX = ::GetRandomNumber(0, RESOLUTION_WIDTH);
							m_pOCImg[m_nOCNum].oPosition.nY = ::GetRandomNumber(0, RESOLUTION_HEIGHT);
							m_pOCImg[m_nOCNum].oPosition.nZ = 0;
							m_pOCImg[m_nOCNum].nFrame = 0;
							m_nOCNum++;
						}
						break;
					case BACKGROUND_CLOUD_IMAGE:
						for (j = 0; j < MAX_CLOUD_IMAGE; j ++)
						{
							pIni->GetInteger(szSection, "AreaLeft", 0, &nValue);
							BCArea.left = nValue;
							pIni->GetInteger(szSection, "AreaTop", 0, &nValue);
							BCArea.top = nValue;
							pIni->GetInteger(szSection, "AreaRight", 0, &nValue);
							BCArea.right = nValue;
							pIni->GetInteger(szSection, "AreaBottom", 0, &nValue);
							BCArea.bottom = nValue;
							m_pBCImg[m_nBCNum].nType = ISI_T_SPR;
							m_pBCImg[m_nBCNum].Color.Color_b.a = 255;
							m_pBCImg[m_nBCNum].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
							m_pBCImg[m_nBCNum].uImage = 0;
							m_pBCImg[m_nBCNum].nISPosition = IMAGE_IS_POSITION_INIT;
							m_pBCImg[m_nBCNum].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
							sprintf(szKeyName, "Image%d", m_nBCNum);
							pIni->GetString(szSection, szKeyName, "", m_pBCImg[m_nBCNum].szImage, sizeof(m_pBCImg[m_nBCNum].szImage));
							m_pBCImg[m_nBCNum].oPosition.nX = ::GetRandomNumber(0, RESOLUTION_WIDTH);
							m_pBCImg[m_nBCNum].oPosition.nY = ::GetRandomNumber(0, RESOLUTION_HEIGHT);
							m_pBCImg[m_nBCNum].oPosition.nZ = 0;
							m_pBCImg[m_nBCNum].nFrame = 0;
							m_nBCNum++;
						}
						break;
					}
				}
			}
			m_bBGImg = m_nBGNum > 0;
			m_bOCImg = m_nOCNum > 0;
			m_bBCImg = m_nBCNum > 0;					
		}
	}
}

BOOL KScenePlaceC::PaintBackGround()
{
	BOOL bRet = TRUE;
	if (m_bBGImg)
	{
		for (int i = 0; i < m_nBGNum; i++)
		{
			if ((m_FocusRegion.x > BGArea[i].left || BGArea[i].left == 0) && 
			(m_FocusRegion.y < BGArea[i].bottom || BGArea[i].bottom == 0) && 
			(m_FocusRegion.x < BGArea[i].right || BGArea[i].right == 0) && 
			(m_FocusRegion.y > BGArea[i].top || BGArea[i].top == 0))
			{
				g_pRepresent->DrawPrimitives(1, &m_pBGImg[i], RU_T_IMAGE, true);
				bRet = FALSE;
			}
		}
	}
	return bRet;
}

void KScenePlaceC::PaintOverCloud()
{
	if (m_bOCImg && 
		(m_FocusRegion.x > OCArea.left || OCArea.left == 0) && 
		(m_FocusRegion.y < OCArea.bottom || OCArea.bottom == 0) && 
		(m_FocusRegion.x < OCArea.right || OCArea.right == 0) && 
		(m_FocusRegion.y > OCArea.top || OCArea.top == 0))
	{
		for (int i = 0; i < m_nOCNum; i++)
		{
			KImageParam Param;
			if (g_pRepresent->GetImageParam(m_pOCImg[i].szImage, &Param, ISI_T_SPR) == true)
			{
				m_pOCImg[i].oPosition.nX--;		
				if (m_pOCImg[i].oPosition.nX < -Param.nWidth)
					m_pOCImg[i].oPosition.nX = RESOLUTION_WIDTH;
				if (m_pOCImg[i].oPosition.nX > RESOLUTION_WIDTH)
					m_pOCImg[i].oPosition.nX = -Param.nWidth;
			}
			g_pRepresent->DrawPrimitives(1, &m_pOCImg[i], RU_T_IMAGE, true);
		}
	}
}

void KScenePlaceC::PaintBackGroundCloud()
{
	if (m_bBCImg && 
		(m_FocusRegion.x > BCArea.left || BCArea.left == 0) && 
		(m_FocusRegion.y < BCArea.bottom || BCArea.bottom == 0) && 
		(m_FocusRegion.x < BCArea.right || BCArea.right == 0) && 
		(m_FocusRegion.y > BCArea.top || BCArea.top == 0))
	{
		for (int i = 0; i < m_nBCNum; i++)
		{
			KImageParam Param;
			if (g_pRepresent->GetImageParam(m_pBCImg[i].szImage, &Param, ISI_T_SPR) == true)
			{
				m_pBCImg[i].oPosition.nX--;
				if (m_pBCImg[i].oPosition.nX < -Param.nWidth)
					m_pBCImg[i].oPosition.nX = RESOLUTION_WIDTH;
				if (m_pBCImg[i].oPosition.nX > RESOLUTION_WIDTH)
					m_pBCImg[i].oPosition.nX = -Param.nWidth;
			}
			g_pRepresent->DrawPrimitives(1, &m_pBCImg[i], RU_T_IMAGE, true);
		}
	}
}

void KScenePlaceC::SetRouteTargetMps(int x,int y){m_Map.SetRouteTargetMps(x,y);}
