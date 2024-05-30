#include <memory>
#include <string.h>

#include <openvr_driver.h>
#include <core/CServerDriver.h>

#ifdef _WIN32
#define EXPORT_FUNC extern "C" __declspec(dllexport)
#else
#define EXPORT_FUNC extern "C"
#endif

CServerDriver g_serverDriver;

EXPORT_FUNC void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
    if (strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName) == 0) {
        return dynamic_cast<vr::IServerTrackedDeviceProvider*>(&g_serverDriver);
    }

    *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    return nullptr;
}