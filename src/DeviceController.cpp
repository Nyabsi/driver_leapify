#include "DeviceController.hpp"

#include <Windows.h>

void DeviceController::AddController(const std::string& serial)
{
	if (m_controllers[0].m_serial.length() == 0)
		m_controllers[0].m_serial = serial;

	if (m_controllers[1].m_serial.length() == 0)
		m_controllers[1].m_serial = serial;
}

void DeviceController::SetControllerIdAndRole(const std::string& serial, uint32_t objectId, vr::ETrackedControllerRole role)
{
	for (auto& controller : m_controllers)
	{
		if (controller.m_serial == serial)
		{
			controller.m_objectId = objectId;
			controller.m_role = role;
		}
	}
}

void DeviceController::UpdateControllerPose(uint32_t objectId, const vr::DriverPose_t& pose)
{
	for (auto& controller : m_controllers)
	{
		if (controller.m_objectId == objectId)
		{
			controller.m_pose = pose;
		}
	}
}

Controller DeviceController::GetController(vr::ETrackedControllerRole role)
{
	for (auto& controller : m_controllers)
	{
		if (controller.m_role == role)
		{
			return controller;
		}
	}

	return Controller();
}

bool DeviceController::isValidController(uint32_t objectId)
{
	for (auto& controller : m_controllers)
	{
		if (controller.m_objectId == objectId)
		{
			return true;
		}
	}

	return false;
}
