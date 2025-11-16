#define mat4 mat4x4
#define r32 float
#define s32 int
#define u32 int unsigned
#define lerp(a, t, b) mix(a, b, t)
#define clamp01(t) clamp(t, 0, 1)
#define clamp(min, t, max) clamp(t, min, max)
#define inner(a, b) dot(a, b)
#define length_sq(a) dot(a, a)
#define square_root(a) sqrt(a)
#define square(a) ((a)*(a))
#define absolute_value(a) abs(a)
