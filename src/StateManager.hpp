#pragma once

#include <Singleton.hpp>
#include <openvr_driver.h>

#include <vector>
#include <map>

struct TransformHook {
	vr::VRInputComponentHandle_t handle;
	vr::ETrackedControllerRole role;
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

	void clearTransformHooks() { m_transformHooks.clear(); }

	void updateControllerState(int device, bool newState) { m_controllerStates[device] = newState; }
	std::map<int, bool> getControllerStates() { return m_controllerStates; }

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

	vr::DriverPose_t getFallbackPose(vr::ETrackedControllerRole role) const
	{
		if (role == vr::TrackedControllerRole_LeftHand)
			return m_fallbackPoseLeft;
		if (role == vr::TrackedControllerRole_RightHand)
			return m_fallbackPoseRight;
		return vr::DriverPose_t();
	}

	void setFallbackPose(vr::DriverPose_t pose, vr::ETrackedControllerRole role)
	{
		if (role == vr::TrackedControllerRole_LeftHand)
			m_fallbackPoseLeft = pose;
		else
			m_fallbackPoseRight = pose;
	}

	bool getShouldUseFallback() const
	{
		return m_shouldUseFallback;
	}

	void setShouldUseFallback(bool value)
	{
		m_shouldUseFallback = value;
	}
private:
	vr::VRBoneTransform_t* m_passThroughTransformLeft { nullptr };
	vr::VRBoneTransform_t* m_passThroughTransformRight { nullptr };
	std::vector<TransformHook> m_transformHooks { };
	std::map<int, bool> m_controllerStates { };
	vr::DriverPose_t m_passthroughPoseLeft { };
	vr::DriverPose_t m_passthroughPoseRight { };
	vr::DriverPose_t m_fallbackPoseLeft{ };
	vr::DriverPose_t m_fallbackPoseRight{ };
	bool m_shouldUseFallback { false };
};