#pragma once

#include <Singleton.hpp>
#include <openvr_driver.h>

class StateManager {
public:
	static inline StateManager& Get() { return Singleton<StateManager>::get(); }
	vr::VRBoneTransform_t* getLeapTransform() const { return m_passThroughTransform; }
	void setLeapTransform(vr::VRBoneTransform_t* transform) { m_passThroughTransform = transform;  }
private:
	vr::VRBoneTransform_t* m_passThroughTransform;
};