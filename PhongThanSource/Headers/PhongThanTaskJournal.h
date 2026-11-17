#ifndef PHONGTHAN_TASK_JOURNAL_H
#define PHONGTHAN_TASK_JOURNAL_H
#include <stdio.h>
#include <string.h>
// Cleanup snapshots are synchronization, not player completion events.
inline bool PhongThanTaskShouldNotify(int header,int action,const char* text){
    if((header>=3000&&header<=3099)||(header>=4001&&header<=4004)||!text)return false;
    while(*text==' '||*text=='\r'||*text=='\n')++text;
    return *text&&(action==0||action==2);
}
// Terminal reward steps from the active original ui3/taskinfo.ini.
// A request to return for a reward remains active (notably tasks42/852...).
inline bool PhongThanTaskRewardStep(int task,int step)
{
    static const int endings[][2]={{1,8},{2,4},{3,1},{4,1},{5,4},{7,10},{8,8},{9,1},
        {10,3},{11,10},{13,8},{14,5},{15,1},{16,6},{17,7},{22,5},{25,3},{26,5},
        {30,42},{32,42},{35,39},{37,1},{38,1},{39,2},{40,2},{41,2},
        {43,6},{44,6},{45,6},{46,6},{47,9}};
    for(int i=0;i<sizeof(endings)/sizeof(endings[0]);++i)
        if(endings[i][0]==task && endings[i][1]==step)return true;
    return false;
}
inline int PhongThanTaskRecordId(unsigned int header,const char* text)
{
    int task=-1,step=0;
    if(text && sscanf(text,"Task %d - step %d",&task,&step)==2 && task>=0 && task<4096 &&
       ((unsigned int)task==header || (unsigned int)(task%36)==header))return task;
    return header<4096?(int)header:-1;
}
inline int PhongThanTaskRecordStep(int task,unsigned int reserved,const char* text)
{
    int parsed=-1,step=0;
    if(text && sscanf(text,"Task %d - step %d",&parsed,&step)==2 && parsed==task)return step;
    return (int)reserved;
}
// Substitute only the two placeholder kinds present in the original registry.
// Never use player/script strings as a printf format.
inline void PhongThanFormatTaskStep(char* output,int capacity,const char* format,const char* record)
{
    const char* args=record?strchr(record,':'):0;
    if(args){++args;while(*args==' ')++args;}
    int used=0;
    while(*format && used<capacity-1)
    {
        if(format[0]=='%' && (format[1]=='s' || format[1]=='d') && args && *args)
        {
            const char* end=strstr(args,", ");if(!end)end=args+strlen(args);
            while(args<end && used<capacity-1)output[used++]=*args++;
            args=*end?end+2:end;format+=2;
        }
        else output[used++]=*format++;
    }
    output[used]=0;
}
#endif
