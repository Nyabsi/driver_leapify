#pragma once

#include <Singleton.hpp>
#include <openvr_driver.h>

#include <vector>

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
private:
	vr::VRBoneTransform_t* m_passThroughTransformLeft { nullptr };
	vr::VRBoneTransform_t* m_passThroughTransformRight{ nullptr };
	std::vector<TransformHook> m_transformHooks { };
};