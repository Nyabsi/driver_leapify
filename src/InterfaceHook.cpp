#include <InterfaceHook.hpp>
#include <DeviceController.hpp>

#include <stdint.h>
#include <string>

#include <openvr_driver.h>
#include <rcmp.hpp>

#include <glm/gtx/quaternion.hpp>

// vtable starting index varies between compiler ABIs
#ifdef _WIN32
    constexpr int vtable_offset = 0;
#else
    constexpr int vtable_offset = 1;
#endif

void InterfaceHook::Init(vr::IVRDriverContext* pDriverContext)
{
    void** vtable = *((void***)pDriverContext);

    rcmp::hook_indirect_function<void*(*)(void* self, const char* pchInterfaceVersion, void* peError)>(vtable + 0 + vtable_offset, [this](auto orig, void* self, const char* pchInterfaceVersion, void* peError) -> void* {
        void* interfacePtr = orig(self, pchInterfaceVersion, peError);
        GetGenericInterface(interfacePtr, pchInterfaceVersion);
        return interfacePtr;
    });
}

void InterfaceHook::GetGenericInterface(void* interfacePtr, const char* pchInterfaceVersion)
{
   std::string interfaceName(pchInterfaceVersion);

   if (interfaceName == "IVRServerDriverHost_006")
   {
       void** vtable = *((void***)interfacePtr);

       if (!m_IVRServerDriverHostHooked_006)
       {
           rcmp::hook_indirect_function<void(*)(void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize)>(vtable + 1 + vtable_offset, [](auto orig, void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) -> void
           {
                auto pose = newPose;

                auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(unWhichDevice);

                auto device = vr::VRProperties()->GetInt32Property(props, vr::ETrackedDeviceProperty::Prop_DeviceClass_Int32);
                auto serial = vr::VRProperties()->GetStringProperty(props, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);
                auto type = vr::VRProperties()->GetStringProperty(props, vr::ETrackedDeviceProperty::Prop_ControllerType_String);
                auto role = static_cast<vr::ETrackedControllerRole>(vr::VRProperties()->GetInt32Property(props, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32));

                if (device == vr::TrackedDeviceClass_Controller && serial.find("Leap_Hand") == -1)
                {
                    if (pose.deviceIsConnected && pose.poseIsValid && pose.vecPosition[0] != 0 && pose.vecPosition[1] != 0 && pose.vecPosition[2] != 0)
                    {
                        if (DeviceController::get().GetController(role).objectId == 999)
                        {
                            DeviceController::get().UpdateController(role, unWhichDevice, serial, type == "knuckles"); // TODO: parse profile configuration
                        }
                        else
                        {
                            if (DeviceController::get().GetController(role).objectId == unWhichDevice)
                            {
                                DeviceController::get().UpdateControllerPose(role, pose);
                                pose.deviceIsConnected = true;
                            }
                        }
                    }
                    else
                    {
                        if (DeviceController::get().GetController(role).objectId == unWhichDevice)
                        {
                            DeviceController::get().UpdateController(role, 999, "", "");
                        }
                    }
                }

                orig(self, unWhichDevice, pose, unPoseStructSize);
           });

           m_IVRServerDriverHostHooked_006 = true;
       }
   }

   if (interfaceName == "IVRDriverInput_003")
   {
       void** vtable = *((void***)interfacePtr);

       if (!m_IVRDriverInputHooked_003)
       {
           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle)>(vtable + 0 + vtable_offset, [this](auto orig, void* self,vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) -> vr::EVRInputError
           {
                vr::EVRInputError result;
                result = orig(self, ulContainer, pchName, pHandle);
                CreateBooleanComponent(ulContainer, pchName, pHandle);
                return result;
           });

           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset)>(vtable + 1 + vtable_offset, [](auto orig, void* self, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset) -> vr::EVRInputError
           {
                auto left = DeviceController::get().GetController(vr::TrackedControllerRole_LeftHand);
                auto right = DeviceController::get().GetController(vr::TrackedControllerRole_RightHand);

                for (auto& component : DeviceController::get().GetComponents())
                {
                    if (left.serial == component.first || right.serial == component.first)
                    {
                        for (auto& inputs : component.second)
                        {
                            if (inputs.second.orig == ulComponent)
                            {
                                return orig(self, DeviceController::get().getComponent(inputs.first).override, bNewValue, 0);
                            }
                        }
                    }
                }

                return orig(self, ulComponent, bNewValue, fTimeOffset);
           });

           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits)>(vtable + 2 + vtable_offset, [this](auto orig, void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits) -> vr::EVRInputError
           {
                vr::EVRInputError result;
                result = orig(self, ulContainer, pchName, pHandle, eType, eUnits);
                CreateScalarComponent(ulContainer, pchName, pHandle);
                return result;
           });

           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset)>(vtable + 3 + vtable_offset, [](auto orig, void* self, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset) -> vr::EVRInputError
           {
                auto left = DeviceController::get().GetController(vr::TrackedControllerRole_LeftHand);
                auto right = DeviceController::get().GetController(vr::TrackedControllerRole_RightHand);

                for (auto& component : DeviceController::get().GetComponents())
                {
                    if (left.serial == component.first || right.serial == component.first)
                    {
                        for (auto& inputs : component.second)
                        {
                            if (inputs.second.orig == ulComponent)
                            {
                                if (left.hasCurl == false)
                                {
                                    if (inputs.first == 8)
                                    {
                                        orig(self, DeviceController::get().getComponent(40).override, fNewValue, 0);
                                    }

                                    if (inputs.first == 22)
                                    {
                                        orig(self, DeviceController::get().getComponent(42).override, fNewValue, 0);
                                        orig(self, DeviceController::get().getComponent(44).override, fNewValue, 0);
                                        orig(self, DeviceController::get().getComponent(46).override, fNewValue, 0);
                                    }
                                }

                                if (right.hasCurl == false)
                                {
                                    if (inputs.first == 9)
                                    {
                                        orig(self, DeviceController::get().getComponent(41).override, fNewValue, 0);
                                    }

                                    if (inputs.first == 23)
                                    {
                                        orig(self, DeviceController::get().getComponent(43).override, fNewValue, 0);
                                        orig(self, DeviceController::get().getComponent(45).override, fNewValue, 0);
                                        orig(self, DeviceController::get().getComponent(47).override, fNewValue, 0);
                                    }
                                }

                                return orig(self, DeviceController::get().getComponent(inputs.first).override, fNewValue, 0);
                            }
                        }
                    }
                }
                return orig(self, ulComponent, fNewValue, fTimeOffset);
           });

           m_IVRDriverInputHooked_003 = true;
       }
   }
}

void InterfaceHook::CreateBooleanComponent(vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle)
{
    auto serial = vr::VRProperties()->GetStringProperty(ulContainer, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);
    auto role = vr::VRProperties()->GetInt32Property(ulContainer, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);

    std::string input(pchName);

    if (input == "/input/system/click")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 0).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 1).orig = *pHandle;
    }

    if (input == "/input/system/touch")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 2).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 3).orig = *pHandle;
    }

    if (input == "/input/trigger/click")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 4).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 5).orig = *pHandle;
    }

    if (input == "/input/trigger/touch")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 6).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 7).orig = *pHandle;
    }

    if (input == "/input/a/click")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 32).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 33).orig = *pHandle;
    }

    if (input == "/input/b/click")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 34).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 35).orig = *pHandle;
    }

    if (input == "/input/a/touch")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 34).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 35).orig = *pHandle;
    }

    if (input == "/input/b/touch")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 38).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 39).orig = *pHandle;
    }

    if (input == "/input/thumbstick/click")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 24).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 25).orig = *pHandle;
    }

    if (input == "/input/thumbstick/touch")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 26).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 27).orig = *pHandle;
    }

    if (input == "/input/x/click")
    {
        DeviceController::get().getComponent(serial, 32).orig = *pHandle;
    }

    if (input == "/input/a/click")
    {
        DeviceController::get().getComponent(serial, 33).orig = *pHandle;
    }

    if (input == "/input/y/click")
    {
        DeviceController::get().getComponent(serial, 36).orig = *pHandle;
    }

    if (input == "/input/b/click")
    {
        DeviceController::get().getComponent(serial, 37).orig = *pHandle;
    }

    if (input == "/input/x/touch")
    {
        DeviceController::get().getComponent(serial, 34).orig = *pHandle;
    }

    if (input == "/input/a/touch")
    {
        DeviceController::get().getComponent(serial, 35).orig = *pHandle;
    }

    if (input == "/input/y/touch")
    {
        DeviceController::get().getComponent(serial, 38).orig = *pHandle;
    }

    if (input == "/input/b/touch")
    {
        DeviceController::get().getComponent(serial, 39).orig = *pHandle;
    }

    if (input == "/input/joystick/click")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 24).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 25).orig = *pHandle;
    }

    if (input == "/input/joystick/touch")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 26).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 27).orig = *pHandle;
    }


    if (input == "/input/grip/touch")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 18).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 19).orig = *pHandle;
    }

    if (input == "/input/trackpad/touch")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 14).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 15).orig = *pHandle;
    }

    if (input == "/input/trackpad/force")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 16).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 17).orig = *pHandle;
    }
}

void InterfaceHook::CreateScalarComponent(vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle)
{
    auto serial = vr::VRProperties()->GetStringProperty(ulContainer, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);
    auto role = vr::VRProperties()->GetInt32Property(ulContainer, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);

    std::string input(pchName);

    if (input == "/input/joystick/x")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 28).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 29).orig = *pHandle;
    }

    if (input == "/input/joystick/y")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 30).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 31).orig = *pHandle;
    }

    if (input == "/input/thumbstick/x")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 28).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 29).orig = *pHandle;
    }

    if (input == "/input/thumbstick/y")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 30).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 31).orig = *pHandle;
    }

    if (input == "/input/trigger/value")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 8).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 9).orig = *pHandle;
    }

    if (input == "/input/grip/force")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 20).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 21).orig = *pHandle;
    }

    if (input == "/input/grip/value")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 22).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 23).orig = *pHandle;
    }

    if (input == "/input/finger/index")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 40).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 41).orig = *pHandle;
    }

    if (input == "/input/finger/middle")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 42).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 43).orig = *pHandle;
    }

    if (input == "/input/finger/ring")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 44).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 45).orig = *pHandle;
    }

    if (input == "/input/finger/pinky")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 46).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 47).orig = *pHandle;
    }

    if (input == "/input/trackpad/x")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 10).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 11).orig = *pHandle;
    }

    if (input == "/input/trackpad/y")
    {
        if (role == vr::TrackedControllerRole_LeftHand)
            DeviceController::get().getComponent(serial, 12).orig = *pHandle;
        if (role == vr::TrackedControllerRole_RightHand)
            DeviceController::get().getComponent(serial, 13).orig = *pHandle;
    }
}