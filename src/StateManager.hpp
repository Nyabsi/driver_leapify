#pragma once

#include <Singleton.hpp>
#include <openvr_driver.h>

#include <vector>
#include <map>

struct TransformHook {
	vr::VRInputComponentHandle_t handle;
	vr::ETrackedControllerRole role;
	std::string path; // reserved
};

struct ControllerState {
	bool isIdle{ false };
	float lastVelocity[3] = { 0.0f, 0.0f, 0.0f };
	int32_t role;
	bool touch{ false };
	bool grip{ false };
	bool trigger{ false };
	bool pad{ false };
};

class StateManager {
public:
	static inline StateManager& Get() { return Singleton<StateManager>::get(); }
	vr::VRBoneTransform_t* getLeapTransform(vr::ETrackedControllerRole role) const
	{ 
		if (role == vr::TrackedControllerRole_LeftHand)
			return m_passThroughTransformLeft;
		if (role == vr::TrackedControllerRole_RightHand)
			return m_passThroughTransformRight;

		return nullptr;
	}

	void setLeapTransform(vr::VRBoneTransform_t* transform, vr::ETrackedControllerRole role) 
	{ 
		if (role == vr::TrackedControllerRole_LeftHand)
			m_passThroughTransformLeft = transform;
		else
			m_passThroughTransformRight = transform;
	}

	void addTransformHook(vr::VRInputComponentHandle_t handle, vr::ETrackedControllerRole role) 
	{
		TransformHook hook {};
		hook.handle = handle;
		hook.role = role;

		m_transformHooks.push_back(hook);
	}

	vr::ETrackedControllerRole getTransformHookRole(vr::VRInputComponentHandle_t handle)
	{
		for (auto& hook : m_transformHooks)
		{
			if (hook.handle == handle)
				return hook.role;
		}

		return vr::TrackedControllerRole_Invalid;
	}

	void addButtonHook(vr::VRInputComponentHandle_t handle, vr::ETrackedControllerRole role, std::string path)
	{
		TransformHook hook{};
		hook.handle = handle;
		hook.role = role;
		hook.path = path;

		m_buttonHooks.push_back(hook);
	}

	TransformHook getButton(vr::VRInputComponentHandle_t handle)
	{
		for (auto& hook : m_buttonHooks)
		{
			if (hook.handle == handle)
				return hook;
		}

		return {};
	}

	void clearTransformHooks() { m_transformHooks.clear(); }

	void updateControllerState(int device, ControllerState newState) { m_controllerStates[device] = newState; }
	std::map<int, ControllerState> getControllerStates() { return m_controllerStates; }
	ControllerState& getState(int device) {
		return m_controllerStates[device];
	}

	ControllerState& getStatePerRole(vr::ETrackedControllerRole role)
	{
		for (auto& dev : m_controllerStates) {
			if (dev.second.role == role)
				return dev.second;
		}
	}

	vr::DriverPose_t getLeapPose(vr::ETrackedControllerRole role) const
	{ 
		if (role == vr::TrackedControllerRole_LeftHand)
			return m_passthroughPoseLeft;
		if (role == vr::TrackedControllerRole_RightHand)
			return m_passthroughPoseRight;
		return vr::DriverPose_t();
	}

	void setLeapPose(vr::DriverPose_t pose, vr::ETrackedControllerRole role)
	{ 
		if (role == vr::TrackedControllerRole_LeftHand)
			m_passthroughPoseLeft = pose;
		else
			m_passthroughPoseRight = pose;
	}
private:
	vr::VRBoneTransform_t* m_passThroughTransformLeft { nullptr };
	vr::VRBoneTransform_t* m_passThroughTransformRight { nullptr };
	std::vector<TransformHook> m_transformHooks { };
	std::vector<TransformHook> m_buttonHooks{ };
	std::map<int, ControllerState> m_controllerStates{ };
	vr::DriverPose_t m_passthroughPoseLeft { };
	vr::DriverPose_t m_passthroughPoseRight { };
};