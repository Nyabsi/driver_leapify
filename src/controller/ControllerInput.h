#pragma once

#include <stdint.h>
#include <map>

#include <controller/LeapController.h>
#include <thread>

class ControllerInput {
	struct Device {
		bool connected{ false };
		int handle{ -1 };
	};

	bool m_inputEnabled;
	int32_t m_deviceCount;
	std::map<uint8_t, Device> m_devices;
public:
	explicit ControllerInput();
	~ControllerInput();

	bool IsConnected();
	void Update(LeapController* left, LeapController* right);
};