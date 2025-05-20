#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <openvr_driver.h>

namespace hooks {
    namespace IVRDriverInput {
        bool UpdateBooleanComponent(vr::VRInputComponentHandle_t& ulComponent, bool& bNewValue, double& fTimeOffset) {

            return false;
        }

        void CreateSkeletonComponent(vr::PropertyContainerHandle_t& ulContainer, char*& pchName, char*& pchSkeletonPath, char*& pchBasePosePath, vr::EVRSkeletalTrackingLevel& eSkeletalTrackingLevel, vr::VRBoneTransform_t*& pGripLimitTransforms, uint32_t& unGripLimitTransformCount, vr::VRInputComponentHandle_t*& pHandle) {

        }

		bool UpdateSkeletonComponent(vr::VRInputComponentHandle_t& ulComponent, vr::EVRSkeletalMotionRange& eMotionRange, vr::VRBoneTransform_t*& pTransforms, uint32_t& unTransformCount) {

            return false;
        }
    };
}