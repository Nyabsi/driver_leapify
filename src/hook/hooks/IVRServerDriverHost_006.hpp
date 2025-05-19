#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <openvr_driver.h>

namespace hooks {
    namespace IVRServerDriverHost {
        bool TrackedDeviceAdded(const char*& pchDeviceSerialNumber, vr::ETrackedDeviceClass& eDeviceClass, vr::ITrackedDeviceServerDriver*& pDriver) {
            return false;
        }

        bool TrackedDevicePoseUpdated(uint32_t& unWhichDevice, const vr::DriverPose_t& newPose, uint32_t& unPoseStructSize) {
            return false;
        }
    };
}