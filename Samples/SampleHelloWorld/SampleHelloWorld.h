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


#ifndef SAMPLE_HELLO_WORLD_H
#define SAMPLE_HELLO_WORLD_H

#include "PhysXSample.h"
#include "SampleCCTCameraController.h"
#include "SampleCCTActor.h"
#include "SampleCCTJump.h"
#include "characterkinematic/PxController.h"
#include "characterkinematic/PxControllerBehavior.h"

	class SampleHelloWorld : public PhysXSample,
         public PxUserControllerHitReport, public PxControllerBehaviorCallback, public PxQueryFilterCallback
	{
		public:
												SampleHelloWorld(PhysXSampleApplication& app);
		virtual									~SampleHelloWorld();

		virtual	void							onTickPreRender(float dtime);
		virtual	void							onTickPostRender(float dtime);
		virtual	void							customizeSceneDesc(PxSceneDesc&);

		virtual	void							newMesh(const RAWMesh&);
		virtual	void							onInit();
        void onShutdown();
        virtual	void						    onInit(bool restart) { onInit(); }

		virtual void							collectInputEvents(std::vector<const SampleFramework::InputEvent*>& inputEvents);
		virtual void							helpRender(PxU32 x, PxU32 y, PxU8 textAlpha);
		virtual	void							descriptionRender(PxU32 x, PxU32 y, PxU8 textAlpha);
		virtual PxU32							getDebugObjectTypes() const;

        // Implements PxUserControllerHitReport
        virtual void							onShapeHit(const PxControllerShapeHit& hit);
        virtual void							onControllerHit(const PxControllersHit& hit) {}
        virtual void							onObstacleHit(const PxControllerObstacleHit& hit) {}

        ///////////////////////////////////////////////////////////////////////////////

        // Implements PxControllerBehaviorCallback
        virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxShape& shape, const PxActor& actor);
        virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxController& controller);
        virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxObstacle& obstacle);

        ///////////////////////////////////////////////////////////////////////////////

        // Implements PxQueryFilterCallback
        virtual	PxQueryHitType::Enum		preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags);
        virtual	PxQueryHitType::Enum		postFilter(const PxFilterData& filterData, const PxQueryHit& hit);

        ///////////////////////////////////////////////////////////////////////////////


		//Camera management
		SampleCCTCameraController*		mCCTCamera;
		ControlledActor*				mActor;
		PxControllerManager*			mControllerManager;
	//	PxExtendedVec3					mControllerInitialPosition;
	//	PxCapsuleController*			mController;
		PxExtendedVec3							mControllerInitialPosition;
		bool                            mCCTActive;
		PxVec3                          mCCTDisplacement;
		PxVec3                          mCCTDisplacementPrevStep;
		PxReal                          mCCTTimeStep;
	//	SampleCharacterClothJump		mJump;


  
		// Funzioni inserite per test import/rendering
		RAWMesh * createRAWMeshFromObjMesh(const char * inObjFileName, const PxTransform & inPos, PxU32 inMaterialID, RAWMesh & outRawMesh);
		PxTriangleMesh * generateTriMesh(PxU32 vertCount, PxU32 triCount, const PxVec3 * vers, const PxU32 * indices);
		PxRigidActor * createRigidActor		(PxScene & scene, PxPhysics & physics, PxShape* & shape, const PxTransform & pose, const PxGeometry & geometry, PxMaterial & material);
		PxRigidDynamic * createDynamicActor	(PxScene & scene, PxPhysics & physics, PxShape *& shape, const PxTransform & pose, const PxGeometry & geometry, PxMaterial & material);
		PxRigidStatic * buildTest();

		// Dati di materiali e mesh salvati globalmente
		RenderMaterial*                 tryMaterial, *tryMaterial1, *tryMaterial2;

		RAWMesh data, data1, data2;


		std::vector<PxShape*> dataShape;
		std::vector<PxTransform> dataPos; 
		std::vector<RenderBaseActor*> dataRender;

	};

#endif

