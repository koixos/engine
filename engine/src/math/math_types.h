#pragma once

#include "defines.h"

typedef union vector2_u {
    f32 elems[2];   //arr of x and y
    struct {
        union {
            //the first elem
            f32 x, r, s, u;
        };
        union {
            //the second elem
            f32 y, g, t, v;
        };
    };
} vec2;

typedef union vector3_u {
    f32 elems[3];   //arr of x, y, z
    struct {
        union {
            //the first elem
            f32 x, r, s, u;
        };
        union {
            //the second elem
            f32 y, g, t, v;
        };
        union {
            //the third elem
            f32 z, b, p, w;
        };
    };
} vec3;

typedef union vector4_u {
/*#if defined(KUSE_SIMD)
    //used for SIMD ops
    alignas(16) __m128 data;
#endif
    alignas(16)*/ f32 elems[4];   //arr of x, y, z, w
    struct {
        union {
            //the first elem
            f32 x, r, s;
        };
        union {
            //the second elem
            f32 y, g, t;
        };
        union {
            //the third elem
            f32 z, b, p;
        };
        union {
            //the fourth elem
            f32 w, a, q;
        };
    };
} vec4;

typedef vec4 quat;

typedef union mat4_u {
    /*alignas(16)*/ f32 data[16];

/*#if defined(KUSE_SIMD)
    //used for SIMD ops
    alignas(16) vec4 rows[4];
#endif*/
} mat4;