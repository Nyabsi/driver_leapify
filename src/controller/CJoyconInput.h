#pragma once

#include <stdint.h>

#include <controller/CLeapIndexController.h>

class CJoyconInput {
	struct Joycon {
		bool connected;
		int id;
	};

	Joycon m_joycons[2];
	int32_t m_deviceCount;
public:
	explicit CJoyconInput();
	~CJoyconInput();

	bool IsConnected();
	void Reconnect();

	void Update(CLeapIndexController* left, CLeapIndexController* right);
};