#include "PhongThanStarterBag.h"
void KPlayer::DoScriptAction(KPhongThanScriptAction* action)
{
	if (!action) return;
#ifdef _SERVER
	if (!action->ServerOwned || m_nIndex <= 0 || m_nIndex >= MAX_NPC) return;
	const int world = Npc[m_nIndex].m_SubWorldIndex;
	if (world < 0 || world >= MAX_SUBWORLD) return;
	action->MapId = SubWorld[world].m_SubWorldID;
	if (action->Operation == PHONGTHAN_SCRIPT_CLOSE)
	{
		m_UiDialogToken = 0;
		m_UiDialogView = 0;
		m_bWaitingPlayerFeedBack = false;
		m_nAvailableAnswerNum = 0;
	}
	else if (action->Operation == PHONGTHAN_SCRIPT_SHOW &&
		(action->View == UI_SELECTDIALOG || action->View == UI_TALKDIALOG || action->View == UI_NUMBER_INPUT || action->View == UI_ROULETTE))
	{
		if (++m_UiDialogSerial == 0) ++m_UiDialogSerial;
		m_UiDialogToken = m_UiDialogSerial;
		m_UiDialogView = PhongThanEncodeScriptView(action->View);
		action->DialogToken = m_UiDialogToken;
	}
	// Server-controlled players consume the same issued token/options internally.
	if(m_bIsBot)return;
	PHONGTHAN_U8 packet[sizeof(PHONGTHAN_UI_ACTION_HEADER) + PHONGTHAN_UI_MAX_KEY + PHONGTHAN_UI_MAX_CONTENT];
	const unsigned int size = PhongThanBuildScriptPacket(*action, packet, sizeof(packet));
	HRESULT publishResult = E_FAIL;
	if (size && g_pServer)
		// A dialog is a targeted response. Keep its native frame independent
		// of legacy aggregate messages whose unknown length stops client parsing.
		publishResult = g_pServer->SendData(m_nNetConnectIdx, packet, size);
	FILE* dialogLog = fopen("ui_action_diag.log", "a");
	if (dialogLog)
	{
		fprintf(dialogLog, "server tick=%lu player=%d map=%u token=%u op=%d view=%d options=%d content=%d size=%u hr=%08X\n",
			(unsigned long)GetTickCount(), m_nPlayerIndex, action->MapId, action->DialogToken,
			action->Operation, action->View, action->OptionCount, action->ContentLength,
			size, (unsigned int)publishResult);
		fclose(dialogLog);
	}
	g_DebugLog("[PhongThanUI] server publish player=%d conn=%d map=%u token=%u op=%d view=%d options=%d content=%d size=%u hr=0x%08X",
		m_nPlayerIndex, m_nNetConnectIdx, action->MapId, action->DialogToken,
		action->Operation, action->View, action->OptionCount,
		action->ContentLength, size, (unsigned int)publishResult);
	if (!size || !g_pServer || FAILED(publishResult))
	{
		if (action->DialogToken && action->DialogToken == m_UiDialogToken)
		{
			m_UiDialogToken = 0;
			m_bWaitingPlayerFeedBack = false;
			m_nAvailableAnswerNum = 0;
		}
		g_DebugLog("[PhongThanUI] Cannot publish script action");
	}
#else
	if (!action->ServerOwned) OnScriptAction(action);
#endif
}

void KPlayer::ExecuteUiSelection(int selection, int view)
{
	if (selection == -1)
	{
		m_nAvailableAnswerNum = 0;
		m_bWaitingPlayerFeedBack = false;
		m_UiDialogToken = 0;
		m_UiDialogView = 0;
		return;
	}
	if (selection < 0 || selection >= m_nAvailableAnswerNum || selection >= MAX_ANSWERNUM ||
		m_nIndex <= 0 || m_nIndex >= MAX_NPC || (view != SELECT_TALKUI && view != SELECT_SELECTUI)) return;
	char callback[sizeof(m_szTaskAnswerFun[0])];
	g_StrCpyLen(callback, m_szTaskAnswerFun[view == SELECT_TALKUI ? 0 : selection], sizeof(callback));
	const DWORD scriptId = view == SELECT_TALKUI ? m_TalkUiScriptId : m_SelUiScriptId;
	const bool task20Trace = m_nPlayerIndex == 1 && m_UiDialogToken == 5 && selection == 4 && view == SELECT_SELECTUI;
	if (task20Trace)
	{
		KLuaScript* script = (KLuaScript*)g_GetScript(scriptId);
		FILE* trace = fopen("task20_post_dispatch_trace.log", "a");
		if (trace)
		{
			fprintf(trace, "SELECT player_index=%d player_id=%lu npc_index=%d npc_template=%d npc_name=%s token=5 selection=4 callback=%s sel_script_id=%u talk_script_id=%u resolved_script=%s task20_before=%d\n", m_nPlayerIndex, (unsigned long)m_dwID, m_nIndex, Npc[m_nIndex].m_NpcSettingIdx, Npc[m_nIndex].Name, callback, (unsigned int)m_SelUiScriptId, (unsigned int)m_TalkUiScriptId, script ? script->m_szScriptName : "<missing>", m_cTask.GetSaveVal(20));
			fclose(trace);
		}
	}
	// Consume before Lua: a new prompt created by the callback remains valid.
	m_bWaitingPlayerFeedBack = false;
	m_nAvailableAnswerNum = 0;
	m_UiDialogToken = 0;
#ifdef _SERVER
	if (PhongThanStarterBagChoice(*this, callback)) return;
#endif
	if (callback[0])
	{
		if(!m_bIsBot){
            KLuaScript* origin=(KLuaScript*)g_GetScript(scriptId);
            if(origin && (strstr(origin->m_szScriptName,"super") || strstr(origin->m_szScriptName,"giapsi") || strstr(origin->m_szScriptName,"thuong_chu"))){FILE* t=fopen("ancient_transition_trace.log","a");if(t){fprintf(t,"utc=%lu player=%d role=%lu selected=%d script=%lu path=%s callback=%s\n",(unsigned long)time(NULL),m_nPlayerIndex,m_dwID,selection,scriptId,origin->m_szScriptName,callback);fclose(t);}}
        }
        const BOOL result = ExecuteScript(scriptId, callback, selection);
		if (task20Trace)
		{
			KLuaScript* script = (KLuaScript*)g_GetScript(scriptId);
			FILE* trace = fopen("task20_post_dispatch_trace.log", "a");
			if (trace)
			{
				fprintf(trace, "EXECUTE result=%s callback=%s script_id=%u resolved_script=%s task20_after=%d\n", result ? "PASS" : "FAIL", callback, (unsigned int)scriptId, script ? script->m_szScriptName : "<missing>", m_cTask.GetSaveVal(20));
				fclose(trace);
			}
		}
	}
}

void KPlayer::ProcessPlayerSelectFromUI(BYTE* data)
{
#ifdef _SERVER
    PTPerfScope choicePerformance(PTPERF_NPC_CHOICE);
	const PHONGTHAN_UI_CHOICE_REQUEST* choice = (const PHONGTHAN_UI_CHOICE_REQUEST*)data;
	const int rawSelection = data ? choice->Selection : 0;
	const int npcIndex = m_nIndex;
	const int npcIndexValid = npcIndex > 0 && npcIndex < MAX_NPC;
	const int world = npcIndexValid ? Npc[npcIndex].m_SubWorldIndex : -1;
	const int npcResolvePass = npcIndexValid && world >= 0 && world < MAX_SUBWORLD;
	const unsigned int mapId = npcResolvePass ? SubWorld[world].m_SubWorldID : 0;
	const int npcTemplate = npcIndexValid ? Npc[npcIndex].m_NpcSettingIdx : -1;
	const char* npcName = npcIndexValid ? Npc[npcIndex].Name : "<unresolved>";
	const int tokenPass = npcResolvePass && m_UiDialogToken != 0 &&
		choice->MapId == mapId && choice->DialogToken == m_UiDialogToken;
	const int viewPass = npcResolvePass && choice->View == m_UiDialogView;
	const int answerPass = npcResolvePass && choice->Selection >= -1 &&
		(choice->Selection == -1 || (choice->Selection < m_nAvailableAnswerNum &&
			choice->Selection < PHONGTHAN_UI_MAX_ANSWERS));
	FILE* trace = fopen("task20_ui_select_trace.log", "a");
	if (trace)
	{
		fprintf(trace, "timestamp=%lu player=%d usn=%lu raw_selection=%d decoded_selection=%d dialog_token=%u dialog_view=%u answer_count=%d npc_dialog_index=%d npc_index=%d npc_template=%d npc_name=%.32s map=%u token=%s view=%s answer=%s npc_resolve=%s dispatch=NOT_REACHED\n",
			(unsigned long)GetTickCount(), m_nPlayerIndex, (unsigned long)m_dwID, rawSelection, choice->Selection,
			m_UiDialogToken, m_UiDialogView, m_nAvailableAnswerNum, m_nIndex, npcIndex, npcTemplate,
			npcName, mapId, tokenPass ? "PASS" : "FAIL", viewPass ? "PASS" : "FAIL",
			answerPass ? "PASS" : "FAIL", npcResolvePass ? "PASS" : "FAIL");
	}
	if (!npcResolvePass)
	{
		if (trace)
		{
			fprintf(trace, "timestamp=%lu player=%d usn=%lu raw_selection=%d decoded_selection=%d dialog_token=%u dialog_view=%u answer_count=%d npc_dialog_index=%d npc_index=%d npc_template=%d npc_name=%.32s map=%u token=%s view=%s answer=%s npc_resolve=FAIL dispatch=NOT_REACHED\n",
				(unsigned long)GetTickCount(), m_nPlayerIndex, (unsigned long)m_dwID, rawSelection, choice->Selection,
				m_UiDialogToken, m_UiDialogView, m_nAvailableAnswerNum, m_nIndex, npcIndex, npcTemplate,
				npcName, mapId, tokenPass ? "PASS" : "FAIL", viewPass ? "PASS" : "FAIL", answerPass ? "PASS" : "FAIL");
			fclose(trace);
		}
		return;
	}
	if (!tokenPass || !viewPass || !answerPass)
	{
		if (trace)
		{
			fprintf(trace, "timestamp=%lu player=%d usn=%lu raw_selection=%d decoded_selection=%d dialog_token=%u dialog_view=%u answer_count=%d npc_dialog_index=%d npc_index=%d npc_template=%d npc_name=%.32s map=%u token=%s view=%s answer=%s npc_resolve=PASS dispatch=NOT_REACHED\n",
				(unsigned long)GetTickCount(), m_nPlayerIndex, (unsigned long)m_dwID, rawSelection, choice->Selection,
				m_UiDialogToken, m_UiDialogView, m_nAvailableAnswerNum, m_nIndex, npcIndex, npcTemplate,
				npcName, mapId, tokenPass ? "PASS" : "FAIL", viewPass ? "PASS" : "FAIL", answerPass ? "PASS" : "FAIL");
			fclose(trace);
		}
		return;
	}
	if (trace)
	{
		fprintf(trace, "timestamp=%lu player=%d usn=%lu raw_selection=%d decoded_selection=%d dialog_token=%u dialog_view=%u answer_count=%d npc_dialog_index=%d npc_index=%d npc_template=%d npc_name=%.32s map=%u token=PASS view=PASS answer=PASS npc_resolve=PASS dispatch=ENTER\n",
			(unsigned long)GetTickCount(), m_nPlayerIndex, (unsigned long)m_dwID, rawSelection, choice->Selection,
			m_UiDialogToken, m_UiDialogView, m_nAvailableAnswerNum, m_nIndex, npcIndex, npcTemplate, npcName, mapId);
		fclose(trace);
	}
	ExecuteUiSelection(choice->Selection, choice->View == PHONGTHAN_VIEW_CONVERSATION ? SELECT_TALKUI : SELECT_SELECTUI);
#else
	(void)data;
#endif
}

#ifndef _SERVER
void KPlayer::OnSelectFromUI(KPhongThanUiSelection* selection, UIInfo view)
{
	FILE* trace = fopen("ui_choice_send_trace.log", "a");
	const char* operation = view == UI_SELECTDIALOG ? "QUESTION_CHOOSE" : "INFORMATION_CONFIRM_NOTIFY";
	if (trace) fprintf(trace, "TRACE_READY\n");
	if (!selection)
	{
		if (trace) { fprintf(trace, "ENTER operation=%s selection=<null>\nSELECTION_NULL view=%d\n", operation, view); fclose(trace); }
		return;
	}
	if (trace) fprintf(trace, "ENTER operation=%s selection=%d answer_count=%d token=%u map=%d view=%d\n",
		operation, selection->Selection, g_bUISelLastSelCount, m_UiDialogToken, SubWorld[0].m_SubWorldID, view);
	if (view != UI_TALKDIALOG && view != UI_SELECTDIALOG)
	{
		if (trace) { fprintf(trace, "VIEW_INVALID view=%d selection=%d\n", view, selection->Selection); fclose(trace); }
		return;
	}
	const bool server = view == UI_TALKDIALOG ? g_bUISpeakActiveWithServer != 0 : g_bUISelIntelActiveWithServer != 0;
	if (!server)
	{
		if (trace) { fprintf(trace, "SERVER_INACTIVE operation=%s selection=%d\n", operation, selection->Selection); fclose(trace); }
		ExecuteUiSelection(selection->Selection, selection->View);
		return;
	}
	PHONGTHAN_UI_CHOICE_REQUEST request;
	ZeroMemory(&request, sizeof(request));
	PhongThanInitializeWireHeader(&request.Header, PHONGTHAN_MSG_UI_CHOICE_REQUEST,
		sizeof(request), PHONGTHAN_WIRE_FLAG_REQUEST, 0);
	request.MapId = SubWorld[0].m_SubWorldID;
	request.DialogToken = m_UiDialogToken;
	request.View = PhongThanEncodeScriptView(view);
	request.Selection = selection->Selection;
	const int valid = PhongThanValidateUiChoice(&request, sizeof(request));
	if (!g_pClient)
	{
		if (trace) fprintf(trace, "CLIENT_NULL operation=%s selection=%d\n", operation, selection->Selection);
	}
	else if (!valid)
	{
		if (trace)
		{
			if (!request.DialogToken) fprintf(trace, "TOKEN_INVALID token=%u\n", request.DialogToken);
			else if (!request.MapId) fprintf(trace, "MAP_INVALID map=%u\n", request.MapId);
			else if (request.View != PHONGTHAN_VIEW_QUESTION && request.View != PHONGTHAN_VIEW_CONVERSATION)
				fprintf(trace, "VIEW_INVALID wire_view=%u\n", request.View);
			else fprintf(trace, "PhongThanValidateUiChoice_FAIL selection=%d reserved=%u\n", request.Selection, request.Reserved);
		}
	}
	else
	{
		if (trace) fprintf(trace, "SEND_5102 magic=%08X opcode=%u flag=%u map=%u token=%u view=%u selection=%d reserved=%u packet_size=%u\n",
			request.Header.Magic, request.Header.MessageType, request.Header.Flags, request.MapId,
			request.DialogToken, request.View, request.Selection, request.Reserved, (unsigned int)sizeof(request));
		const HRESULT result = g_pClient->SendPackToServer(&request, sizeof(request));
		if (trace) fprintf(trace, "SEND_5102_RESULT hr=%08X\n", (unsigned int)result);
	}
	if (trace) fclose(trace);
}
#else
void KPlayer::S2CExecuteScript(char* scriptName, char* parameters)
{
	if (!scriptName || !scriptName[0]) return;
	KPhongThanScriptAction action;
	ZeroMemory(&action, sizeof(action));
	action.Operation = PHONGTHAN_SCRIPT_EXECUTE;
	action.ServerOwned = 1;
	const unsigned int scriptLength = strlen(scriptName);
	const unsigned int parameterLength = parameters ? strlen(parameters) : 0;
	if (scriptLength + parameterLength + 2 >= sizeof(action.Content)) return;
	strcpy(action.Content, scriptName);
	if (parameterLength) { strcat(action.Content, "|"); strcat(action.Content, parameters); }
	action.ContentLength = strlen(action.Content);
	DoScriptAction(&action);
}
#endif
