#include <TrackedController.hpp>
#include <StateManager.hpp>
#include <Math.hpp>

#include <cstring>

TrackedController::TrackedController(vr::ETrackedControllerRole role)
{
    m_role = role;

    m_pose = { 0 };
    m_pose.deviceIsConnected = false;

    for (size_t i = 0U; i < 3; i++)
    {
        m_pose.vecAcceleration[i] = 0;
        m_pose.vecAngularAcceleration[i] = 0;
        m_pose.vecAngularVelocity[i] = 0;
        m_pose.vecDriverFromHeadTranslation[i] = 0;
    }

    m_pose.poseTimeOffset = .0;
    m_pose.qDriverFromHeadRotation = { 1, 0, 0, 0 };
    m_pose.qRotation = { 1, 0, 0, 0 };
    m_pose.qWorldFromDriverRotation = { 1, 0, 0, 0 };
    m_pose.result = vr::TrackingResult_Uninitialized;
    m_pose.shouldApplyHeadModel = false;
    m_pose.willDriftInYaw = false;

    for (size_t i = 0U; i < SB_Count; i++)
        m_boneTransform[i] = g_openHandGesture[i];

    if (m_role == vr::TrackedControllerRole_RightHand)
    {
        // Transformation inversion along 0YZ plane
        for (size_t i = 1U; i < SB_Count; i++)
        {
            m_boneTransform[i].position.v[0] *= -1.f;

            switch (i)
            {
            case SB_Wrist:
            {
                m_boneTransform[i].orientation.y *= -1.f;
                m_boneTransform[i].orientation.z *= -1.f;
            } break;
            case SB_Thumb0:
            case SB_IndexFinger0:
            case SB_MiddleFinger0:
            case SB_RingFinger0:
            case SB_PinkyFinger0:
            {
                m_boneTransform[i].orientation.z *= -1.f;
                std::swap(m_boneTransform[i].orientation.x, m_boneTransform[i].orientation.w);
                m_boneTransform[i].orientation.w *= -1.f;
                std::swap(m_boneTransform[i].orientation.y, m_boneTransform[i].orientation.z);
            } break;
            }
        }
    }
}

vr::EVRInitError TrackedController::Activate(uint32_t unObjectId)
{
	vr::EVRInitError result = vr::VRInitError_Driver_Failed;

	if (m_objectId == 999)
	{
		m_objectId = unObjectId;

        auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_objectId);

        vr::VRProperties()->SetStringProperty(props, vr::Prop_ManufacturerName_String, "Ultraleap");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_TrackingSystemName_String, "leapmotion");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_SerialNumber_String, m_role == vr::TrackedControllerRole_LeftHand ? "Leap_Left_Hand" : "Leap_Right_Hand");

        vr::VRProperties()->SetInt32Property(props, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_Controller);
        vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerHandSelectionPriority_Int32, 999);
        vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32, m_role);

        vr::VRProperties()->SetBoolProperty(props, vr::Prop_WillDriftInYaw_Bool, false);
        vr::VRProperties()->SetBoolProperty(props, vr::Prop_DeviceIsWireless_Bool, true);
        vr::VRProperties()->SetBoolProperty(props, vr::Prop_DeviceCanPowerOff_Bool, true);
        vr::VRProperties()->SetBoolProperty(props, vr::Prop_Identifiable_Bool, false);

        vr::VRProperties()->SetBoolProperty(props, vr::Prop_DeviceIsCharging_Bool, false);
        vr::VRProperties()->SetBoolProperty(props, vr::Prop_DeviceProvidesBatteryStatus_Bool, false);

        vr::VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "Leap_Hand");

        vr::VRProperties()->SetStringProperty(props, vr::Prop_ResourceRoot_String, "leapify");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_AdditionalDeviceSettingsPath_String, "{leapify}/settings/settingsschema.vrsettings");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String, "{leapify}/input/index_controller_profile.json");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}ultraleap_hand_left" : "{leapify}ultraleap_hand_right");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_hand_status_off.png" : "{leapify}/icons/right_hand_status_off.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_hand_status_searching.gif" : "{leapify}/icons/right_hand_status_searching.gif");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_hand_status_searching_alert.gif" : "{leapify}/icons//right_hand_status_searching_alert.gif");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_hand_status_ready.png" : "{leapify}/icons//right_hand_status_ready.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_hand_status_ready_alert.png" : "{leapify}/icons//right_hand_status_ready_alert.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_hand_status_error.png" : "{leapify}/icons//right_hand_status_error.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_hand_status_off.png" : "{leapify}/icons//right_hand_status_off.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_hand_status_ready_low.png" : "{leapify}/icons//right_hand_status_ready_low.png");

        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/system/click", &m_menuClick);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/trigger/value", &m_triggerValue, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);

        if (m_role == vr::TrackedControllerRole_LeftHand)
            vr::VRDriverInput()->CreateSkeletonComponent(props, "/input/skeleton/left", "/skeleton/hand/left", "/pose/raw", vr::VRSkeletalTracking_Full, nullptr, 0, &m_skeletonHandle);
        else
            vr::VRDriverInput()->CreateSkeletonComponent(props, "/input/skeleton/right", "/skeleton/hand/right", "/pose/raw", vr::VRSkeletalTracking_Full, nullptr, 0, &m_skeletonHandle);

        result = vr::VRInitError_None;
    }

	return result;
}

void TrackedController::Deactivate()
{
}

void TrackedController::EnterStandby()
{
}

void* TrackedController::GetComponent(const char* pchComponentNameAndVersion)
{
    return nullptr;
}

void TrackedController::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
}

vr::DriverPose_t TrackedController::GetPose()
{
    StateManager::Get().setLeapPose(m_pose, m_role);
    return m_pose;
}

void TrackedController::Update(LeapHand hand)
{
    // TODO: settings manager
    if (!vr::VRSettings()->GetBool("driver_leapify", "handTrackingEnabled")) {
        m_pose.deviceIsConnected = false;
        m_pose.poseIsValid = false;
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_objectId, GetPose(), sizeof(vr::DriverPose_t));
        return;
    }

    UpdatePose(hand);
    UpdateSkeletalPose(hand);
    UpdateInput(hand);
}

void TrackedController::UpdatePose(LeapHand hand)
{
    m_isControllerConnected = false;

    if (vr::VRSettings()->GetBool("driver_leapify", "skeletalDataPassthrough") || vr::VRSettings()->GetBool("driver_leapify", "positionalDataPassthrough"))
    {
        m_pose.deviceIsConnected = false;
    }
    else
    {
        if (vr::VRSettings()->GetBool("driver_leapify", "automaticControllerSwitching"))
        {
            auto const states = StateManager::Get().getControllerStates();

            bool flag = false;
            for (auto& state : states)
            {
                if (state.second.isIdle == false)
                {
                    flag = true;
                    break;
                }
            }

            m_isControllerConnected = flag;
            m_pose.deviceIsConnected = !m_isControllerConnected;
        }
        else {
            m_pose.deviceIsConnected = true;
        }
    }

    if (hand.role != vr::TrackedControllerRole_Invalid && !m_isControllerConnected)
    {
        float offset = -((static_cast<float>(LeapGetNow() - hand.timestamp) / 1000000));

        vr::TrackedDevicePose_t pose;
        vr::VRServerDriverHost()->GetRawTrackedDevicePoses(offset, &pose, 1);

        if (pose.bPoseIsValid)
        {
            for (size_t i = 0U; i < 3; i++)
                m_pose.vecWorldFromDriverTranslation[i] = pose.mDeviceToAbsoluteTracking.m[i][3];

            glm::mat4 hmdMatrix(1.0f);
            ConvertMatrix(pose.mDeviceToAbsoluteTracking, hmdMatrix);
            const glm::quat headRotation = glm::quat_cast(hmdMatrix);

            glm::quat root = headRotation *
                glm::angleAxis(glm::pi<float>(), glm::vec3(0, 0, 1)) *
                glm::angleAxis(-glm::half_pi<float>(), glm::vec3(1, 0, 0));

            m_pose.qDriverFromHeadRotation.w = 1;
            m_pose.qWorldFromDriverRotation = { root.w, root.x, root.y, root.z };
            m_pose.qRotation = { hand.palm.orientation.w, hand.palm.orientation.x, hand.palm.orientation.y, hand.palm.orientation.z };

            m_pose.vecPosition[0] = hand.arm.next_joint.x * 0.001f;
            m_pose.vecPosition[1] = hand.arm.next_joint.y * 0.001f;
            m_pose.vecPosition[2] = hand.arm.next_joint.z * 0.001f;

            m_pose.poseIsValid = true;
            m_pose.result = vr::TrackingResult_Running_OK;
        }
    }
    else {
        m_pose.poseIsValid = true;
        m_pose.result = vr::TrackingResult_Running_OutOfRange;
    }

    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_objectId, GetPose(), sizeof(vr::DriverPose_t));
}

void TrackedController::UpdateSkeletalPose(LeapHand hand)
{
    if (!vr::VRSettings()->GetBool("driver_leapify", "handTrackingEnabled"))
        return;

    auto QuatFromEuler = [](float pitch, float yaw, float roll) -> glm::quat {
        return glm::angleAxis(roll, glm::vec3(0, 0, 1)) *
            glm::angleAxis(yaw, glm::vec3(0, 1, 0)) *
            glm::angleAxis(pitch, glm::vec3(1, 0, 0));
        };

    auto LeapToGlmQuat = [](const LEAP_QUATERNION& q) -> glm::quat {
        return glm::normalize(glm::quat(q.w, q.x, q.y, q.z));
    };

    auto LeapToGlmVec3 = [](const LEAP_VECTOR& v) -> glm::vec3 {
        return glm::vec3(v.x, v.y, v.z) * 0.001f;
    };

    auto GetBoneRotation = [&](size_t bone) -> glm::quat {
        if (bone == SB_Root || bone == SB_Wrist) {
            return LeapToGlmQuat(hand.palm.orientation);
        }
        if (bone < SB_IndexFinger0) {
            size_t idx = std::min(bone - SB_Thumb0 + 1, size_t(3));
            return LeapToGlmQuat(hand.thumb.bones[idx].rotation);
        }
        if (bone < SB_Aux_Thumb) {
            size_t digit = (bone - SB_IndexFinger0) / 5 + 1;
            size_t idx = std::min((bone - SB_IndexFinger0) % 5, size_t(3));
            return LeapToGlmQuat(hand.digits[digit].bones[idx].rotation);
        }
        return glm::quat(1, 0, 0, 0);
    };

    auto GetBonePosition = [&](size_t bone) -> glm::vec3 {
        if (bone == SB_Root) {
            return LeapToGlmVec3(hand.palm.position);
        }
        if (bone == SB_Wrist) {
            return LeapToGlmVec3(hand.arm.next_joint);
        }
        if (bone < SB_IndexFinger0) {
            if (bone == SB_Thumb3) {
                return LeapToGlmVec3(hand.thumb.distal.next_joint);
            }
            return LeapToGlmVec3(hand.thumb.bones[bone - SB_Thumb0 + 1].prev_joint);
        }
        if (bone < SB_Aux_Thumb) {
            size_t digit = (bone - SB_IndexFinger0) / 5 + 1;
            size_t idx = (bone - SB_IndexFinger0) % 5;
            if (idx == 4) {
                return LeapToGlmVec3(hand.digits[digit].bones[idx - 1].next_joint);
            }
            return LeapToGlmVec3(hand.digits[digit].bones[idx].prev_joint);
        }
        return glm::vec3(0.0f);
    };

    const bool isLeft = (hand.role == vr::TrackedControllerRole_LeftHand);

    glm::vec3 rootPos = GetBonePosition(SB_Root);
    glm::quat rootRot = GetBoneRotation(SB_Root);
    glm::vec3 wristPos = GetBonePosition(SB_Wrist);
    glm::quat wristRot = GetBoneRotation(SB_Wrist);

    m_boneTransform[SB_Root] = vr::VRBoneTransform_t {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f, 0.0f }
    };

    glm::vec3 wristLocalPos = glm::inverse(rootRot) * (wristPos - rootPos);
    glm::quat wristLocalRot = glm::inverse(rootRot) * wristRot;

    glm::quat wristHandednessCorrection = QuatFromEuler(
        0.0f,
        glm::pi<float>(),
        isLeft ? -glm::half_pi<float>() : glm::half_pi<float>()
    );

    wristLocalRot = wristHandednessCorrection * wristLocalRot;

    m_boneTransform[SB_Wrist] = vr::VRBoneTransform_t{
        {wristLocalPos.x, wristLocalPos.y, wristLocalPos.z},
        {wristLocalRot.w, wristLocalRot.x, wristLocalRot.y, wristLocalRot.z}
    };

    glm::quat handedOrientation = QuatFromEuler(
        -glm::pi<float>(),
        isLeft ? glm::half_pi<float>() : -glm::half_pi<float>(),
        isLeft ? 0.0f : glm::pi<float>()
    );

    auto ComputeFingerTransforms = [&](size_t startBone, size_t endBone) {
        glm::vec3 parentPos = wristPos;
        glm::quat parentRot = wristRot;

        for (size_t bone = startBone; bone <= endBone; ++bone) {
            glm::vec3 bonePos = GetBonePosition(bone);
            glm::quat boneRot = GetBoneRotation(bone);

            boneRot = boneRot * handedOrientation;

            glm::vec3 localPos = glm::inverse(parentRot) * (bonePos - parentPos);
            glm::quat localRot = glm::inverse(parentRot) * boneRot;

            if (bone == startBone) {
                glm::quat wristCorrection = QuatFromEuler(
                    0.0f,
                    glm::pi<float>(),
                    isLeft ? -glm::half_pi<float>() : glm::half_pi<float>()
                );
                localPos = wristCorrection * localPos;
                localRot = glm::inverse(wristCorrection) * localRot;
            }

            m_boneTransform[bone] = vr::VRBoneTransform_t{
                {localPos.x, localPos.y, localPos.z},
                {localRot.w, localRot.x, localRot.y, localRot.z}
            };

            parentPos = bonePos;
            parentRot = boneRot;
        }
        };

    ComputeFingerTransforms(SB_Thumb0, SB_Thumb3);
    ComputeFingerTransforms(SB_IndexFinger0, SB_IndexFinger4);
    ComputeFingerTransforms(SB_MiddleFinger0, SB_MiddleFinger4);
    ComputeFingerTransforms(SB_RingFinger0, SB_RingFinger4);
    ComputeFingerTransforms(SB_PinkyFinger0, SB_PinkyFinger4);

    auto SetAuxBone = [&](size_t auxBone, size_t sourceBone) {
        glm::vec3 auxPos = (GetBonePosition(sourceBone) - rootPos) * rootRot;
        glm::quat auxRot = glm::inverse(rootRot) * GetBoneRotation(sourceBone);

        m_boneTransform[auxBone] = vr::VRBoneTransform_t{
            {auxPos.x, auxPos.y, auxPos.z},
            {auxRot.w, auxRot.x, auxRot.y, auxRot.z}
        };
    };

    SetAuxBone(SB_Aux_Thumb, SB_Thumb2);
    SetAuxBone(SB_Aux_IndexFinger, SB_IndexFinger3);
    SetAuxBone(SB_Aux_MiddleFinger, SB_MiddleFinger3);
    SetAuxBone(SB_Aux_RingFinger, SB_RingFinger3);
    SetAuxBone(SB_Aux_PinkyFinger, SB_PinkyFinger3);

    StateManager::Get().setLeapTransform(m_boneTransform, m_role);

    if (!vr::VRSettings()->GetBool("driver_leapify", "skeletalDataPassthrough")) {
        vr::VRDriverInput()->UpdateSkeletonComponent(m_skeletonHandle, vr::VRSkeletalMotionRange_WithoutController, m_boneTransform, SB_Count);
        vr::VRDriverInput()->UpdateSkeletonComponent(m_skeletonHandle, vr::VRSkeletalMotionRange_WithController, m_boneTransform, SB_Count);
    }
}

void TrackedController::UpdateInput(LeapHand hand)
{
    float offset = -((static_cast<float>(LeapGetNow() - hand.timestamp) / 1000000));

    vr::VRDriverInput()->UpdateBooleanComponent(m_menuClick, hand.gestures.menu, offset);
    vr::VRDriverInput()->UpdateScalarComponent(m_triggerValue, hand.gestures.index ? 1.0f : 0.0f, offset);
}
