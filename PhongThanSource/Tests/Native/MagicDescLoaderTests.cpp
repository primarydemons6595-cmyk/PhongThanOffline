#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <vector>
#include "../../Headers/PhongThanMagicDescLoader.h"
#include "../../Sources/Core/Src/KMagicAttrib.h"
static const char* keys[]={
#define MAGIC_ATTRIB_ENTRY(id,symbol,key) key,
#include "../../Sources/Core/Src/KMagicAttribRegistry.inc"
#undef MAGIC_ATTRIB_ENTRY
};
static int resolve(char* key) {
    for(int i=0;i<magic_normal_end;++i) if(!strcmp(keys[i],key)) return i;
    const char* oldNames[]={"skill_reserve5","requirevit","requireeng","damage_reserve2","damage_reserve3","damage_reserve4","damage_reserve5"};
    const char* nativeNames[]={"skill_eventskilllevel","requirecon","requireint","damage_confuse_p","damage_stun_p","damage_freeze_p","damage_hurt_p"};
    for(i=0;i<7;++i)if(!strcmp(key,oldNames[i]))return resolve((char*)nativeNames[i]);
    return -1;
}
static char output[magic_normal_end][256];
static char expected[magic_normal_end][256];
int main(int argc,char**argv) {
    assert(argc==2);
    FILE* file=fopen(argv[1],"rb"); assert(file);
    fseek(file,0,SEEK_END);long size=ftell(file);rewind(file);
    std::vector<char> data(size+1);assert(fread(&data[0],1,size,file)==size);fclose(file);data[size]=0;
    std::string original(&data[0],size);
    assert(PhongThanLoadMagicDesc(&data[0],output,magic_normal_end,resolve)==357);
    assert(output[36][0] && output[45][0] && output[126][0] && output[146][0]);
    assert(!strcmp(keys[146],"deadlystrikeenhance_p"));
    memcpy(expected,output,sizeof(output));
    // A reordered registry must also load an INI with every key in reverse
    // order. Check every value, including intentionally empty descriptions.
    std::string shuffled="[Descript]\n";
    for(int i=magic_normal_end-1;i>=0;--i) { shuffled+=keys[i];shuffled+="=";shuffled+=expected[i];shuffled+="\n"; }
    data.assign(shuffled.begin(),shuffled.end());data.push_back(0);
    assert(PhongThanLoadMagicDesc(&data[0],output,magic_normal_end,resolve)==magic_normal_end);
    assert(!memcmp(expected,output,sizeof(output)));
    shuffled+="deadlystrikeenhance_p=last description\n";
    data.assign(shuffled.begin(),shuffled.end());data.push_back(0);
    assert(PhongThanLoadMagicDesc(&data[0],output,magic_normal_end,resolve)==magic_normal_end);
    assert(!strcmp(output[146],"last description"));
    char missing[]="[Descript]\nskill_begin=\n";
    assert(PhongThanLoadMagicDesc(missing,output,magic_normal_end,resolve)==1);
    assert(!output[146][0]);
    puts("PASS: legacy INI mapped to canonical IDs, reverse order, duplicate override and sparse keys");
    return 0;
}
