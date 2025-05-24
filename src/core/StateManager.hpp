#pragma once

#include <util/Singleton.hpp>
#include <openvr_driver.h>

#include <utility>
#include <vector>
#include <map>

// #include <util/SimpleKalmanFilter.hpp>

/*
struct TransformHook {
	vr::VRInputComponentHandle_t handle;
	vr::ETrackedControllerRole role;
};
*/

struct DeviceState {
	std::string manufacturerName { "Undefined" };
	std::string trackingSystem { "Undefined" };
	int32_t deviceClass { -1 };
	int32_t roleHint { -1 };
	double accelMagnitude { 0.0 };
	bool shouldWeBlockPose { false };
};

class StateManager {
private:
	std::map<uint32_t, DeviceState> m_devices {};
public:
	static inline StateManager& Get() { return Singleton<StateManager>::get(); }

	bool IsDeviceRegistered(uint32_t deviceId) {
		return m_devices.count(deviceId) > 0;
	}

	void RegisterDevice(uint32_t deviceId, DeviceState device) {
		m_devices[deviceId] = device;
	}

	void UpdateDevice(uint32_t deviceId, std::function<void(DeviceState&)> updater) {
    if (m_devices.count(deviceId))
        updater(m_devices[deviceId]);
	}

	std::vector<std::pair<uint32_t, DeviceState>> GetDeviceByClass(int32_t deviceClass) {
		std::vector<std::pair<uint32_t, DeviceState>> result = {};
		for (auto& [id, device] : m_devices) {
			if (device.deviceClass == deviceClass)
				result.push_back(std::make_pair(id, device));
		}
		return result;
	}

	std::pair<uint32_t, DeviceState> GetDeviceByHint(int32_t roleHint) {
		for (auto& [id, device] : m_devices) {
			if (device.roleHint == roleHint)
				return std::make_pair(id, device);
		}
		return std::make_pair(-1, DeviceState());
	}

	bool ShouldWeBlockDevicePose(uint32_t deviceId) {
		return m_devices[deviceId].shouldWeBlockPose;
	}
};