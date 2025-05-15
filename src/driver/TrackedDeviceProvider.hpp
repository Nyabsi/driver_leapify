#pragma once

#include <driver/TrackedController.hpp>
#include <driver/TrackedObject.hpp>
#include <hook/HookManager.hpp>
#include <core/LeapConnection.hpp>

#include <vector>

#include <openvr_driver.h>

class TrackedDeviceProvider : public vr::IServerTrackedDeviceProvider 
{
public:
    ~TrackedDeviceProvider() { }

    vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext);

    void Cleanup();

    const char* const* GetInterfaceVersions() { return vr::k_InterfaceVersions; }

    void RunFrame();

    bool ShouldBlockStandbyMode() { return false; }

    void EnterStandby() { }
    
    void LeaveStandby() { }
private:
    TrackedController m_Left { vr::ETrackedControllerRole::TrackedControllerRole_LeftHand };
    TrackedController m_Right { vr::ETrackedControllerRole::TrackedControllerRole_RightHand };
    HookManager m_HookManager { };
    LeapConnection m_LeapConnection { };
    std::vector<TrackedObject> m_trackers { };
};