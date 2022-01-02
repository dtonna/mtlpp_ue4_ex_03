//
//  math.hpp
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 15/12/2564 BE.
//

#ifndef math_hpp
#define math_hpp


#include <simd/simd.h>

matrix_float4x4 matrix4x4_translation(float tx, float ty, float tz);

matrix_float4x4 matrix4x4_rotation(float radians, vector_float3 axis);

matrix_float4x4 matrix_perspective_right_hand(float fovyRadians, float aspect, float nearZ, float farZ);

matrix_float4x4 matrix_make_rows(
                                 float m00, float m10, float m20, float m30,
                                 float m01, float m11, float m21, float m31,
                                 float m02, float m12, float m22, float m32,
                                 float m03, float m13, float m23, float m33);

matrix_float4x4 matrix4x4_scale(float sx, float sy, float sz);


#endif /* math_hpp */
