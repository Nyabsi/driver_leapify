#include "DeviceController.hpp"

#include <openvr_driver.h>

DeviceController::DeviceController()
{
	m_controllers[0].m_objectId = 999;
	m_controllers[0].m_role = vr::TrackedControllerRole_LeftHand;
	m_controllers[0].m_pose = { 0 };

	m_controllers[1].m_objectId = 999;
	m_controllers[1].m_role = vr::TrackedControllerRole_RightHand;
	m_controllers[1].m_pose = { 0 };
}

void DeviceController::UpdateControllerPose(vr::ETrackedControllerRole role, vr::DriverPose_t pose)
{
	for (int i = 0; i < 2; i++)
	{
		if (m_controllers[i].m_role == role)
		{
			m_controllers[i].m_pose = pose;
		}
	}
}

void DeviceController::UpdateController(vr::ETrackedControllerRole role, uint32_t objectId, const std::string& serial, const std::string& manufacturer)
{
	for (int i = 0; i < 2; i++)
	{
		if (m_controllers[i].m_role == role)
		{
			m_controllers[i].m_objectId = objectId;
			m_controllers[i].m_serial = serial;
			m_controllers[i].m_manufacturer = manufacturer;
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


Component& DeviceController::getComponent(std::string serial, int index)
{
	return m_components[serial][index];
}

Component& DeviceController::getComponentOverride(int index)
{
	return m_componentOverrides[index];
}
