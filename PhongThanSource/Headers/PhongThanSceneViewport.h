#ifndef PHONGTHAN_SCENE_VIEWPORT_H
#define PHONGTHAN_SCENE_VIEWPORT_H

// LookAt normally centres on the physical render surface. The scene can occupy
// only its left part when a separate chat column extends that surface.
template<class Represent>
void PTAlignSceneViewport(Represent* represent,int focusX,int focusY,int width,int height)
{
    if(!represent||width<=0||height<=0)return;
    represent->LookAt(focusX,focusY,0);
    int centreX=width/2,centreY=height/2;
    represent->ViewPortCoordToSpaceCoord(centreX,centreY,0);
    represent->LookAt(focusX+(focusX-centreX),focusY+(focusY-centreY),0);
}
#endif
