#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#pragma comment(lib,"user32.lib")
#include "LegacyDropEngine.h"
using namespace LegacyDrop;
static int checks,failed;
static std::string unitFixturePath;
// Bernstein bound with fixed family budget M=50000 and alpha=1e-6.
// Unlike a normal z cutoff, this remains valid for rare Bernoulli events.
static bool BinomialOK(int hits,int n,double p) {
    if(p==0)return hits==0;if(p==1)return hits==n;
    const double L=log(2.0*50000/1e-6);
    return fabs(hits-n*p)<=sqrt(2*n*p*(1-p)*L)+2*L/3;
}
#define CHECK(x) do{++checks;if(!(x)){++failed;printf("FAIL line=%d %s\n",__LINE__,#x);}}while(0)
struct PureDataFixture {int calls;PureDataFixture():calls(0){}bool Load(const std::string&,std::string&,std::string& error){++calls;error="MISSING_TABLE";return false;}};
static void UnitTests() {
    const char* good="[Main]\nCount=2\nRandRange=10\nMagicRate=1\nUnknownField=keep\n[1]\nGenre=0\nDetail=0\nParticular=0\nRandRate=7\n[2]\nGenre=3\nDetail=15\nParticular=0\nRandRate=8\n";
    Table t=Parse(good);CHECK(t.errors.empty());CHECK(Find(t,"Main")->fields.find("UnknownField")->second=="keep");
    PreparedTable prepared;std::string error;std::vector<int> selected;Random safe(123),same(123);unsigned long result=77;
    CHECK(!safe.TryBelow(0,result)&&result==77);CHECK(safe.Next()==same.Next());
    CHECK(prepared.Prepare(t,error));CHECK(prepared.Draw(safe,selected,error));
    CHECK(!prepared.Prepare(Parse("[Main]\nCount=2147483647\nRandRange=10\n"),error));
    CHECK(!prepared.Draw(safe,selected,error)&&selected.empty());
    CHECK(!Parse(std::string(2*1024*1024+1,'x')).errors.empty());
    CHECK(!Parse(std::string("[Main]\0hidden",13)).errors.empty());
    CHECK(!Parse(std::string("\xff\xfe",2)).errors.empty());
    Random fuzz(314159);for(int f=0;f<10000;++f){std::string input;for(unsigned long c=0,length=fuzz.Below(256);c<length;++c)input+=(char)fuzz.Below(256);Table garbage=Parse(input);PreparedTable plan;if(plan.Prepare(garbage,error))CHECK(plan.Draw(fuzz,selected,error));CHECK(_heapchk()==_HEAPOK);}
    CHECK(!Parse(std::string(good)+"[1]\nGenre=0\n").errors.empty());
    CHECK(!Parse("[Main]\nCount=1\nRandRange=1\n").errors.empty());
    CHECK(!Parse("[Main]\nCount=0\nRandRange=0\n").errors.empty());
    CHECK(!Parse("[Main]\nCount=-1\nRandRange=2\n").errors.empty());
    CHECK(!Parse("[Main]\nCount=0\nRandRange=1\nCount=1\n").errors.empty());
    int n;CHECK(!Integer("2147483648",n));CHECK(!Integer("1x",n));CHECK(!Integer("+",n));
    for(int range=1;range<50;++range)for(int rate=0;rate<=range;++rate){int hits=0;for(int r=0;r<range;++r)hits+=Selected(rate,range,r);CHECK(hits==rate);}
    CHECK(!Selected(2,1,0));CHECK(!Selected(1,1,-1));CHECK(!Selected(1,1,1));
    NpcSnapshot snapshot;snapshot.templateId=82;snapshot.entity=123;snapshot.treasure=80;snapshot.treasure1=10;
    snapshot.slots[0]="a.ini";snapshot.slots[1]="b.ini";snapshot.slots[2]="a.ini";snapshot.slots[3]="c.ini";
    NpcSnapshot saved=snapshot;snapshot.slots[0]="freed";
    std::string path;CHECK(Resolve(saved,0,path)&&path=="\\settings\\item\\droprate\\a.ini");CHECK(Resolve(saved,2,path)&&path=="\\settings\\item\\droprate\\a.ini");CHECK(!Resolve(saved,4,path));
    const char* badPaths[]={"../a.ini","C:\\a.ini","\\settings\\item\\droprate\\..\\a.ini","a?.ini","a*.ini","a|.ini","a\n.ini"};
    for(int bad=0;bad<7;++bad){NpcSnapshot invalid=saved;invalid.slots[0]=badPaths[bad];CHECK(!Resolve(invalid,0,path));}
    NpcSnapshot embedded=saved;embedded.slots[0]=std::string("a.ini\0suffix",12);CHECK(!Resolve(embedded,0,path));
    Quarantine quarantine;CHECK(!quarantine.Allows(saved,error));CHECK(!quarantine.Load(NULL,error));
    FILE* qfile=fopen(unitFixturePath.c_str(),"w+b");CHECK(qfile!=NULL);if(!qfile)return;fputs("V1\nT \\settings\\item\\droprate\\b.ini\n",qfile);rewind(qfile);CHECK(quarantine.Load(qfile,error));fclose(qfile);CHECK(!quarantine.Allows(saved,error));
    qfile=fopen(unitFixturePath.c_str(),"w+b");CHECK(qfile!=NULL);if(!qfile)return;fputs("V1\n",qfile);rewind(qfile);CHECK(quarantine.Load(qfile,error));fclose(qfile);CHECK(quarantine.Allows(saved,error));
    qfile=fopen(unitFixturePath.c_str(),"w+b");CHECK(qfile!=NULL);if(!qfile)return;fputs("V1\nN 82\nN 82\n",qfile);rewind(qfile);CHECK(!quarantine.Load(qfile,error));fclose(qfile);CHECK(!quarantine.Allows(saved,error));
    PureDataFixture fixture;CHECK(prepared.Prepare(t,error));CHECK(!BuildLabPlan(saved,0,quarantine,fixture,prepared,error));CHECK(fixture.calls==0);CHECK(!prepared.Draw(safe,selected,error)&&selected.empty());
    CHECK(prepared.Prepare(t,error));CHECK(!BuildProductionPlan(prepared,error));CHECK(error=="PRODUCTION_POLICY_NOT_APPROVED");CHECK(!prepared.Draw(safe,selected,error));
    DeathToken life;CHECK(life.Consume());CHECK(!life.Consume());DeathToken respawn;CHECK(respawn.Consume());
    Random a(82),b(82);for(int k=0;k<10000;++k)CHECK(a.Below(150000)==b.Below(150000));
    // Analytic multiplicity/no-drop expectations for overflow-sum table.
    for(int size=10000;size<=1000000;size*=10){Random rng(20261017);int hit[2]={0,0},zero=0,multi=0;
        for(int i=0;i<size;++i){bool x=Selected(7,10,rng.Below(10)),y=Selected(8,10,rng.Below(10));hit[0]+=x;hit[1]+=y;zero+=!x&&!y;multi+=x&&y;}
        double p[]={.7,.8,.06,.56};int observed[]={hit[0],hit[1],zero,multi};
        for(int j=0;j<4;++j){double z=(observed[j]-size*p[j])/sqrt(size*p[j]*(1-p[j]));CHECK(BinomialOK(observed[j],size,p[j]));printf("SYNTHETIC n=%d metric=%d expected=%.9f observed=%.9f z=%.6f\n",size,j,p[j],double(observed[j])/size,z);}
    }
}
int main(int argc,char** argv) {
    if(argc!=4){puts("usage: LegacyDropNativeTest drop-directory output.csv simulation-size");return 2;}
    unitFixturePath=std::string(argv[2])+".unit_quarantine.txt";
    UnitTests();
    wchar_t directory[2048],pattern[2100];MultiByteToWideChar(CP_UTF8,0,argv[1],-1,directory,2048);wsprintfW(pattern,L"%s\\*.ini",directory);
    WIN32_FIND_DATAW data;HANDLE h=FindFirstFileW(pattern,&data);if(h==INVALID_HANDLE_VALUE)return 3;
    FILE* out=fopen(argv[2],"wb");if(!out)return 4;
    fprintf(out,"file,section,kind,deaths,expected,observed,z,status\n");
    int files=0,rejected=0,rows=0,size=atoi(argv[3]);if(size!=10000&&size!=100000&&size!=1000000)return 5;
    do {
        if(data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)continue;
        wchar_t full[2100];wsprintfW(full,L"%s\\%s",directory,data.cFileName);
        FILE* f=_wfopen(full,L"rb");if(!f){++failed;continue;}
        std::string bytes;char chunk[4096];size_t got;while((got=fread(chunk,1,sizeof(chunk),f))!=0)bytes.append(chunk,got);fclose(f);
        char name[2048];WideCharToMultiByte(CP_UTF8,0,data.cFileName,-1,name,sizeof(name),0,0);
        Table table=Parse(bytes);++files;
        if(!table.errors.empty()){++rejected;for(size_t e=0;e<table.errors.size();++e)fprintf(out,"\"%s\",,LOADER,0,0,0,0,%s\n",name,table.errors[e].c_str());continue;}
        Random rng(20261017);std::vector<int> rates,hits;std::vector<std::string> names;
        for(size_t s=0;s<table.sections.size();++s){int rate;if(Get(&table.sections[s],"RandRate",rate)){rates.push_back(rate);hits.push_back(0);names.push_back(table.sections[s].name);}}
        PreparedTable prepared;std::string error;CHECK(prepared.Prepare(table,error));std::vector<int> selection;
        double p0=1,mean=0,var=0;int zero=0;double countSum=0;int quantity[4]={0,0,0,0};
        std::vector<double> distribution(rates.size()+1,0);distribution[0]=1;
        for(size_t j=0;j<rates.size();++j){double p=double(rates[j])/table.range;p0*=1-p;mean+=p;var+=p*(1-p);}
        for(j=0;j<rates.size();++j){double p=double(rates[j])/table.range;for(int k=(int)j+1;k>=0;--k)distribution[k]=distribution[k]*(1-p)+(k?distribution[k-1]*p:0);}
        for(int trial=0;trial<size;++trial){if(!prepared.Draw(rng,selection,error)){CHECK(false);break;}int count=(int)selection.size();for(size_t pick=0;pick<selection.size();++pick)++hits[selection[pick]];zero+=count==0;countSum+=count;++quantity[count<3?count:3];}
        for(j=0;j<rates.size();++j){double p=double(rates[j])/table.range,z=p>0&&p<1?(hits[j]-size*p)/sqrt(size*p*(1-p)):0;bool ok=BinomialOK(hits[j],size,p);CHECK(ok);++rows;
            fprintf(out,"\"%s\",%s,ROW,%d,%.12f,%.12f,%.8f,%s\n",name,names[j].c_str(),size,p,double(hits[j])/size,z,ok?"FUNCTIONAL_PASS":"FAIL");}
        double z=p0>0&&p0<1?(zero-size*p0)/sqrt(size*p0*(1-p0)):0;bool ok=BinomialOK(zero,size,p0);CHECK(ok);
        fprintf(out,"\"%s\",,NO_DROP,%d,%.12f,%.12f,%.8f,%s\n",name,size,p0,double(zero)/size,z,ok?"FUNCTIONAL_PASS":"FAIL");
        double mz=var>0?(countSum-size*mean)/sqrt(size*var):0;
        bool meanOK=var>0?fabs(countSum-size*mean)<=sqrt(2*size*var*log(1e11))+2*log(1e11)/3:countSum==size*mean;CHECK(meanOK);
        fprintf(out,"\"%s\",,ITEM_COUNT_MEAN,%d,%.12f,%.12f,%.8f,%s\n",name,size,mean,countSum/size,mz,meanOK?"FUNCTIONAL_PASS":"FAIL");
        double quantityP[4]={0,0,0,0};for(size_t bin=0;bin<distribution.size();++bin)quantityP[bin<3?bin:3]+=distribution[bin];
        for(int q=0;q<4;++q){double p=quantityP[q];if(p>1&&p<1+1e-12)p=1;bool qok=BinomialOK(quantity[q],size,p);CHECK(qok);fprintf(out,"\"%s\",%d,QUANTITY_BIN,%d,%.12f,%.12f,0,%s\n",name,q,size,p,double(quantity[q])/size,qok?"FUNCTIONAL_PASS":"FAIL");}
    }while(FindNextFileW(h,&data));FindClose(h);fclose(out);
    CHECK(_heapchk()==_HEAPOK);
    printf("NATIVE_RESULT files=%d rejected=%d rows=%d checks=%d failed=%d seed=20261017 policy=LAB_INDEPENDENT_ROWS_V1 parity=PARITY_UNVERIFIED fuzz_cases=10000 quantity_bins=0,1,2,3plus\n",files,rejected,rows,checks,failed);
    return failed?1:0;
}
