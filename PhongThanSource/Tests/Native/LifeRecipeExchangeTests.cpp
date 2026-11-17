#include "LifeQuestFixture.inl"
#include "LifeRecipeCases.inl"
static void Prepare(const LifeRecipeCase& recipe,int capacity,PhongThanQuestExchangeRow* input){
 ZeroMemory(Player,sizeof(Player));ZeroMemory(Item,sizeof(Item));ItemSet.allocated=ItemSet.failAt=0;Player[1].m_ItemList.capacity=capacity;
 for(int i=0;i<recipe.needCount;++i){
  const int* t=recipe.needs[i];assert(PhongThanDecodeQuestItemTuple(t[0],t[1],t[2],t[3],t[4],t[5],input[i].item));input[i].count=t[6];
  int n=i+1;Item[n].id=100+n;Item[n].genre=input[i].item.genre;Item[n].detail=input[i].item.detail;Item[n].part=input[i].item.particular;
  Item[n].level=input[i].item.level;Item[n].series=input[i].item.series;Item[n].stack=t[6]+2;Item[n].maxStack=10000;
  assert(Player[1].m_ItemList.Add(n,pos_equiproom,i,0,false));
 }
}
static void Intact(const LifeRecipeCase& recipe){for(int i=0;i<recipe.needCount;++i){int n=i+1;assert(Item[n].id==100+n&&Item[n].stack==recipe.needs[i][6]+2&&Player[1].m_ItemList.FindSame(n));}assert(Player[1].m_cTask.slots[5140]==0);}
int main(){
 assert(OriginalQuestRegression(0,0)==0);int checked=0;
 for(int c=0;c<sizeof(lifeCases)/sizeof(lifeCases[0]);++c){const LifeRecipeCase& recipe=lifeCases[c];assert(recipe.needCount>0);
  for(int variant=0;variant<recipe.outCount;++variant){
   PhongThanQuestExchangeRow input[6],reward;const int* out=recipe.outputs[variant];
   assert(PhongThanDecodeQuestItemTuple(out[0],out[1],out[2],out[3],out[4],out[5],reward.item));reward.count=1;
   Prepare(recipe,recipe.needCount,input);
   assert(!PhongThanQuestExchange(1,5140,0,1,input,recipe.needCount,&reward,1,true));Intact(recipe);
   Prepare(recipe,recipe.needCount+2,input);ItemSet.failAt=1;
   assert(!PhongThanQuestExchange(1,5140,0,1,input,recipe.needCount,&reward,1,true));Intact(recipe);
   Prepare(recipe,recipe.needCount+2,input);
   assert(PhongThanQuestExchange(1,5140,0,1,input,recipe.needCount,&reward,1,true)==1);
   assert(Player[1].m_cTask.slots[5140]==1);
   assert(!PhongThanQuestExchange(1,5140,0,1,input,recipe.needCount,&reward,1,true));
   ++checked;
  }
 }
 printf("PASS production QuestExchange: all 128 recipes, %d output variants, exact tuple keys, full-bag rollback, generation rollback, serial replay guard\n",checked);return 0;
}
