#include "CLeapIndexController.h"
#include "CControllerButton.h"

#include <leap/CLeapHand.h>
#include <utils/Utils.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>

#include <string.h>

extern const std::array<vr::VRBoneTransform_t, 31U> g_openHandGesture;
extern const glm::mat4 g_identityMatrix;
extern const glm::vec4 g_zeroPoint;
const float g_pi = glm::pi<float>();
const float g_piHalf = g_pi * 0.5f;

// This is ... something ...
const glm::mat4 g_wristOffsetLeft = glm::inverse(glm::translate(g_identityMatrix, glm::vec3(-0.0445230342f, 0.0301547553f, 0.16438961f)) * glm::toMat4(glm::quat(1.50656376e-07f, -1.77612698e-08f, 0.927827835f, -0.373008907f)));
const glm::mat4 g_wristOffsetRight = glm::inverse(glm::translate(g_identityMatrix, glm::vec3(0.0445230342f, 0.0301547553f, 0.16438961f)) * glm::toMat4(glm::quat(1.50656376e-07f, -1.77612698e-08f, -0.927827835f, 0.373008907f)));
const glm::quat g_skeletonOffsetLeft = glm::quat(glm::vec3(g_pi, 0.f, g_piHalf));
const glm::quat g_skeletonOffsetRight = glm::quat(glm::vec3(g_pi, 0.f, -g_piHalf));
const glm::quat g_thumbOffset = glm::quat(glm::vec3(-g_piHalf * 0.5, 0.f, 0.f)) * glm::quat(glm::vec3(-g_piHalf, g_piHalf, -g_piHalf));
const glm::quat g_metacarpalOffset = glm::quat(glm::vec3(-g_piHalf, g_piHalf, 0.f));
const glm::quat g_mirroringOffset = glm::quat(glm::vec3(g_pi, 0.f, 0.f));

double CLeapIndexController::ms_headPosition[] = { .0, .0, .0 };
vr::HmdQuaternion_t CLeapIndexController::ms_headRotation = { 1.0, .0, .0, .0 };

enum TypeName : size_t
{
    TN_Button,
    TN_Axis
};
const std::vector<std::string> g_typeNames =
{
    "button", "axis"
};

enum InputName : size_t
{
    IN_A,
    IN_B,
    IN_System,
    IN_Thumbstick,
    IN_Touchpad,
};
const std::vector<std::string> g_inputNames =
{
    "a", "b", "system", "thumbstick", "touchpad"
};

enum StateName : size_t
{
    ST_None,
    ST_Touched,
    ST_Clicked,
};
const std::vector<std::string> g_stateNames =
{
    "none", "touched", "clicked"
};

CLeapIndexController::CLeapIndexController(bool p_left)
{
    m_isLeft = p_left;
    m_serialNumber.assign(m_isLeft ? "Leap_Left_Hand" : "Leap_Right_Hand");

    m_trackedDevice = vr::k_unTrackedDeviceIndexInvalid;
    m_propertyContainer = vr::k_ulInvalidPropertyContainer;
    m_skeletonHandle = vr::k_ulInvalidInputComponentHandle;
    m_haptic = vr::k_ulInvalidPropertyContainer;

    m_pose = { 0 };
    m_pose.deviceIsConnected = false;
    for(size_t i = 0U; i < 3U; i++)
    {
        m_pose.vecAcceleration[i] = .0;
        m_pose.vecAngularAcceleration[i] = .0;
        m_pose.vecAngularVelocity[i] = .0;
        m_pose.vecDriverFromHeadTranslation[i] = .0;
    }
    m_pose.poseTimeOffset = .0;
    m_pose.qDriverFromHeadRotation = { 1.0, .0, .0, .0 };
    m_pose.qRotation = { 1.0, .0, .0, .0 };
    m_pose.qWorldFromDriverRotation = { 1.0, .0, .0, .0 };
    m_pose.result = vr::TrackingResult_Uninitialized;
    m_pose.shouldApplyHeadModel = false;
    m_pose.willDriftInYaw = false;

    for(size_t i = 0U; i < SB_Count; i++)
        m_boneTransform[i] = g_openHandGesture[i];

    if(!m_isLeft)
    {
        // Transformation inversion along 0YZ plane
        for(size_t i = 1U; i < SB_Count; i++)
        {
            m_boneTransform[i].position.v[0] *= -1.f;

            switch(i)
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

CLeapIndexController::~CLeapIndexController()
{
    for(auto l_button : m_buttons)
        delete l_button;
}

vr::EVRInitError CLeapIndexController::Activate(uint32_t unObjectId)
{
    vr::EVRInitError result = vr::VRInitError_Driver_Failed;

    if (m_trackedDevice == vr::k_unTrackedDeviceIndexInvalid)
    {
        m_trackedDevice = unObjectId;
        m_propertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_trackedDevice);

        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ManufacturerName_String, "Ultraleap");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_TrackingSystemName_String, "leapmotion");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_SerialNumber_String, m_serialNumber.c_str());

        vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_Controller);
        vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerHandSelectionPriority_Int32, 999);
        vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, m_isLeft ? vr::TrackedControllerRole_LeftHand : vr::TrackedControllerRole_RightHand);

        vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_WillDriftInYaw_Bool, false);
        vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_DeviceIsWireless_Bool, true);
        vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_DeviceCanPowerOff_Bool, true);
        vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_Identifiable_Bool, true);

        vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_DeviceIsCharging_Bool, false);
        vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_DeviceProvidesBatteryStatus_Bool, true);
        vr::VRProperties()->SetFloatProperty(m_propertyContainer, vr::Prop_DeviceBatteryPercentage_Float, 1.0f); // TODO: read controller battery information.

        vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_TrackPad);
        vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_Axis1Type_Int32, vr::k_eControllerAxis_Trigger);
        vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_Axis2Type_Int32, vr::k_eControllerAxis_Trigger);

        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ModelNumber_String, "Leap_Hand");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ControllerType_String, "knuckles"); // TODO: remove on VRChat implements Skeletal Input
        
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ResourceRoot_String, "indexcontroller");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_InputProfilePath_String, "{indexcontroller}/input/index_controller_profile.json");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, m_isLeft ? "{indexcontroller}valve_controller_knu_1_0_left" : "{indexcontroller}valve_controller_knu_1_0_right");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceOff_String, m_isLeft ? "{indexcontroller}/icons/left_controller_status_off.png" : "{indexcontroller}/icons/right_controller_status_off.png");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, m_isLeft ? "{indexcontroller}/icons/left_controller_status_searching.gif" : "{indexcontroller}/icons/right_controller_status_searching.gif");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, m_isLeft ? "{indexcontroller}/icons/left_controller_status_searching_alert.gif" : "{indexcontroller}/icons//right_controller_status_searching_alert.gif");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReady_String, m_isLeft ? "{indexcontroller}/icons/left_controller_status_ready.png" : "{indexcontroller}/icons//right_controller_status_ready.png");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, m_isLeft ? "{indexcontroller}/icons/left_controller_status_ready_alert.png" : "{indexcontroller}/icons//right_controller_status_ready_alert.png");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, m_isLeft ? "{indexcontroller}/icons/left_controller_status_error.png" : "{indexcontroller}/icons//right_controller_status_error.png");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, m_isLeft ? "{indexcontroller}/icons/left_controller_status_off.png" : "{indexcontroller}/icons//right_controller_status_off.png");
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, m_isLeft ? "{indexcontroller}/icons/left_controller_status_ready_low.png" : "{indexcontroller}/icons//right_controller_status_ready_low.png");

        for (size_t i = 0; i < IB_Count; i++)
            m_buttons.push_back(new CControllerButton());

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/system/click", &m_buttons[IB_SystemClick]->GetHandleRef());
        m_buttons[IB_SystemClick]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/system/touch", &m_buttons[IB_SystemTouch]->GetHandleRef());
        m_buttons[IB_SystemTouch]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/trigger/click", &m_buttons[IB_TriggerClick]->GetHandleRef());
        m_buttons[IB_TriggerClick]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/trigger/touch", &m_buttons[IB_TriggerTouch]->GetHandleRef());
        m_buttons[IB_TriggerClick]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/trigger/value", &m_buttons[IB_TriggerValue]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        m_buttons[IB_TriggerValue]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/trackpad/x", &m_buttons[IB_TrackpadX]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
        m_buttons[IB_TrackpadX]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/trackpad/y", &m_buttons[IB_TrackpadY]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
        m_buttons[IB_TrackpadY]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/trackpad/touch", &m_buttons[IB_TrackpadTouch]->GetHandleRef());
        m_buttons[IB_TrackpadTouch]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/trackpad/force", &m_buttons[IB_TrackpadForce]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        m_buttons[IB_TrackpadForce]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/grip/touch", &m_buttons[IB_GripTouch]->GetHandleRef());
        m_buttons[IB_GripTouch]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/grip/force", &m_buttons[IB_GripForce]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        m_buttons[IB_GripForce]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/grip/value", &m_buttons[IB_GripValue]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        m_buttons[IB_GripValue]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/thumbstick/click", &m_buttons[IB_ThumbstickClick]->GetHandleRef());
        m_buttons[IB_ThumbstickClick]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/thumbstick/touch", &m_buttons[IB_ThumbstickTouch]->GetHandleRef());
        m_buttons[IB_ThumbstickTouch]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/thumbstick/x", &m_buttons[IB_ThumbstickX]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
        m_buttons[IB_ThumbstickX]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/thumbstick/y", &m_buttons[IB_ThumbstickY]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
        m_buttons[IB_ThumbstickY]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/a/click", &m_buttons[IB_AClick]->GetHandleRef());
        m_buttons[IB_AClick]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/a/touch", &m_buttons[IB_ATouch]->GetHandleRef());
        m_buttons[IB_ATouch]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/b/click", &m_buttons[IB_BClick]->GetHandleRef());
        m_buttons[IB_BClick]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/b/touch", &m_buttons[IB_BTouch]->GetHandleRef());
        m_buttons[IB_BTouch]->SetInputType(CControllerButton::IT_Boolean);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/finger/index", &m_buttons[IB_FingerIndex]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        m_buttons[IB_FingerIndex]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/finger/middle", &m_buttons[IB_FingerMiddle]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        m_buttons[IB_FingerMiddle]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/finger/ring", &m_buttons[IB_FingerRing]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        m_buttons[IB_FingerRing]->SetInputType(CControllerButton::IT_Float);

        vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/finger/pinky", &m_buttons[IB_FingerPinky]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        m_buttons[IB_FingerPinky]->SetInputType(CControllerButton::IT_Float);

        if (m_isLeft)
            vr::VRDriverInput()->CreateSkeletonComponent(m_propertyContainer, "/input/skeleton/left", "/skeleton/hand/left", "/pose/raw", vr::VRSkeletalTracking_Full, nullptr, 0U, &m_skeletonHandle);
        else
            vr::VRDriverInput()->CreateSkeletonComponent(m_propertyContainer, "/input/skeleton/right", "/skeleton/hand/right", "/pose/raw", vr::VRSkeletalTracking_Full, nullptr, 0U, &m_skeletonHandle);

        vr::VRDriverInput()->CreateHapticComponent(m_propertyContainer, "/output/haptic", &m_haptic);

        result = vr::VRInitError_None;
    }

    return result;
}

void CLeapIndexController::Deactivate()
{
    ResetControls();
    m_trackedDevice = vr::k_unTrackedDeviceIndexInvalid;
}

void CLeapIndexController::EnterStandby()
{
}

void* CLeapIndexController::GetComponent(const char* pchComponentNameAndVersion)
{
    void *l_result = nullptr;
    if (!strcmp(pchComponentNameAndVersion, vr::ITrackedDeviceServerDriver_Version)) 
        l_result = dynamic_cast<vr::ITrackedDeviceServerDriver*>(this);
    return l_result;
}

void CLeapIndexController::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{

}

vr::DriverPose_t CLeapIndexController::GetPose()
{
    return m_pose;
}

const std::string& CLeapIndexController::GetSerialNumber() const
{
    return m_serialNumber;
}

void CLeapIndexController::SetEnabled(bool p_state)
{
    m_pose.deviceIsConnected = p_state;
    if(m_trackedDevice != vr::k_unTrackedDeviceIndexInvalid) vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_trackedDevice, m_pose, sizeof(vr::DriverPose_t));
}

void CLeapIndexController::ResetControls()
{
    for(auto l_button : m_buttons)
    {
        l_button->SetValue(0.f);
        l_button->SetState(false);
    }
}

void CLeapIndexController::RunFrame(const CLeapHand *p_hand)
{
    if(m_trackedDevice != vr::k_unTrackedDeviceIndexInvalid)
    {
        if(m_pose.deviceIsConnected)
        {
            UpdatePose(p_hand);
            UpdateSkeletalInput(p_hand);
            UpdateInput(p_hand);
        }
        else vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_trackedDevice, m_pose, sizeof(vr::DriverPose_t));
    }
}

void CLeapIndexController::UpdatePose(const CLeapHand *p_hand)
{
    m_pose.poseIsValid = (p_hand && p_hand->IsVisible());

    if(m_pose.poseIsValid)
    {
        memcpy(m_pose.vecWorldFromDriverTranslation, ms_headPosition, sizeof(double) * 3U);
        memcpy(&m_pose.qWorldFromDriverRotation, &ms_headRotation, sizeof(vr::HmdQuaternion_t));

        // Root represents Leap Motion mount point on HMD
        // Root offset
        glm::quat l_headRot(ms_headRotation.w, ms_headRotation.x, ms_headRotation.y, ms_headRotation.z);
        glm::vec3 l_globalOffset = l_headRot * glm::vec3(0.0f, 0.0f, 0.0f);

        m_pose.vecWorldFromDriverTranslation[0] += l_globalOffset.x;
        m_pose.vecWorldFromDriverTranslation[1] += l_globalOffset.y;
        m_pose.vecWorldFromDriverTranslation[2] += l_globalOffset.z;

        // Root angle
        glm::quat l_globalRot = l_headRot * glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
        ConvertQuaternion(l_globalRot, m_pose.qWorldFromDriverRotation);

        glm::vec3 l_resultVelocity = l_globalRot * p_hand->GetVelocity();
        m_pose.vecVelocity[0] = l_resultVelocity.x;
        m_pose.vecVelocity[1] = l_resultVelocity.y;
        m_pose.vecVelocity[2] = l_resultVelocity.z;

        // Local transformation of device
        // Rotation
        glm::quat l_poseRotation = p_hand->GetRotation() * (m_isLeft ? g_skeletonOffsetLeft : g_skeletonOffsetRight);
        glm::mat4 l_mat = glm::translate(g_identityMatrix, p_hand->GetPosition()) * glm::toMat4(l_poseRotation);
        l_mat *= (m_isLeft ? g_wristOffsetLeft : g_wristOffsetRight);
        l_poseRotation = glm::toQuat(l_mat);
        m_pose.qRotation.x = l_poseRotation.x;
        m_pose.qRotation.y = l_poseRotation.y;
        m_pose.qRotation.z = l_poseRotation.z;
        m_pose.qRotation.w = l_poseRotation.w;

        // Positon
        glm::vec4 l_posePosition = l_mat * g_zeroPoint;
        m_pose.vecPosition[0] = l_posePosition.x + p_hand->GetOffset().x;
        m_pose.vecPosition[1] = l_posePosition.y + p_hand->GetOffset().y;
        m_pose.vecPosition[2] = l_posePosition.z + p_hand->GetOffset().z;

        m_pose.result = vr::TrackingResult_Running_OK;
    }
    else
    {
        for(size_t i = 0U; i < 3U; i++)
            m_pose.vecVelocity[i] = .0;

        m_pose.result = vr::TrackingResult_Running_OK;
        m_pose.poseIsValid = true;
    }

    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_trackedDevice, m_pose, sizeof(vr::DriverPose_t));
}

void CLeapIndexController::UpdateInput(const CLeapHand *p_hand)
{
    float l_trigger = p_hand->GetFingerBend(CLeapHand::Finger::Index);
    m_buttons[IB_FingerIndex]->SetValue(l_trigger);
    m_buttons[IB_FingerMiddle]->SetValue(p_hand->GetFingerBend(CLeapHand::Finger::Middle));
    m_buttons[IB_FingerRing]->SetValue(p_hand->GetFingerBend(CLeapHand::Finger::Ring));
    m_buttons[IB_FingerPinky]->SetValue(p_hand->GetFingerBend(CLeapHand::Finger::Pinky));

    for(auto l_button : m_buttons)
    {
        if(l_button->IsUpdated())
        {
            switch(l_button->GetInputType())
            {
                case CControllerButton::IT_Boolean:
                    vr::VRDriverInput()->UpdateBooleanComponent(l_button->GetHandle(), l_button->GetState(), .0);
                    break;
                case CControllerButton::IT_Float:
                    vr::VRDriverInput()->UpdateScalarComponent(l_button->GetHandle(), l_button->GetValue(), .0);
                    break;
            }
            l_button->ResetUpdate();
        }
    }
}

void CLeapIndexController::UpdateSkeletalInput(const CLeapHand *p_hand)
{
    // Skeletal structure update
    for(size_t i = 0U; i < 5U; i++)
    {
        size_t l_indexFinger = GetFingerBoneIndex(i);

        if (i == 0U)
        {
            for (size_t j = 0U; j < 3U; j++)
            {
                glm::quat l_rot;
                glm::vec3 l_pos;

                p_hand->GetThumbBoneLocalRotation(i, j + 1U, l_rot);
                p_hand->GetFingerBoneLocalPosition(i, j + 1U, l_pos);

                ChangeBoneOrientation(l_rot);
                if (j == 0U) FixMetacarpalBone(l_rot, false);
                ChangeBonePosition(l_pos);

                ConvertQuaternion(l_rot, m_boneTransform[l_indexFinger + j].orientation);
                if (j > 0U) ConvertVector3(l_pos, m_boneTransform[l_indexFinger + j].position);
            }
        }
        else
        {
            for (size_t j = 0U; j < 4U; j++)
            {
                glm::quat l_rot;
                glm::vec3 l_pos;

                p_hand->GetFingerBoneLocalRotation(i, j, l_rot);
                p_hand->GetFingerBoneLocalPosition(i, j, l_pos);

                ChangeBoneOrientation(l_rot);
                if (j == 0U)
                {
                    FixMetacarpalBone(l_rot, i == 0);
                }
                ChangeBonePosition(l_pos);

                ConvertQuaternion(l_rot, m_boneTransform[l_indexFinger + j].orientation);
                if (j > 0U) ConvertVector3(l_pos, m_boneTransform[l_indexFinger + j].position);
            }
        }
    }

    // Update aux bones
    glm::vec3 l_position;
    glm::quat l_rotation;
    ConvertVector3(m_boneTransform[SB_Wrist].position, l_position);
    ConvertQuaternion(m_boneTransform[SB_Wrist].orientation, l_rotation);
    const glm::mat4 l_wristMat = glm::translate(g_identityMatrix, l_position) * glm::mat4_cast(l_rotation);
    for(size_t i = HF_Thumb; i < HF_Count; i++)
    {
        glm::mat4 l_chainMat(l_wristMat);
        const size_t l_chainIndex = GetFingerBoneIndex(i);
        for(size_t j = 0U; j < ((i == HF_Thumb) ? 3U : 4U); j++)
        {
            ConvertVector3(m_boneTransform[l_chainIndex + j].position, l_position);
            ConvertQuaternion(m_boneTransform[l_chainIndex + j].orientation, l_rotation);
            l_chainMat = l_chainMat * (glm::translate(g_identityMatrix, l_position)*glm::mat4_cast(l_rotation));
        }
        l_position = l_chainMat * g_zeroPoint;
        l_rotation = glm::quat_cast(l_chainMat);
        if(m_isLeft)
            ChangeAuxTransformation(l_position, l_rotation);

        ConvertVector3(l_position, m_boneTransform[SB_Aux_Thumb + i].position);
        ConvertQuaternion(l_rotation, m_boneTransform[SB_Aux_Thumb + i].orientation);
    }

    vr::VRDriverInput()->UpdateSkeletonComponent(m_skeletonHandle, vr::VRSkeletalMotionRange_WithoutController, m_boneTransform, SB_Count);
}

void CLeapIndexController::SetButtonState(size_t p_button, bool p_state)
{
    if(p_button < m_buttons.size())
        m_buttons[p_button]->SetState(p_state);
}
void CLeapIndexController::SetButtonValue(size_t p_button, float p_value)
{
    if(p_button < m_buttons.size())
        m_buttons[p_button]->SetValue(p_value);
}

void CLeapIndexController::ChangeBoneOrientation(glm::quat &p_rot) const
{
    std::swap(p_rot.x, p_rot.z);
    p_rot.z *= -1.f;
    if(m_isLeft)
    {
        p_rot.x *= -1.f;
        p_rot.y *= -1.f;
    }
}

void CLeapIndexController::ChangeBonePosition(glm::vec3& p_pos) const
{
    std::swap(p_pos.x, p_pos.z);
    p_pos.z *= -1.f;

    if (m_isLeft)
    {
        p_pos.x *= -1.f;
        p_pos.y *= 1.f;
    }
}

void CLeapIndexController::FixThumbBone(glm::quat & p_rot) const
{
    p_rot = g_thumbOffset * p_rot;
    if(m_isLeft)
        p_rot = g_mirroringOffset * p_rot;
}

void CLeapIndexController::FixMetacarpalBone(glm::quat& p_rot, bool p_thumb) const
{
    if (p_thumb)
        p_rot = g_thumbOffset * p_rot;
    else
        p_rot = g_metacarpalOffset * p_rot;
    if (m_isLeft)
        p_rot = g_mirroringOffset * p_rot;
}

void CLeapIndexController::ChangeAuxTransformation(glm::vec3 &p_pos, glm::quat &p_rot)
{
    p_pos.y *= -1.f;
    p_pos.z *= -1.f;

    std::swap(p_rot.x, p_rot.w);
    p_rot.w *= -1.f;
    std::swap(p_rot.y, p_rot.z);
    p_rot.y *= -1.f;
}

size_t CLeapIndexController::GetFingerBoneIndex(size_t p_id)
{
    size_t l_result = 0U;
    switch(p_id)
    {
        case HF_Thumb:
            l_result = SB_Thumb0;
            break;
        case HF_Index:
            l_result = SB_IndexFinger0;
            break;
        case HF_Middle:
            l_result = SB_MiddleFinger0;
            break;
        case HF_Ring:
            l_result = SB_RingFinger0;
            break;
        case HF_Pinky:
            l_result = SB_PinkyFinger0;
            break;
    }
    return l_result;
}

void CLeapIndexController::UpdateHMDCoordinates()
{
    vr::TrackedDevicePose_t l_hmdPose;
    vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0.f, &l_hmdPose, 1U); // HMD has device ID 0
    if(l_hmdPose.bPoseIsValid)
    {
        glm::mat4 l_rotMat(1.f);
        ConvertMatrix(l_hmdPose.mDeviceToAbsoluteTracking, l_rotMat);

        const glm::quat l_headRot = glm::quat_cast(l_rotMat);
        ConvertQuaternion(l_headRot, ms_headRotation);

        for(size_t i = 0U; i < 3U; i++)
            ms_headPosition[i] = l_hmdPose.mDeviceToAbsoluteTracking.m[i][3];
    }
}
