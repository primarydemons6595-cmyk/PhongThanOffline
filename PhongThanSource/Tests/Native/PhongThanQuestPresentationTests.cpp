#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanNpcDialogRules.h"
#include "../../Headers/PhongThanTaskJournal.h"
#define CHECK(x) do {if(!(x)){printf("FAIL line=%d\n",__LINE__);return 1;}}while(0)
int main(){
    CHECK(PhongThanIsDialogExit("\xA7\xE3ng"));
    CHECK(PhongThanIsDialogExit("K\xD5t th\xF3" "c"));
    CHECK(PhongThanIsDialogExit("K\xD5t th\xF3" "c \xAE\xE8i tho\xB9i"));
    CHECK(!PhongThanIsDialogExit("Hoan thanh nhiem vu"));
    CHECK(!PhongThanIsDialogExit("Thoat khoi bang"));
    const char encodedExit[]={2,0,(char)255,0,(char)0xa7,(char)0xe3,'n','g',3};
    CHECK(PhongThanIsDialogExit(encodedExit,sizeof(encodedExit)));
    CHECK(PhongThanTaskRecordId(3024,"<color=cyan>Dong Hai (225,207)<color>")==3024);
    CHECK(PhongThanTaskRecordStep(3024,27,"Thu thap ngu huyet")==27);
    CHECK(!PhongThanTaskRewardStep(3024,27));
    CHECK(PhongThanTaskRecordId(27,"Task 27 - step 11")==27);
    CHECK(!PhongThanTaskShouldNotify(27,2," "));
    CHECK(!PhongThanTaskShouldNotify(20,2," \r\n"));
    CHECK(!PhongThanTaskShouldNotify(3004,2," "));
    CHECK(!PhongThanTaskShouldNotify(3004,1,"progress"));
    CHECK(!PhongThanTaskShouldNotify(4003,1,"expired"));
    CHECK(PhongThanTaskShouldNotify(1,2,"finished"));
    CHECK(PhongThanTaskShouldNotify(1,0,"accepted"));
    CHECK(!PhongThanDialogUsesButtons(0,0));
    CHECK(PhongThanDialogUsesButtons(PHONGTHAN_DIALOG_BUTTONS,0));
    puts("PASS QUEST PRESENTATION: TCVN3 exits separate from quest choices; literal journal identities and phases survive intact.");
    return 0;
}
