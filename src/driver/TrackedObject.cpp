#include <driver/TrackedObject.hpp>
#include <cstring>
#include <glm/gtx/quaternion.hpp>

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

constexpr glm::mat4 identityMatrix = glm::mat4(1.0f);
constexpr glm::vec4 zeroPoint = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

vr::EVRInitError TrackedObject::Activate(uint32_t unObjectId)
{
    m_objectId = unObjectId;

    auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_objectId);

    vr::VRProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 4); // is this really needed?!! !!!

    vr::VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, getSerial());

    vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_OptOut);
    vr::VRProperties()->SetInt32Property(props, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_GenericTracker);
    vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerHandSelectionPriority_Int32, -1);

    vr::VRProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, "{htc}/rendermodels/vr_tracker_vive_3_0");

    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, "{htc}/icons/tracker_status_ready.png");

    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, "{htc}/icons/tracker_status_off.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, "{htc}/icons/tracker_status_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{htc}/icons/tracker_status_ready_alert.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{htc}/icons/tracker_status_ready_alert.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, "{htc}/icons/tracker_status_error.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, "{htc}/icons/tracker_status_standby.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, "{htc}/icons/tracker_status_ready_low.png");

    return vr::EVRInitError::VRInitError_None;
}

void TrackedObject::Deactivate()
{
}

void TrackedObject::EnterStandby()
{
}

void* TrackedObject::GetComponent(const char* pchComponentNameAndVersion)
{
	return nullptr;
}

void TrackedObject::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
}

vr::DriverPose_t TrackedObject::GetPose()
{
	return m_pose;
}

void TrackedObject::Update(LeapHand hand)
{
    if (hand.role != vr::TrackedControllerRole_Invalid)
    { 
        switch (m_JointId)
        {
        case 1:
            ConvertPosition(hand.arm.next_joint, m_position);
            break;
        case 2:
            ConvertPosition(hand.arm.prev_joint, m_position);
            break;
        default:
            break;
        }

        ConvertRotation(hand.palm.orientation, m_rotation);
        m_rotation = glm::normalize(m_rotation);

        m_offset = -(((static_cast<float>(LeapGetNow() - hand.timestamp) / 1000000)));

        vr::TrackedDevicePose_t pose;
        vr::VRServerDriverHost()->GetRawTrackedDevicePoses(m_offset, &pose, 1);

        if (pose.bPoseIsValid)
        {
            for (size_t i = 0U; i < 3; i++)
                m_pose.vecWorldFromDriverTranslation[i] = pose.mDeviceToAbsoluteTracking.m[i][3];

            glm::mat4 hmdMatrix(1.0f);
            ConvertMatrix(pose.mDeviceToAbsoluteTracking, hmdMatrix);

            headRotation = glm::quat_cast(hmdMatrix);

            memcpy(&m_pose.qWorldFromDriverRotation, &headRotation, sizeof(glm::quat));
        }
    }

    m_pose.deviceIsConnected = true;

    float offset = m_offset;

     m_pose.qDriverFromHeadRotation.w = 1;

     glm::quat root = headRotation * glm::quat(glm::radians(glm::vec3(vr::VRSettings()->GetFloat("driver_leapify", "rotationOffsetX"), vr::VRSettings()->GetFloat("driver_leapify", "rotationOffsetY"), vr::VRSettings()->GetFloat("driver_leapify", "rotationOffsetZ"))));
     ConvertQuaternion(root, m_pose.qWorldFromDriverRotation);
        
     glm::quat rotation = m_rotation;

     glm::mat4 matrix = glm::translate(identityMatrix, m_position) * glm::toMat4(rotation);
     matrix *= rotationMatrix;

     rotation = glm::toQuat(matrix);

     m_pose.qRotation.x = rotation.x;
     m_pose.qRotation.y = rotation.y;
     m_pose.qRotation.z = rotation.z;
     m_pose.qRotation.w = rotation.w;

     glm::vec4 position = matrix * zeroPoint;
     m_pose.vecPosition[0] = position.x;
     m_pose.vecPosition[1] = position.y;
     m_pose.vecPosition[2] = position.z;

     m_pose.poseIsValid = true;
     m_pose.result = vr::TrackingResult_Running_OK;

    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_objectId, GetPose(), sizeof(vr::DriverPose_t));
}
