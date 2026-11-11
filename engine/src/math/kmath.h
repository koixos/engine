#pragma once

#include "defines.h"
#include "math_types.h"

#include "core/logger.h"

#define K_PI                    3.14159265358979323846f
#define K_PI_2                  (2.0f * K_PI)
#define K_HALF_PI               (0.5f * K_PI)
#define K_QUARTER_PI            (0.25f * K_PI)
#define K_ONE_OVER_PI           (1.0f / K_PI)
#define K_ONE_OVER_TWO_PI       (1.0f / K_PI_2)
#define K_SQRT_TWO              1.41421356237309504880f
#define K_SQRT_THREE            1.73205080756887729352f
#define K_SQRT_ONE_OVER_TWO     0.70710678118654752440f
#define K_SQRT_ONE_OVER_THREE   0.57735026918962576450f
#define K_DEG2RAD_MULTIPLIER    (K_PI / 180.0f)
#define K_RAD2DEG_MULTIPLIER    (180.0f/ K_PI)
#define K_SEC2MS_MULTIPLIER     1000.0f
#define K_MS2SEC_MULTIPLIER     0.001f
#define K_INF                   1e30f
#define K_FLOAT_EPS             1.192092896e-07f    #smallest pos num where 1.0 + FLOAT_EPSILON != 0

/* GENERAL MATH FUNCS */

KAPI f32 ksin(f32 x);
KAPI f32 kcos(f32 x);
KAPI f32 ktan(f32 x);
KAPI f32 kacos(f32 x);
KAPI f32 ksqrt(f32 x);
KAPI f32 kabs(f32 x);

/**
 * Indicates if val is a power of 2 - 0 is considered _not_ a power of 2.
 * @param value The val to be interpreted.
 * @returns True if a pow of 2, otherwise false.f
 */
KINLINE b8 is_power_of_2(u64 val) {
    return (val != 0) && ((val & (val - 1)) == 0);
}

KAPI i32 krand();
KAPI i32 krand_in_range(i32 min, i32 max);

KAPI f32 fkrand();
KAPI f32 fkrand_in_range(f32 min, f32 max);

/* VECTOR 2 */

/**
 * @brief Creates and returns a new 2-element vec using the supplied vals
 * @param x The x val
 * @param y The y val
 * @return A new 2-elem vec
 */
KINLINE vec2 vec2_create(f32 x, f32 y) {
    return (vec2){x, y};
}

/**
 * @brief Creates and returns a new 2-component vec with all components set to 0.0f
*/
KINLINE vec2 vec2_zero() {
    return (vec2){0.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 2-component vec with all components set to 1.0f
*/
KINLINE vec2 vec2_one() {
    return (vec2){1.0f, 1.0f};
}

/**
 * @brief Creates and returns a new 2-component vec pointing up
*/
KINLINE vec2 vec2_up() {
    return (vec2){0.0f, 1.0f};
}

/**
 * @brief Creates and returns a new 2-component vec pointing down
*/
KINLINE vec2 vec2_down() {
    return (vec2){0.0f, -1.0f};
}

/**
 * @brief Creates and returns a new 2-component vec pointing left
*/
KINLINE vec2 vec2_left() {
    return (vec2){-1.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 2-component vec pointing right
*/
KINLINE vec2 vec2_right() {
    return (vec2){1.0f, 0.0f};
}

/**
 * @brief Adds vec_1 to vec_2 and returns a copy of the result
 */
KINLINE vec2 vec2_add(vec2 vec_1, vec2 vec_2) {
    return (vec2) {
        vec_1.x + vec_2.x,
        vec_1.y + vec_2.y
    };
}

/**
 * @brief Subtracts vec_2 from vec_1 and returns a copy of the result
 */
KINLINE vec2 vec2_sub(vec2 vec_1, vec2 vec_2) {
    return (vec2) {
        vec_1.x - vec_2.x,
        vec_1.y - vec_2.y
    };
}

/**
 * @brief MUltiplies vec_1 by vec_2 and returns a copy of the result
 */
KINLINE vec2 vec2_mul(vec2 vec_1, vec2 vec_2) {
    return (vec2) {
        vec_1.x * vec_2.x,
        vec_1.y * vec_2.y
    };
}

/**
 * @brief Divides vec_1 by vec_2 and returns a copy of the result
 */
KINLINE vec2 vec2_div(vec2 vec_1, vec2 vec_2) {
    if (vec_2.x == 0 || vec_2.y == 0) {
        KERROR("Vector division failed - division by zero!");
        return (vec2){};
    }
    
    return (vec2) {
        vec_1.x / vec_2.x,
        vec_1.y / vec_2.y
    };
}

/**
 * @brief Returns the squared length of the provided vec
 */
KINLINE f32 vec2_len_squared(vec2 vec) {
    return vec.x * vec.x + vec.y * vec.y;
}

/**
 * @brief Returns the length of the provided vec
 */
KINLINE f32 vec2_len(vec2 vec) {
    return ksqrt(vec2_len_squared(vec));
}

/**
 * @brief Normalizes the provided vec in place to a unit vec
 */
KINLINE void vec2_normalize(vec2 *vec) {
    const f32 len = vec2_len(*vec);
    vec->x /= len;
    vec->y /= len;
}

/**
 * @brief Returns a normalized copy of the provided vec
 */
KINLINE vec2 vec2_normalized(vec2 vec) {
    vec2_normalize(&vec);
    return vec;
}

/**
 * @brief Compares all elements of vec_1 and vec_2 and ensures the difference is less than the tolerance
 * @param vec_1 The first vec
 * @param vec_2 The second vec
 * @param tolerance The difference tolerance, typically K_FLOAT_EPS
 * @return TRUE if all differences within tolerance; otherwise FALSE
 */
KINLINE b8 vec2_compare(vec2 vec_1, vec2 vec_2, f32 tolerance) {
    if (kabs(vec_1.x - vec_2.x) > tolerance ||
        kabs(vec_1.y - vec_2.y) > tolerance)
            return FALSE;
    return TRUE;
}

/**
 * @brief Returns the distance between vec_1 and vec_2
 */
KINLINE f32 vec2_distance(vec2 vec_1, vec2 vec_2) {
    vec2 d = (vec2) {
        vec_1.x - vec_2.x,
        vec_1.y - vec_2.y
    };
    return vec2_len(d);
}

/* VECTOR 3 */

/**
 * @brief Creates and returns a new 3-element vec using the supplied vals
 * @param x The x val
 * @param y The y val
 * @param z The z val
 * @return A new 3-elem vec
 */
KINLINE vec3 vec3_create(f32 x, f32 y, f32 z) {
    return (vec3){x, y, z};
}

/**
 * @brief Returns a new vec3 containing the x, y, and z components
 * of the provided vec4, essentially dropping the w component
*/
KINLINE vec3 vec3_from_vec4(vec4 vec) {
    return (vec3){vec.x, vec.y, vec.z};
}

/**
 * @brief Returns a new vec4 containing the x, y, and z components
 * of the provided vec3, and w value for the w component
*/
KINLINE vec4 vec3_to_vec4(vec3 vec, f32 w) {
    return (vec4){vec.x, vec.y, vec.z, w};
}

/**
 * @brief Creates and returns a new 3-component vec with all components set to 0.0f
*/
KINLINE vec3 vec3_zero() {
    return (vec3){0.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 3-component vec with all components set to 1.0f
*/
KINLINE vec3 vec3_one() {
    return (vec3){1.0f, 1.0f, 1.0f};
}

/**
 * @brief Creates and returns a new 3-component vec pointing up
*/
KINLINE vec3 vec3_up() {
    return (vec3){0.0f, 1.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 3-component vec pointing down
*/
KINLINE vec3 vec3_down() {
    return (vec3){0.0f, -1.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 3-component vec pointing left
*/
KINLINE vec3 vec3_left() {
    return (vec3){-1.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 3-component vec pointing right
*/
KINLINE vec3 vec3_right() {
    return (vec3){1.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 3-component vec pointing forward
*/
KINLINE vec3 vec3_forward() {
    return (vec3){0.0f, 0.0f, -1.0f};
}

/**
 * @brief Creates and returns a new 3-component vec pointing backward
*/
KINLINE vec3 vec3_backward() {
    return (vec3){0.0f, 0.0f, 1.0f};
}

/**
 * @brief Adds vec_1 to vec_2 and returns a copy of the result
 */
KINLINE vec3 vec3_add(vec3 vec_1, vec3 vec_2) {
    return (vec3) {
        vec_1.x + vec_2.x,
        vec_1.y + vec_2.y,
        vec_1.z + vec_2.z
    };
}

/**
 * @brief Subtracts vec_2 from vec_1 and returns a copy of the result
 */
KINLINE vec3 vec3_sub(vec3 vec_1, vec3 vec_2) {
    return (vec3) {
        vec_1.x - vec_2.x,
        vec_1.y - vec_2.y,
        vec_1.z - vec_2.z
    };
}

/**
 * @brief Multiplies vec_1 by vec_2 and returns a copy of the result
 */
KINLINE vec3 vec3_mul(vec3 vec_1, vec3 vec_2) {
    return (vec3) {
        vec_1.x * vec_2.x,
        vec_1.y * vec_2.y,
        vec_1.z * vec_2.z
    };
}

/**
 * @brief Multiplies all elements of vector by the provided scalar and returns a copy of the result
 */
KINLINE vec3 vec3_mul_scalar(vec3 vec, f32 scalar) {
    return (vec3) {
        vec.x * scalar,
        vec.y * scalar,
        vec.z * scalar
    };
}

/**
 * @brief Divides vec_1 by vec_2 and returns a copy of the result
 */
KINLINE vec3 vec3_div(vec3 vec_1, vec3 vec_2) {
    if (vec_2.x == 0 || vec_2.y == 0 || vec_2.z == 0) {
        KERROR("Vector division failed - division by zero!");
        return (vec3){};
    }
    
    return (vec3) {
        vec_1.x / vec_2.x,
        vec_1.y / vec_2.y,
        vec_1.z / vec_2.z
    };
}

/**
 * @brief Returns the squared length of the provided vec
 */
KINLINE f32 vec3_len_squared(vec3 vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

/**
 * @brief Returns the length of the provided vec
 */
KINLINE f32 vec3_len(vec3 vec) {
    return ksqrt(vec3_len_squared(vec));
}

/**
 * @brief Normalizes the provided vec in place to a unit vec
 */
KINLINE void vec3_normalize(vec3 *vec) {
    const f32 len = vec3_len(*vec);
    vec->x /= len;
    vec->y /= len;
    vec->z /= len;
}

/**
 * @brief Returns a normalized copy of the provided vec
 */
KINLINE vec3 vec3_normalized(vec3 vec) {
    vec3_normalize(&vec);
    return vec;
}

/**
 * @brief Returns the dot product between vec_1 and vec_2, typically used to calculate
 * the difference in direction
 */
KINLINE f32 vec3_dot(vec3 vec_1, vec3 vec_2) {
    return vec_1.x * vec_2.x + vec_1.y * vec_2.y + vec_1.z * vec_2.z;
}

/**
 * @brief Returns the cross product of vec_1 and vec_2, which is a new vector that is orthogonal to both
 * of the provided vectors
 */
KINLINE vec3 vec3_cross(vec3 vec_1, vec3 vec_2) {
    return (vec3) {
        vec_1.y * vec_2.z - vec_1.z * vec_2.y,
        vec_1.z * vec_2.x - vec_1.x * vec_2.z,
        vec_1.x * vec_2.y - vec_1.y * vec_2.x
    };
}

/**
 * @brief Compares all elements of vec_1 and vec_2 and ensures the difference is less than the tolerance
 * @param vec_1 The first vec
 * @param vec_2 The second vec
 * @param tolerance The difference tolerance, typically K_FLOAT_EPS
 * @return TRUE if all differences within tolerance; otherwise FALSE
 */
KINLINE b8 vec3_compare(vec3 vec_1, vec3 vec_2, f32 tolerance) {
    if (kabs(vec_1.x - vec_2.x) > tolerance ||
        kabs(vec_1.y - vec_2.y) > tolerance ||
        kabs(vec_1.z - vec_2.z) > tolerance)
            return FALSE;
    return TRUE;
}

/**
 * @brief Returns the distance between vec_1 and vec_2
 */
KINLINE f32 vec3_distance(vec3 vec_1, vec3 vec_2) {
    vec3 d = (vec3) {
        vec_1.x - vec_2.x,
        vec_1.y - vec_2.y,
        vec_1.z - vec_2.z
    };
    return vec3_len(d);
}

/* VECTOR 4 */

/**
 * @brief Creates and returns a new 4-element vec using the supplied vals
 * @param x The x val
 * @param y The y val
 * @param z The z val
 * @param w The w val
 * @return A new 4-elem vec
 */
KINLINE vec4 vec4_create(f32 x, f32 y, f32 z, f32 w) {
    vec4 out_vec;
#if defined(KUSE_SIMD)
    out_vec.data = _mm_setr_ps(x, y, z, w);
#else
    out_vec.x = x;
    out_vec.y = y;
    out_vec.z = z;
    out_vec.w = w;
#endif
    return out_vec;
}

/**
 * @brief Returns a new vec4 containing the x, y, and z components
 * of the provided vec3, and w value for the w component
*/
KINLINE vec4 vec4_from_vec3(vec3 vec, f32 w) {
#if defined(KUSE_SIMD)
    vec4 out_vec;
    out_vec.data = _mm_setr_ps(x, y, z, w);
    return out_vec;
#else
    return (vec4){vec.x, vec.y, vec.z, w};
#endif
}

/**
 * @brief Returns a new vec3 containing the x, y, and z components
 * of the provided vec4, essentially dropping the w component
*/
KINLINE vec3 vec4_to_vec3(vec4 vec) {
    return (vec3){vec.x, vec.y, vec.z};
}

/**
 * @brief Creates and returns a new 4-component vec with all components set to 0.0f
*/
KINLINE vec4 vec4_zero() {
    return (vec4){0.0f, 0.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 4-component vec with all components set to 1.0f
*/
KINLINE vec4 vec4_one() {
    return (vec4){1.0f, 1.0f, 1.0f, 1.0f};
}

/**
 * @brief Adds vec_1 to vec_2 and returns a copy of the result
 */
KINLINE vec4 vec4_add(vec4 vec_1, vec4 vec_2) {
    return (vec4) {
        vec_1.x + vec_2.x,
        vec_1.y + vec_2.y,
        vec_1.z + vec_2.z,
        vec_1.w + vec_2.w
    };
}

/**
 * @brief Subtracts vec_2 from vec_1 and returns a copy of the result
 */
KINLINE vec4 vec4_sub(vec4 vec_1, vec4 vec_2) {
    return (vec4) {
        vec_1.x - vec_2.x,
        vec_1.y - vec_2.y,
        vec_1.z - vec_2.z,
        vec_1.w - vec_2.w
    };
}

/**
 * @brief Multiplies vec_1 by vec_2 and returns a copy of the result
 */
KINLINE vec4 vec4_mul(vec4 vec_1, vec4 vec_2) {
    return (vec4) {
        vec_1.x * vec_2.x,
        vec_1.y * vec_2.y,
        vec_1.z * vec_2.z,
        vec_1.w * vec_2.w
    };
}

/**
 * @brief Divides vec_1 by vec_2 and returns a copy of the result
 */
KINLINE vec4 vec4_div(vec4 vec_1, vec4 vec_2) {
    if (vec_2.x == 0 || vec_2.y == 0 || vec_2.z == 0 || vec_2.w == 0) {
        KERROR("Vector division failed - division by zero!");
        return (vec4){};
    }
    
    return (vec4) {
        vec_1.x / vec_2.x,
        vec_1.y / vec_2.y,
        vec_1.z / vec_2.z,
        vec_1.w / vec_2.w
    };
}

/**
 * @brief Returns the squared length of the provided vec
 */
KINLINE f32 vec4_len_squared(vec4 vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
}

/**
 * @brief Returns the length of the provided vec
 */
KINLINE f32 vec4_len(vec4 vec) {
    return ksqrt(vec4_len_squared(vec));
}

/**
 * @brief Normalizes the provided vec in place to a unit vec
 */
KINLINE void vec4_normalize(vec4 *vec) {
    const f32 len = vec4_len(*vec);
    vec->x /= len;
    vec->y /= len;
    vec->z /= len;
    vec->w /= len;
}

/**
 * @brief Returns a normalized copy of the provided vec
 */
KINLINE vec4 vec4_normalized(vec4 vec) {
    vec4_normalize(&vec);
    return vec;
}

KINLINE f32 vec4_dot_f32(
    f32 a0, f32 a1, f32 a2, f32 a3,
    f32 b0, f32 b1, f32 b2, f32 b3
) {
    return a0 * b0 + a1 * b1 + a2 * b2 + a3 * b3;
}