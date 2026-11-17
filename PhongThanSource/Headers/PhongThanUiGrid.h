#ifndef PHONGTHAN_UI_GRID_H
#define PHONGTHAN_UI_GRID_H
// Skill selectors grow upward; emoji selectors expand as two adjacent panels.
inline int PtSkillGridIndex(int x,int y,int count,int columns,int width,int height) {
    if(x<0 || y<0 || columns<=0 || width<=0 || height<=0 || count<=0)return -1;
    int rows=(count+columns-1)/columns;
    if(x>=columns*width || y>=rows*height)return -1;
    int index=(rows-1-y/height)*columns+x/width;
    return index<count?index:-1;
}
inline void PtSkillGridPosition(int index,int count,int columns,int width,int height,int& x,int& y) {
    x=index%columns*width;y=((count+columns-1)/columns-1-index/columns)*height;
}
inline int PtFaceGridIndex(int x,int y,int count,int columns,int rows,int width,int height,bool expanded) {
    if(x<0 || y<0 || columns<=0 || rows<=0 || width<=0 || height<=0)return -1;
    if(x>=columns*width*(expanded?2:1) || y>=rows*height)return -1;
    int col=x/width,row=y/height,perPanel=columns*rows;
    int index=col/columns*perPanel+row*columns+col%columns;
    int slots=perPanel*(expanded?2:1)-1;
    return index<count && index<slots?index:-1;
}
#endif
