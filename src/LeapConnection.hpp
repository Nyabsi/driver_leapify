#pragma once

#include <LeapC.h>
#include <openvr_driver.h>
#include <thread>
#include <atomic>
#include <array>

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

struct LeapHand
{
	vr::ETrackedControllerRole role { vr::TrackedControllerRole_Invalid };
	union {
		struct {
			LEAP_DIGIT thumb;
			LEAP_DIGIT index;
			LEAP_DIGIT middle;
			LEAP_DIGIT ring;
			LEAP_DIGIT pinky;
		};
		LEAP_DIGIT digits[5] { 0 };
	};
	LEAP_PALM palm { 0 };
	LEAP_BONE arm { 0 };

	int64_t timestamp { 0 };
	LEAP_VECTOR accelerometer{ 0 };
	bool menu{ false };
};

class LeapConnection 
{
public:
	explicit LeapConnection() { }
	~LeapConnection() { }
	bool Init();
	void Begin();
	void End();
	bool IsConnected() const { return m_isPolling && m_DeviceCount > 0; }
	LeapHand GetHand(vr::ETrackedControllerRole role) const;
private:
	void Poll();
	std::thread m_pollThread { };
	std::atomic<bool> m_isPolling { false };
	std::atomic<uint32_t> m_DeviceCount { 0 };
	std::atomic<LeapHand> m_LeftHand { };
	std::atomic<LeapHand> m_RightHand { };
	std::atomic<int64_t> m_frameId { };
	std::atomic<LEAP_VECTOR> m_accelerometer { };
	LEAP_CONNECTION m_connection { nullptr };
};