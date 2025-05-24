#include <driver/TrackedDeviceProvider.hpp>
#include <hook/HookManager.hpp>
#include <core/StateManager.hpp>

#include <openvr_driver.h>

#ifdef _WIN32
#include <string>
#include <Windows.h>
#endif

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
    auto [leftId, leftDevice] = StateManager::Get().GetDeviceByHint(vr::TrackedControllerRole_LeftHand);
    auto [rightId, rightDevice] = StateManager::Get().GetDeviceByHint(vr::TrackedControllerRole_RightHand);

    if (leftId != -1 && rightId != -1) {
        double combinedMagnitude = leftDevice.accelMagnitude * rightDevice.accelMagnitude;
        #ifdef _WIN32
        MessageBoxA(NULL, std::to_string(combinedMagnitude).c_str(), "Validation (SPAM)", MB_OK);
        #endif
        if (combinedMagnitude >= 1) {
            #ifdef _WIN32
            MessageBoxA(NULL, "Hit two controllers together.", "Validation", MB_OK);
            #endif
        }
    }
}