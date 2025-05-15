#pragma once

#include <openvr_driver.h>

namespace hooks {
    namespace IVRServerDriverHost {
        bool TrackedDeviceAdded(const char *pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, vr::ITrackedDeviceServerDriver *pDriver) {
            // this keeps crashing, bruv mf.
        }

        void TrackedDevicePoseUpdated(uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) {
            // TODO: stuff
        }
    };
}