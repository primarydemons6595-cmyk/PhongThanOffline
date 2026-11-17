"""Extend explicit script-view routing for the restored roulette panel."""
from repair_npc_dialog_exit import change
change('Sources/GameClient/Ui/GameSpaceChangedNotify.cpp','#include "KWin32.h"','#include "KWin32.h"\n#include "../../Core/Src/PhongThanScriptAction.h"')
change('Sources/Core/Src/PhongThanScriptAction.h','UI_TOPMESSAGE, UI_SCROLLMESSAGE, UI_NUMBER_INPUT','UI_TOPMESSAGE, UI_SCROLLMESSAGE, UI_NUMBER_INPUT, UI_ROULETTE')
change('Headers/PhongThanUiProtocol.h','PHONGTHAN_VIEW_NUMBER_INPUT = 211','PHONGTHAN_VIEW_NUMBER_INPUT = 211,\n\tPHONGTHAN_VIEW_ROULETTE = 212')
change('Headers/PhongThanUiProtocol.h','ui->View > PHONGTHAN_VIEW_NUMBER_INPUT','ui->View > PHONGTHAN_VIEW_ROULETTE')
change('Headers/PhongThanUiProtocol.h','\t\tif (ui->View == PHONGTHAN_VIEW_NUMBER_INPUT &&',
       '\t\tif (ui->View == PHONGTHAN_VIEW_ROULETTE && (!ui->MapId || !ui->DialogToken || ui->OptionCount != 0 || ui->NumberArgument < 0 || ui->NumberArgument > 11)) return 0;\n\t\tif (ui->View == PHONGTHAN_VIEW_NUMBER_INPUT &&')
change('Sources/Core/Src/PhongThanScriptWire.h','case UI_NUMBER_INPUT: return PHONGTHAN_VIEW_NUMBER_INPUT;',
       'case UI_NUMBER_INPUT: return PHONGTHAN_VIEW_NUMBER_INPUT;\n\tcase UI_ROULETTE: return PHONGTHAN_VIEW_ROULETTE;')
change('Sources/Core/Src/PhongThanScriptWire.h','case PHONGTHAN_VIEW_NUMBER_INPUT: return UI_NUMBER_INPUT;',
       'case PHONGTHAN_VIEW_NUMBER_INPUT: return UI_NUMBER_INPUT;\n\tcase PHONGTHAN_VIEW_ROULETTE: return UI_ROULETTE;')
change('Sources/Core/Src/PhongThanPlayerDialog.inl','action->View == UI_NUMBER_INPUT))','action->View == UI_NUMBER_INPUT || action->View == UI_ROULETTE))')
change('Sources/Core/Src/KPlayer.cpp','pScriptAction->View == UI_NUMBER_INPUT))','pScriptAction->View == UI_NUMBER_INPUT || pScriptAction->View == UI_ROULETTE))')
change('Sources/Core/Src/KPlayer.cpp','                case UI_NUMBER_INPUT:',
       '                case UI_ROULETTE:\n                    CoreDataChanged(0x505452, pScriptAction->NumberArgument, (int)pScriptAction);\n                    break;\n                case UI_NUMBER_INPUT:')
change('Sources/Core/Src/CoreShell.cpp','\tcase GOI_INPUT:',
       '''\tcase 0x505453: // Roulette completion: only acknowledge this panel's token.
\t\t{
\t\t\tKPlayer& player = Player[CLIENT_PLAYER_INDEX];
\t\t\tif (!g_pClient || player.m_UiDialogView != PHONGTHAN_VIEW_ROULETTE || !uParam || player.m_UiDialogToken != uParam) return 0;
\t\t\tPHONGTHAN_UI_NUMBER_REQUEST request; memset(&request, 0, sizeof(request));
\t\t\tPhongThanInitializeWireHeader(&request.Header, PHONGTHAN_MSG_UI_NUMBER_INPUT, sizeof(request), PHONGTHAN_WIRE_FLAG_REQUEST, 0);
\t\t\trequest.MapId = SubWorld[0].m_SubWorldID; request.DialogToken = uParam; request.Value = 1;
\t\t\tconst HRESULT sent = g_pClient->SendPackToServer(&request, sizeof(request));
\t\t\tif (FAILED(sent)) return 0;
\t\t\tplayer.m_UiDialogToken = 0; player.m_UiDialogView = 0;
\t\t\treturn 1;
\t\t}
\tcase GOI_INPUT:''')
change('Sources/GameClient/Ui/UiCase/UiEnchase.cpp','#include "UiVngCompose.inl"',
       '#include "UiRoulette.inl"\nvoid OpenPhongThanRoulette(const KPhongThanScriptAction* action) { KUiPtRoulette::Open(action); }\n#include "UiVngCompose.inl"')
change('Sources/GameClient/Ui/GameSpaceChangedNotify.cpp','        case GDCNI_ENCHASE:',
       '        case 0x505452: {\n            extern void OpenPhongThanRoulette(const KPhongThanScriptAction*);\n            OpenPhongThanRoulette((const KPhongThanScriptAction*)nParam);\n            break;\n        }\n        case GDCNI_ENCHASE:')
print('Roulette view and client/server acknowledgement routing updated.')
