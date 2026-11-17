#include <stdio.h>
#include "../../Headers/PhongThanSetCatalogIdentity.h"
int main(){
 int whites=0,sets=0;
 for(unsigned int i=0;i<sizeof(ptSetCatalog)/sizeof(ptSetCatalog[0]);++i){
  const PhongThanSetCatalogIdentity& r=ptSetCatalog[i];
  int actual=PhongThanCatalogSetEffect(r.detail,r.row,r.part,r.level);
  if(r.part==310){if(actual)return 1;++whites;}
  else {if(actual!=r.effect)return 2;++sets;}
 }
 if(!whites||!sets)return 3;
 printf("PASS white warrior family: %d inferred entries excluded; %d genuine set entries unchanged\n",whites,sets);
 return 0;
}
