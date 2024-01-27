#include <math.h>

#include "mathematics.h"

v2 Math_IntersectSegments(v2 a0, v2 a1, v2 b0, v2 b1) {
    const f32 d =
        ((a0.x - a1.x) * (b0.y - b1.y)) - ((a0.y - a1.y) * (b0.x - b1.x));

    if (fabsf(d) < 0.000001f) { return (v2) { NAN, NAN }; }

    const f32
        t = (((a0.x - b0.x) * (b0.y - b1.y)) - ((a0.y - b0.y) * (b0.x - b1.x))) / d,
        u = (((a0.x - b0.x) * (a0.y - a1.y)) - ((a0.y - b0.y) * (a0.x - a1.x))) / d;

    return (t >= 0 && t <= 1 && u >= 0 && u <= 1) ?
        ((v2) {
            a0.x + (t * (a1.x - a0.x)),
            a0.y + (t * (a1.y - a0.y)) })
        : ((v2) { NAN, NAN });
}

v2 Math_Rotate(v2 p, f32 a)
{
    return (v2) {
        p.x * cos(a) - p.y * sin(a),
        p.x * sin(a) - p.y * cos(a)
    };
}

v2 Math_TranslateRotate(v2 p, v2 r, f32 a)
{
    float sine = sin(a / 180.0 * M_PI);
    float cosine = cos(a / 180.0 * M_PI);

    return (v2) {
        (p.x - r.x) * cosine - (p.y - r.y) * sine,
        (p.y - r.y) * cosine + (p.x - r.x) * sine
    };  
}

f32 Math_NormalizeAngle(f32 a) {
    return a - (TAU * floor((a + PI) / TAU));
}

int Math_ScreenAngleToX(f32 angle, i32 screenWidth, f32 hfov) {
    return ((int) (screenWidth / 2)) * (1.0f - tan(((angle + (hfov / 2.0)) / hfov) * PI_2 - PI_4));
}