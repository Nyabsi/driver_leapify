#include "DeviceController.hpp"

#include <openvr_driver.h>

DeviceController::DeviceController()
{
	m_controllers[0].objectId = 999;
	m_controllers[0].role = vr::TrackedControllerRole_LeftHand;
	m_controllers[0].pose = { 0 };

	m_controllers[1].objectId = 999;
	m_controllers[1].role = vr::TrackedControllerRole_RightHand;
	m_controllers[1].pose = { 0 };
}

void DeviceController::UpdateControllerPose(vr::ETrackedControllerRole role, vr::DriverPose_t pose)
{
	for (auto& controller : m_controllers)
	{
		if (controller.role == role)
		{
			controller.pose = pose;
		}
	}
}

void DeviceController::UpdateController(vr::ETrackedControllerRole role, uint32_t objectId, const std::string& serial, bool hasCurl)
{
	for (auto& controller : m_controllers)
	{
		if (controller.role == role)
		{
			controller.objectId = objectId;
			controller.serial = serial;
			controller.hasCurl = hasCurl;
		}
	}
}

Controller DeviceController::GetController(vr::ETrackedControllerRole role)
{
	for (auto& controller : m_controllers)
	{
		if (controller.role == role)
			return controller;
	}

	return Controller();
}


Component& DeviceController::getComponent(std::string serial, int index)
{
	return m_components[serial][index];
}

Component& DeviceController::getComponent(int index)
{
	return m_componentOverrides[index];
}
