#include <TrackedDeviceProvider.hpp>
#include <InterfaceHook.hpp>

#include <openvr_driver.h>

InterfaceHook g_InterfaceHook;

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    g_InterfaceHook.Init(pDriverContext);
    return vr::VRInitError_None;
}

void TrackedDeviceProvider::Cleanup()
{

}

void TrackedDeviceProvider::RunFrame()
{

}