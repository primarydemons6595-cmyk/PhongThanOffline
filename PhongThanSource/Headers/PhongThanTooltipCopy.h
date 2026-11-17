#ifndef PHONGTHAN_TOOLTIP_COPY_H
#define PHONGTHAN_TOOLTIP_COPY_H
#include <string.h>
inline void PTCopyTooltipWithFooter(char* output,unsigned int capacity,const char* body,const char* footer) {
    if(!output || !capacity)return;
    unsigned int tail=(unsigned int)strlen(footer);if(tail>=capacity)tail=capacity-1;
    unsigned int available=capacity-1-tail,head=(unsigned int)strlen(body);
    if(head>available) {
        head=available;
        // Keep whole property lines and complete markup before the footer.
        while(head && body[head-1]!='\n')--head;
    }
    memcpy(output,body,head);memcpy(output+head,footer,tail);output[head+tail]=0;
}
#endif
