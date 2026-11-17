#ifndef PT_UI_LAYOUT_RULES_H
#define PT_UI_LAYOUT_RULES_H
// Uniform compact choice cells. Text wraps; labels never resize a button.
inline int PhongThanNpcChoiceColumns(int longest){return 3;}
inline int PhongThanNpcChoiceRows(int columns,int longest){return 2;}
inline int PhongThanNpcChoiceHeight(int columns,int longest){return 18;}
struct PTNpcButtonLayout {int PromptHeight,ChoiceTop,Rows,VisibleRows,Top,MaxScroll;};
inline PTNpcButtonLayout PhongThanNpcButtonLayout(int promptLines,int answers){
 PTNpcButtonLayout v;v.PromptHeight=promptLines*14;
 if(v.PromptHeight<28)v.PromptHeight=28;if(v.PromptHeight>84)v.PromptHeight=84;
 v.ChoiceTop=40+v.PromptHeight+8;v.Rows=(186-v.ChoiceTop+6)/34;if(v.Rows<1)v.Rows=1;
 int total=(answers+2)/3;v.VisibleRows=total<v.Rows?total:v.Rows;
 v.Top=186-v.VisibleRows*34+6;v.MaxScroll=total-v.Rows;if(v.MaxScroll<0)v.MaxScroll=0;
 return v;
}
struct PTNpcListLayout {int PromptHeight,ChoiceTop,ChoiceHeight;};
inline PTNpcListLayout PhongThanNpcListLayout(int promptLines,int footerTop){
 PTNpcListLayout layout;layout.PromptHeight=42;layout.ChoiceTop=82;
 if(promptLines>2){layout.PromptHeight=promptLines>=5?85:promptLines*17;layout.ChoiceTop=40+layout.PromptHeight+8;}
 layout.ChoiceHeight=footerTop-8-layout.ChoiceTop;if(layout.ChoiceHeight>100)layout.ChoiceHeight=100;
 return layout;
}
inline int PhongThanShopAlignedTop(int inventoryTop,int inventoryHeight,int shopHeight,int screenHeight){
    int top=inventoryTop;
    int maxTop=screenHeight-shopHeight;if(maxTop<0)maxTop=0;
    return top<0?0:top>maxTop?maxTop:top;
}
#endif
