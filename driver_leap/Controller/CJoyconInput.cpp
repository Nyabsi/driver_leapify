#include "CJoyconInput.h"

#include <thread>

#include <JoyShockLibrary.h>

using namespace std::chrono_literals;

CJoyconInput::CJoyconInput()
{
	for (size_t i = 0U; i < 2U; i++)
	{
		m_joycons[i].connected = false;
		m_joycons[i].id = -1;
	}

	m_deviceCount = JslConnectDevices();
}

CJoyconInput::~CJoyconInput()
{
	JslDisconnectAndDisposeAll();
}

bool CJoyconInput::IsConnected()
{
	int* handles = new int[m_deviceCount];
	JslGetConnectedDeviceHandles(handles, m_deviceCount);

    m_joycons[0].connected = false;
    m_joycons[1].connected = false;

    for (int i = 0; i < m_deviceCount; i++)
    {
        int handle = handles[i];
        int type = JslGetControllerType(handle);

        switch (type)
        {
            case JS_TYPE_JOYCON_LEFT:
                m_joycons[0].connected = true;
                m_joycons[0].id = handle;
                break;
            case JS_TYPE_JOYCON_RIGHT:
                m_joycons[1].connected = true;
                m_joycons[1].id = handle;
                break;
        }
    }

	return m_joycons[0].connected && m_joycons[1].connected;
}

void CJoyconInput::Reconnect()
{
    m_deviceCount = JslConnectDevices();

    if (m_deviceCount >= 2)
        IsConnected();
}

void CJoyconInput::Update(CLeapIndexController* left, CLeapIndexController* right)
{
    if (m_joycons[0].connected) // Left Hand
    {
        JOY_SHOCK_STATE state = JslGetSimpleState(m_joycons[0].id);

        // Home
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x20000);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x20000);

        // A
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x00002);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x00002);

        // B
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x00001);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x00001);

        // Joystick XY
        left->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickX, state.stickLX);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickY, state.stickLY);
    }

    if (m_joycons[1].connected) // Right Hand
    {
        JOY_SHOCK_STATE state = JslGetSimpleState(m_joycons[1].id);

        // Home
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x10000);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x10000);

        // A
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x01000);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x01000);

        // B
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x08000);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x08000);

        // Joystick XY
        right->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickX, state.stickRX);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickY, state.stickRY);
    }
}