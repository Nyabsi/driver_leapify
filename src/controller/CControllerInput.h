#pragma once

#include <stdint.h>

#include <controller/CLeapIndexController.h>

class CControllerInput {
	struct ControllerState {
		bool connected;
		int id;
	};

	ControllerState m_joycons[2];
	ControllerState m_dualShock;

	int32_t m_deviceCount;
	bool m_anyActiveControllers;
public:
	explicit CControllerInput();
	~CControllerInput();

	bool IsConnected();
	void Reconnect();

	void Update(CLeapIndexController* left, CLeapIndexController* right);
};