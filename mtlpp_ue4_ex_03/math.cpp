//
//  math.cpp
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 15/12/2564 BE.
//

#include "math.hpp"

matrix_float4x4 matrix4x4_translation(float tx, float ty, float tz)
{
    return (matrix_float4x4) {{
        { 1,   0,  0,  0 },
        { 0,   1,  0,  0 },
        { 0,   0,  1,  0 },
        { tx, ty, tz,  1 }
    }};
}

matrix_float4x4 matrix4x4_rotation(float radians, vector_float3 axis)
{
    axis = vector_normalize(axis);
    float ct = cosf(radians);
    float st = sinf(radians);
    float ci = 1 - ct;
    float x = axis.x, y = axis.y, z = axis.z;
    
    return (matrix_float4x4) {{
        { ct + x * x * ci,     y * x * ci + z * st, z * x * ci - y * st, 0},
        { x * y * ci - z * st,     ct + y * y * ci, z * y * ci + x * st, 0},
        { x * z * ci + y * st, y * z * ci - x * st,     ct + z * z * ci, 0},
        {                   0,                   0,                   0, 1}
    }};
}

matrix_float4x4 matrix_perspective_right_hand(float fovyRadians, float aspect, float nearZ, float farZ)
{
    float ys = 1 / tanf(fovyRadians * 0.5);
    float xs = ys / aspect;
    float zs = farZ / (nearZ - farZ);
    
    return (matrix_float4x4) {{
        { xs,   0,          0,  0 },
        {  0,  ys,          0,  0 },
        {  0,   0,         zs, -1 },
        {  0,   0, nearZ * zs,  0 }
    }};
}

matrix_float4x4 matrix_make_rows(
                                 float m00, float m10, float m20, float m30,
                                 float m01, float m11, float m21, float m31,
                                 float m02, float m12, float m22, float m32,
                                 float m03, float m13, float m23, float m33) {
    return (matrix_float4x4){ {
        { m00, m01, m02, m03 },     // each line here provides column data
        { m10, m11, m12, m13 },
        { m20, m21, m22, m23 },
        { m30, m31, m32, m33 } } };
}

matrix_float4x4 matrix4x4_scale(float sx, float sy, float sz) {
    return matrix_make_rows(sx,  0,  0, 0,
                            0, sy,  0, 0,
                            0,  0, sz, 0,
                            0,  0,  0, 1 );
}
