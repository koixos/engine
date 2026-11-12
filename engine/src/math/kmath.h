#pragma once

#include "defines.h"
#include "math_types.h"

#include "core/logger.h"
#include "core/kmemory.h"

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

KAPI i32 krand();
KAPI i32 krand_in_range(i32 min, i32 max);

KAPI f32 fkrand();
KAPI f32 fkrand_in_range(f32 min, f32 max);

/**
 * Indicates if val is a power of 2 - 0 is considered _not_ a power of 2.
 * @param value The val to be interpreted.
 * @returns True if a pow of 2, otherwise false.f
 */
KINLINE b8 is_power_of_2(u64 val) {
    return (val != 0) && ((val & (val - 1)) == 0);
}

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

/**
 * @brief Creates and returns an identity matrix:
 * 
 *              {1, 0, 0, 0},         
 *              {0, 1, 0, 0},         
 *              {0, 0, 1, 0},         
 *              {0, 0, 0, 1}         
 */
KINLINE mat4 mat4_identity() {
    mat4 out_mat;
    kzero_mem(out_mat.data, sizeof(f32) * 16);
    out_mat.data[0] = out_mat.data[5] = out_mat.data[10] = out_mat.data[15] = 1.0f;
    return out_mat;
}

/**
 * @brief Returns the result of multiplying mat_1 and mat_2    
 */
KINLINE mat4 mat4_mul(mat4 mat_1, mat4 mat_2) {
    const f32 *m1_ptr = mat_1.data;
    const f32 *m2_ptr = mat_2.data;
    
    mat4 out_mat = mat4_identity();
    f32 *dest_ptr = out_mat.data;

    for (i32 i = 0; i < 4; i++) {
        for (i32 j = 0; j < 4; j++) {
            *dest_ptr = m1_ptr[0] * m2_ptr[0 + j] + 
                        m1_ptr[1] * m2_ptr[4 + j] + 
                        m1_ptr[2] * m2_ptr[8 + j] + 
                        m1_ptr[3] * m2_ptr[12 + j];
            ++dest_ptr;
        }
        //i can use array indexing instead of ptr arithmetic (exp: m1_ptr[i * 4 + 0]),
        // but for know, this code is very efficient,
        // tho idk if arr indx.ing affect that much
        m1_ptr += 4;
    }
    return out_mat;
}

/**
 * @brief Creates and returns an orthographic projection matrix that is typically used to
 * render flat or 2D scenes
 * @param left The left side of the view frustum
 * @param right The right side of the view frustum
 * @param bottom The bottom side of the view frustum
 * @param top The top side of the view frustum
 * @param near_clip The near clipping plane distance
 * @param far_clip The far clipping plane distance
 * @return A new orthographic projection matrix
 */
KINLINE mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_clip, f32 far_clip) {    
    mat4 out_mat = mat4_identity();

    f32 lr = 1.0f / (left - right);
    f32 bt = 1.0f / (bottom - top);
    f32 nf = 1.0f / (near_clip - far_clip);

    out_mat.data[0] = -2.0f * lr;
    out_mat.data[5] = 2.0f * bt;
    out_mat.data[10] = 2.0f * nf;
    
    out_mat.data[12] = (left + right) * lr;
    out_mat.data[13] = (top + bottom) * bt;
    out_mat.data[14] = (far_clip + near_clip) * nf;

    return out_mat;
}

/**
 * @brief Creates and returns a perspective projection matrix that is typically used to
 * render 3D scenes
 * @param fov_radians The field of view in radians
 * @param aspect_ratio The aspect ratio
 * @param near_clip The near clipping plane distance
 * @param far_clip The far clipping plane distance
 * @return A new perspective projection matrix
 */
KINLINE mat4 mat4_perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip, f32 far_clip) {    
    mat4 out_mat;
    kzero_mem(out_mat.data, sizeof(f32) * 16);

    f32 half_tan_fov = ktan(fov_radians * 0.5f);

    out_mat.data[0] = 1.0f / (aspect_ratio * half_tan_fov);
    out_mat.data[5] = 1.0f / half_tan_fov;
    out_mat.data[10] = -((far_clip + near_clip) / (far_clip - near_clip));
    out_mat.data[11] = 1.0f;
    out_mat.data[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));

    return out_mat;
}

/**
 * @brief Creates and returns a look-at matrix, or a matrix looking at target
 * from the perspective of position
 * @param pos The position of the matrix
 * @param target The position to "look at"
 * @param up The up vec
 * @return A matrix looking at target from the perspective of position
 */
KINLINE mat4 mat4_look_at(vec3 pos, vec3 target, vec3 up) {    
    mat4 out_mat;

    vec3 z_axis;
    z_axis.x = target.x - pos.x;
    z_axis.y = target.y - pos.y;
    z_axis.z = target.z - pos.z;
    z_axis = vec3_normalized(z_axis);

    vec3 x_axis = vec3_normalized(vec3_cross(z_axis, up));
    vec3 y_axis = vec3_cross(x_axis, z_axis);

    out_mat.data[0] = x_axis.x;
    out_mat.data[1] = y_axis.x;
    out_mat.data[2] = -z_axis.x;
    out_mat.data[3] = 0;
    out_mat.data[4] = x_axis.y;
    out_mat.data[5] = y_axis.y;
    out_mat.data[6] = -z_axis.y;
    out_mat.data[7] = 0;
    out_mat.data[8] = x_axis.z;
    out_mat.data[9] = y_axis.z;
    out_mat.data[10] = -z_axis.z;
    out_mat.data[11] = 0;
    out_mat.data[12] = -vec3_dot(x_axis, pos);
    out_mat.data[13] = -vec3_dot(y_axis, pos);
    out_mat.data[14] = vec3_dot(z_axis, pos);
    out_mat.data[15] = 1.0f;

    return out_mat;
}

/**
 * @brief Returns a transposed copy of the provided matrix (rows->colums)
 */
KINLINE mat4 mat4_transposed(mat4 mat) {
    mat4 out_mat = mat4_identity();
    out_mat.data[0] = mat.data[0];
    out_mat.data[1] = mat.data[4];
    out_mat.data[2] = mat.data[8];
    out_mat.data[3] = mat.data[12];
    out_mat.data[4] = mat.data[1];
    out_mat.data[5] = mat.data[5];
    out_mat.data[6] = mat.data[9];
    out_mat.data[7] = mat.data[13];
    out_mat.data[8] = mat.data[2];
    out_mat.data[9] = mat.data[6];
    out_mat.data[10] = mat.data[10];
    out_mat.data[11] = mat.data[14];
    out_mat.data[12] = mat.data[3];
    out_mat.data[13] = mat.data[7];
    out_mat.data[14] = mat.data[11];
    out_mat.data[15] = mat.data[15];
    return out_mat;
}

/**
 * @brief Creates and returns an inverse of the provided matrix
 * @param mat The matrix to be inverted
 * @return A inverted copy of the provided matrix.
 */
KINLINE mat4 mat4_inverse(mat4 mat) {
    const f32 *m = mat.data;

    f32 t0 = m[10] * m[15],
        t1 = m[14] * m[11],
        t2 = m[6] * m[15],
        t3 = m[14] * m[7],
        t4 = m[6] * m[11],
        t5 = m[10] * m[7],
        t6 = m[2] * m[15],
        t7 = m[14] * m[3],
        t8 = m[2] * m[11],
        t9 = m[10] * m[3],
        t10 = m[2] * m[7],
        t11 = m[6] * m[3],
        t12 = m[8] * m[13],
        t13 = m[12] * m[9],
        t14 = m[4] * m[13],
        t15 = m[12] * m[5],
        t16 = m[4] * m[9],
        t17 = m[8] * m[5],
        t18 = m[0] * m[13],
        t19 = m[12] * m[1],
        t20 = m[0] * m[9],
        t21 = m[8] * m[1],
        t22 = m[0] * m[5],
        t23 = m[4] * m[1];

    mat4 out_mat;
    f32 *o = out_mat.data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    f32 d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return out_mat;
}

/**
 * @brief Creates and returns a matrix holding poisitonal info provided by a vec
 */
KINLINE mat4 mat4_translation(vec3 pos) {
    mat4 out_mat = mat4_identity();
    out_mat.data[12] = pos.x;
    out_mat.data[13] = pos.y;
    out_mat.data[14] = pos.z;
    return out_mat;
}

/**
 * @brief Returns a scale matrix using the provided scale
 */
KINLINE mat4 mat4_scale(vec3 scale) {
    mat4 out_mat = mat4_identity();
    out_mat.data[0] = scale.x;
    out_mat.data[5] = scale.y;
    out_mat.data[10] = scale.z;
    return out_mat;
}

/* Funcs to create matrices from Euler angles */

KINLINE mat4 mat4_euler_x(f32 angle_radians) {
    mat4 out_mat = mat4_identity();

    f32 c = kcos(angle_radians);
    f32 s = ksin(angle_radians);

    out_mat.data[5] = c;
    out_mat.data[6] = s;
    out_mat.data[9] = -s;
    out_mat.data[10] = c;

    return out_mat;
}

KINLINE mat4 mat4_euler_y(f32 angle_radians) {
    mat4 out_mat = mat4_identity();

    f32 c = kcos(angle_radians);
    f32 s = ksin(angle_radians);

    out_mat.data[0] = c;
    out_mat.data[2] = -s;
    out_mat.data[8] = s;
    out_mat.data[10] = c;
    
    return out_mat;
}

KINLINE mat4 mat4_euler_z(f32 angle_radians) {
    mat4 out_mat = mat4_identity();

    f32 c = kcos(angle_radians);
    f32 s = ksin(angle_radians);

    out_mat.data[0] = c;
    out_mat.data[1] = s;
    out_mat.data[4] = -s;
    out_mat.data[5] = c;
    
    return out_mat;
}

KINLINE mat4 mat4_euler_xyz(f32 x_radians, f32 y_radians, f32 z_radians) {
    mat4 rx = mat4_euler_x(x_radians);
    mat4 ry = mat4_euler_y(y_radians);
    mat4 rz = mat4_euler_z(z_radians);
    
    mat4 out_mat = mat4_mul(rx, ry);    
    out_mat = mat4_mul(out_mat, rz);   

    return out_mat;
}

/**
 * @brief Returns a forward vec relative to the provided matrix
 * @param mat The matrix from which to base the vec
 * @return A 3-component directional vec
 */
KINLINE vec3 mat4_forward(mat4 mat) {
    vec3 forw;
    forw.x = -mat.data[2];
    forw.y = -mat.data[6];
    forw.z = -mat.data[10];
    vec3_normalize(&forw);    
    return forw;
}

/**
 * @brief Returns a backward vec relative to the provided matrix
 * @param mat The matrix from which to base the vec
 * @return A 3-component directional vec
 */
KINLINE vec3 mat4_backward(mat4 mat) {
    vec3 backw;
    backw.x = mat.data[2];
    backw.y = mat.data[6];
    backw.z = mat.data[10];
    vec3_normalize(&backw);    
    return backw;
}

/**
 * @brief Returns a upward vec relative to the provided matrix
 * @param mat The matrix from which to base the vec
 * @return A 3-component directional vec
 */
KINLINE vec3 mat4_upward(mat4 mat) {
    vec3 up;
    up.x = mat.data[1];
    up.y = mat.data[5];
    up.z = mat.data[9];
    vec3_normalize(&up);    
    return up;
}

/**
 * @brief Returns a downward vec relative to the provided matrix
 * @param mat The matrix from which to base the vec
 * @return A 3-component directional vec
 */
KINLINE vec3 mat4_downward(mat4 mat) {
    vec3 down;
    down.x = -mat.data[1];
    down.y = -mat.data[5];
    down.z = -mat.data[9];
    vec3_normalize(&down);    
    return down;
}

/**
 * @brief Returns a left vec relative to the provided matrix
 * @param mat The matrix from which to base the vec
 * @return A 3-component directional vec
 */
KINLINE vec3 mat4_left(mat4 mat) {
    vec3 left;
    left.x = -mat.data[0];
    left.y = -mat.data[4];
    left.z = -mat.data[8];
    vec3_normalize(&left);    
    return left;
}

/**
 * @brief Returns a right vec relative to the provided matrix
 * @param mat The matrix from which to base the vec
 * @return A 3-component directional vec
 */
KINLINE vec3 mat4_right(mat4 mat) {
    vec3 right;
    right.x = mat.data[0];
    right.y = mat.data[4];
    right.z = mat.data[8];
    vec3_normalize(&right);    
    return right;
}

/* QUATERNION */

KINLINE quat quat_identity() {
    return (quat){0, 0, 0, 1.0f};
}

KINLINE f32 quat_normal(quat q) {
    return ksqrt(
        q.x * q.x +
        q.y * q.y +
        q.z * q.z +
        q.w * q.w
    );
}

KINLINE quat quat_normalize(quat q) {
    f32 norm = quat_normal(q);
    return (quat) {
        q.x / norm,
        q.y / norm,
        q.z / norm,
        q.w / norm
    };
}

KINLINE quat quat_conjugate(quat q) {
    return (quat) {
        -q.x,
        -q.y,
        -q.z,
        q.w
    };
}

KINLINE quat quat_inverse(quat q) {
    return quat_normalize(quat_conjugate(q));
}

KINLINE quat quat_mul(quat q_1, quat q_2) {
    quat out_quat;

    out_quat.x = q_1.x * q_2.w +
                q_1.y * q_2.z -
                q_1.z * q_2.y +
                q_1.w * q_2.x;

    out_quat.y = -q_1.x * q_2.z +
                q_1.y * q_2.w +
                q_1.z * q_2.x +
                q_1.w * q_2.y;

    out_quat.z =q_1.x * q_2.y -
                q_1.y * q_2.x +
                q_1.z * q_2.w +
                q_1.w * q_2.z;

    out_quat.w = -q_1.x * q_2.x -
                q_1.y * q_2.y -
                q_1.z * q_2.z +
                q_1.w * q_2.w;

    return out_quat;
}

KINLINE f32 quat_dot(quat q_1, quat q_2) {
    return q_1.x * q_2.x + q_1.y * q_2.y + q_1.z * q_2.z + q_1.w * q_2.w;
}

KINLINE mat4 quat_to_mat4(quat q) {
    // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix
    mat4 out_mat = mat4_identity();
    quat n = quat_normalize(q);

    out_mat.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    out_mat.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    out_mat.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    out_mat.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    out_mat.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    out_mat.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    out_mat.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    out_mat.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    out_mat.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return out_mat;
}

/**
 * @brief Calculates a rotation matrix based on the quaternion and the passed in center point
 */
KINLINE mat4 quat_to_rotation_matrix(quat q, vec3 center) {
    mat4 out_mat;

    f32 *o = out_mat.data;
    o[0] = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[1] = 2.0f * ((q.x * q.y) + (q.z * q.w));
    o[2] = 2.0f * ((q.x * q.z) - (q.y * q.w));
    o[3] = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];

    o[4] = 2.0f * ((q.x * q.y) - (q.z * q.w));
    o[5] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[6] = 2.0f * ((q.y * q.z) + (q.x * q.w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];

    o[8] = 2.0f * ((q.x * q.z) + (q.y * q.w));
    o[9] = 2.0f * ((q.y * q.z) - (q.x * q.w));
    o[10] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];

    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;

    return out_mat;
}

KINLINE quat quat_from_axis_angle(vec3 axis, f32 angle, b8 normalize) {
    const f32 half_angle = 0.5f * angle;
    
    f32 c = kcos(half_angle);
    f32 s = ksin(half_angle);

    quat q = (quat){s * axis.x, s * axis.y, s * axis.z, c};

    if (normalize)
        return quat_normalize(q);
    
    return q;
}

KINLINE quat quat_slerp(quat q_0, quat q_1, f32 percentage) {
    quat out_quat;

    //src: https://en.wikipedia.org/wiki/Slerp
    //only unit quaternions are valid rotations, normalize to avoid undef behav
    quat v0 = quat_normalize(q_0);
    quat v1 = quat_normalize(q_1);

    //compute the cos of the angle between the two vecs
    f32 dot = quat_dot(v0, v1);

    //if the dot product is negative, slerp won't take the shorter path
    // note that v1 and -v1 are equivalent when the negation is applied to all four components,
    // fix by reversing one quaternion
    if (dot < 0.0f) {
        v1.x = -v1.x;
        v1.y = -v1.y;
        v1.z = -v1.z;
        v1.w = -v1.w;
        dot = -dot;
    }

    const f32 DOT_THRESHOLD = 0.9995f;

    if (dot > DOT_THRESHOLD) {
        //if inps are too close for comfort, linearly interpolate and normalize the result
        out_quat = (quat) {
            v0.x + ((v1.x - v0.x) * percentage),
            v0.y + ((v1.y - v0.y) * percentage),
            v0.z + ((v1.z - v0.z) * percentage),
            v0.w + ((v1.w - v0.w) * percentage)
        };

        return quat_normalize(out_quat);
    }

    //since dot is in range [0, DOT_THRESHOLD], acos is safe
    f32 theta_0 = kacos(dot);          //theta_0 = angle between inp vecs
    f32 theta = theta_0 * percentage;  //theta = angle between v0 and res
    f32 sin_theta = ksin(theta);       //compute this val only once
    f32 sin_theta_0 = ksin(theta_0);   //compute this val only once

    f32 s0 = kcos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    f32 s1 = sin_theta / sin_theta_0;

    return (quat) {
        (v0.x * s0) + (v1.x * s1),
        (v0.y * s0) + (v1.y * s1),
        (v0.z * s0) + (v1.z * s1),
        (v0.w * s0) + (v1.w * s1)
    };
}

KINLINE f32 deg2rad(f32 degrees) {
    return degrees * K_DEG2RAD_MULTIPLIER;
}

KINLINE f32 rad2deg(f32 radians) {
    return radians * K_RAD2DEG_MULTIPLIER;
}