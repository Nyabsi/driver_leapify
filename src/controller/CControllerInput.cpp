#include "CControllerInput.h"

#include <thread>

#include <JoyShockLibrary.h>

using namespace std::chrono_literals;

CControllerInput::CControllerInput()
{
	for (size_t i = 0U; i < 2U; i++)
	{
		m_joycons[i].connected = false;
		m_joycons[i].id = -1;
	}

    m_dualShock.connected = false;
    m_dualShock.id = -1;

	m_deviceCount = JslConnectDevices();
    m_anyActiveControllers = m_deviceCount >= 1;
}

CControllerInput::~CControllerInput()
{
	JslDisconnectAndDisposeAll();
}

bool CControllerInput::IsConnected()
{
	int* handles = new int[m_deviceCount];
	JslGetConnectedDeviceHandles(handles, m_deviceCount);

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
            case JS_TYPE_DS4:
                m_dualShock.connected = true;
                m_dualShock.id = handle;
                break;
        }
    }

	return m_anyActiveControllers;
}

void CControllerInput::Reconnect()
{
    m_deviceCount = JslConnectDevices();

    if (m_deviceCount >= 1)
    {
        m_anyActiveControllers = true;
        IsConnected();
    }
}

void CControllerInput::Update(CLeapIndexController* left, CLeapIndexController* right)
{
    if (m_joycons[0].connected) // Left Hand
    {
        JOY_SHOCK_STATE state = JslGetSimpleState(m_joycons[0].id);

        // Home
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x20000);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x20000);

        // A
        left->SetButtonState(CLeapIndexController::IndexButton::IB_AClick, state.buttons & 0x00002);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_ATouch, state.buttons & 0x00002);

        // B
        left->SetButtonState(CLeapIndexController::IndexButton::IB_BClick, state.buttons & 0x00001);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_BClick, state.buttons & 0x00001);

        // Joystick XY
        left->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickClick, state.buttons & 0x00040);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickTouch, state.stickLX > 0 && state.stickLY > 0);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_ThumbstickX, state.stickLX);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_ThumbstickY, state.stickLY);

        // Trigger
        left->SetButtonState(CLeapIndexController::IndexButton::IB_TriggerClick, state.buttons & 0x00400);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_TriggerValue, state.lTrigger);

        // Grip
        left->SetButtonState(CLeapIndexController::IndexButton::IB_GripTouch, state.buttons & 0x00100);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_GripValue, state.buttons & 0x00100 ? 1.0f : 0.0f);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_GripForce, state.buttons & 0x00100 ? 1.0f : 0.0f);
    }

    if (m_joycons[1].connected) // Right Hand
    {
        JOY_SHOCK_STATE state = JslGetSimpleState(m_joycons[1].id);

        // Home
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x10000);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x10000);

        // A
        right->SetButtonState(CLeapIndexController::IndexButton::IB_AClick, state.buttons & 0x01000);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_AClick, state.buttons & 0x01000);

        // B
        right->SetButtonState(CLeapIndexController::IndexButton::IB_BClick, state.buttons & 0x08000);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_BClick, state.buttons & 0x08000);

        // Joystick XY
        right->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickClick, state.buttons & 0x00080);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickTouch, state.stickRX > 0 && state.stickRY > 0);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_ThumbstickX, state.stickRX);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_ThumbstickY, state.stickRY);

        // Trigger
        right->SetButtonState(CLeapIndexController::IndexButton::IB_TriggerClick, state.buttons & 0x00800);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_TriggerValue, state.rTrigger);

        // Grip
        right->SetButtonState(CLeapIndexController::IndexButton::IB_GripTouch, state.buttons & 0x00200);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_GripValue, state.buttons & 0x00200 ? 1.0f : 0.0f);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_GripForce, state.buttons & 0x00200 ? 1.0f : 0.0f);
    }

    if (m_dualShock.connected)
    {
        JOY_SHOCK_STATE state = JslGetSimpleState(m_dualShock.id);

        // Home
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x10000);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x10000);

        // A
        left->SetButtonState(CLeapIndexController::IndexButton::IB_AClick, state.buttons & 0x00002);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_ATouch, state.buttons & 0x00002);

        // B
        left->SetButtonState(CLeapIndexController::IndexButton::IB_BClick, state.buttons & 0x00001);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_BClick, state.buttons & 0x00001);

        // Joystick XY
        left->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickClick, state.buttons & 0x00040);
        left->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickTouch, state.stickLX > 0 && state.stickLY > 0);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_ThumbstickX, state.stickLX);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_ThumbstickY, state.stickLY);

        // Trigger
        left->SetButtonState(CLeapIndexController::IndexButton::IB_TriggerClick, state.buttons & 0x00400);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_TriggerValue, state.lTrigger);

        // Grip
        left->SetButtonState(CLeapIndexController::IndexButton::IB_GripTouch, state.buttons & 0x00100);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_GripValue, state.buttons & 0x00100 ? 1.0f : 0.0f);
        left->SetButtonValue(CLeapIndexController::IndexButton::IB_GripForce, state.buttons & 0x00100 ? 1.0f : 0.0f);

        // Home
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemTouch, state.buttons & 0x10000);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_SystemClick, state.buttons & 0x10000);

        // A
        right->SetButtonState(CLeapIndexController::IndexButton::IB_AClick, state.buttons & 0x01000);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_AClick, state.buttons & 0x01000);

        // B
        right->SetButtonState(CLeapIndexController::IndexButton::IB_BClick, state.buttons & 0x08000);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_BClick, state.buttons & 0x08000);

        // Joystick XY
        right->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickClick, state.buttons & 0x00080);
        right->SetButtonState(CLeapIndexController::IndexButton::IB_ThumbstickTouch, state.stickRX > 0 && state.stickRY > 0);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_ThumbstickX, state.stickRX);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_ThumbstickY, state.stickRY);

        // Trigger
        right->SetButtonState(CLeapIndexController::IndexButton::IB_TriggerClick, state.buttons & 0x00800);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_TriggerValue, state.rTrigger);

        // Grip
        right->SetButtonState(CLeapIndexController::IndexButton::IB_GripTouch, state.buttons & 0x00200);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_GripValue, state.buttons & 0x00200 ? 1.0f : 0.0f);
        right->SetButtonValue(CLeapIndexController::IndexButton::IB_GripForce, state.buttons & 0x00200 ? 1.0f : 0.0f);
    }

    m_joycons[0].connected = false;
    m_joycons[1].connected = false;
    m_dualShock.connected = false;
}