#pragma once

#include <hook/HookManager.hpp>

#include <vector>

#include <openvr_driver.h>

class TrackedDeviceProvider : public vr::IServerTrackedDeviceProvider 
{
private:
    HookManager m_HookManager;
public:
    TrackedDeviceProvider();
    ~TrackedDeviceProvider();

    vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext);

    void Cleanup();

    const char* const* GetInterfaceVersions() { return vr::k_InterfaceVersions; }

    void RunFrame();

    bool ShouldBlockStandbyMode() { return false; }

    void EnterStandby() { }
    
    void LeaveStandby() { }
};