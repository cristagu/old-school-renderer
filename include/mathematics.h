#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include <stdlib.h>

#include "types.h"

#define PI 3.14159265359f
#define PI_2 (PI / 2.0f)
#define PI_4 (PI / 4.0f)
#define TAU (2.0f * PI)

#define DEG2RAD(_d) ((_d) * (PI / 180.0f))
#define RAD2DEG(_d) ((_d) * (180.0f / PI))
#define min(_a, _b) ({ __typeof__(_a) __a = (_a), __b = (_b); __a < __b ? __a : __b; })
#define max(_a, _b) ({ __typeof__(_a) __a = (_a), __b = (_b); __a > __b ? __a : __b; })
#define clamp(_x, _mi, _ma) (min(max(_x, _mi), _ma))

typedef struct v2  { f32 x, y; } v2;
typedef struct v2i { i32 x, y; } v2i;

v2  Math_IntersectSegments(v2 a0, v2 a1, v2 b0, v2 b1);
v2  Math_Rotate(v2 p, f32 a);
v2  Math_TranslateRotate(v2 p, v2 r, f32 a);
f32 Math_NormalizeAngle(f32 a);
int Math_ScreenAngleToX(f32 angle, i32 screenWidth, f32 hfov);

#endif