#include "ControllerInput.h"

#include <JoyShockLibrary.h>

ControllerInput::ControllerInput()
{
    m_inputEnabled = true;
    m_deviceCount = 0;
}

ControllerInput::~ControllerInput()
{
    JslDisconnectAndDisposeAll();
}

bool ControllerInput::IsConnected()
{
    // NOTE: Joycons work as a pair, so we check if both are still alive, otherwise this check will fail.
    if ((m_devices[JS_TYPE_JOYCON_LEFT].connected && m_devices[JS_TYPE_JOYCON_RIGHT].connected) || m_devices[JS_TYPE_DS4].connected)
        return true;

    m_deviceCount = JslConnectDevices();

    std::vector<int> l_handles(m_deviceCount);
    JslGetConnectedDeviceHandles(l_handles.data(), m_deviceCount);

    for (auto l_handle : l_handles)
    {
        int l_type = JslGetControllerType(l_handle);
        m_devices[l_type].connected = true;
        m_devices[l_type].handle = l_handle;
    }

    // For other controllers, just one of them is enough
    if (m_devices[JS_TYPE_DS4].connected)
        return true;

    // For joycons, we need both of them connected
    return (m_devices[JS_TYPE_JOYCON_LEFT].connected && m_devices[JS_TYPE_JOYCON_RIGHT].connected);
}

void ControllerInput::Update(LeapController* p_left, LeapController* p_right)
{
    if (m_devices[JS_TYPE_JOYCON_LEFT].connected)
    {
        JOY_SHOCK_STATE l_state = JslGetSimpleState(m_devices[JS_TYPE_JOYCON_LEFT].handle);

        if (l_state.buttons & JSMASK_SR)
            m_inputEnabled = !m_inputEnabled;

        if (m_inputEnabled)
        {
            // Home
            p_left->SetButtonState(LeapController::IndexButton::IB_SystemClick, l_state.buttons & JSMASK_CAPTURE);

            // A
            p_left->SetButtonState(LeapController::IndexButton::IB_AClick, l_state.buttons & JSMASK_DOWN);

            // B
            p_left->SetButtonState(LeapController::IndexButton::IB_BClick, l_state.buttons & JSMASK_UP);

            // Joystick XY
            p_left->SetButtonState(LeapController::IndexButton::IB_ThumbstickClick, l_state.buttons & JSMASK_LCLICK);
            p_left->SetButtonValue(LeapController::IndexButton::IB_ThumbstickX, l_state.stickLX);
            p_left->SetButtonValue(LeapController::IndexButton::IB_ThumbstickY, l_state.stickLY);

            // Trigger
            p_left->SetButtonState(LeapController::IndexButton::IB_TriggerClick, l_state.buttons & JSMASK_ZL);
            p_left->SetButtonValue(LeapController::IndexButton::IB_TriggerValue, l_state.lTrigger);

            // Grip
            p_left->SetButtonValue(LeapController::IndexButton::IB_GripValue, (l_state.buttons & JSMASK_L) ? 1.f : 0.f);
            p_left->SetButtonValue(LeapController::IndexButton::IB_GripForce, (l_state.buttons & JSMASK_L) ? 1.f : 0.f);

            // Trackpad/touchpad
            p_left->SetButtonState(LeapController::IndexButton::IB_TrackpadTouch, l_state.buttons & JSMASK_SL);
            p_left->SetButtonValue(LeapController::IndexButton::IB_TrackpadForce, ((l_state.buttons & JSMASK_SL) && (l_state.buttons & JSMASK_SR)) ? 1.f : 0.f);
        }
    }

    if (m_devices[JS_TYPE_JOYCON_RIGHT].connected)
    {
        JOY_SHOCK_STATE l_state = JslGetSimpleState(m_devices[JS_TYPE_JOYCON_RIGHT].handle);

        if (l_state.buttons & JSMASK_SR)
            m_inputEnabled = !m_inputEnabled;

        if (m_inputEnabled)
        {
            // Home
            p_right->SetButtonState(LeapController::IndexButton::IB_SystemClick, l_state.buttons & JSMASK_HOME);

            // A
            p_right->SetButtonState(LeapController::IndexButton::IB_AClick, l_state.buttons & JSMASK_S);

            // B
            p_right->SetButtonState(LeapController::IndexButton::IB_BClick, l_state.buttons & JSMASK_N);

            // Joystick XY
            p_right->SetButtonState(LeapController::IndexButton::IB_ThumbstickClick, l_state.buttons & JSMASK_RCLICK);
            p_right->SetButtonValue(LeapController::IndexButton::IB_ThumbstickX, l_state.stickRX);
            p_right->SetButtonValue(LeapController::IndexButton::IB_ThumbstickY, l_state.stickRY);

            // Trigger
            p_right->SetButtonState(LeapController::IndexButton::IB_TriggerClick, l_state.buttons & JSMASK_ZR);
            p_right->SetButtonValue(LeapController::IndexButton::IB_TriggerValue, l_state.rTrigger);

            // Grip
            p_right->SetButtonValue(LeapController::IndexButton::IB_GripValue, (l_state.buttons & JSMASK_R) ? 1.0f : 0.0f);
            p_right->SetButtonValue(LeapController::IndexButton::IB_GripForce, (l_state.buttons & JSMASK_R) ? 1.0f : 0.0f);

            // Trackpad/touchpad
            p_right->SetButtonState(LeapController::IndexButton::IB_TrackpadTouch, l_state.buttons & JSMASK_SR);
            p_right->SetButtonValue(LeapController::IndexButton::IB_TrackpadForce, ((l_state.buttons & JSMASK_SR) && (l_state.buttons & JSMASK_SL)) ? 1.f : 0.f);
        }
    }

    if (m_devices[JS_TYPE_DS4].connected)
    {
        JOY_SHOCK_STATE l_state = JslGetSimpleState(m_devices[JS_TYPE_DS4].handle);

        // Home
        p_left->SetButtonState(LeapController::IndexButton::IB_SystemClick, l_state.buttons & JSMASK_HOME);

        // A
        p_left->SetButtonState(LeapController::IndexButton::IB_AClick, l_state.buttons & JSMASK_DOWN);

        // B
        p_left->SetButtonState(LeapController::IndexButton::IB_BClick, l_state.buttons & JSMASK_UP);

        // Joystick XY
        p_left->SetButtonState(LeapController::IndexButton::IB_ThumbstickClick, l_state.buttons & JSMASK_LCLICK);
        p_left->SetButtonValue(LeapController::IndexButton::IB_ThumbstickX, l_state.stickLX);
        p_left->SetButtonValue(LeapController::IndexButton::IB_ThumbstickY, l_state.stickLY);

        // Trigger
        p_left->SetButtonState(LeapController::IndexButton::IB_TriggerClick, l_state.buttons & JSMASK_ZL);
        p_left->SetButtonValue(LeapController::IndexButton::IB_TriggerValue, l_state.lTrigger);

        // Grip
        p_left->SetButtonValue(LeapController::IndexButton::IB_GripValue, (l_state.buttons & JSMASK_L) ? 1.f : 0.f);
        p_left->SetButtonValue(LeapController::IndexButton::IB_GripForce, (l_state.buttons & JSMASK_L) ? 1.f : 0.f);

        // Home
        p_right->SetButtonState(LeapController::IndexButton::IB_SystemClick, l_state.buttons & JSMASK_HOME);

        // A
        p_right->SetButtonState(LeapController::IndexButton::IB_AClick, l_state.buttons & JSMASK_S);

        // B
        p_right->SetButtonState(LeapController::IndexButton::IB_BClick, l_state.buttons & JSMASK_N);

        // Joystick XY
        p_right->SetButtonState(LeapController::IndexButton::IB_ThumbstickClick, l_state.buttons & JSMASK_RCLICK);
        p_right->SetButtonValue(LeapController::IndexButton::IB_ThumbstickX, l_state.stickRX);
        p_right->SetButtonValue(LeapController::IndexButton::IB_ThumbstickY, l_state.stickRY);

        // Trigger
        p_right->SetButtonState(LeapController::IndexButton::IB_TriggerClick, l_state.buttons & JSMASK_ZR);
        p_right->SetButtonValue(LeapController::IndexButton::IB_TriggerValue, l_state.rTrigger);

        // Grip
        p_right->SetButtonValue(LeapController::IndexButton::IB_GripValue, (l_state.buttons & JSMASK_R) ? 1.f : 0.f);
        p_right->SetButtonValue(LeapController::IndexButton::IB_GripForce, (l_state.buttons & JSMASK_R) ? 1.f : 0.f);
    }

    for (int i = 0, j = static_cast<int>(m_devices.size()); i < j; i++)
        m_devices[i].connected = JslStillConnected(m_devices[i].handle);
}