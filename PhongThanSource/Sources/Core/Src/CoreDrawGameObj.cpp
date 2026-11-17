#include "KCore.h"
#include "PhongThanVisibility.h"
#include "../../../Headers/PhongThanEscortRider.h"
#include "KNpc.h"
#include "KMissle.h"
#include "KSkillList.h"
#include "KItem.h"
#include "KBuySell.h"
#include "KPlayer.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "KSubWorldSet.h"
#include "scene/KScenePlaceC.h"
#include "ImgRef.h"
#include "GameDataDef.h"
#include "KObjSet.h"
#include "KObj.h"
#include "../../../Headers/PhongThanMiningCrater.h"
#ifndef _SERVER
#include "KOption.h"
#endif

#define  PHYSICSSKILLICON "\\spr\\Ui\\����ͼ��\\icon_sk_ty_ap.spr"

#define SHOW_SPACE_HEIGHT	5
#define	ITEM_CELL_WIDTH		26
#define	ITEM_CELL_HEIGHT	26

void	CoreDrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam)
{
//	static int nSpeed = 0;
	switch(uObjGenre)
	{
	case CGOG_NPC:
        if (uId > 0)
        {
            if(PhongThanClientVisualHidden(uId))break;
            if(PhongThanEscortHiddenEntity(uId))break;
            if(PhongThanIsEscortRider(uId) && (nParam & IPOT_RL_INFRONTOF_ALL)==IPOT_RL_INFRONTOF_ALL)break;
			// NPC/monster visibility is a property of the object, not of a hard
			// coded map-id range.  Region_S content exists outside 1001..1118 too.
			BOOL bForcePhongThanNpcInfo =
				(Npc[uId].m_Kind == kind_normal || Npc[uId].m_Kind == kind_dialoger);
			BOOL bForcePhongThanPlayerInfo = (Npc[uId].m_Kind == kind_player);
			// Phong Than VNG ships 14/16/18 pixel fonts. Font id 12 belongs
			// to the inherited VLTK UI and can make world labels disappear.
			const int nWorldInfoFontSize =
				(bForcePhongThanNpcInfo || bForcePhongThanPlayerInfo) ? 14 : NORMAL_FONTSIZE;
			if(!bForcePhongThanNpcInfo && !bForcePhongThanPlayerInfo &&
				Npc[uId].m_CurrentCamp == camp_audience)
				break;
			if ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)
			{
				// Names/life use the final scene overlay; retain chat and damage here.
				if (bForcePhongThanPlayerInfo)
				{
					const int nPate = Npc[uId].GetNpcPate();
					Npc[uId].PaintBlood(nPate / 2);
					Npc[uId].PaintChat(nPate);
					break;
				}
				if (bForcePhongThanNpcInfo)
				{
				}
				int nHeight = Npc[uId].GetNpcPate();
				
				Npc[uId].PaintBlood(nHeight / 2);

				int nnHeight = nHeight;
				nHeight = Npc[uId].PaintChat(nnHeight);	//������Ϣ��������
				if (nHeight == nnHeight)	//û��������Ϣʱ����������Ϣ
				{
					if (!bForcePhongThanNpcInfo && !bForcePhongThanPlayerInfo && NpcSet.CheckShowLife())
					{	
						if(Npc[uId].m_Kind == kind_normal ||
						   Npc[uId].m_Kind == kind_player ||
						   Npc[uId].m_Kind == kind_partner)
						{	
							nHeight = Npc[uId].PaintLife(nnHeight, true);
						}
					}
					else if (!bForcePhongThanNpcInfo && !bForcePhongThanPlayerInfo)
					{
						if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc() && 
							Player[CLIENT_PLAYER_INDEX].GetTargetNpc() == uId)
						{
							if(Npc[uId].m_Kind == kind_normal)
								nHeight = Npc[uId].PaintLife(nnHeight, true);
						}
					}

					if (NpcSet.CheckShowName() || bForcePhongThanNpcInfo || bForcePhongThanPlayerInfo)
					{
						if (nnHeight != nHeight)	//��������ʾʱ
						{
							nHeight += SHOW_SPACE_HEIGHT;//�ÿ�
						}
							if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc() && Player[CLIENT_PLAYER_INDEX].GetTargetNpc() == uId)
								nHeight = Npc[uId].PaintInfo(nHeight, nWorldInfoFontSize + 2, 0xff000000);	//��ѡ�е������Ŵ���ʾ
							else
								nHeight = Npc[uId].PaintInfo(nHeight, nWorldInfoFontSize, 0xff000000);

						Npc[uId].PaintTop(nHeight, nnHeight, nWorldInfoFontSize, 0xff000000);

					//	g_DebugLog("TamLTM debug id cua target nhan vat uId %d - %d x %d", uId, x, y);
					}
					else
					{
						if (nnHeight != nHeight)	//��������ʾʱ
						{
							nHeight += SHOW_SPACE_HEIGHT;//�ÿ�
						}
						if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc() && Player[CLIENT_PLAYER_INDEX].GetTargetNpc() == uId)
							nHeight = Npc[uId].PaintInfo(nHeight, nWorldInfoFontSize, 0xff000000);	//��ѡ�е������Ŵ���ʾ

						Npc[uId].PaintTop(nnHeight, nnHeight, nWorldInfoFontSize, 0xff000000);
					}

				//	g_DebugLog("TamLTM debug id cua target nhan vat uId %d", uId);
				}
				else
				{
					Npc[uId].PaintTop(nnHeight, nnHeight, 12, 0xff000000);
					if (!bForcePhongThanNpcInfo && !bForcePhongThanPlayerInfo && NpcSet.CheckShowLife())
					{	
						if(Npc[uId].m_Kind == kind_normal ||
						   Npc[uId].m_Kind == kind_player ||
						   Npc[uId].m_Kind == kind_partner)
						{	
							nHeight = Npc[uId].PaintLife(nnHeight, true);
						}
					}
					else if (!bForcePhongThanNpcInfo && !bForcePhongThanPlayerInfo)
					{
						if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc() && 
							Player[CLIENT_PLAYER_INDEX].GetTargetNpc() == uId)
						{
							if(Npc[uId].m_Kind == kind_normal)
								nHeight = Npc[uId].PaintLife(nnHeight, true);
						}
					}
				}
			}
			else if ((nParam & IPOT_RL_OBJECT) == IPOT_RL_OBJECT)
			{
				Npc[uId].Paint();
				if ((int)uId == Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
				{
				   if (Npc[uId].m_HideState.nTime <= 0 && 
					   Npc[uId].m_CurrentCamp != camp_audience)
				   {
						if (Npc[uId].m_Kind == kind_normal && Option.GetLow(LowNpc))
						{
						}
						else
							Npc[uId].DrawBorder();
				   }
				}
			}			
		}
		break;
	case CGOG_MISSLE:
			if (uId > 0)
			{
				Missle[uId].Paint();
			}
		break;
	case CGOG_PLAYERSELLITEM:
	case CGOG_PLAYERVIEWITEM:
	case CGOG_IME_ITEM:
	case CGOG_ITEM:
		if (uId == 0)
			break;
		if (uId >= MAX_ITEM)
			break;
		if (nParam == 2 && Width > 0 && Height > 0)
		{
			Item[uId].PaintInventorySlot(x, y, Width, Height);
			break;
		}
		if (nParam == 1 && Width > 0 && Height > 0)
		{
			Item[uId].PaintEquipmentSlot(x, y, Width, Height);
			break;
		}
		if (Width == 0 && Height == 0)
		{
			Width = Item[uId].GetWidth() * ITEM_CELL_WIDTH;
			Height = Item[uId].GetHeight() * ITEM_CELL_HEIGHT;
			x -= Width / 2;
			y -= Height / 2;
		}
		else
		{
			x += (Width - Item[uId].GetWidth() * ITEM_CELL_WIDTH) / 2;
			y += (Height - Item[uId].GetHeight() * ITEM_CELL_HEIGHT) / 2;
		}

        // Only the treasure map pulses, using synchronized task coordinates.
        if(uObjGenre==CGOG_IME_ITEM&&Item[uId].GetGenre()==item_magicscript&&Item[uId].GetDetailType()==110){
            KPlayer& player=Player[CLIENT_PLAYER_INDEX];int actor=player.m_nIndex;
            if(actor>0&&actor<MAX_NPC&&player.m_cTask.GetSaveVal(382)>0){
                int world=Npc[actor].m_SubWorldIndex,map=player.m_cTask.GetSaveVal(381);if(map>0&&map<100)map+=1000;
                if(world>=0&&world<MAX_SUBWORLD&&SubWorld[world].m_SubWorldID==map){
                    int px,py;Npc[actor].GetMpsPos(&px,&py);px=px/32-player.m_cTask.GetSaveVal(382);py=py/32-player.m_cTask.GetSaveVal(383);
                    __int64 distance=(__int64)px*px+(__int64)py*py;
                    if(distance<=2500){KRUShadow glow;memset(&glow,0,sizeof(glow));
                        glow.Color.Color_dw=(GetTickCount()&256)?0xb0ffe040:0x40a09020;
                        glow.oPosition.nX=x-2;glow.oPosition.nY=y-2;glow.oEndPos.nX=x+ITEM_CELL_WIDTH+2;glow.oEndPos.nY=y+ITEM_CELL_HEIGHT+2;
                        g_pRepresent->DrawPrimitives(1,&glow,RU_T_SHADOW,true);
                    }
                }
            }
        }
		if (uObjGenre == CGOG_IME_ITEM)
			Item[uId].Paint(x, y, false, false);
		else
			Item[uId].Paint(x, y);
	
		break;
	case CGOG_NPC_BLUR_DETAIL(0):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(0);
		break;
	case CGOG_NPC_BLUR_DETAIL(1):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(1);
		break;
	case CGOG_NPC_BLUR_DETAIL(2):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(2);
		break;
	case CGOG_NPC_BLUR_DETAIL(3):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(3);
		break;
	case CGOG_NPC_BLUR_DETAIL(4):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(4);
		break;
	case CGOG_NPC_BLUR_DETAIL(5):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(5);
		break;
	case CGOG_NPC_BLUR_DETAIL(6):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(6);
		break;
	case CGOG_SKILL:
	case CGOG_SKILL_FIGHT:
	case CGOG_SKILL_SHORTCUT:
		{
		int nSkillId = (int)uId;
		if (nSkillId > 0)
		{
			int i = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(nSkillId);
			ISkill *pISkill = g_SkillManager.GetSkill(nSkillId, 1);
			if (pISkill)
            {
                if (uObjGenre == CGOG_SKILL_FIGHT &&
                    Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nSkillId) <= 0)
                {
                    // Render the original SPR palette dimly. Do not draw a
                    // generic shadow primitive over the icon's RLE pixels.
                    KRUImage icon;memset(&icon,0,sizeof(icon));
                    icon.nType=ISI_T_SPR;icon.nISPosition=IMAGE_IS_POSITION_INIT;
                    icon.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;
                    icon.Color.Color_dw=0xff606060;
                    strcpy(icon.szImage,((KSkill*)pISkill)->m_szSkillIcon);
                    icon.oPosition.nX=x;icon.oPosition.nY=y;
                    g_pRepresent->DrawPrimitives(1,&icon,RU_T_IMAGE,true);
                }
                else pISkill->DrawSkillIcon(x,y,Width,Height);
            }
			if(i)
			{
			int nDelay = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.m_Skills[i].NextCastTime -
			SubWorld[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex].m_dwCurrentTime;
			int nTotal = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.m_Skills[i].TotalTime;
			if(nDelay > 0 && nTotal > 0)
			{
			KRUShadow	Shadow;
			Shadow.Color.Color_dw = 0x16000000;
			Shadow.oPosition.nX = x;
			Shadow.oPosition.nY = y;
			Shadow.oEndPos.nX = x + Width;
			Shadow.oEndPos.nY = y + Height;
			g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
			Shadow.Color.Color_dw = 0x16ffff00;
			Shadow.oPosition.nY = y + (nTotal - nDelay)*Height/nTotal;
			g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
			}}
		}
		else
		{
			if(uId == -1) 
			{
				KRUImage RUIconImage;
				RUIconImage.nType = ISI_T_SPR;
				RUIconImage.Color.Color_b.a = 255;
				RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				RUIconImage.uImage = 0;
				RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
				RUIconImage.bRenderFlag = 0;
				strcpy(RUIconImage.szImage, PHYSICSSKILLICON);
				RUIconImage.oPosition.nX = x;
				RUIconImage.oPosition.nY = y;
				RUIconImage.oPosition.nZ = 0;
				RUIconImage.nFrame = 0;
				g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, 1);
			}
		}
		}
		break;
	case CGOG_OBJECT:
		if (uId)
		{
			if ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)
			{
				if(PhongThanIsMiningCrater(Object[uId].m_nDataID))Object[uId].Draw();
                else if (NpcSet.CheckShowObj() || (int)uId == Player[CLIENT_PLAYER_INDEX].GetTargetObj())
					Object[uId].DrawInfo();
			}
			else
			{
				Object[uId].Draw();
				if ((int)uId == Player[CLIENT_PLAYER_INDEX].GetTargetObj())
					Object[uId].DrawBorder();
			}
		}
		break;
	case CGOG_NPCSELLITEM:
		if (uId < 0)
			break;
		BuySell.PaintItem(uId, x, y);
		break;
	default:
		break;
	}
}

void	CoreGetGameObjLightInfo(unsigned int uObjGenre, unsigned int uId, KLightInfo *pLightInfo)
{
	switch (uObjGenre)
	{
	case CGOG_NPC:
		if (uId > 0 && uId < MAX_NPC && uId == Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			Npc[uId].GetMpsPos(&pLightInfo->oPosition.nX, &pLightInfo->oPosition.nY);
			pLightInfo->oPosition.nZ = Npc[uId].m_Height;
			pLightInfo->dwColor = 0;
			if (Npc[uId].m_RedLum > 255)
			{
				Npc[uId].m_RedLum = 255;
			}
			if (Npc[uId].m_GreenLum > 255)
			{
				Npc[uId].m_GreenLum = 255;
			}
			if (Npc[uId].m_BlueLum > 255)
			{
				Npc[uId].m_BlueLum = 255;
			}
			pLightInfo->dwColor = 0xff000000 | Npc[uId].m_RedLum << 16 | Npc[uId].m_GreenLum << 8 | Npc[uId].m_BlueLum;
			pLightInfo->nRadius = Npc[uId].m_CurrentVisionRadius;
		}
		break;
	case CGOG_OBJECT:
        if(uId>0&&uId<MAX_OBJECT&&PhongThanIsMiningCrater(Object[uId].m_nDataID)){
            Object[uId].GetMpsPos(&pLightInfo->oPosition.nX,&pLightInfo->oPosition.nY);pLightInfo->oPosition.nZ=0;
            pLightInfo->nRadius=Object[uId].m_nState==OBJ_PROP_STATE_DISPLAY?Object[uId].m_sObjLight.m_nRadius:0;
            pLightInfo->dwColor=0xff000000|(Object[uId].m_sObjLight.m_nRed<<16)|(Object[uId].m_sObjLight.m_nGreen<<8)|Object[uId].m_sObjLight.m_nBlue;
        }
        break;
	case CGOG_MISSLE:
		if (uId > 0 && uId < MAX_MISSLE)
		{
			if (Missle[uId].m_nMissleId > 0)
			{
				Missle[uId].GetLightInfo(pLightInfo);
			}
		}
		break;
	default:
		break;
	}
}
