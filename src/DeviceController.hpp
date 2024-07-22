#pragma once

#include <string>
#include <array>
#include <map>

#include <Singleton.hpp>

#include <openvr_driver.h>

struct Controller {
	uint32_t objectId { 999 };
	vr::ETrackedControllerRole role { vr::TrackedControllerRole_Invalid };
	vr::DriverPose_t pose { 0 };
	std::string serial { "" };
	bool hasCurl { false };
};

struct Component {
	vr::VRInputComponentHandle_t orig;
	vr::VRInputComponentHandle_t override;
};

class DeviceController {
public:
	explicit DeviceController();
	inline static DeviceController& get() { return Singleton<DeviceController>::get(); }

	bool isHandTrackingAvailable() const { return m_isHandTrackingAvailable; }
	void isHandTrackingAvailable(bool value) { m_isHandTrackingAvailable = value; }

	bool isHandTrackingEnabled() const { return m_isHandTrackingEnabled; }
	void isHandTrackingEnabled(bool value) { m_isHandTrackingEnabled = value; }

	void UpdateControllerPose(vr::ETrackedControllerRole role, vr::DriverPose_t pose);
	void UpdateController(vr::ETrackedControllerRole role, uint32_t objectId, const std::string& serial, bool hasCurl);
	Controller GetController(vr::ETrackedControllerRole role);

	Component& getComponent(std::string serial, int index);
	Component& getComponent(int index);

	std::map<std::string, std::map<int, Component>> GetComponents() { return m_components; }
private:
	bool m_isHandTrackingAvailable { false };
	bool m_isHandTrackingEnabled { false };
	std::array<Controller, 2> m_controllers;
	std::map<std::string, std::map<int, Component>> m_components;
	std::map<int, Component> m_componentOverrides;
};