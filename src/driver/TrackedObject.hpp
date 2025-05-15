#pragma once

#include <core/LeapConnection.hpp>
#include <openvr_driver.h>

class TrackedObject : public vr::ITrackedDeviceServerDriver {
public:
	TrackedObject(std::string serial, bool isLeft = false, int32_t jointId = -1) : m_serial(serial), m_leftHandedness(isLeft), m_JointId(jointId) {}

	vr::EVRInitError Activate(uint32_t unObjectId);

	void Deactivate();

	void EnterStandby();

	void* GetComponent(const char* pchComponentNameAndVersion);

	void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);

	vr::DriverPose_t GetPose();

	const char* getSerial() { return m_serial.c_str(); }

	bool isLeft() const { return m_leftHandedness; }

	void Update(LeapHand hand);
private:
	std::string m_serial { "" };
	bool m_leftHandedness { false };
	int32_t m_JointId { 0 };
	vr::DriverPose_t m_pose { 0 };
	int32_t m_objectId { -1 };
	float m_offset { 0 };
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::quat headRotation;
};