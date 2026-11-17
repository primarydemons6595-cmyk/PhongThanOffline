#include <stdio.h>
#include <assert.h>
#include "../../Headers/PhongThanUiLayoutRules.h"
int main(){
 for(int prompt=0;prompt<=100;++prompt)for(int count=1;count<=20;++count){
  PTNpcButtonLayout b=PhongThanNpcButtonLayout(prompt,count);
  assert(40+b.PromptHeight+8<=b.Top);
  assert(b.Top+(b.VisibleRows-1)*34+28==186);
  assert(b.MaxScroll==0||b.Rows*3<count);
 }
 // Reproduce the three-line travel prompt and all four destinations.
 PTNpcListLayout travel=PhongThanNpcListLayout(3,194);
 assert(travel.PromptHeight==51&&travel.ChoiceTop==99&&travel.ChoiceHeight>=4*17);
 for(int lines=0;lines<=200;++lines){PTNpcListLayout list=PhongThanNpcListLayout(lines,194);
  assert(list.ChoiceHeight>=3*17&&list.ChoiceTop+list.ChoiceHeight<=186);
  assert(40+list.PromptHeight<=list.ChoiceTop);
  assert(list.PromptHeight>=34&&list.PromptHeight<=85);
 }
 puts("PASS NPC layouts: uniform bottom-aligned buttons, scroll overflow, separate prompt; travel prompt and legacy lists stay above footer");return 0;
}
