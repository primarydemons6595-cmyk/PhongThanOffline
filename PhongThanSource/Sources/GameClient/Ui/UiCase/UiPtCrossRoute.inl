#include "../Elem/WndList.h"
#include "../../../Headers/PhongThanCrossRoute.h"
class PTCrossRoute : public KWndImage {
    struct Map {char Key[80],Name[80];int Id,EndX,EndY;};
    struct Edge {int From,To,X,Y;};
    Map maps[128];Edge edges[512];int mapCount,edgeCount,currentMap,destination,stepMap;
    int path[128],pathCount,pathStep;
    bool running,finishing;int finalX,finalY;DWORD lastRetry;
    KWndList source,target;KWndScrollBar sourceScroll,targetScroll;KWndImage scrollFrames[2];KWndButton scrollUp[2],scrollDown[2];
    KWndButton close,search;KWndText512 info,result;KWndText256 status;
    static PTCrossRoute* self;
    int Index(int runtime){for(int i=0;i<mapCount;++i)if(maps[i].Id==runtime)return i;return -1;}
    int EdgeIndex(int from,int to){for(int i=0;i<edgeCount;++i)if(edges[i].From==from&&edges[i].To==to)return i;return -1;}
    int Plan(int from,int to,int* out,int skip=-1){
        int previous[128],queue[128],head=0,tail=0;
        for(int i=0;i<128;++i)previous[i]=-1;
        previous[from]=from;queue[tail++]=from;
        while(head<tail){int node=queue[head++];if(node==to)break;
            for(int e=0;e<edgeCount;++e)if(e!=skip&&edges[e].From==node&&previous[edges[e].To]<0){
                previous[edges[e].To]=node;queue[tail++]=edges[e].To;
            }
        }
        if(previous[to]<0)return 0;
        int reverse[128],n=0,node=to;while(n<128){reverse[n++]=node;if(node==from)break;node=previous[node];}
        for(int p=0;p<n;++p)out[p]=reverse[n-p-1];return n;
    }
    void Start(bool move){
        running=false;finishing=false;
        int from=source.GetCurSel(),to=target.GetCurSel();if(from<0||to<0||from>=mapCount||to>=mapCount)return;
        if(move&&(maps[to].EndX<=0||maps[to].EndY<=0)){status.SetText("Khu v\371c n\265y c\307n NPC ho\306c \256i\322u ki\326n ri\252ng \256\323 \256i t\355i.");return;}
        pathCount=Plan(from,to,path);pathStep=0;
        if(!pathCount){result.SetText("Kh\253ng c\343 \256\255\352ng li\252n khu v\371c \256\267 x\270c minh.");return;}
        char text[512];text[0]=0;
        for(int i=0;i<pathCount;++i){if(strlen(text)+strlen(maps[path[i]].Name)+5>=sizeof(text))break;if(i)strcat(text," > ");strcat(text,maps[path[i]].Name);}
        result.SetText(text);
        if(!move)return;
        if(maps[from].Id!=currentMap){status.SetText("Ch\344n \256i\323m \256i l\265 khu v\371c hi\326n t\271i.");return;}
        destination=to;finalX=maps[to].EndX*32+16;finalY=maps[to].EndY*32+16;stepMap=-1;running=true;finishing=false;Advance(currentMap);
    }
    void Advance(int runtime){
        if(runtime<=0)return;
        currentMap=runtime;if(!running)return;
        if(finishing){
            PTCrossRoutePosition pos;if(!g_pCoreShell->OperationRequest(PT_GOI_CROSS_ROUTE_STATUS,(unsigned int)&pos,0))return;
            if(pos.MapId!=maps[destination].Id){running=false;finishing=false;return;}
            __int64 dx=pos.X-finalX,dy=pos.Y-finalY;
            if(dx*dx+dy*dy<=128*128){running=false;finishing=false;status.SetText("Da den dung NPC dich cuoi.");return;}
            if(!pos.Moving&&(DWORD)(GetTickCount()-lastRetry)>=1000){lastRetry=GetTickCount();PTCrossRouteStep retry;retry.MapId=pos.MapId;retry.X=finalX;retry.Y=finalY;retry.Portal=0;g_pCoreShell->OperationRequest(PT_GOI_CROSS_ROUTE_STEP,(unsigned int)&retry,0);}
            return;
        }
        if(runtime==stepMap){
            if(pathStep<pathCount&&(DWORD)(GetTickCount()-lastRetry)>=1000){PTCrossRoutePosition pos;
                if(g_pCoreShell->OperationRequest(PT_GOI_CROSS_ROUTE_STATUS,(unsigned int)&pos,0)&&pos.MapId==runtime&&!pos.Moving){
                    int edge=pathStep>0?EdgeIndex(path[pathStep-1],path[pathStep]):-1;
                    if(edge>=0){PTCrossRouteStep retry;retry.MapId=runtime;retry.X=edges[edge].X*32+16;retry.Y=edges[edge].Y*32+16;retry.Portal=1;g_pCoreShell->OperationRequest(PT_GOI_CROSS_ROUTE_STEP,(unsigned int)&retry,0);lastRetry=GetTickCount();}
                }
            }return;
        }
        if(pathStep>=pathCount||maps[path[pathStep]].Id!=runtime){running=false;status.SetText("\247\267 d\365ng: khu v\371c thay \256\346i ngo\265i l\351 tr\327nh.");return;}
        int x,y;
        if(pathStep+1<pathCount){int e=EdgeIndex(path[pathStep],path[pathStep+1]);if(e<0){running=false;return;}x=edges[e].X;y=edges[e].Y;}
        else {x=maps[destination].EndX;y=maps[destination].EndY;}
        PTCrossRouteStep command;command.MapId=runtime;command.X=pathStep+1<pathCount?x*32+16:finalX;command.Y=pathStep+1<pathCount?y*32+16:finalY;
        if(command.X<=0||command.Y<=0||command.X>1048575||command.Y>1048575){running=false;status.SetText("T\344a \256\351 \256\335ch kh\253ng h\356p l\326.");return;}
        command.Portal=pathStep+1<pathCount?1:0;
        int accepted=g_pCoreShell->OperationRequest(PT_GOI_CROSS_ROUTE_STEP,(unsigned int)&command,0);
        if(accepted<0)return; // map metadata can arrive before the local actor enters it
        if(!accepted){
            running=false;status.SetText("Kh\253ng th\323 b\276t \256\307u t\327m \256\255\352ng t\271i khu v\371c n\265y.");return;
        }
        stepMap=runtime;lastRetry=GetTickCount();++pathStep;
        if(pathStep==pathCount){finishing=true;status.SetText("\247ang \256i \256\325n m\364c ti\252u cu\350i.");}
        else status.SetText("\247ang t\371 \256\351ng \256i xuy\252n khu v\371c. Space \256\323 d\365ng.");
    }
    bool Initialize(){
        char scheme[256],file[384];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));sprintf(file,"%s\\UiPtCrossRoute.ini",scheme);KIniFile ini;
        if(!ini.Load(file)||!Init(&ini,"Main"))return false;
        source.Init(&ini,"ListSrc");target.Init(&ini,"ListDest");sourceScroll.Init(&ini,"ScrollSrc_Bar");targetScroll.Init(&ini,"ScrollDest_Bar");
        close.Init(&ini,"BtnClose");search.Init(&ini,"BtnSearch");info.Init(&ini,"TxtInfo");result.Init(&ini,"TxtResult");status.Init(&ini,"TxtSearchInfo");
        AddChild(&source);AddChild(&target);AddChild(&sourceScroll);AddChild(&targetScroll);AddChild(&close);AddChild(&search);AddChild(&info);AddChild(&result);AddChild(&status);
        KWndScrollBar* bars[]={&sourceScroll,&targetScroll};
        const char* scrollNames[]={"ScrollSrc","ScrollDest"};
        for(int sc=0;sc<2;++sc){
            char key[64];int x=0,y=0,dx=0,dy=0;
            ini.GetInteger(scrollNames[sc],"Left",0,&x);ini.GetInteger(scrollNames[sc],"Top",0,&y);
            sprintf(key,"%s_Bar",scrollNames[sc]);ini.GetInteger(key,"Left",0,&dx);ini.GetInteger(key,"Top",0,&dy);bars[sc]->SetPosition(x+dx,y+dy);
            sprintf(key,"%s_BtnUp",scrollNames[sc]);scrollUp[sc].Init(&ini,key);AddChild(&scrollUp[sc]);ini.GetInteger(key,"Left",0,&dx);ini.GetInteger(key,"Top",0,&dy);scrollUp[sc].SetPosition(x+dx,y+dy);
            sprintf(key,"%s_BtnDown",scrollNames[sc]);scrollDown[sc].Init(&ini,key);AddChild(&scrollDown[sc]);ini.GetInteger(key,"Left",0,&dx);ini.GetInteger(key,"Top",0,&dy);scrollDown[sc].SetPosition(x+dx,y+dy);
        }
        source.SetScrollbar(&sourceScroll);target.SetScrollbar(&targetScroll);
        ini.GetInteger("MapInfo","MapCount",0,&mapCount);if(mapCount<1||mapCount>128)return false;
        for(int i=0;i<mapCount;++i){char key[40];sprintf(key,"%d",i);ini.GetString("MapInfo",key,"",maps[i].Key,sizeof(maps[i].Key));
            sprintf(key,"%d_ShowName",i);ini.GetString("MapInfo",key,"",maps[i].Name,sizeof(maps[i].Name));if(maps[i].Name[0]=='$')memmove(maps[i].Name,maps[i].Name+1,strlen(maps[i].Name));
            sprintf(key,"%d_MapId",i);ini.GetInteger("MapInfo",key,-1,&maps[i].Id);maps[i].Id+=1000;
            sprintf(key,"%d_EndX",i);ini.GetInteger("MapInfo",key,-1,&maps[i].EndX);sprintf(key,"%d_EndY",i);ini.GetInteger("MapInfo",key,-1,&maps[i].EndY);
        }
        edgeCount=0;
        for(int from=0;from<mapCount;++from){int count=0;ini.GetInteger(maps[from].Key,"MapCount",0,&count);if(count<0||count>128)return false;
            for(int n=0;n<count&&edgeCount<512;++n){char key[40],dest[80];sprintf(key,"%d",n);ini.GetString(maps[from].Key,key,"",dest,sizeof(dest));
                for(int to=0;to<mapCount;++to)if(!strcmp(maps[to].Key,dest)){
                    Edge e;e.From=from;e.To=to;sprintf(key,"%d_TrapX",n);ini.GetInteger(maps[from].Key,key,-1,&e.X);sprintf(key,"%d_TrapY",n);ini.GetInteger(maps[from].Key,key,-1,&e.Y);
                    if(e.X>0&&e.Y>0)edges[edgeCount++]=e;break;
                }
            }
        }
        source.SetContent((BYTE*)maps,mapCount,sizeof(Map),(int)((char*)maps[0].Name-(char*)maps));
        target.SetContent((BYTE*)maps,mapCount,sizeof(Map),(int)((char*)maps[0].Name-(char*)maps));
        int sw,sh;Wnd_GetScreenSize(sw,sh);SetPosition(max(0,(sw-m_Width)/2),max(0,(sh-m_Height)/2));
        running=false;stepMap=-1;currentMap=0;Wnd_AddWindow(this);return true;
    }
public:
    static int Navigate(int map,int x,int y){
        if(x<=0||y<=0||x>1048575||y>1048575||!g_pCoreShell)return 0;
        if(!self){self=new PTCrossRoute;if(!self->Initialize()){delete self;self=NULL;return 0;}}
        PTCrossRoutePosition position;if(!g_pCoreShell->OperationRequest(PT_GOI_CROSS_ROUTE_STATUS,(unsigned int)&position,0))return 0;
        int from=self->Index(position.MapId),to=self->Index(map);if(from<0||to<0)return 0;
        int count=self->Plan(from,to,self->path);if(!count)return 0;
        self->pathCount=count;self->pathStep=0;self->currentMap=position.MapId;self->destination=to;self->finalX=x;self->finalY=y;self->stepMap=-1;self->running=true;self->finishing=false;self->Advance(position.MapId);return 1;
    }
    static void Open(int map){if(!self){self=new PTCrossRoute;if(!self->Initialize()){delete self;self=NULL;return;}}self->currentMap=map;self->source.SetCurSel(self->Index(map));self->Show();self->BringToTop();}
    static void Cancel(){if(self){self->running=false;self->stepMap=-1;}if(g_pCoreShell){g_pCoreShell->SetAutoRun(FALSE);g_pCoreShell->SetFlagAutoRun(FALSE,0,0);g_pCoreShell->SetPaintMode(FALSE);}}
    static void Reset(){if(self){self->running=false;self->currentMap=0;self->stepMap=-1;self->Hide();}}
    static void Update(int map){if(self){if(GetAsyncKeyState(VK_SPACE)&0x8000){Cancel();self->status.SetText("\247\267 d\365ng t\327m \256\255\352ng.");}else self->Advance(map);}}
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WND_N_BUTTON_CLICK){for(int sc=0;sc<2;++sc){if(param==(unsigned int)(KWndWindow*)&scrollUp[sc]){(sc?targetScroll:sourceScroll).ScrollLine(true);return 1;}if(param==(unsigned int)(KWndWindow*)&scrollDown[sc]){(sc?targetScroll:sourceScroll).ScrollLine(false);return 1;}}if(param==(unsigned int)(KWndWindow*)&close){Hide();return 1;}if(param==(unsigned int)(KWndWindow*)&search){Start(true);return 1;}}
        if(msg==WND_N_BUTTON_LCLICK&&param==(unsigned int)(KWndWindow*)&search){Start(true);return 1;}
        if(msg==WND_N_SCORLLBAR_POS_CHANGED){if(param==(unsigned int)(KWndWindow*)&sourceScroll){source.SetTopItemIndex(data);return 1;}if(param==(unsigned int)(KWndWindow*)&targetScroll){target.SetTopItemIndex(data);return 1;}}
        if(msg==WND_N_LIST_ITEM_SEL){Start(false);return 1;}
        if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Hide();return 1;}return KWndImage::WndProc(msg,param,data);
    }
};
PTCrossRoute* PTCrossRoute::self=NULL;
