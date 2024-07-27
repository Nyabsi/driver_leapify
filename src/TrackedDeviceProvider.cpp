#include <TrackedDeviceProvider.hpp>
#include <TrackedController.hpp>
#include <InterfaceHook.hpp>
#include <LeapConnection.hpp>

#include <openvr_driver.h>

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    m_InterfaceHook.Init(pDriverContext);

    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Left_Hand", vr::TrackedDeviceClass_Controller, &m_Left);
    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Right_Hand", vr::TrackedDeviceClass_Controller, &m_Right);

    bool connected = m_LeapConnection.Init();

    if (connected)
        m_LeapConnection.Begin();

    return vr::VRInitError_None;
}

void TrackedDeviceProvider::Cleanup()
{
    m_LeapConnection.End();

    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

void TrackedDeviceProvider::RunFrame()
{
    LeapHand left { };
    LeapHand right { };

    if (m_LeapConnection.IsConnected())
    {
        left = m_LeapConnection.GetHand(vr::TrackedControllerRole_LeftHand);
        right = m_LeapConnection.GetHand(vr::TrackedControllerRole_RightHand);
    }

    m_Left.Update(left);
    m_Right.Update(right);
}