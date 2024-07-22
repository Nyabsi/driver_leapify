#include <TrackedDeviceProvider.hpp>

#include <TrackedController.hpp>
#include <InterfaceHook.hpp>
#include <DeviceController.hpp>
#include <LeapConnection.hpp>

#include <openvr_driver.h>

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    m_InterfaceHook.Init(pDriverContext);

    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Hand_Left", vr::TrackedDeviceClass_Controller, &m_Left);
    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Hand_Right", vr::TrackedDeviceClass_Controller, &m_Right);

    bool connected = m_LeapConnection.Init();
    DeviceController::get().isHandTrackingEnabled(connected);

    if (connected)
        m_LeapConnection.Begin();

    return vr::VRInitError_None;
}

void TrackedDeviceProvider::Cleanup()
{
    m_LeapConnection.End();
}

void TrackedDeviceProvider::RunFrame()
{
    m_Left.Update();
    m_Right.Update();
}