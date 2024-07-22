#pragma once

#include <LeapC.h>
#include <openvr_driver.h>
#include <thread>
#include <atomic>
#include <array>

struct LeapHand
{
	vr::ETrackedControllerRole role;
	union {
		struct {
			LEAP_DIGIT thumb;
			LEAP_DIGIT index;
			LEAP_DIGIT middle;
			LEAP_DIGIT ring;
			LEAP_DIGIT pinky;
		};
		LEAP_DIGIT digits[5];
	};
	LEAP_BONE arm;
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
	LEAP_CONNECTION m_connection { nullptr };
};