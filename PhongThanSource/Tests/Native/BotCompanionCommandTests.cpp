#include <assert.h>
#include <stdio.h>
#include "../../Headers/PhongThanBotCommand.h"
#include "../../Headers/PhongThanBotPartySpeech.h"
int main(){
 struct Case {const char* text;int expected;} cases[]={
  {"ban di cung toi duoc khong?",PTBC_INVITE},{"di voi tui nhe",PTBC_INVITE},
  {"dung cho o day",PTBC_WAIT},{"ong dung yen",PTBC_WAIT},
  {"danh quai di",PTBC_FIGHT},{"di train di",PTBC_FIGHT},{"di danh boss",PTBC_BOSS},
  {"nhat do giup tui",PTBC_LOOT},{"di mua thuoc",PTBC_SHOP},{"giao dich nhan qua",PTBC_GIFT},
  {"ok!",PTBC_CONFIRM},{"dong y",PTBC_CONFIRM},{"khong di nua",PTBC_CANCEL},
  {"dung danh quai",PTBC_CANCEL},{"khong nhat do",PTBC_CANCEL},{"huy lenh",PTBC_CANCEL},
  {"cu theo sau toi",PTBC_FOLLOW},{"di theo toi",PTBC_FOLLOW},{"di danh con quai",PTBC_FIGHT},{"danh con boss",PTBC_BOSS},{"ong dang di train a?",PTBC_NONE},{"sao ong dung yen?",PTBC_NONE},{"di train o dau?",PTBC_NONE},
  {"chao ong",PTBC_NONE},{"cho tui vao doi",PTBC_NONE},{"vu khi ong dang deo",PTBC_NONE},
  {"khong can nua",PTBC_CANCEL},{"ko can ong nua",PTBC_CANCEL},{"thoi",PTBC_CANCEL},{"khoi cho tui",PTBC_CANCEL},{"dung cho nua",PTBC_CANCEL},{"tu di train di",PTBC_CANCEL},{"ong lam viec cua ong di",PTBC_CANCEL},{"thoi di danh boss",PTBC_BOSS},
 };
 for(int i=0;i<sizeof(cases)/sizeof(cases[0]);++i){int actual=PTBotCommandMode(cases[i].text);if(actual!=cases[i].expected){printf("FAIL %s: %d != %d\n",cases[i].text,actual,cases[i].expected);return 1;}}
 // The same original-client Vietnamese fold used by live whispers.
 char folded[256];PTBotFoldPartySpeech("B\271n \256i c\357ng t\253i \256\255\356c kh\253ng?",folded,sizeof(folded));assert(PTBotCommandMode(folded)==PTBC_INVITE);
 printf("PASS 27 native command cases including original Vietnamese glyphs\n");return 0;
}
