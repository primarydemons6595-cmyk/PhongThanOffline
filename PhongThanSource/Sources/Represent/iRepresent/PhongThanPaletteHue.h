#ifndef PHONGTHAN_PALETTE_HUE_H
#define PHONGTHAN_PALETTE_HUE_H

// Compatible reconstruction of original engine g_Pal24To16AdjustHue:
// HSL hue rotates in degrees; saturation/lightness and achromatic colors stay.
// Original represent2 style6 uses this, separately from global RGB adjustment.
inline float PhongThanHueComponent(float low, float high, float hue)
{
    if (hue > 360.0f) hue -= 360.0f;
    if (hue < 0.0f) hue += 360.0f;
    if (hue < 60.0f) return low + (high - low) * hue / 60.0f;
    if (hue < 180.0f) return high;
    if (hue < 240.0f) return low + (high - low) * (240.0f - hue) / 60.0f;
    return low;
}

inline void PhongThanRotatePaletteHue(int& red, int& green, int& blue, int degrees)
{
    if (degrees <= 0 || degrees >= 360) return;
    int maximum = red > green ? red : green;
    if (blue > maximum) maximum = blue;
    int minimum = red < green ? red : green;
    if (blue < minimum) minimum = blue;
    if (maximum == minimum) return;
    const float delta = (float)(maximum - minimum);
    const float sum = (float)(maximum + minimum);
    const float lightness = sum / 510.0f;
    const float saturation = delta / (lightness <= 0.5f ? sum : 510.0f - sum);
    float hue = 0.0f;
    if (red == maximum) hue = (green - blue) * 60.0f / delta;
    if (green == maximum) hue = 120.0f + (blue - red) * 60.0f / delta;
    if (blue == maximum) hue = 240.0f + (red - green) * 60.0f / delta;
    if (hue < 0.0f) hue += 360.0f;
    hue += degrees;
    if (hue > 360.0f) hue -= 360.0f;
    const float high = lightness <= 0.5f ? lightness * (1.0f + saturation) :
        lightness + saturation - lightness * saturation;
    const float low = 2.0f * lightness - high;
    red = (int)(PhongThanHueComponent(low, high, hue + 120.0f) * 255.0f);
    green = (int)(PhongThanHueComponent(low, high, hue) * 255.0f);
    blue = (int)(PhongThanHueComponent(low, high, hue - 120.0f) * 255.0f);
    // Roundoff must never wrap a component when converted to RGB565.
    if (red < 0) red = 0; else if (red > 255) red = 255;
    if (green < 0) green = 0; else if (green > 255) green = 255;
    if (blue < 0) blue = 0; else if (blue > 255) blue = 255;
}
#endif
