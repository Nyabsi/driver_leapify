#include <TrackedDeviceProvider.hpp>
#include <TrackedController.hpp> 
#include <InterfaceHook.hpp>

#include <openvr_driver.h>

InterfaceHook g_InterfaceHook;

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    g_InterfaceHook.Init(pDriverContext);

    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Hand_Left", vr::TrackedDeviceClass_Controller, &m_Left);
    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Hand_Right", vr::TrackedDeviceClass_Controller, &m_Right);

    return vr::VRInitError_None;
}

void TrackedDeviceProvider::Cleanup()
{

}

void TrackedDeviceProvider::RunFrame()
{
    m_Left.Update();
    m_Right.Update();
}