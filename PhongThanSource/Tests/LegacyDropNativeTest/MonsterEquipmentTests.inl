// Included inside the existing real native catalog/generator test executable.
{
    Check(ItemGen.Catalog().GetMagicScript(2020)==0,"2020 native catalog identity really absent");
    PhongThanQuestItemTuple missing;Check(adapter.Map(6,1,2020,missing),"2020 tuple spelling is recognized, not an alias");missing.level=1;int missingItem=0;
    Check(!adapter.GenerateMapped(missing,missingItem,error)&&missingItem==0,"2020 native generation rejects missing catalog");
    const int blocked[5]={0,7,11,15,86};
    for(int bi=0;bi<5;++bi){NpcSnapshot n;n.templateId=blocked[bi];char v[1024];npcs.GetInteger(blocked[bi]+2,"Treasure",-1,&n.treasure);npcs.GetInteger(blocked[bi]+2,"Treasure1",-1,&n.treasure1);
        for(int sl=0;sl<4;++sl){char key[32];if(sl)sprintf(key,"DropRateFile%d",sl);else strcpy(key,"DropRateFile");npcs.GetString(blocked[bi]+2,key,"",v,sizeof(v));n.slots[sl]=v;}
        Config c;c.profile=PROFILE_A;c.treasure=SUCCESS_CAP;c.level=EXPERIMENTAL_MAIN_CATALOG;c.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;c.money=SYNTHETIC_FLAT_BERNOULLI;c.npcLevel=30;Plan p;
        bool ok=Build(n,q,c,loader,catalog,p,error);Check(!ok&&error==(blocked[bi]==86?"NPC_QUARANTINED":"TABLE_QUARANTINED"),"unresolved field NPC stays safely blocked");printf("FIELD_BLOCKED_NATIVE npc=%d reason=%s\n",blocked[bi],error.c_str());
    }
    NpcSnapshot npc;npc.templateId=18;npc.entity=19;npc.treasure=npc.treasure1=1;
    for(int s=0;s<4;++s){char k[32],v[1024];if(s)sprintf(k,"DropRateFile%d",s);else strcpy(k,"DropRateFile");npcs.GetString(20,k,"",v,sizeof(v));npc.slots[s]=v;}
    const int levels[5]={10,20,40,80,100};
    for(int li=0;li<5;++li){Config c;c.profile=PROFILE_A;c.treasure=SUCCESS_CAP;c.level=EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG;c.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;c.money=SYNTHETIC_FLAT_BERNOULLI;c.npcLevel=levels[li];
        Plan plan;bool ok=Build(npc,q,c,loader,catalog,plan,error);if(!ok)printf("MONSTER_PLAN_REJECT npcLevel=%d reason=%s section=%d\n",levels[li],error.c_str(),plan.RejectedRow());Check(ok,"NPC18 equipment plan at multiple monster levels");if(!ok)continue;
        int low,high;Check(MonsterEquipmentResolver::Bounds(c.npcLevel,1,10,20,10,false,0,low,high,error),"divisor bounds");
        Random rng(20261018);int count=0,minLevel=999,maxLevel=-1,maxRequired=0;
        for(int draw=0;draw<1000;++draw){Result result;Check(plan.Draw(rng,result,error),"monster plan draw");
            for(size_t z=0;z<result.items.size();++z){const Intent& in=result.items[z];std::string p,b;Resolve(npc,in.slot,p);Check(loader.Load(p,b,error),"actual table load");Table table=Parse(b);char key[24];sprintf(key,"%d",in.row);const Section* row=Find(table,key);int g,d,pt;Get(row,"Genre",g);Get(row,"Detail",d);Get(row,"Particular",pt);
                PhongThanQuestItemTuple tuple;Check(adapter.Map(g,d,pt,tuple),"exact native identity");tuple.level=in.level;int item=0;Check(adapter.GenerateMapped(tuple,item,error),"actual native equipment generation");
                if(item){Check(Item[item].GetLevel()==in.level,"actual native Level matches intent");
                    if(g==0){int required=-1;Check(catalog.EquipmentRequirement(d,pt,in.level,required),"catalog equipment requirement");Check(required<=c.npcLevel&&in.level>=low&&in.level<=high,"monster valid requirement and tier");
                        int actual=0;for(int rq=0;rq<6;++rq){const KItemNormalAttrib& req=*(KItemNormalAttrib*)Item[item].GetRequirement(rq);if(req.nAttribType==magic_requirelevel&&req.nValue[0]>actual)actual=req.nValue[0];}Check(actual==required,"generated requirement agrees with catalog");
                        if(in.level<minLevel)minLevel=in.level;if(in.level>maxLevel)maxLevel=in.level;if(actual>maxRequired)maxRequired=actual;++count;
                    }ItemSet.Remove(item);
                }
            }
        }
        Check(count>0,"actual equipment hit samples");printf("MONSTER_NATIVE npc=18 npcLevel=%d bounds=%d..%d generated=%d actualLevel=%d..%d maxRequired=%d\n",c.npcLevel,low,high,count,minLevel,maxLevel,maxRequired);
    }
    int lo,hi;Check(!MonsterEquipmentResolver::Bounds(20,1,10,0,10,false,0,lo,hi,error),"zero divisor rejects");
    Check(!MonsterEquipmentResolver::Bounds(20,1,10,10,20,false,0,lo,hi,error),"inverted divisor rejects");
    Check(!MonsterEquipmentResolver::Bounds(20,1,10,20,10,true,10,lo,hi,error),"explicit high equipment rejects instead of clamping");
    Check(MonsterEquipmentResolver::Bounds(20,1,10,20,10,true,2,lo,hi,error)&&lo==2&&hi==2,"explicit valid equipment preserved");
    // Real native non-equipment catalog identities. Differential Draw traces
    // must remain identical to MAIN_CATALOG at every monster level.
    const int genres[13]={1,3,4,5,6,7,3,3,3,3,3,3,3};
    for(int gi=0;gi<13;++gi){int g=genres[gi],d=gi>=6?15+gi-6:-1,pt=gi>=6?0:-1;
        for(int dd=0;dd<256&&d<0;++dd)for(int pp=0;pp<256;++pp){bool any=false;for(int lv=0;lv<=10;++lv)if(catalog.Has(g,dd,pp,lv)){any=true;break;}if(any){d=dd;pt=pp;break;}}
        Check(d>=0,"real non equipment tuple found");if(d<0)continue;
        char bytes[1024];sprintf(bytes,"[Main]\nCount=1\nRandRange=100\nMagicRate=1\nMoneyRate=0\nMoneyScale=0\nMinItemLevel=1\nMaxItemLevel=10\nMinItemLevelScale=20\nMaxItemLevelScale=10\n[1]\nGenre=%d\nDetail=%d\nParticular=%d\nRandRate=100\n",g,d,pt);
        Table t=Parse(bytes);NpcSnapshot n;n.templateId=18;n.treasure=n.treasure1=1;n.slots[0]="a.ini";n.slots[1]="b.ini";Quarantine empty;
        for(int li=0;li<5;++li){Config a;a.profile=PROFILE_A;a.treasure=SUCCESS_CAP;a.level=EXPERIMENTAL_MAIN_CATALOG;a.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;a.money=SYNTHETIC_FLAT_BERNOULLI;a.npcLevel=levels[li];Config b=a;b.level=EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG;Plan p1,p2;Check(p1.Prepare(n,q,a,t,t,catalog,error),"non equipment main plan");Check(p2.Prepare(n,q,b,t,t,catalog,error),"non equipment monster plan");
            Random r1(17),r2(17);for(int trial=0;trial<10;++trial){Result x,y;Check(p1.Draw(r1,x,error)&&p2.Draw(r2,y,error)&&x.items.size()==y.items.size(),"non equipment differential counts");for(size_t z=0;z<x.items.size()&&z<y.items.size();++z){Check(x.items[z].level==y.items[z].level,"non equipment levels unchanged");PhongThanQuestItemTuple tuple;Check(adapter.Map(g,d,pt,tuple),"non equipment identity");tuple.level=y.items[z].level;int item=0;Check(adapter.GenerateMapped(tuple,item,error),"non equipment actual native generation");if(item)ItemSet.Remove(item);}Check(r1.Next()==r2.Next(),"non equipment RNG trace unchanged");}
        }printf("NON_EQUIPMENT_DIFFERENTIAL genre=%d detail=%d particular=%d levels=5 PASS\n",g,d,pt);
    }
}
