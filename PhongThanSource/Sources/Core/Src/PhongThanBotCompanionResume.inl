// Supplies are an interruption of the current job, not its replacement. A saved
// command can resume only for the same live role generation after trade closes.
static CompanionState companionResume[sizeof(companions)/sizeof(companions[0])];
static void CompanionPrepareGift(Runtime& r,int from,int mode){
 int slot=&r-bots;CompanionState& c=companions[slot];
 if(mode!=PTBC_GIFT){companionResume[slot]=CompanionState();return;}
 if(c.Mode==PTBC_GIFT)return;
 bool resumable=c.Mode==PTBC_FOLLOW||c.Mode==PTBC_WAIT||c.Mode==PTBC_SHOP||PTBotCombatCommand(c.Mode)||c.Mode==PTBC_GOTO||c.Mode==PTBC_VTT;
 companionResume[slot]=resumable&&CompanionOwns(r,from)?c:CompanionState();
}
static bool CompanionResumeGift(Runtime& r,DWORD now){
 int slot=&r-bots;CompanionState saved=companionResume[slot];companionResume[slot]=CompanionState();CompanionState& c=companions[slot];
 if(!saved.Mode||saved.Owner!=c.Owner||saved.OwnerId!=c.OwnerId||strcmp(saved.Peer,c.Peer)||!CompanionOwns(r,c.Owner)){c=CompanionState();training[slot].Path.clear();training[slot].Target=0;training[slot].Identity=0;training[slot].QuestTarget=training[slot].QuestMap=0;return false;}
 c=saved;c.GiftUntil=0;c.Started=now;c.Until=now+1800000;c.LastNotice=0;return true;
}
