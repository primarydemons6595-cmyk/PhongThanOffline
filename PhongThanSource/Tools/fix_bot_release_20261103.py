"""Byte-preserving edits for native companion lifecycle."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]/'Sources/Core/Src'
def edit(name,old,new):
 p=root/name;data=p.read_bytes();assert data.count(old)==1,(name,data.count(old));p.write_bytes(data.replace(old,new))
edit('PhongThanBotAIgent.inl',b'int command=forcedSemantic?forcedCommand:PTBotCommandMode(commandQuery);',b'int command=PTBotReleaseCommand(commandQuery)?PTBC_CANCEL:forcedSemantic?forcedCommand:PTBotCommandMode(commandQuery);')
edit('PhongThanBotAIgent.inl',b'if(expires>=(unsigned long)time(0)&&QueueChat(from,bot-bots,text,length))',b'if(command!=PTBC_CANCEL&&expires>=(unsigned long)time(0)&&QueueChat(from,bot-bots,text,length))')
edit('PhongThanBotCompanion.inl',b'CompanionEvent(r,"gift_wait_expired","supply wait expired; previous job restored only for same live owner");}',b'CompanionEvent(r,"gift_wait_expired","supply wait expired; previous job restored only for same live owner");if(!c.Mode)return false;}')
# A cancelled negotiation without a running mode must release its owner too.
# ResetCompanion already restores PK and clears path/targets; return to normal tick.
p=Path(__file__).resolve().parents[1]/'Tests/Native/BotCompanionDecisionTests.cpp'
d=p.read_bytes().replace(b'companions[0].Mode==PTBC_WAIT',b'companions[0].Mode==PTBC_NONE')
p.write_bytes(d)
