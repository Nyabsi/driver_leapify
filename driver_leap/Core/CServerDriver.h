#pragma once

#include <openvr_driver.h>
#include <LeapC.h>

class CLeapPoller;
class CLeapFrame;
class CLeapIndexController;
class CJoyconInput;

class CServerDriver final : public vr::IServerTrackedDeviceProvider
{
    static const char* const ms_interfaces[];

    bool m_connectionState;
    CLeapPoller *m_leapPoller;
    CLeapFrame *m_leapFrame;
    CLeapIndexController *m_leftController;
    CLeapIndexController *m_rightController;
    CJoyconInput *m_joyconInput;

    CServerDriver(const CServerDriver &that) = delete;
    CServerDriver& operator=(const CServerDriver &that) = delete;

    // vr::IServerTrackedDeviceProvider
    vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext);
    void Cleanup();
    const char* const* GetInterfaceVersions();
    void RunFrame();
    bool ShouldBlockStandbyMode();
    void EnterStandby();
    void LeaveStandby();
public:
    CServerDriver();
    ~CServerDriver();
};
