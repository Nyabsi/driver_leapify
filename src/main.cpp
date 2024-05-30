#include <Windows.h>

#include "Core/CServerDriver.h"

CServerDriver g_serverDriver;

extern "C" __declspec(dllexport) void* HmdDriverFactory(const char *pInterfaceName, int *pReturnCode)
{
    void *l_result = nullptr;
    if(!strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName)) l_result = dynamic_cast<vr::IServerTrackedDeviceProvider*>(&g_serverDriver);
    else
    {
        if(pReturnCode) *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    }
    return l_result;
}
