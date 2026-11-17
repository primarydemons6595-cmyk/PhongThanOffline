#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WndMessage.h"
static int closed,calls;
struct Requester {unsigned int param;int value;void WndProc(unsigned int,unsigned int p,int v){++calls;param=p;value=v;}};
struct KUiGetMoney;
struct Edit {char text[32];KUiGetMoney* owner;DWORD GetIntNumber(){return strtoul(text,0,10);}void GetText(char* out,int n,bool){lstrcpynA(out,text,n);}void SetIntText(DWORD n);};
struct KUiGetMoney {Edit m_Money;bool m_CheckingInput;int m_nMaxMoney;Requester* m_pRequester;unsigned int m_uRequesterParam;void OnCheckInput();void OnOk();void OnCancel();static void CloseWindow(bool){++closed;}};
void Edit::SetIntText(DWORD n){sprintf(text,"%lu",n);owner->OnCheckInput();}
#include "money-ui-production.inl"
static int checks;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL %d\n",__LINE__);return 1;}}while(0)
int main(){KUiGetMoney ui;memset(&ui,0,sizeof(ui));Requester r;memset(&r,0,sizeof(r));ui.m_Money.owner=&ui;ui.m_pRequester=&r;ui.m_nMaxMoney=10000;ui.m_uRequesterParam=123;
 strcpy(ui.m_Money.text,"99999");ui.OnCheckInput();CHECK(!strcmp(ui.m_Money.text,"10000")&&!ui.m_CheckingInput&&calls==0); // recursive edit notifications terminate
 ui.OnOk();CHECK(calls==1&&r.param==123&&r.value==WND_OPER_RESULT(10000)&&closed==1);
 ui.m_nMaxMoney=0;strcpy(ui.m_Money.text,"4");ui.OnCheckInput();CHECK(!strcmp(ui.m_Money.text,"0"));
 ui.OnCancel();CHECK(calls==2&&r.value==WND_OPER_RESULT(-1)&&closed==2);
 ui.m_nMaxMoney=2147483647;strcpy(ui.m_Money.text,"2000000000");ui.OnCheckInput();CHECK(!strcmp(ui.m_Money.text,"2000000000"));
 printf("PASS bank money input: %d checks\n",checks);return 0;}
