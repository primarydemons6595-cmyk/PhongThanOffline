// Immutable SPR metadata for the equipped-item paint path. No frame pointers
// are retained: the renderer still owns/validates the actual image allocation.
#ifndef PHONGTHAN_UI_SPRITE_CACHE_INL
#define PHONGTHAN_UI_SPRITE_CACHE_INL
#include <map>
#include <string>
struct PtUiSpriteFrame { KRPosition2 Offset,Size; bool Valid; DWORD Retry; PtUiSpriteFrame():Valid(false),Retry(0){} };
struct PtUiSpriteEntry {
 KImageParam Info; bool Valid; DWORD Retry;
 std::map<int,PtUiSpriteFrame> Frames;
 PtUiSpriteEntry():Valid(false),Retry(0){ZeroMemory(&Info,sizeof(Info));}
};
static std::map<std::string,PtUiSpriteEntry>& PtUiSpriteEntries(){static std::map<std::string,PtUiSpriteEntry> entries;return entries;}
static PtUiSpriteEntry* PtUiSprite(const char* path){
 if(!g_pRepresent||!path||!path[0])return 0;
 std::map<std::string,PtUiSpriteEntry>& entries=PtUiSpriteEntries();
 std::map<std::string,PtUiSpriteEntry>::iterator found=entries.find(path);
 if(found==entries.end()){
  // Bound the metadata cache even when many equipment variants are viewed.
  if(entries.size()>=512)entries.clear();
  entries[path]=PtUiSpriteEntry();found=entries.find(path);
 }
 PtUiSpriteEntry& e=found->second;DWORD now=GetTickCount();
 if(!e.Valid&&(!e.Retry||(int)(now-e.Retry)>=0)){
  ZeroMemory(&e.Info,sizeof(e.Info));
  e.Valid=g_pRepresent->GetImageParam(path,&e.Info,ISI_T_SPR)&&e.Info.nNumFrames>0&&e.Info.nNumFrames<=4096&&e.Info.nWidth>0&&e.Info.nHeight>0;
  e.Retry=now+5000;if(!e.Retry)e.Retry=1;
 }
 return e.Valid?&e:0;
}
static bool PtUiSpriteFrameInfo(const char* path,int frame,KRPosition2& offset,KRPosition2& size){
 PtUiSpriteEntry* e=PtUiSprite(path);if(!e||frame<0||frame>=e->Info.nNumFrames)return false;
 PtUiSpriteFrame& f=e->Frames[frame];DWORD now=GetTickCount();
 if(!f.Valid&&(!f.Retry||(int)(now-f.Retry)>=0)){
  f.Valid=g_pRepresent->GetImageFrameParam(path,frame,&f.Offset,&f.Size,ISI_T_SPR)&&f.Size.nX>0&&f.Size.nY>0;
  f.Retry=now+5000;if(!f.Retry)f.Retry=1;
 }
 if(!f.Valid)return false;offset=f.Offset;size=f.Size;return true;
}
#endif
