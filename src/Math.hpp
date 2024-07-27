#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include <LeapC.h>

template<class T, class U> void ConvertQuaternion(const T& p_src, U& p_dst)
{
    p_dst.x = p_src.x;
    p_dst.y = p_src.y;
    p_dst.z = p_src.z;
    p_dst.w = p_src.w;
}

void ConvertMatrix(const vr::HmdMatrix34_t& p_matVR, glm::mat4& p_mat)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 3; j++) p_mat[i][j] = p_matVR.m[j][i];
    }
    for (int i = 0; i < 3; i++) p_mat[i][3] = 0.f;
    p_mat[3][3] = 1.f;
}


void ConvertPosition(const LEAP_VECTOR& p_src, glm::vec3& p_dst)
{
    // In desktop mode: +X - right, +Y - up, -Z - forward (as OpenGL)
    // In HMD mode: +X - left, +Y - forward, +Z - down (same basis, just rotated)
    p_dst.x = -0.001f * p_src.x;
    p_dst.y = -0.001f * p_src.z;
    p_dst.z = -0.001f * p_src.y;
}

void ConvertRotation(const LEAP_QUATERNION& p_src, glm::quat& p_dst)
{
    // Simple rotation is enough because Leap has same basis as SteamVR
    glm::quat l_rot(p_src.w, p_src.x, p_src.y, p_src.z);
    p_dst = glm::quat(0.f, 0.f, 0.70106769f, -0.70106769f) * l_rot;
}

void ConvertVector3(const vr::HmdVector4_t& src, glm::vec3& dst)
{
    for (auto i = 0U; i < 3; i++)
        dst[i] = src.v[i];
}

void ConvertVector3(const glm::vec3& src, vr::HmdVector4_t& dst)
{
    for (auto i = 0U; i < 3; i++) 
        dst.v[i] = src[i];
}
