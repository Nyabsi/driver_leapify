#pragma once

#include <openvr_driver.h>
#include <LeapConnection.hpp>

#include <glm/gtx/quaternion.hpp>

enum SkeletonBone : size_t
{
    SB_Root = 0U,
    SB_Wrist,
    SB_Thumb0,
    SB_Thumb1,
    SB_Thumb2,
    SB_Thumb3, // Last, no effect
    SB_IndexFinger0,
    SB_IndexFinger1,
    SB_IndexFinger2,
    SB_IndexFinger3,
    SB_IndexFinger4, // Last, no effect
    SB_MiddleFinger0,
    SB_MiddleFinger1,
    SB_MiddleFinger2,
    SB_MiddleFinger3,
    SB_MiddleFinger4, // Last, no effect
    SB_RingFinger0,
    SB_RingFinger1,
    SB_RingFinger2,
    SB_RingFinger3,
    SB_RingFinger4, // Last, no effect
    SB_PinkyFinger0,
    SB_PinkyFinger1,
    SB_PinkyFinger2,
    SB_PinkyFinger3,
    SB_PinkyFinger4, // Last, no effect
    SB_Aux_Thumb,
    SB_Aux_IndexFinger,
    SB_Aux_MiddleFinger,
    SB_Aux_RingFinger,
    SB_Aux_PinkyFinger,

    SB_Count
};
enum HandFinger : size_t
{
    HF_Thumb = 0U,
    HF_Index,
    HF_Middle,
    HF_Ring,
    HF_Pinky,

    HF_Count
};

const std::array<vr::VRBoneTransform_t, 31U> g_openHandGesture =
{
    vr::VRBoneTransform_t {{ 0.000000f, 0.000000f, 0.000000f, 1.000000f }, { 1.000000f, -0.000000f, -0.000000f, 0.000000f }},
    vr::VRBoneTransform_t {{ -0.0445230342f,0.0301547553f,0.16438961f, 1.000000f}, { 1.50656376e-07f, -1.77612698e-08f,0.927827835f,-0.373008907f }}, // Wrist values from SteamVR Unity package with disabled skeleton blending
    vr::VRBoneTransform_t {{ -0.012083f, 0.028070f, 0.025050f, 1.000000f }, { 0.464112f, 0.567418f, 0.272106f, 0.623374f }},
    vr::VRBoneTransform_t {{ 0.040406f, 0.000000f, -0.000000f, 1.000000f }, { 0.994838f, 0.082939f, 0.019454f, 0.055130f }},
    vr::VRBoneTransform_t {{ 0.032517f, 0.000000f, 0.000000f, 1.000000f }, { 0.974793f, -0.003213f, 0.021867f, -0.222015f }},
    vr::VRBoneTransform_t {{ 0.030464f, -0.000000f, -0.000000f, 1.000000f }, { 1.000000f, -0.000000f, -0.000000f, 0.000000f }},
    vr::VRBoneTransform_t {{ 0.000632f, 0.026866f, 0.015002f, 1.000000f }, { 0.644251f, 0.421979f, -0.478202f, 0.422133f }},
    vr::VRBoneTransform_t {{ 0.074204f, -0.005002f, 0.000234f, 1.000000f }, { 0.995332f, 0.007007f, -0.039124f, 0.087949f }},
    vr::VRBoneTransform_t {{ 0.043930f, -0.000000f, -0.000000f, 1.000000f }, { 0.997891f, 0.045808f, 0.002142f, -0.045943f }},
    vr::VRBoneTransform_t {{ 0.028695f, 0.000000f, 0.000000f, 1.000000f }, { 0.999649f, 0.001850f, -0.022782f, -0.013409f }},
    vr::VRBoneTransform_t {{ 0.022821f, 0.000000f, -0.000000f, 1.000000f }, { 1.000000f, -0.000000f, 0.000000f, -0.000000f }},
    vr::VRBoneTransform_t {{ 0.002177f, 0.007120f, 0.016319f, 1.000000f }, { 0.546723f, 0.541276f, -0.442520f, 0.460749f }},
    vr::VRBoneTransform_t {{ 0.070953f, 0.000779f, 0.000997f, 1.000000f }, { 0.980294f, -0.167261f, -0.078959f, 0.069368f }},
    vr::VRBoneTransform_t {{ 0.043108f, 0.000000f, 0.000000f, 1.000000f }, { 0.997947f, 0.018493f, 0.013192f, 0.059886f }},
    vr::VRBoneTransform_t {{ 0.033266f, 0.000000f, 0.000000f, 1.000000f }, { 0.997394f, -0.003328f, -0.028225f, -0.066315f }},
    vr::VRBoneTransform_t {{ 0.025892f, -0.000000f, 0.000000f, 1.000000f }, { 0.999195f, -0.000000f, 0.000000f, 0.040126f }},
    vr::VRBoneTransform_t {{ 0.000513f, -0.006545f, 0.016348f, 1.000000f }, { 0.516692f, 0.550143f, -0.495548f, 0.429888f }},
    vr::VRBoneTransform_t {{ 0.065876f, 0.001786f, 0.000693f, 1.000000f }, { 0.990420f, -0.058696f, -0.101820f, 0.072495f }},
    vr::VRBoneTransform_t {{ 0.040697f, 0.000000f, 0.000000f, 1.000000f }, { 0.999545f, -0.002240f, 0.000004f, 0.030081f }},
    vr::VRBoneTransform_t {{ 0.028747f, -0.000000f, -0.000000f, 1.000000f }, { 0.999102f, -0.000721f, -0.012693f, 0.040420f }},
    vr::VRBoneTransform_t {{ 0.022430f, -0.000000f, 0.000000f, 1.000000f }, { 1.000000f, 0.000000f, 0.000000f, 0.000000f }},
    vr::VRBoneTransform_t {{ -0.002478f, -0.018981f, 0.015214f, 1.000000f }, { 0.526918f, 0.523940f, -0.584025f, 0.326740f }},
    vr::VRBoneTransform_t {{ 0.062878f, 0.002844f, 0.000332f, 1.000000f }, { 0.986609f, -0.059615f, -0.135163f, 0.069132f }},
    vr::VRBoneTransform_t {{ 0.030220f, 0.000000f, 0.000000f, 1.000000f }, { 0.994317f, 0.001896f, -0.000132f, 0.106446f }},
    vr::VRBoneTransform_t {{ 0.018187f, 0.000000f, 0.000000f, 1.000000f }, { 0.995931f, -0.002010f, -0.052079f, -0.073526f }},
    vr::VRBoneTransform_t {{ 0.018018f, 0.000000f, -0.000000f, 1.000000f }, { 1.000000f, 0.000000f, 0.000000f, 0.000000f }},
    vr::VRBoneTransform_t {{ -0.006059f, 0.056285f, 0.060064f, 1.000000f }, { 0.737238f, 0.202745f, 0.594267f, 0.249441f }},
    vr::VRBoneTransform_t {{ -0.040416f, -0.043018f, 0.019345f, 1.000000f }, { -0.290331f, 0.623527f, -0.663809f, -0.293734f }},
    vr::VRBoneTransform_t {{ -0.039354f, -0.075674f, 0.047048f, 1.000000f }, { -0.187047f, 0.678062f, -0.659285f, -0.265683f }},
    vr::VRBoneTransform_t {{ -0.038340f, -0.090987f, 0.082579f, 1.000000f }, { -0.183037f, 0.736793f, -0.634757f, -0.143936f }},
    vr::VRBoneTransform_t {{ -0.031806f, -0.087214f, 0.121015f, 1.000000f }, { -0.003659f, 0.758407f, -0.639342f, -0.126678f }}
};

class TrackedController: public vr::ITrackedDeviceServerDriver
{
public:
	TrackedController(vr::ETrackedControllerRole role);

	vr::EVRInitError Activate(uint32_t unObjectId);
	void Deactivate();
	void EnterStandby();
	void* GetComponent(const char* pchComponentNameAndVersion);
	void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);
	vr::DriverPose_t GetPose();
	void Update(LeapHand hand);
private:
	void UpdatePose(LeapHand hand);
	void UpdateSkeletalPose(LeapHand hand);

	vr::ETrackedControllerRole m_role { vr::ETrackedControllerRole::TrackedControllerRole_Invalid };
	uint32_t m_objectId { 999 };
	vr::DriverPose_t m_pose { 0 };
	vr::VRInputComponentHandle_t m_skeletonHandle { };
    vr::VRInputComponentHandle_t m_triggerClick{ };
    vr::VRInputComponentHandle_t m_triggerValue{ };
    vr::VRInputComponentHandle_t m_menuClick{ };
	vr::VRBoneTransform_t m_boneTransform[31] { };
    glm::quat headRotation;
    bool m_isControllerConnected { };
};