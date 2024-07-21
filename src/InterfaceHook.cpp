#include <InterfaceHook.hpp>
#include <DeviceController.hpp>

#include <stdint.h>
#include <string>

#include <openvr_driver.h>
#include <rcmp.hpp>

#include <Windows.h>

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
                   std::string serial(pchDeviceSerialNumber);

                   if (serial != "Leap_Hand_Left" && serial != "Leap_Hand_Right")
                   {
                       DeviceController::get().AddController(pchDeviceSerialNumber);
                   }
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
}