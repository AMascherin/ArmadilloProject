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


#ifndef SAMPLE_ARMADILLO_GAME_H
#define SAMPLE_ARMADILLO_GAME_H

#define SWITCH_LEVER_NAME   "switch_key_model.obj"
#define BRIDGE_NAME         "bridge_model.obj"

#include "PhysXSample.h"
#include "ArmadilloCameraController.h"
#include "characterkinematic/PxController.h"
#include "characterkinematic/PxControllerBehavior.h"
#include "AudioManager.h"
#include "SampleUtils.h"
#include "SampleConsole.h"
#include "RendererMemoryMacros.h"
#include "RenderMeshActor.h"

#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "RenderBoxActor.h"

#include <SampleBaseInputEventIds.h>
#include <SamplePlatform.h>
#include <SampleUserInput.h>
#include <SampleUserInputIds.h>
#include <SampleUserInputDefines.h>

#include "PxTkStream.h"
#include "wavefront.h"

	class ArmadilloGame : public PhysXSample
	{
		public:
												ArmadilloGame(PhysXSampleApplication& app);
		virtual									~ArmadilloGame();

		virtual void							onSubstep(float dtime);
		virtual	void							onTickPreRender(float dtime);
		virtual	void							onTickPostRender(float dtime);		
		virtual	void							customizeSceneDesc(PxSceneDesc&);
		virtual	void							onInit();
        void                                    onShutdown();
        virtual	void						    onInit(bool restart) { onInit(); }

		virtual void							collectInputEvents(std::vector<const SampleFramework::InputEvent*>& inputEvents);
		virtual void							helpRender(PxU32 x, PxU32 y, PxU8 textAlpha);
		virtual	void							descriptionRender(PxU32 x, PxU32 y, PxU8 textAlpha);

        ///////////////////////////////////////////////////////////////////////////////
        
        //Controllori del gioco e attori
		ArmadilloCameraController*      mArmadilloCamera;
		PxRigidDynamic*					mControllerActor;
        PxRigidActor*					mFloatingActor;
        PxRigidDynamic*					mBridgeActor;
		PxControllerManager*			mControllerManager;
		PxExtendedVec3					mControllerInitialPosition;
        AudioManager*                   audioManager;

        ///////////////////////////////////////////////////////////////////////////////
		
        // Funzioni inserite per test import/rendering
		RAWMesh*        createRAWMeshFromObjMesh    (const char * inObjFileName, const PxTransform & inPos, PxU32 inMaterialID, RAWMesh & outRawMesh);
		PxTriangleMesh* generateTriMesh             (PxU32 vertCount, PxU32 triCount, const PxVec3 * vers, const PxU32 * indices);
		PxRigidActor*   createRigidActor            (PxScene & scene, PxPhysics & physics, PxShape* & shape, const PxTransform & pose, const PxGeometry & geometry, PxMaterial & material);
        PxRigidDynamic* createDynamicActor          (PxScene & scene, PxPhysics & physics, PxShape *& shape, const PxTransform & pose, const PxGeometry & geometry, PxMaterial & material, const char * shapeName);
		PxRigidDynamic* createDynamicActor	        (PxScene & scene, PxPhysics & physics, PxShape *& shape, const PxTransform & pose, const PxGeometry & geometry, PxMaterial & material);
        PxRigidActor*   createRigidActor            (PxScene & scene, PxPhysics & physics, PxShape *& shape, const PxTransform & pose, const PxGeometry & geometry, PxMaterial & material, const char * shapeName);

        void            BuildScene();
        void            ImportTexture();
        
        void            PrepareActor                (const PxTransform& pos, int MaterialID, const char* inObjFileName, bool isRigid);
		void            PrepareActor                (const PxTransform & pos, std::vector<int> MaterialID, std::vector<const char*> inObjFileNames, bool isRigid, 
                                                     const PxTransform & jointpos, float mass, const PxVec3 & tensor);
        void            PrepareSwitchActor          (const PxTransform & pos, std::vector<int> MaterialID, std::vector<const char*> inObjFileNames, bool isRigid,
                                                     const PxTransform & jointpos, float mass, const PxVec3 & tensor);
        
        ///////////////////////////////////////////////////////////////////////////////
		// Dati di materiali e mesh salvati globalmente in vettori e mappe
		
		std::map<const char*, RAWMesh>  dataMesh;
		std::vector<PxShape*>           dataShape;
		std::vector<PxTransform>        dataPos; 
		std::vector<RenderBaseActor*>   dataRender;
        bool	                        isGameStarted;

        ///////////////////////////////////////////////////////////////////////////////
        // Funzioni per la logica e gli eventi di gioco

		void onLeverPulled();
		void onKeyReached();
		void printConclusionMessage();
		void printIntroMessage();

        ///////////////////////////////////////////////////////////////////////////////
		const char* switch_lever = "switch_key_model.obj";
		const char* bridge = "bridge_model.obj";
	};

#endif

