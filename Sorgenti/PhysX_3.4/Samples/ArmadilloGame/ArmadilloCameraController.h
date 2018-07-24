// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2017 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef _ARMADILLO_CAMERA_CONTROLLER_H
#define _ARMADILLO_CAMERA_CONTROLLER_H

#include "PhysXSampleApplication.h"
#include "SampleCameraController.h"
#include "PhysXSample.h"
#include "SampleConsole.h"
#include <SampleFrameworkInputEventIds.h>
#include <SamplePlatform.h>
#include <SampleUserInput.h>
#include <SampleUserInputIds.h>
#include <SampleUserInputDefines.h>

class ArmadilloCameraController : public DefaultCameraController
{
public:
    ArmadilloCameraController() : mFollowingMode(true) {}
    ~ArmadilloCameraController() {}

    ///////////////////////////////////////////////////////////////////////////////
    // Gestione update e posizione camera

    virtual void update(Camera& camera, PxReal dtime)
    {
        if (!mFollowingMode)
            DefaultCameraController::update(camera, dtime);
    }

    // Aggiornamento della posizione della camera in modo che segua il player e in base all'imput
    void updateFollowingMode(Camera& camera, PxReal dtime, const PxVec3& targetPos)
    {
        PX_ASSERT(mFollowingMode);
        DefaultCameraController::update(camera, dtime);
        mTargetYaw += mGamepadYawInc * dtime;
        mTargetPitch += mGamepadPitchInc * dtime;

        if (mTargetPitch < mPitchMin)	setMinPitch();
        if (mTargetPitch > mPitchMax)	setMaxPitch();

        camera.setRot(PxVec3(-mTargetPitch, -mTargetYaw, 0));
        PxVec3 target = targetPos - camera.getViewDir()*mCameraDistance;
        camera.setPos(target);
    }

    // La camera viene impostata in modo che segua il player
    void setFollowingMode(bool follow) { mFollowingMode = follow; }

    ///////////////////////////////////////////////////////////////////////////////
    // Getter/Setter flag della camera

    PxVec3  getForce();
    void    resetForce();

    bool    getStartGameFlag();
    void    resetStartGameFlag();

    PxReal  getTargetYaw() { return mTargetYaw; };
    PxReal  getTargetPitch() { return mTargetPitch; };

    void    setMinPitch() { mTargetPitch = mPitchMin; }
    void    setMaxPitch() { mTargetPitch = mPitchMax; }

    ///////////////////////////////////////////////////////////////////////////////
    // Funzioni di gestione dell'input

    virtual	void onDigitalInputEvent    (const SampleFramework::InputEvent&, bool val);
    virtual	void onAnalogInputEvent     (const SampleFramework::InputEvent&, float val);
    virtual	void onPointerInputEvent    (const SampleFramework::InputEvent&, physx::PxU32, physx::PxU32, physx::PxReal, physx::PxReal, bool val);
    virtual	void collectInputEvents     (std::vector<const SampleFramework::InputEvent*>& inputEvents);

    void         handleInput(Camera &camera, bool isNotJumpingl);

    ///////////////////////////////////////////////////////////////////////////////

private:
    // Costanti
    const PxReal    mPitchMin = -5 * PxPi / 12;
    const PxReal    mPitchMax = 0.f;
    
    // Variabili private
    bool    mFollowingMode;
    bool	mFwd = false, mBwd = false, mLeft = false, mRight = false, mJump = false, 
            mHeightUp = false, mHeightDown = false, mGameStart = false;

    PxReal	mTargetYaw = -PxPi;
    PxReal	mTargetPitch = 0.0f;

    PxReal	mGamepadPitchInc, mGamepadYawInc;
    PxReal	mGamepadForwardInc, mGamepadLateralInc;

    PxReal	mSensibility = 0.001f;
    PxReal	mGamepadWalkingSpeed = 15.0f;
    PxReal	mCameraDistance = 15.0f;
};

#endif