struct BotAutonomyPolicy {
 int recover,training,bag,boss,vtt,tc,rps,xuVan,buyVan,spend,powerValue;
 int nationMode,maxNations,contribution,land,totem,defend,pvp,callAllies,maxAllies,pvpMinutes,pvpMinLevel,pvpGap,domination;
 std::vector<int> pvpMaps;
 struct Price {int genre,detail,particular,level,quality,minimum,reference,maximum;};
 std::vector<Price> prices;
 BotAutonomyPolicy():recover(1),training(1),bag(1),boss(1),vtt(1),tc(1),rps(1),xuVan(60),buyVan(5000),spend(25),powerValue(1000),nationMode(2),maxNations(3),contribution(5),land(0),totem(0),defend(1),pvp(0),callAllies(1),maxAllies(2),pvpMinutes(20),pvpMinLevel(30),pvpGap(15),domination(25){}
};
static BotAutonomyPolicy botPolicy;
static void BotLoadAutonomy(DWORD now){
 static DWORD last=0;if(last&&now-last<5000)return;last=now;
 // Runtime control files are not game assets and must bypass loose_index/PAK.
 // Parse the bounded numeric projection once, then read the in-memory INI.
 FILE* file=fopen("BotAIgent\\autonomy.ini","rb");if(!file)return;
 fseek(file,0,SEEK_END);long bytes=ftell(file);rewind(file);if(bytes<1||bytes>262144){fclose(file);return;}
 KIniFile ini;char line[256],section[32]={0};
 while(fgets(line,sizeof(line),file)){
  if(line[0]=='['){char name[32];if(sscanf(line,"[%31[^]]]",name)==1)lstrcpyA(section,name);else section[0]=0;}
  else if(section[0]&&line[0]!=';'){char key[48];int value;if(sscanf(line,"%47[^=]=%d",key,&value)==2)ini.WriteInteger(section,key,value);}
 }
 bool readOk=!ferror(file);fclose(file);if(!readOk)return;
 int version=0;ini.GetInteger("Policy","Version",0,&version);if(version!=1)return;
 BotAutonomyPolicy next;
 const char* keys[]={"AutoRecover","AdaptiveTraining","AutoBag","BossTeams","VanTienTran","ThuongChu","RockPaperScissors","XuVan","MaxBuyVan","SpendPercent","EquipmentPowerValue"};
 int* values[]={&next.recover,&next.training,&next.bag,&next.boss,&next.vtt,&next.tc,&next.rps,&next.xuVan,&next.buyVan,&next.spend,&next.powerValue};
 for(int i=0;i<11;++i)ini.GetInteger("Policy",keys[i],*values[i],values[i]);
 for(i=0;i<7;++i)if(*values[i]<0||*values[i]>1)return;
 if(next.xuVan<1||next.xuVan>50000||next.buyVan<1||next.buyVan>50000||next.spend<1||next.spend>50||next.powerValue<1||next.powerValue>100000)return;
 const char* extraKeys[]={"NationMode","MaxBotNations","ContributionPercent","CityLand","CityTotem","SelfDefence","ProactivePvp","CallAllies","MaxAllies","PvpMinutes","PvpMinLevel","PvpLevelGap","DominationPercent"};
 int* extraValues[]={&next.nationMode,&next.maxNations,&next.contribution,&next.land,&next.totem,&next.defend,&next.pvp,&next.callAllies,&next.maxAllies,&next.pvpMinutes,&next.pvpMinLevel,&next.pvpGap,&next.domination};
 for(i=0;i<13;++i)ini.GetInteger("Policy",extraKeys[i],*extraValues[i],extraValues[i]);
 if(next.nationMode<0||next.nationMode>2||next.maxNations<1||next.maxNations>25||next.contribution<0||next.contribution>25||next.land<0||next.land>4||next.totem<0||next.totem>2||next.defend<0||next.defend>1||next.pvp<0||next.pvp>1||next.callAllies<0||next.callAllies>1||next.maxAllies<0||next.maxAllies>5||next.pvpMinutes<1||next.pvpMinutes>1440||next.pvpMinLevel<12||next.pvpMinLevel>200||next.pvpGap<0||next.pvpGap>50||next.domination<0||next.domination>100)return;
 int maps=0;ini.GetInteger("Policy","PvpMapCount",0,&maps);if(maps<0||maps>128)return;
 for(i=0;i<maps;++i){char key[24];sprintf(key,"PvpMap%d",i);int map=0;ini.GetInteger("Policy",key,0,&map);if(map<1001||map>9999)return;for(int j=0;j<(int)next.pvpMaps.size();++j)if(next.pvpMaps[j]==map)return;next.pvpMaps.push_back(map);}
 int count=0;ini.GetInteger("Policy","PriceCount",0,&count);if(count<0||count>128)return;
 for(i=0;i<count;++i){char section[24];sprintf(section,"Price%d",i);BotAutonomyPolicy::Price p;
  ini.GetInteger(section,"Genre",-1,&p.genre);ini.GetInteger(section,"Detail",-1,&p.detail);ini.GetInteger(section,"Particular",-1,&p.particular);ini.GetInteger(section,"Level",-1,&p.level);ini.GetInteger(section,"Quality",-1,&p.quality);
  ini.GetInteger(section,"MinVan",0,&p.minimum);ini.GetInteger(section,"PriceVan",0,&p.reference);ini.GetInteger(section,"MaxVan",0,&p.maximum);
  if(p.genre< -1||p.genre>20||p.detail< -1||p.detail>100000||p.particular< -1||p.particular>100000||p.level< -1||p.level>1000||p.quality< -1||p.quality>20||p.minimum<1||p.minimum>p.reference||p.reference>p.maximum||p.maximum>50000||(p.genre==4&&(p.detail==46||p.detail==47)))return;
  next.prices.push_back(p);
 }
 botPolicy=next;
}
static PTBotQuote BotQuote(KItem& item){
 int power=0,quality=item.GetGenre()==item_equip?item.GetPhongThanQuality():0;
 bool known=item.GetGenre()!=item_equip||item.GetEquipmentPower(power);int match=-1,specific=-1;
 for(int i=0;i<(int)botPolicy.prices.size();++i){const BotAutonomyPolicy::Price& p=botPolicy.prices[i];
  if((p.genre>=0&&p.genre!=item.GetGenre())||(p.detail>=0&&p.detail!=item.GetDetailType())||(p.particular>=0&&p.particular!=item.GetParticular())||(p.level>=0&&p.level!=item.GetLevel())||(p.quality>=0&&p.quality!=quality))continue;
  int score=(p.genre>=0)+(p.detail>=0)+(p.particular>=0)+(p.level>=0)+(p.quality>=0);if(score>specific){specific=score;match=i;}
 }
 if(match>=0){const BotAutonomyPolicy::Price& p=botPolicy.prices[match];return PTBotWholeVanQuote(PTBotReferenceQuote(0,0,0,item.GetInventoryCount(),botPolicy.powerValue,p.minimum,p.reference,p.maximum));}
 if(!known)return PTBotQuote();
 return PTBotWholeVanQuote(PTBotReferenceQuote(item.GetSalePrice(),power,quality,item.GetInventoryCount(),botPolicy.powerValue));
}
