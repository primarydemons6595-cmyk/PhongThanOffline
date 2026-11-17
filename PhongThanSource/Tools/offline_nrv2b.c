/* Offline-only bounded NRV2B accelerator; no game ABI or source changes. */
typedef unsigned char U8;
typedef unsigned int U32;
typedef struct { const U8* p; U32 size, pos, bits; } Input;
static int bit(Input* s) {
    if (s->bits & 127) s->bits *= 2;
    else {
        if (s->pos >= s->size) return -1;
        s->bits = (U32)s->p[s->pos++] * 2 + 1;
    }
    return (s->bits >> 8) & 1;
}
#define BIT(v) do { int z = bit(&s); if (z < 0) return 2; v = (U32)z; } while (0)
__declspec(dllexport) int __cdecl pt_nrv2b(const U8* src, U32 size, U8* out, U32 expected) {
    Input s = {src, size, 0, 0};
    U32 n = 0, last = 1, off, len, b, c, count;
    if (!src || !out || expected > 67108864U) return 2;
    for (;;) {
        for (;;) {
            BIT(b);
            if (!b) break;
            if (s.pos >= s.size || n >= expected) return 2;
            out[n++] = src[s.pos++];
        }
        off = 1;
        for (;;) {
            BIT(b); off = off * 2 + b;
            if (off > 0x1000002U) return 2;
            BIT(b); if (b) break;
        }
        if (off == 2) off = last;
        else {
            if (s.pos >= s.size) return 2;
            off = (off - 3) * 256 + src[s.pos++];
            if (off == 0xffffffffU) return n == expected && s.pos == s.size ? 0 : 2;
            last = ++off;
        }
        BIT(b); BIT(c); len = b * 2 + c;
        if (!len) {
            len = 1;
            for (;;) {
                BIT(b); len = len * 2 + b;
                if (len > expected) return 2;
                BIT(b); if (b) break;
            }
            len += 2;
        }
        len += off > 0xd00U;
        count = len + 1;
        if (!off || off > n || count > expected - n) return 2;
        while (count--) { out[n] = out[n - off]; ++n; }
    }
}
