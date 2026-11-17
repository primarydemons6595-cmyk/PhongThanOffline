#ifndef PT_UI_ATTENTION_H
#define PT_UI_ATTENTION_H
struct PTUiAttention {
    bool quest,friendAdded;
    PTUiAttention():quest(false),friendAdded(false){}
    bool Lit(bool pending,unsigned long now) const {return pending && (now/400)%2==0;}
    void Clear(){quest=friendAdded=false;}
};
#endif
