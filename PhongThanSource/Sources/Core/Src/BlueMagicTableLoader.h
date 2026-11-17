#ifndef PT_BLUE_MAGIC_TABLE_LOADER_H
#define PT_BLUE_MAGIC_TABLE_LOADER_H
#include "LegacyDropNativeLoader.h"
#include <wincrypt.h>
namespace BlueMagic {
inline bool Sha256(const std::string& bytes,std::string& hex){
    hex="";HMODULE lib=LoadLibraryA("advapi32.dll");if(!lib)return false;
    typedef BOOL (WINAPI *Acquire)(HCRYPTPROV*,LPCSTR,LPCSTR,DWORD,DWORD);
    typedef BOOL (WINAPI *Create)(HCRYPTPROV,ALG_ID,HCRYPTKEY,DWORD,HCRYPTHASH*);
    typedef BOOL (WINAPI *Hash)(HCRYPTHASH,const BYTE*,DWORD,DWORD);
    typedef BOOL (WINAPI *Get)(HCRYPTHASH,DWORD,BYTE*,DWORD*,DWORD);
    typedef BOOL (WINAPI *Destroy)(HCRYPTHASH);typedef BOOL (WINAPI *Release)(HCRYPTPROV,DWORD);
    Acquire acquire=(Acquire)GetProcAddress(lib,"CryptAcquireContextA");Create create=(Create)GetProcAddress(lib,"CryptCreateHash");
    Hash hash=(Hash)GetProcAddress(lib,"CryptHashData");Get get=(Get)GetProcAddress(lib,"CryptGetHashParam");
    Destroy destroy=(Destroy)GetProcAddress(lib,"CryptDestroyHash");Release release=(Release)GetProcAddress(lib,"CryptReleaseContext");
    HCRYPTPROV provider=0;HCRYPTHASH digest=0;BYTE output[32];DWORD size=sizeof(output);bool ok=false;
    if(acquire&&create&&hash&&get&&destroy&&release&&acquire(&provider,0,0,24,CRYPT_VERIFYCONTEXT)){
        if(create(provider,0x800c,0,0,&digest)){
            ok=hash(digest,(const BYTE*)bytes.c_str(),bytes.size(),0)&&get(digest,HP_HASHVAL,output,&size,0)&&size==32;
            destroy(digest);
        }release(provider,0);
    }
    FreeLibrary(lib);if(!ok)return false;char b[3];for(int i=0;i<32;++i){sprintf(b,"%02x",output[i]);hex+=b;}return true;
}
typedef std::vector<std::string> Cells;
inline Cells Split(const std::string& s,char delimiter){Cells c;size_t begin=0;for(;;){size_t end=s.find(delimiter,begin);c.push_back(s.substr(begin,end==std::string::npos?end:end-begin));if(end==std::string::npos)break;begin=end+1;}return c;}
inline bool Lines(const std::string& bytes,std::vector<Cells>& rows,std::string& error){
    rows.clear();if(bytes.empty()||bytes.find('\0')!=std::string::npos){error="BLUE_INVALID_TSV_BYTES";return false;}
    Cells lines=Split(bytes,'\n');for(size_t i=0;i<lines.size();++i){std::string s=lines[i];if(!s.empty()&&s[s.size()-1]=='\r')s.erase(s.size()-1);
        if(s.empty()){if(i+1==lines.size())continue;error="BLUE_EMPTY_TSV_ROW";return false;}rows.push_back(Split(s,'\t'));}return true;
}
struct Row {
    int sourceRow,prefix,level,type,minimum[3],maximum[3],weights[3][11];
    std::string element,reason;Cells raw;
    Row():sourceRow(0),prefix(-1),level(-1),type(-1){}
};
class BlueMagicTableLoader {
public:
    std::vector<Row> rows;Cells header;std::string path,sha256;int quarantined;
    bool valid;
    BlueMagicTableLoader():quarantined(0),valid(false){}
    bool Parse(const std::string& bytes,std::string& error){
        rows.clear();header.clear();sha256="";quarantined=0;valid=false;std::vector<Cells> records;
        if(!Lines(bytes,records,error)||records.empty()||records[0].size()!=45){error="BLUE_HEADER_COLUMNS";return false;}
        header=records[0];if(!Sha256(bytes,sha256)){error="BLUE_SHA256_UNAVAILABLE";return false;}
        for(size_t i=1;i<records.size();++i){const Cells& c=records[i];if(c.size()!=45){error="BLUE_ROW_COLUMNS";return false;}Row r;r.sourceRow=i+1;r.raw=c;r.element=c[2];
            bool numeric=LegacyDrop::Integer(c[1],r.prefix)&&LegacyDrop::Integer(c[3],r.level)&&LegacyDrop::Integer(c[4],r.type);
            for(int p=0;p<3;++p){if(!LegacyDrop::Integer(c[5+p*2],r.minimum[p])||!LegacyDrop::Integer(c[6+p*2],r.maximum[p]))numeric=false;}
            for(int g=0;g<3;++g)for(int t=0;t<11;++t){if(!LegacyDrop::Integer(c[12+g*11+t],r.weights[g][t])||r.weights[g][t]<0)numeric=false;}
            if(!numeric)r.reason="BLUE_NONINTEGER_OR_NEGATIVE_WEIGHT";
            else if(r.prefix!=0&&r.prefix!=1)r.reason="BLUE_PREFIX_NONBOOLEAN";
            else if(r.level<1||r.level>10||r.type<=0||r.type>=357)r.reason="BLUE_ATTRIBUTE_LEVEL_OR_TYPE_UNSUPPORTED";
            else for(int k=0;k<3;++k)if(r.minimum[k]>r.maximum[k]||(double)r.maximum[k]-r.minimum[k]>=INT_MAX)r.reason="BLUE_INVALID_PARAMETER_RANGE";
            int element;if(!r.element.empty()&&(!LegacyDrop::Integer(r.element,element)||element<0||element>4))r.reason="BLUE_ELEMENT_MAPPING_UNRESOLVED";
            if(!r.reason.empty())++quarantined;rows.push_back(r);
        }
        if(rows.size()!=480){error="BLUE_EXPECTED_480_ROWS";return false;}valid=true;error="";return true;
    }
    bool Load(const std::string& source,std::string& error){LegacyDropNativeLoader loader;std::string bytes;path=source;valid=false;return loader.Load(source,bytes,error)&&Parse(bytes,error);}
};
class BlueLimits {
public:
    int caps[4][10],version;bool valid;std::string path,sha256;std::vector<Cells> raw;
    BlueLimits():version(-1),valid(false){}
    bool Parse(const std::string& bytes,std::string& error){
        raw.clear();valid=false;if(!Lines(bytes,raw,error)||raw.size()!=5){error="BLUE_LIMIT_ROWS";return false;}
        if(!Sha256(bytes,sha256)){error="BLUE_SHA256_UNAVAILABLE";return false;}
        for(int r=0;r<5;++r){if(raw[r].size()!=11){error="BLUE_LIMIT_COLUMNS";return false;}
            for(int c=1;c<=10;++c){int value;if(!LegacyDrop::Integer(raw[r][c],value)||(r==0?value!=c:value<0||value>6)){error="BLUE_LIMIT_VALUE";return false;}if(r)caps[r-1][c-1]=value;}}
        valid=true;error="";return true;
    }
    bool Load(const std::string& source,std::string& error){LegacyDropNativeLoader loader;std::string bytes;path=source;valid=false;version=source.find("\\000\\")!=std::string::npos?0:source.find("\\001\\")!=std::string::npos?1:-1;return loader.Load(source,bytes,error)&&Parse(bytes,error);}
    bool Limit(int detail,int tier,int& count,std::string& error) const {
        if(tier<1||tier>10){error="BLUE_LIMIT_TIER_UNSUPPORTED";return false;}
        // Original PAK Pendant catalogs identify detail9 as phi phong (cloak),
        // matching the cloak limit row. Ring/amulet/cuff remain unbound.
        int row=detail==0||detail==1||detail==2?0:detail==5||detail==6||detail==7?1:detail==9?2:detail==10?3:-1;
        if(row<0){error="BLUE_LIMIT_CLASS_UNRESOLVED";return false;}count=caps[row][tier-1];return true;
    }
};
}
#endif
