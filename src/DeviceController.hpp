#pragma once

#include <string>
#include <array>

#include <Singleton.hpp>

#include <openvr_driver.h>

struct Controller {
	uint32_t m_objectId { 999 };
	vr::ETrackedControllerRole m_role { vr::ETrackedControllerRole::TrackedControllerRole_Invalid };
	vr::DriverPose_t m_pose { 0 };
};

class DeviceController {
public:
	explicit DeviceController();
	inline static DeviceController& get() { return Singleton<DeviceController>::get(); }

	bool isHandTrackingEnabled() const { return m_isHandTrackingEnabled; }
	bool areControllersAvailable() const { return m_areControllersAvailable; }

	void UpdateControllerPose(vr::ETrackedControllerRole role, vr::DriverPose_t pose);
	void UpdateControllerId(vr::ETrackedControllerRole role, uint32_t objectId);
	Controller GetController(vr::ETrackedControllerRole role);
private:
	bool m_isHandTrackingEnabled { false };
	bool m_areControllersAvailable { false };
	std::array<Controller, 2> m_controllers;
};