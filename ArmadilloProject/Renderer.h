#pragma once
#ifndef PHYSX_RENDER_H
#define PHYSX_RENDER_H

#include "PxPhysicsAPI.h"
#include "foundation/PxPreprocessor.h"
#include <windows.h>
#pragma warning(disable: 4505)
#include "glut.h"


namespace ArmadilloProject
{
	void setupDefaultWindow(const char* name);
	void setupDefaultRenderState();

	void startRender(const physx::PxVec3& cameraEye, const physx::PxVec3& cameraDir);
	void renderActors(physx::PxRigidActor** actors, const physx::PxU32 numActors, bool shadows = false, const physx::PxVec3 & color = physx::PxVec3(0.0f, 0.75f, 0.0f));
	void finishRender();
}

#define MAX_NUM_ACTOR_SHAPES 128

#endif //PHYSX_RENDER_H
