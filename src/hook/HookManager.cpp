#include <hook/HookManager.hpp>
#include <util/HashString.hpp>

#include <rcmp.hpp>
#include <openvr_driver.h>

#include <hook/hooks/IVRDriverInput_003.hpp>
#include <hook/hooks/IVRServerDriverHost_006.hpp>

#ifdef _WIN32
    constexpr int vtable_offset = 0;
#else
    constexpr int vtable_offset = 1;
#endif

HookManager::HookManager()
{
    m_IVRDriverInputHooked = false;
    m_IVRServerDriverHostHooked = false;
}

HookManager::~HookManager()
{

}

void HookManager::Initialize(vr::IVRDriverContext* pContext)
{
    void** pVtable = *((void***)pContext);

    auto GetGenericInterface = [this](auto orig, void* self, const char* pchInterfaceVersion, void* peError) -> void* {
        void* pTablePtr = orig(self, pchInterfaceVersion, peError);
        hookGenericInterface(pTablePtr);
        return pTablePtr;
    };

    rcmp::hook_indirect_function<void*(*)(void* self, const char* pchInterfaceVersion, void* peError)>(pVtable + 0 + vtable_offset, GetGenericInterface);
}

void HookManager::hookGenericInterface(void* pTablePtr)
{
    uint64_t hash = util::hashString(std::string(pchInterfaceVersion));
    switch (hash) {
        case "IVRDriverInput_003"_hash:
            hookIVRDriverInput(pTablePtr);
            break;
        case "IVRServerDriverHost_006"_hash:
            hookIVRServerDriverHost(pTablePtr);
            break;
        default:
            break; // ¯\_(ツ)_/¯
    }
}

void HookManager::hookIVRDriverInput(void* pTablePtr)
{
    void** pVtable = *((void***)pTablePtr);

    if (!m_IVRDriverInputHooked)
    {
        auto _UpdateBooleanComponent = [&](auto orig, void* self, vr::VRInputComponentHandle_t& ulComponent, bool& bNewValue, double& fTimeOffset) -> vr::EVRInputError
        {
            bool block = hooks::IVRDriverInput::UpdateBooleanComponent(ulComponent, bNewValue, fTimeOffset);
            if (!block)
                return orig(self, ulComponent, bNewValue, fTimeOffset);
            else
                return vr::EVRInputError::VRInputError_None;
        }

        auto _CreateSkeletonComponent = [&](auto orig, void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle) -> vr::EVRInputError
        {
            // NOTE: orig needs to be called before running hook, otherwise the pointer of "pHandle" will be invalid, we don't want to discard the result of this function anyways.
            auto result = orig(self, ulContainer, pchName, pchSkeletonPath, pchBasePosePath, eSkeletalTrackingLevel, pGripLimitTransforms, unGripLimitTransformCount, pHandle);
            hooks::IVRDriverInput::CreateSkeletonComponent(ulContainer, pchName, pchSkeletonPath, pchBasePosePath, eSkeletalTrackingLevel, pGripLimitTransforms, unGripLimitTransformCount, pHandle);
            return result;
        };

        auto _UpdateSkeletonComponent = [&](auto orig, void* self, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount) -> vr::EVRInputError
        {
            bool block = hooks::IVRDriverInput::UpdateSkeletonComponent(ulComponent, eMotionRange, pTransforms, unTransformCount);
            if (!block)
                return orig(self, ulComponent, eMotionRange, pTransforms, unTransformCount);
            else
                return vr::EVRInputError::VRInputError_None;
        };

        rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset)>(vtable + 1 + vtable_offset, _CreateSkeletonComponent);
        rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle)>(vtable + 5 + vtable_offset, _CreateSkeletonComponent);   
        rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount)>(vtable + 6 + vtable_offset, _UpdateSkeletonComponent);
        
        m_IVRDriverInputHooked = true;
    }
}

void HookManager::hookIVRServerDriverHost(void* pTablePtr)
{
    void** pVtable = *((void***)pTablePtr);

    if (!m_IVRServerDriverHostHooked)
    {
        auto _TrackedDevicePoseUpdated = [&](auto orig, void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) -> void {
            bool block = hooks::IVRServerDriverHost::TrackedDevicePoseUpdated(unWhichDevice, newPose, unPoseStructSize);
            if (!block)
                orig(self, unWhichDevice, pose, unPoseStructSize);
        };

        rcmp::hook_indirect_function<void(*)(void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize)>(pVtable + 1 + vtable_offset, _TrackedDevicePoseUpdated);

        m_IVRServerDriverHostHooked = true;
    }
}