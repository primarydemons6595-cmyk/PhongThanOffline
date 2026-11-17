static bool PtSocketPresentationTables(KTabFile& holes,KTabFile& gems,KTabFile& positions){
 static bool ready=false;
 if(!ready)ready=holes.Load("\\settings\\item\\da_kong_xiang_qian\\kong_shu_xing_biao_sc.txt")&&gems.Load("\\settings\\item\\da_kong_xiang_qian\\bao_shi_shu_xing_biao_sc.txt")&&positions.Load("\\settings\\item\\da_kong_xiang_qian\\zhuang_bei_kong_hui_zhi_kong_zhi_biao_c.txt");return ready;
}
static KTabFile ptSocketHoleNames,ptSocketGemRows,ptSocketPositions;
int PtSocketPower(int param){
 PT_SOCKET_STATE state;if(!PtSocketRead(param,state)||!PtSocketPresentationTables(ptSocketHoleNames,ptSocketGemRows,ptSocketPositions))return 0;
 int power=0;for(int s=0;s<4;++s)if(state.Gems[s]){char key[24];sprintf(key,"%d",state.Gems[s]);int n=0;ptSocketGemRows.GetInteger(ptSocketGemRows.FindRow(key),59,0,&n);power+=n;
  if(state.Bonus[s]){sprintf(key,"%d",state.Holes[s]);ptSocketHoleNames.GetInteger(ptSocketHoleNames.FindRow(key),8,0,&n);power+=n;}}
 return power;
}
bool PtSocketVisuals(KItem& item,PT_SOCKET_VISUALS& visuals){
 ZeroMemory(&visuals,sizeof(visuals));PT_SOCKET_STATE state;
 if(item.GetGenre()!=item_equip||!PtSocketRead(item.GetParam(),state)||!PtSocketPresentationTables(ptSocketHoleNames,ptSocketGemRows,ptSocketPositions))return false;
 char key[24];sprintf(key,"%d",item.GetDetailType());int row=ptSocketPositions.FindRow(key);if(row<2)return false;
 for(int s=0;s<4;++s)if(state.Holes[s]){int at=visuals.Count++;visuals.Slot[at]=s;char xy[40];ptSocketPositions.GetString(row,s+2,"",xy,sizeof(xy));if(sscanf(xy,"%d,%d",&visuals.X[at],&visuals.Y[at])!=2)return false;
  KTabFile& table=state.Gems[s]?ptSocketGemRows:ptSocketHoleNames;sprintf(key,"%d",state.Gems[s]?state.Gems[s]:state.Holes[s]);
  char path[120];table.GetString(table.FindRow(key),state.Gems[s]?6:4,"",path,sizeof(path));if(!path[0])return false;sprintf(visuals.Image[at],"\\%s",path[0]=='\\'?path+1:path);
  sprintf(key,"%d",state.Holes[s]);int holeRow=ptSocketHoleNames.FindRow(key);
  ptSocketHoleNames.GetString(holeRow,4,"",path,sizeof(path));if(!path[0])return false;sprintf(visuals.HoleImage[at],"\\%s",path[0]=='\\'?path+1:path);
  ptSocketHoleNames.GetString(holeRow,5,"",path,sizeof(path));if(!path[0])return false;sprintf(visuals.MenuImage[at],"\\%s",path[0]=='\\'?path+1:path);
 }return true;
}
