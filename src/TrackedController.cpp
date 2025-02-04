#include <TrackedController.hpp>
#include <StateManager.hpp>
#include <Math.hpp>

#include <cstring>

constexpr glm::mat4 identityMatrix = glm::mat4(1.0f);
constexpr glm::vec4 zeroPoint = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
constexpr float pi = glm::pi<float>();
constexpr float piHalf = pi * 0.5f;

const glm::mat4 wristOffsetLeft = glm::inverse(glm::translate(identityMatrix, glm::vec3(-0.0445230342f, 0.0301547553f, 0.16438961f)) * glm::toMat4(glm::quat(1.50656376e-07f, -1.77612698e-08f, 0.927827835f, -0.373008907f)));
const glm::mat4 wristOffsetRight = glm::inverse(glm::translate(identityMatrix, glm::vec3(0.0445230342f, 0.0301547553f, 0.16438961f)) * glm::toMat4(glm::quat(1.50656376e-07f, -1.77612698e-08f, -0.927827835f, 0.373008907f)));
const glm::quat skeletonOffsetLeft = glm::quat(glm::vec3(pi, 0.f, piHalf));
const glm::quat skeletonOffsetRight = glm::quat(glm::vec3(pi, 0.f, -piHalf));
const glm::quat thumbOffset = glm::quat(glm::vec3(-piHalf * 0.5, 0.f, 0.f)) * glm::quat(glm::vec3(-piHalf, piHalf, -piHalf));
const glm::quat metacarpalOffset = glm::quat(glm::vec3(-piHalf, piHalf, 0.f));
const glm::quat mirroringOffset = glm::quat(glm::vec3(pi, 0.f, 0.f));

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

        vr::VRProperties()->SetInt32Property(props, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_TrackPad);
        vr::VRProperties()->SetInt32Property(props, vr::Prop_Axis1Type_Int32, vr::k_eControllerAxis_Trigger);
        vr::VRProperties()->SetInt32Property(props, vr::Prop_Axis2Type_Int32, vr::k_eControllerAxis_Trigger);

        vr::VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "Leap_Hand");

        vr::VRProperties()->SetStringProperty(props, vr::Prop_ResourceRoot_String, "leapify");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_AdditionalDeviceSettingsPath_String, "{leapify}/settings/settingsschema.vrsettings");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String, "{leapify}/input/index_controller_profile.json");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}valve_controller_knu_1_0_left" : "{leapify}valve_controller_knu_1_0_right");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_controller_status_off.png" : "{leapify}/icons/right_controller_status_off.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_controller_status_searching.gif" : "{leapify}/icons/right_controller_status_searching.gif");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_controller_status_searching_alert.gif" : "{leapify}/icons//right_controller_status_searching_alert.gif");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_controller_status_ready.png" : "{leapify}/icons//right_controller_status_ready.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_controller_status_ready_alert.png" : "{leapify}/icons//right_controller_status_ready_alert.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_controller_status_error.png" : "{leapify}/icons//right_controller_status_error.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_controller_status_off.png" : "{leapify}/icons//right_controller_status_off.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, m_role == vr::TrackedControllerRole_LeftHand ? "{leapify}/icons/left_controller_status_ready_low.png" : "{leapify}/icons//right_controller_status_ready_low.png");

        // vr::VRDriverInput()->CreateBooleanComponent(props, "/input/system/click", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 0 : 1).override);
        // vr::VRDriverInput()->CreateBooleanComponent(props, "/input/system/touch", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 2 : 3).override);

        // vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trigger/click", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 4 : 5).override);
        // vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trigger/touch", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 6 : 7).override);
        // vr::VRDriverInput()->CreateScalarComponent(props, "/input/trigger/value", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 8 : 9).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);

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
    ConvertPosition(hand.arm.next_joint, m_position);
    ConvertRotation(hand.palm.orientation, m_rotation);
    m_rotation = glm::normalize(m_rotation);

    for (size_t i = 0; i < 5; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            size_t index = i * 4 + j;
            ConvertPosition(hand.digits[i].bones[j].prev_joint, m_bonePositions[index]);
            ConvertRotation(hand.digits[i].bones[j].rotation, m_boneRotations[index]);
        }
    }

    UpdateSkeletalPose(hand);
    UpdatePose(hand);
}

void TrackedController::UpdatePose(LeapHand hand)
{
    if (vr::VRSettings()->GetBool("driver_leapify", "handTrackingEnabled"))
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
                for (auto& state : StateManager::Get().getControllerStates())
                {
                    if (state.second.isIdle == false)
                        m_isControllerConnected = true;
                }

                m_pose.deviceIsConnected = !m_isControllerConnected;
            }
            else {
                m_pose.deviceIsConnected = true;
            }
        }

        if (hand.role != vr::TrackedControllerRole_Invalid && !m_isControllerConnected)
        {
            vr::TrackedDevicePose_t pose;
            vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0, &pose, 1);
            if (pose.bPoseIsValid)
            {
                for (size_t i = 0U; i < 3; i++)
                    m_pose.vecWorldFromDriverTranslation[i] = pose.mDeviceToAbsoluteTracking.m[i][3];

                glm::mat4 hmdMatrix(1.0f);
                ConvertMatrix(pose.mDeviceToAbsoluteTracking, hmdMatrix);

                const glm::quat headRotation = glm::quat_cast(hmdMatrix);
                memcpy(&m_pose.qWorldFromDriverRotation, &headRotation, sizeof(glm::quat));

                m_pose.qDriverFromHeadRotation.w = 1;

                glm::quat root = headRotation * glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
                ConvertQuaternion(root, m_pose.qWorldFromDriverRotation);

                glm::vec3 velocity = root * glm::vec3(-0.001f * hand.palm.velocity.x, -0.001f * hand.palm.velocity.y, -0.001f * hand.palm.velocity.z);
                m_pose.vecVelocity[0] = velocity.x;
                m_pose.vecVelocity[1] = velocity.y;
                m_pose.vecVelocity[2] = velocity.z;

                glm::quat rotation = m_rotation * (m_role == vr::TrackedControllerRole_LeftHand ? skeletonOffsetLeft : skeletonOffsetRight);

                glm::mat4 matrix = glm::translate(identityMatrix, m_position) * glm::toMat4(rotation);
                matrix *= (m_role == vr::TrackedControllerRole_LeftHand ? wristOffsetLeft : wristOffsetRight);

                rotation = glm::toQuat(matrix);

                m_pose.qRotation.x = rotation.x;
                m_pose.qRotation.y = rotation.y;
                m_pose.qRotation.z = rotation.z;
                m_pose.qRotation.w = rotation.w;

                float offset = vr::VRSettings()->GetBool("driver_leapify", "automaticHandOffset") ? -((-0.001f * hand.arm.prev_joint.y - -0.001f * hand.arm.next_joint.y)) : -0.001f * vr::VRSettings()->GetFloat("driver_leapify", "manualHandOffset");

                glm::vec4 position = matrix * zeroPoint;
                m_pose.vecPosition[0] = position.x;
                m_pose.vecPosition[1] = position.y;
                m_pose.vecPosition[2] = position.z + offset;

                m_pose.poseIsValid = true;
                m_pose.result = vr::TrackingResult_Running_OK;
            }
        }
        else {
            m_pose.poseIsValid = false;
        }

        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_objectId, GetPose(), sizeof(vr::DriverPose_t));
    }
    else
    {
        m_pose.deviceIsConnected = false;
        m_pose.poseIsValid = false;
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_objectId, GetPose(), sizeof(vr::DriverPose_t));
    }
}

void TrackedController::UpdateSkeletalPose(LeapHand hand)
{
    if (vr::VRSettings()->GetBool("driver_leapify", "handTrackingEnabled"))
    {
        for (size_t i = 0U; i < 5; i++)
        {
            size_t index = GetFingerBoneIndex(i);
            bool thumb = (i == 0);

            for (size_t j = 0, k = (thumb ? 3 : 4); j < k; j++)
            {
                glm::quat rotation;
                GetFingerBoneLocalRotation(hand, i, (thumb ? (j + 1) : j), rotation, thumb);
                ChangeBoneOrientation(rotation);
                if (j == 0U)
                    FixMetacarpalBone(rotation);
                ConvertQuaternion(rotation, m_boneTransform[index + j].orientation);

                if (j > 0U)
                {
                    glm::vec3 position;
                    GetFingerBoneLocalPosition(hand, i, (thumb ? (j + 1) : j), position, thumb);
                    ChangeBonePosition(position);
                    ConvertVector3(position, m_boneTransform[index + j].position);
                }
            }
        }

        glm::vec3 position;
        glm::quat rotation;
        ConvertVector3(m_boneTransform[SB_Wrist].position, position);
        ConvertQuaternion(m_boneTransform[SB_Wrist].orientation, rotation);

        const glm::mat4 wristMatrix = glm::translate(identityMatrix, position) * glm::mat4_cast(rotation);
        for (size_t i = HF_Thumb; i < HF_Count; i++)
        {
            glm::mat4 chainMatrix(wristMatrix);
            const size_t l_chainIndex = GetFingerBoneIndex(i);
            for (size_t j = 0; j < ((i == HF_Thumb) ? 3 : 4); j++)
            {
                ConvertVector3(m_boneTransform[l_chainIndex + j].position, position);
                ConvertQuaternion(m_boneTransform[l_chainIndex + j].orientation, rotation);
                chainMatrix = chainMatrix * (glm::translate(identityMatrix, position) * glm::mat4_cast(rotation));
            }

            position = chainMatrix * zeroPoint;
            rotation = glm::quat_cast(chainMatrix);
            if (m_role == vr::TrackedControllerRole_LeftHand)
                ChangeAuxTransformation(position, rotation);

            ConvertVector3(position, m_boneTransform[SB_Aux_Thumb + i].position);
            ConvertQuaternion(rotation, m_boneTransform[SB_Aux_Thumb + i].orientation);
        }

        StateManager::Get().setLeapTransform(m_boneTransform, m_role);

        if (!vr::VRSettings()->GetBool("driver_leapify", "skeletalDataPassthrough"))
            vr::VRDriverInput()->UpdateSkeletonComponent(m_skeletonHandle, vr::VRSkeletalMotionRange_WithoutController, m_boneTransform, SB_Count);
    }
}

size_t TrackedController::GetFingerBoneIndex(size_t id)
{
    size_t result = 0;
    switch (id)
    {
    case HF_Thumb:
        result = SB_Thumb0;
        break;
    case HF_Index:
        result = SB_IndexFinger0;
        break;
    case HF_Middle:
        result = SB_MiddleFinger0;
        break;
    case HF_Ring:
        result = SB_RingFinger0;
        break;
    case HF_Pinky:
        result = SB_PinkyFinger0;
        break;
    }
    return result;
}

void TrackedController::GetFingerBoneLocalRotation(LeapHand hand, size_t finger, size_t bone, glm::quat& result, bool ignoreMeta)
{
    if ((finger >= 5) || (bone >= 4) || (ignoreMeta && (bone == 0)))
        return;

    size_t index = finger * 4U + bone;
    result = glm::inverse(((bone == 0) || (ignoreMeta && bone == 1)) ? m_rotation : m_boneRotations[index - 1]) * m_boneRotations[index];
}

void TrackedController::GetFingerBoneLocalPosition(LeapHand hand, size_t finger, size_t bone, glm::vec3& result, bool ignoreMeta)
{
    if ((finger >= 5) || (bone >= 4) || (ignoreMeta && (bone == 0)))
        return;

    size_t l_index = finger * 4U + bone;
    glm::vec3 parentPosition = (((bone == 0U) || (ignoreMeta && (bone == 1))) ? m_position : m_bonePositions[l_index - 1]);
    glm::quat parentRotation = (((bone == 0U) || (ignoreMeta && (bone == 1))) ? m_rotation : m_boneRotations[l_index - 1]);
    glm::mat4 parentMatrix = glm::translate(identityMatrix, parentPosition) * glm::toMat4(parentRotation);
    glm::mat4 childMatrix = glm::translate(identityMatrix, m_bonePositions[l_index]) * glm::toMat4(m_boneRotations[l_index]);
    glm::mat4 childLocal = glm::inverse(parentMatrix) * childMatrix;
    result = childLocal * zeroPoint;
}

void TrackedController::ChangeBoneOrientation(glm::quat& rotation)
{
    std::swap(rotation.x, rotation.z);
    rotation.z *= -1.f;
    if (m_role == vr::TrackedControllerRole_LeftHand)
    {
        rotation.x *= -1.f;
        rotation.y *= -1.f;
    }
}

void TrackedController::ChangeBonePosition(glm::vec3& position)
{
    std::swap(position.x, position.z);
    position.z *= -1.f;

    if (m_role == vr::TrackedControllerRole_LeftHand)
        position.x *= -1.f;
}

void TrackedController::FixMetacarpalBone(glm::quat& rotation)
{
    rotation = metacarpalOffset * rotation;

    if (m_role == vr::TrackedControllerRole_LeftHand)
        rotation = mirroringOffset * rotation;
}

void TrackedController::ChangeAuxTransformation(glm::vec3& position, glm::quat& rotation)
{
    position.y *= -1.f;
    position.z *= -1.f;

    std::swap(rotation.x, rotation.w);
    rotation.w *= -1.f;
    std::swap(rotation.y, rotation.z);
    rotation.y *= -1.f;
}
