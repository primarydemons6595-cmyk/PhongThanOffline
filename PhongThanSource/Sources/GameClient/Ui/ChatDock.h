#ifndef PT_CHAT_DOCK_H
#define PT_CHAT_DOCK_H
// Render surface includes the chat column; ordinary HUD/scene metrics do not.
extern int g_PtChatDockWidth;
void UiRequestChatDockToggle();
void UiApplyPendingChatDock();
void UiCloseChatDock();
#endif
