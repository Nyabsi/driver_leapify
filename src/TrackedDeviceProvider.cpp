#include <TrackedDeviceProvider.hpp>
#include <openvr_driver.h>

// TODO: will fail on Linux, for now...
#include <Windows.h>

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    MessageBoxA(NULL, "Loaded the driver", "NULL", MB_OK);
    return vr::VRInitError_None;
}

void TrackedDeviceProvider::Cleanup()
{

}

void TrackedDeviceProvider::RunFrame()
{
    
}