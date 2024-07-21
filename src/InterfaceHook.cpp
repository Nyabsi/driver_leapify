#include <InterfaceHook.hpp>
#include <DeviceController.hpp>

#include <stdint.h>
#include <string>

#include <openvr_driver.h>
#include <rcmp.hpp>

void InterfaceHook::Init(vr::IVRDriverContext* pDriverContext)
{
    void** vtable = *((void***)pDriverContext);

    rcmp::hook_indirect_function<void*(*)(void* self, const char* pchInterfaceVersion, void* peError)>(vtable + 0, [this](auto orig, void* self, const char* pchInterfaceVersion, void* peError) -> void* {
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
           rcmp::hook_indirect_function<bool(void* self, const char* pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, void* pDriver)>(vtable + 0, [this](auto orig, void* self, const char* pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, void* pDriver) -> bool
           {
               if (eDeviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller)
               {
                   DeviceController::get().AddController(pchDeviceSerialNumber);
               }

               return orig(self, pchDeviceSerialNumber, eDeviceClass, pDriver);
           });

           rcmp::hook_indirect_function<void(*)(void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize)>(vtable + 1, [](auto orig, void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) -> void 
           {
               auto pose = newPose;
               if (DeviceController::get().isValidController(unWhichDevice))
               {
                   DeviceController::get().UpdateControllerPose(unWhichDevice, newPose);
                   pose.deviceIsConnected = !DeviceController::get().isHandTrackingEnabled();
               }

               orig(self, unWhichDevice, pose, unPoseStructSize);
           });

           m_IVRServerDriverHostHooked_006 = true;
       }
   }

   if (interfaceName == "ITrackedDeviceServerDriver_005")
   {
       void** vtable = *((void***)interfacePtr);

       if (!m_ITrackedDeviceServerDriverHooked_005)
       {
           rcmp::hook_indirect_function<vr::EVRInitError(void* self, uint32_t unObjectId)>(vtable + 0, [](auto orig, void* self, uint32_t unObjectId) -> vr::EVRInitError {
               auto result = orig(self, unObjectId);

               auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
               auto serial = vr::VRProperties()->GetStringProperty(props, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);
               auto role = vr::VRProperties()->GetInt32Property(props, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);

               DeviceController::get().SetControllerIdAndRole(serial, unObjectId, static_cast<vr::ETrackedControllerRole>(role));

               return result;
           });

           m_ITrackedDeviceServerDriverHooked_005 = true;
       }
   }
}