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

        vr::VRDriverInput()->CreateBooleanComponent(props, "/input/system/click", &DeviceController::get().getComponent(m_role == vr::TrackedControllerRole_LeftHand ? 0 : 1).getOverride());
	    
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
    return m_lastPose;
}

void TrackedController::Update()
{
    if (m_objectId != 999)
    {
        auto controller = DeviceController::get().GetController(m_role);
        m_lastPose = controller.m_pose;
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_objectId, controller.m_pose, sizeof(vr::DriverPose_t));
    }
}
