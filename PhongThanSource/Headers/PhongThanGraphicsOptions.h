#ifndef PHONGTHAN_GRAPHICS_OPTIONS_H
#define PHONGTHAN_GRAPHICS_OPTIONS_H
// Client rendering preferences. These never change simulation/cast timing.
extern int g_PhongThanAnimationQuality;
extern int g_PhongThanMaxVisiblePlayers;
inline int PhongThanGraphicsFrame(int frame, int quality) {
    if (frame < 0) return frame;
    if (quality < 0) quality = 0;
    if (quality > 3) quality = 3;
    int stride = quality + 1;
    return frame - frame % stride;
}
#endif
