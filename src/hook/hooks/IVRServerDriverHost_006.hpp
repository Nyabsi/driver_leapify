#pragma once

#include <core/StateManager.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <openvr_driver.h>

namespace hooks {
    namespace IVRServerDriverHost {
        bool TrackedDeviceAdded(char*& pchDeviceSerialNumber, vr::ETrackedDeviceClass& eDeviceClass, vr::ITrackedDeviceServerDriver*& pDriver) {
            return false;
        }

        bool TrackedDevicePoseUpdated(uint32_t& unWhichDevice, vr::DriverPose_t& newPose, uint32_t& unPoseStructSize) {

            if (!StateManager::Get().IsDeviceRegistered(unWhichDevice))
            {
                auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(unWhichDevice);

                DeviceState device {};
                device.manufacturerName = vr::VRProperties()->GetStringProperty(props, vr::ETrackedDeviceProperty::Prop_ManufacturerName_String);
                device.trackingSystem = vr::VRProperties()->GetStringProperty(props, vr::ETrackedDeviceProperty::Prop_TrackingSystemName_String);
                device.deviceClass = vr::VRProperties()->GetInt32Property(props, vr::ETrackedDeviceProperty::Prop_DeviceClass_Int32);
                device.roleHint = vr::VRProperties()->GetInt32Property(props, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);

                StateManager::Get().RegisterDevice(unWhichDevice, device);

                #ifdef _WIN32
                MessageBoxA(NULL, "The sea was sailing and we did init register on the controller", "Validation", MB_OK);
                #endif
            }

            if (StateManager::Get().ShouldWeBlockDevicePose(unWhichDevice))
            {
                newPose.deviceIsConnected = false;
            }

            return false;
        }
    };
}