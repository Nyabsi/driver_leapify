#include <TrackedController.hpp>
#include <DeviceController.hpp>

TrackedController::TrackedController(vr::ETrackedControllerRole role)
{
    m_role = role;
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
        vr::VRProperties()->SetStringProperty(props, vr::Prop_SerialNumber_String, m_role == vr::TrackedControllerRole_LeftHand ? "Leap_Hand_Left" : "Leap_Hand_Right");

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
        vr::VRProperties()->SetStringProperty(props, vr::Prop_ControllerType_String, "knuckles");

        vr::VRProperties()->SetStringProperty(props, vr::Prop_ResourceRoot_String, "indexcontroller");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String, "{indexcontroller}/input/index_controller_profile.json");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, m_role == vr::TrackedControllerRole_LeftHand ? "{indexcontroller}valve_controller_knu_1_0_left" : "{indexcontroller}valve_controller_knu_1_0_right");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, m_role == vr::TrackedControllerRole_LeftHand ? "{indexcontroller}/icons/left_controller_status_off.png" : "{indexcontroller}/icons/right_controller_status_off.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, m_role == vr::TrackedControllerRole_LeftHand ? "{indexcontroller}/icons/left_controller_status_searching.gif" : "{indexcontroller}/icons/right_controller_status_searching.gif");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, m_role == vr::TrackedControllerRole_LeftHand ? "{indexcontroller}/icons/left_controller_status_searching_alert.gif" : "{indexcontroller}/icons//right_controller_status_searching_alert.gif");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, m_role == vr::TrackedControllerRole_LeftHand ? "{indexcontroller}/icons/left_controller_status_ready.png" : "{indexcontroller}/icons//right_controller_status_ready.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, m_role == vr::TrackedControllerRole_LeftHand ? "{indexcontroller}/icons/left_controller_status_ready_alert.png" : "{indexcontroller}/icons//right_controller_status_ready_alert.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, m_role == vr::TrackedControllerRole_LeftHand ? "{indexcontroller}/icons/left_controller_status_error.png" : "{indexcontroller}/icons//right_controller_status_error.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, m_role == vr::TrackedControllerRole_LeftHand ? "{indexcontroller}/icons/left_controller_status_off.png" : "{indexcontroller}/icons//right_controller_status_off.png");
        vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, m_role == vr::TrackedControllerRole_LeftHand ? "{indexcontroller}/icons/left_controller_status_ready_low.png" : "{indexcontroller}/icons//right_controller_status_ready_low.png");

        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/system/click", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 0 : 1).override);
        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/system/touch", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 2 : 3).override);

        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trigger/click", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 4 : 5).override);
        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trigger/touch", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 6 : 7).override);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/trigger/value", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 8 : 9).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);

        vr::VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/x", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 10 : 11).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/y", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 12 : 13).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trackpad/touch", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 14 : 15).override);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/force", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 16 : 17).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);

        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/grip/touch", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 18 : 19).override);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/grip/force", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 20 : 21).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/grip/value", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 22 : 23).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);

        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/thumbstick/click", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 24 : 25).override);
        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/thumbstick/touch", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 26 : 27).override);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/thumbstick/x", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 28 : 29).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/thumbstick/y", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 30 : 31).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);

        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/a/click", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 32 : 33).override);
        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/a/touch", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 34 : 35).override);
        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/b/click", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 36 : 37).override);
        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/b/touch", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 38 : 39).override);

        vr::VRDriverInput()->CreateScalarComponent(props, "/input/finger/index", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 40 : 41).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/finger/middle", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 42 : 43).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/finger/ring", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 44 : 45).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
        vr::VRDriverInput()->CreateScalarComponent(props, "/input/finger/pinky", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 46 : 47).override, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);

        if (m_role == vr::TrackedControllerRole_LeftHand)
            vr::VRDriverInput()->CreateSkeletonComponent(props, "/input/skeleton/left", "/skeleton/hand/left", "/pose/raw", vr::VRSkeletalTracking_Full, nullptr, 0U, &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 48 : 49).override);
        else
            vr::VRDriverInput()->CreateSkeletonComponent(props, "/input/skeleton/right", "/skeleton/hand/right", "/pose/raw", vr::VRSkeletalTracking_Full, nullptr, 0U, &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 50 : 51).override);

        vr::VRDriverInput()->CreateHapticComponent(props, "/output/haptic", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 52 : 53).override);

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
    auto controller = DeviceController::get().GetController(m_role);
    return controller.pose;
}

void TrackedController::Update()
{
    if (m_objectId != 999)
    {
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_objectId, GetPose(), sizeof(vr::DriverPose_t));
    }
}
