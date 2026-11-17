#include <windows.h>
#include <assert.h>
#include <stdio.h>
enum{MAX_PLAYER=4,SAVE_REQUEST=1};
struct PHONGTHAN_CHARACTER_STATE_HEADER{int value;};
static bool pending=false,saveOk=true;static int calls=0;
struct Actor{BYTE m_SaveBuffer[100];int m_uMustSave;bool Save(){++calls;return saveOk;}}Player[MAX_PLAYER];
namespace PtCharacterBundle{bool PendingAccount(const char*,const PHONGTHAN_CHARACTER_STATE_HEADER*){return pending;}}
class CoreServerShell{public:void* SavePlayerDataAtOnce(int);};
void* CoreServerShell::SavePlayerDataAtOnce(int nIndex)
{
    const bool retry=nIndex<0;if(retry){if(nIndex==(-2147483647-1))return NULL;nIndex=-nIndex;}

	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return NULL;
	}

    if(retry&&PtCharacterBundle::PendingAccount("CharacterStore",(const PHONGTHAN_CHARACTER_STATE_HEADER*)Player[nIndex].m_SaveBuffer))return &Player[nIndex].m_SaveBuffer;
	if (Player[nIndex].Save())
	{
		Player[nIndex].m_uMustSave = SAVE_REQUEST;
		return &Player[nIndex].m_SaveBuffer;
	}
	else
	{
		return NULL;
	}
}
int main(){CoreServerShell core;pending=true;calls=0;assert(core.SavePlayerDataAtOnce(-1)==&Player[1].m_SaveBuffer&&calls==0);
assert(core.SavePlayerDataAtOnce(1)==&Player[1].m_SaveBuffer&&calls==1);pending=false;assert(core.SavePlayerDataAtOnce(-1)&&calls==2);
saveOk=false;assert(!core.SavePlayerDataAtOnce(1));pending=true;assert(core.SavePlayerDataAtOnce(-1));
assert(!core.SavePlayerDataAtOnce(0)&&!core.SavePlayerDataAtOnce(-2147483647-1)&&!core.SavePlayerDataAtOnce(-4));
puts("PASS production save entry: network retry reuses durable pending buffer, positive logout/shutdown always captures fresh state, invalid indexes and failed final capture rejected");return 0;}
