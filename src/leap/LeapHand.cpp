#include "LeapHand.h"

#include <utils/Utils.h>
#include <glm/gtx/quaternion.hpp>
#include <controller/LeapController.h>

const glm::quat g_hmdRotation(0.f, 0.f, 0.70106769f, -0.70106769f);
const glm::mat4 g_identityMat4(1.f);
const glm::vec3 g_emptyVec3(0.f);
const glm::quat g_identityQuat(1.f, 0.f, 0.f, 0.f);
const float g_pi = glm::pi<float>();
const float g_piHalf = g_pi * 0.5f;
const float g_piQuarter = g_pi * 0.25f;
extern const glm::vec4 g_zeroPoint;

LeapHand::LeapHand(bool p_left)
{
    m_isLeft = p_left;
    m_visible = false;
    m_position = { 0, 0, 0 };
    m_rotation = { 0, 0, 0, 0 };
    m_velocity = { 0, 0, 0 };
    m_handOffset = { 0, 0, 0 };
    m_bonesPositions = {};
    m_bonesRotations = {};
    m_fingersBends = {};
    m_grabValue = 0.0f;
}

bool LeapHand::IsLeft() const
{
    return m_isLeft;
}

bool LeapHand::IsVisible() const
{
    return m_visible;
}

const glm::vec3 & LeapHand::GetPosition() const
{
    return m_position;
}

const glm::quat & LeapHand::GetRotation() const
{
    return m_rotation;
}

const glm::vec3 & LeapHand::GetVelocity() const
{
    return m_velocity;
}

const glm::vec3& LeapHand::GetOffset() const
{
    return m_handOffset;
}

const glm::vec3 & LeapHand::GetFingerBonePosition(size_t p_finger, size_t p_bone) const
{
    if((p_finger >= 5U) || (p_bone >= 4U))
        return g_emptyVec3;

    return m_bonesPositions[p_finger * 4U + p_bone];
}

const glm::quat & LeapHand::GetFingerBoneRotation(size_t p_finger, size_t p_bone) const
{
    if((p_finger >= 5U) || (p_bone >= 4U))
        return g_identityQuat;

    return m_bonesRotations[p_finger * 4U + p_bone];
}

void LeapHand::GetFingerBoneLocalPosition(size_t p_finger, size_t p_bone, glm::vec3 &l_result) const
{
    if((p_finger >= 5U) || (p_bone >= 4U))
        return;

    size_t l_index = p_finger * 4U + p_bone;
    glm::vec3 l_parentPos = ((p_bone == 0U) ? m_position : m_bonesPositions[l_index - 1U]);
    glm::quat l_parentRot = ((p_bone == 0U) ? m_rotation : m_bonesRotations[l_index - 1U]);
    glm::mat4 l_parentMat = glm::translate(g_identityMat4, l_parentPos) * glm::toMat4(l_parentRot);
    glm::mat4 l_childMat = glm::translate(g_identityMat4, m_bonesPositions[l_index]) * glm::toMat4(m_bonesRotations[l_index]);
    glm::mat4 l_childLocal = glm::inverse(l_parentMat) * l_childMat;
    l_result = l_childLocal * g_zeroPoint;
}

void LeapHand::GetFingerBoneLocalRotation(size_t p_finger, size_t p_bone, glm::quat & l_result) const
{
    if((p_finger >= 5U) || (p_bone >= 4U))
        return;

    size_t l_index = p_finger * 4U + p_bone;
    l_result = glm::inverse((p_bone == 0U) ? m_rotation : m_bonesRotations[l_index - 1U]) * m_bonesRotations[l_index];
}

void LeapHand::GetThumbBoneLocalRotation(size_t p_finger, size_t p_bone, glm::quat& l_result) const
{
    if ((p_finger != 0u) || (p_bone >= 4U))
        return;

    size_t l_index = p_finger * 4U + p_bone;
    l_result = glm::inverse((p_bone == 1U) ? m_rotation : m_bonesRotations[l_index - 1U]) * m_bonesRotations[l_index];
}

float LeapHand::GetFingerBend(size_t p_finger) const
{
    return ((p_finger >= 5U) ? 0.f : m_fingersBends[p_finger]);
}

float LeapHand::GetGrabValue() const
{
    return m_grabValue;
}

void LeapHand::Update(const LEAP_HAND & p_hand)
{
    m_visible = true;

    ConvertPosition(p_hand.arm.next_joint, m_position);
    ConvertPosition(p_hand.palm.velocity, m_velocity);
    ConvertRotation(p_hand.palm.orientation, m_rotation);
    m_rotation = glm::normalize(m_rotation);

    m_handOffset = glm::vec3(0.0f, 0.0f, -((-0.001f * p_hand.arm.prev_joint.y - -0.001f * p_hand.arm.next_joint.y) - (-0.001f * p_hand.palm.position.y - -0.001f * p_hand.arm.next_joint.y)));

    // Bends
    for(size_t i = 0U; i < 5U; i++)
    {
        m_fingersBends[i] = 0.f;

        glm::vec3 l_prevDirection;
        size_t l_startBoneIndex = ((i == 0U) ? 1U : 0U);
        for(size_t j = l_startBoneIndex; j < 4U; j++)
        {
            const LEAP_BONE &l_bone = p_hand.digits[i].bones[j];
            glm::vec3 l_direction(l_bone.next_joint.x - l_bone.prev_joint.x, l_bone.next_joint.y - l_bone.prev_joint.y, l_bone.next_joint.z - l_bone.prev_joint.z);
            l_direction = glm::normalize(l_direction);
            if(j > l_startBoneIndex) m_fingersBends[i] += glm::acos(glm::dot(l_direction, l_prevDirection));
            l_prevDirection = l_direction;
        }

        m_fingersBends[i] = NormalizeRange(m_fingersBends[i], (i == 0U) ? 0.f : g_piQuarter, (i == 0U) ? g_piQuarter : g_pi);
    }

    m_grabValue = (NormalizeRange(m_fingersBends[2U], 0.5f, 1.f) + NormalizeRange(m_fingersBends[3U], 0.5f, 1.f) + NormalizeRange(m_fingersBends[3U], 0.5f, 1.f)) / 3.f;

    // Convert fingers to HMD space
    for(size_t i = 0; i < 5U; i++)
    {
        for(size_t j = 0; j < 4U; j++)
        {
            size_t l_index = i * 4U + j;
            ConvertPosition(p_hand.digits[i].bones[j].prev_joint, m_bonesPositions[l_index]);
            ConvertRotation(p_hand.digits[i].bones[j].rotation, m_bonesRotations[l_index]);
        }
    }

    LeapController::UpdateHMDCoordinates();
}

void LeapHand::Update()
{
    m_visible = false;
}

void LeapHand::ConvertPosition(const LEAP_VECTOR & p_src, glm::vec3 & p_dst)
{
    // In desktop mode: +X - right, +Y - up, -Z - forward (as OpenGL)
    // In HMD mode: +X - left, +Y - forward, +Z - down (same basis, just rotated)
    p_dst.x = -0.001f * p_src.x;
    p_dst.y = -0.001f * p_src.z;
    p_dst.z = -0.001f * p_src.y;
}

void LeapHand::ConvertRotation(const LEAP_QUATERNION & p_src, glm::quat & p_dst)
{
    // Simple rotation is enough because Leap has same basis as SteamVR
    glm::quat l_rot(p_src.w, p_src.x, p_src.y, p_src.z);
    p_dst = g_hmdRotation * l_rot;
}
