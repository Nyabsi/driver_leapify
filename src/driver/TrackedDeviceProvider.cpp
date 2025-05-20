#include <driver/TrackedDeviceProvider.hpp>
#include <hook/HookManager.hpp>

#include <openvr_driver.h>

TrackedDeviceProvider::TrackedDeviceProvider()
{

}

TrackedDeviceProvider::~TrackedDeviceProvider()
{

}

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    m_HookManager.Initialize(pDriverContext);

    return vr::VRInitError_None; // TODO: do we ever have exceptions here?
}

void TrackedDeviceProvider::Cleanup()
{
    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

void TrackedDeviceProvider::RunFrame()
{

}