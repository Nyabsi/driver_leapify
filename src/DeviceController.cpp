#include "DeviceController.hpp"

void DeviceController::AddController(const std::string& serial)
{
	if (m_controllers[0].m_serial.length() == 0 && serial.find("_Left") != -1)
	{
		m_controllers[0].m_serial = serial;
		m_controllers[0].m_objectId = 3;
		m_controllers[0].m_role = vr::TrackedControllerRole_LeftHand;
	}

	if (m_controllers[1].m_serial.length() == 0 && serial.find("_Right") != -1)
	{
		m_controllers[1].m_serial = serial;
		m_controllers[1].m_objectId = 4;
		m_controllers[1].m_role = vr::TrackedControllerRole_RightHand;
	}
}

void DeviceController::SetControllerIdAndRole(const std::string& serial, uint32_t objectId, vr::ETrackedControllerRole role)
{
	for (int i = 0; i < 2; i++)
	{
		if (m_controllers[i].m_serial == serial)
		{
			m_controllers[i].m_objectId = objectId;
			m_controllers[i].m_role = role;
		}
	}
}

void DeviceController::UpdateControllerPose(uint32_t objectId, vr::DriverPose_t pose)
{
	for (int i = 0; i < 2; i++)
	{
		if (m_controllers[i].m_objectId == objectId)
		{
			m_controllers[i].m_pose = pose;
		}
	}
}

Controller DeviceController::GetController(vr::ETrackedControllerRole role)
{
	for (int i = 0; i < 2; i++)
	{
		if (m_controllers[i].m_role == role)
		{
			return m_controllers[i];
		}
	}

	return Controller();
}

bool DeviceController::isValidController(uint32_t objectId)
{
	for (int i = 0; i < 2; i++)
	{
		if (m_controllers[i].m_objectId == objectId)
		{
			return true;
		}
	}

	return false;
}
