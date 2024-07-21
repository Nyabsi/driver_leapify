#pragma once

#include <string>
#include <array>

#include <Singleton.hpp>

#include <openvr_driver.h>

struct Controller {
	std::string m_serial { "" };
	uint32_t m_objectId { 999 };
	vr::ETrackedControllerRole m_role { vr::ETrackedControllerRole::TrackedControllerRole_Invalid };
	vr::DriverPose_t m_pose { 0 };
};

class DeviceController {
public:
	explicit DeviceController() { }
	inline static DeviceController& get() { return Singleton<DeviceController>::get(); }

	bool isHandTrackingEnabled() const { return m_isHandTrackingEnabled; }
	bool areControllersAvailable() const { return m_areControllersAvailable; }

	void AddController(const std::string& serial);
	void SetControllerIdAndRole(const std::string& serial, uint32_t objectId, vr::ETrackedControllerRole role);
	void UpdateControllerPose(uint32_t objectId, const vr::DriverPose_t& pose);

	Controller GetController(vr::ETrackedControllerRole role);
	bool isValidController(uint32_t objectId);
private:
	bool m_isHandTrackingEnabled { false };
	bool m_areControllersAvailable { false };
	std::array<Controller, 2> m_controllers;
};