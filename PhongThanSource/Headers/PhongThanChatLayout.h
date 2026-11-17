#ifndef PHONGTHAN_CHAT_LAYOUT_H
#define PHONGTHAN_CHAT_LAYOUT_H
struct PTChatLayout {int Width,RootY,BodyY,BodyHeight,FilterY,InputY,SystemX,SystemY,RootX,SystemBodyHeight;};
inline PTChatLayout PTMakeChatLayout(int screenWidth,int screenHeight,int height) {
    PTChatLayout r;r.Width=255;r.RootX=0;
    if(height<100)height=100;if(height>screenHeight-30)height=screenHeight-30;
    r.RootY=screenHeight-height;r.BodyY=r.RootY+8;r.BodyHeight=height-45;
    r.FilterY=screenHeight-36;r.InputY=screenHeight-19;
    r.SystemBodyHeight=r.BodyHeight;r.SystemX=screenWidth-r.Width;r.SystemY=r.BodyY-25;return r;
}
inline PTChatLayout PTMakeChatDockLayout(int worldWidth,int screenHeight,int normalHeight,int dockWidth){
 PTChatLayout r=PTMakeChatLayout(worldWidth,screenHeight,normalHeight);
 if(dockWidth>0){r.RootX=worldWidth;r.Width=dockWidth;r.RootY=0;r.BodyY=3;r.BodyHeight=screenHeight-45;r.FilterY=screenHeight-36;r.InputY=screenHeight-19;r.SystemX=0;r.SystemBodyHeight=111;r.SystemY=screenHeight-136;}
 return r;
}
#endif
