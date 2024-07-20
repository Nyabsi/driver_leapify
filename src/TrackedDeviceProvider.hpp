#pragma once

#include <openvr_driver.h>

class TrackedDeviceProvider : public vr::IServerTrackedDeviceProvider 
{
public:
    vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext);

    void Cleanup();

    const char* const* GetInterfaceVersions() { return vr::k_InterfaceVersions; }

    void RunFrame();

    bool ShouldBlockStandbyMode() { return false; }

    void EnterStandby() { }
    
    void LeaveStandby() { }
};