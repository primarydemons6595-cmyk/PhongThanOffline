//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KViewItem.cpp
// Date:	2003.07.28
// Code:	边城浪子
// Desc:	KViewItem Class
//---------------------------------------------------------------------------

#include	"KCore.h"

#ifndef _SERVER
#include	"CoreShell.h"
#include	"KItemSet.h"
#include	"KItem.h"
#include	"KViewItem.h"
#include "PhongThanViewedItem.inl"

KViewItem	g_cViewItem;

KViewItem::KViewItem()
{
	Init();
}

void	KViewItem::Init()
{
	m_dwNpcID	= 0;
	m_nLevel	= 1;
	m_szName[0]	= 0;
	memset(this->m_sItem, 0, sizeof(this->m_sItem));
    ZeroMemory(&m_Profile,sizeof(m_Profile));
    ZeroMemory(&m_Owner,sizeof(m_Owner));m_OwnerMale=1;
}

void	KViewItem::ApplyViewEquip(DWORD dwNpcID)
{
	VIEW_EQUIP_COMMAND	sView;
	sView.ProtocolType = c2s_viewequip;
	sView.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sView, sizeof(sView));
}

void	KViewItem::DeleteAll()
{
    ZeroMemory(&m_Profile,sizeof(m_Profile));
    ZeroMemory(&m_Owner,sizeof(m_Owner));m_OwnerMale=1;
	m_dwNpcID	= 0;
	m_nLevel	= 1;
	m_szName[0]	= 0;

	for (int i = 0; i < itempart_num; i ++)
	{
		if (m_sItem[i].nIdx > 0)
			ItemSet.Remove(m_sItem[i].nIdx);
		m_sItem[i].nIdx = 0;
	}
}

void	KViewItem::GetData(BYTE* pMsg)
{
	if (!pMsg)
		return;

	VIEW_EQUIP_SYNC* incoming=(VIEW_EQUIP_SYNC*)pMsg;
    if(!PhongThanViewedOwnerValid(*incoming))return;
    for(int slot=0;slot<itempart_num;++slot)
        if(incoming->m_sInfo[slot].m_nID && !PhongThanViewedItemValid(incoming->m_sInfo[slot]))return;
	DeleteAll();

	int					i,nNpcIdx, nItemIdx;
	VIEW_EQUIP_SYNC		*pView = (VIEW_EQUIP_SYNC*)pMsg;
	KUiPlayerItem	sPlayer;ZeroMemory(&sPlayer,sizeof(sPlayer));

	m_dwNpcID = pView->m_dwNpcID;
    m_Owner=pView->m_Owner;m_OwnerMale=pView->m_OwnerMale;m_nLevel=pView->m_OwnerLevel;lstrcpyn(m_szName,m_Owner.Name,sizeof(m_szName));
    if(PtStatusValidProfile(pView->m_Profile))m_Profile=pView->m_Profile;
	sPlayer.uId = m_dwNpcID;
	nNpcIdx = NpcSet.SearchID(m_dwNpcID);
	if (nNpcIdx > 0)
	{
		m_nLevel = Npc[nNpcIdx].m_Level;
		strcpy(m_szName, Npc[nNpcIdx].Name);
		sPlayer.nIndex = nNpcIdx;
		strcpy(sPlayer.Name, m_szName);
		sPlayer.nData = Npc[nNpcIdx].GetMenuState();
		sPlayer.nTeamID = pView->m_Avatar;//nay ben client nhan,sPlayer thi moi' khai bao
	}
	else
	{
		sPlayer.nIndex = 0;
		lstrcpyn(sPlayer.Name,m_Owner.Name,sizeof(sPlayer.Name));
		sPlayer.nData = 0;
	}

    sPlayer.nTeamID=pView->m_Avatar;
	for (i = 0; i < itempart_num; i ++)
	{
		if (pView->m_sInfo[i].m_nID == 0)
			continue;
		if (pView->m_sInfo[i].m_WireVersion != PHONGTHAN_VIEW_ITEM_VERSION)
			continue;

		nItemIdx = ItemSet.Add(
			pView->m_sInfo[i].m_btGenre,
			pView->m_sInfo[i].m_btSeries,
			pView->m_sInfo[i].m_btLevel,
			pView->m_sInfo[i].m_btLuck,
			pView->m_sInfo[i].m_btDetail,
			pView->m_sInfo[i].m_nParticularType,
			pView->m_sInfo[i].m_btMagicLevel,
			pView->m_sInfo[i].m_wVersion,
			pView->m_sInfo[i].m_dwRandomSeed,
			pView->m_sInfo[i].m_nTemplateRow);

		if (nItemIdx <= 0)
			continue;
		Item[nItemIdx].SetID(pView->m_sInfo[i].m_nID);
		Item[nItemIdx].SetRow(pView->m_sInfo[i].m_nTemplateRow);
        if(!PhongThanReadViewedItem(Item[nItemIdx],pView->m_sInfo[i])){
            ItemSet.Remove(nItemIdx);continue;
        }
		m_sItem[i].nIdx = nItemIdx;
	}

	// 通知界面
	CoreDataChanged(GDCNI_VIEW_PLAYERITEM, (DWORD)&sPlayer, 0);
}





#endif













