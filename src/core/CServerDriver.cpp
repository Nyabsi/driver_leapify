#include "Core/CServerDriver.h"

#include <Leap/CLeapPoller.h>
#include <Leap/CLeapFrame.h>

#include <Controller/CLeapIndexController.h>
#include <Utils/Utils.h>

#include <Controller/CJoyconInput.h>

extern char g_modulePath[];

const char* const CServerDriver::ms_interfaces[]
{
    vr::ITrackedDeviceServerDriver_Version,
        vr::IServerTrackedDeviceProvider_Version,
        nullptr
};

CServerDriver::CServerDriver()
{
    m_leapPoller = nullptr;
    m_leapFrame = nullptr;
    m_connectionState = false;
    m_leftController = nullptr;
    m_rightController = nullptr;
    m_joyconInput = nullptr;
}

CServerDriver::~CServerDriver()
{
}

// vr::IServerTrackedDeviceProvider
vr::EVRInitError CServerDriver::Init(vr::IVRDriverContext *pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    m_leftController = new CLeapIndexController(true);
    m_rightController = new CLeapIndexController(false);

    m_joyconInput = new CJoyconInput();

    vr::VRServerDriverHost()->TrackedDeviceAdded(m_leftController->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, m_leftController);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_rightController->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, m_rightController);

    m_leapFrame = new CLeapFrame();
    m_leapPoller = new CLeapPoller();
    m_leapPoller->Start();
    m_leapPoller->SetTrackingMode(_eLeapTrackingMode::eLeapTrackingMode_HMD);
    m_leapPoller->SetPolicy(eLeapPolicyFlag::eLeapPolicyFlag_OptimizeHMD, eLeapPolicyFlag::eLeapPolicyFlag_OptimizeScreenTop);

    return vr::VRInitError_None;
}

void CServerDriver::Cleanup()
{
    delete m_leftController;
    m_leftController = nullptr;

    delete m_rightController;
    m_rightController = nullptr;

    m_leapPoller->Stop();
    delete m_leapPoller;
    m_leapPoller = nullptr;

    delete m_leapFrame;
    m_leapFrame = nullptr;

    m_connectionState = false;

    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* CServerDriver::GetInterfaceVersions()
{
    return ms_interfaces;
}

void CServerDriver::RunFrame()
{
    CLeapIndexController::UpdateHMDCoordinates();

    if(m_connectionState != m_leapPoller->IsConnected())
    {
        m_connectionState = m_leapPoller->IsConnected();
        m_leftController->SetEnabled(m_connectionState);
        m_rightController->SetEnabled(m_connectionState);

        m_leapPoller->SetTrackingMode(_eLeapTrackingMode::eLeapTrackingMode_HMD);
        m_leapPoller->SetPolicy(eLeapPolicyFlag::eLeapPolicyFlag_OptimizeHMD, eLeapPolicyFlag::eLeapPolicyFlag_OptimizeScreenTop);
    }

    if(m_connectionState && m_leapPoller->GetFrame(m_leapFrame->GetEvent()))
        m_leapFrame->Update();

    if (m_joyconInput->IsConnected())
    {
        m_joyconInput->Update(m_leftController, m_rightController);
    }
    else
    {
        m_joyconInput->Reconnect();
    }

    // Update devices
    m_leftController->RunFrame(m_leapFrame->GetLeftHand());
    m_rightController->RunFrame(m_leapFrame->GetRightHand());
}

bool CServerDriver::ShouldBlockStandbyMode()
{
    return false;
}

void CServerDriver::EnterStandby()
{
}

void CServerDriver::LeaveStandby()
{
}
