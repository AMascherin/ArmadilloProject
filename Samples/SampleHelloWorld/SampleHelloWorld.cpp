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

#include "SamplePreprocessor.h"
#include "SampleHelloWorld.h"
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

using namespace SampleRenderer;
using namespace SampleFramework;

REGISTER_SAMPLE(SampleHelloWorld, "SampleHelloWorld")

///////////////////////////////////////////////////////////////////////////////

SampleHelloWorld::SampleHelloWorld(PhysXSampleApplication& app) :
	PhysXSample(app)
{
}

SampleHelloWorld::~SampleHelloWorld()
{
}

void SampleHelloWorld::onTickPreRender(float dtime)
{
	// Start rendering mesh 1
	PxTransform pos = PxTransform(PxVec3(10, 0, 10));
	RenderBaseActor* renderMesh = createRenderMeshFromRawMesh(data);

	// Non dovrebbe essere necessaria, ho preso tutto da onTick su chuckLoader
	try_data->userData = renderMesh;
	{
		PxSceneReadLock scopedLock(getActiveScene());

		renderMesh->setPhysicsShape(try_data, try_data->getActor());
		renderMesh->setEnableCameraCull(true);

		PxTriangleMeshGeometry geometry;
		try_data->getTriangleMeshGeometry(geometry);
		renderMesh->setMeshScale(geometry.scale);
	}

	// Start rendering mesh 2 -> NUOVO Rendering fatto buildTest
	//PxTransform pos1 = PxTransform(PxVec3(0, 1, 0));
	//renderMesh1 = createRenderMeshFromRawMesh(data1);

	//// Non dovrebbe essere necessaria, ho preso tutto da onTick su chuckLoader
	try_data1->userData = renderMesh1;
	{
		PxSceneReadLock scopedLock(getActiveScene());

		renderMesh1->setPhysicsShape(try_data1, actordyn);
		renderMesh1->setEnableCameraCull(true);

		PxTriangleMeshGeometry geometry1;

		try_data1->getTriangleMeshGeometry(geometry1);
		renderMesh1->setMeshScale(provageo.scale);
    }
    
    PhysXSample::onTickPreRender(dtime);
}

void SampleHelloWorld::onTickPostRender(float dtime)
{
    //mScene->lockWrite();

    // Start rendering mesh 1
    PxTransform pos = PxTransform(PxVec3(10, 0, 10));
    RenderBaseActor* renderMesh = createRenderMeshFromRawMesh(data);

    // Non dovrebbe essere necessaria, ho preso tutto da onTick su chuckLoader
    try_data->userData = renderMesh;
    {
        PxSceneReadLock scopedLock(getActiveScene());

        renderMesh->setPhysicsShape(try_data, try_data->getActor());
        renderMesh->setEnableCameraCull(true);

        PxTriangleMeshGeometry geometry;
        //try_data->getTriangleMeshGeometry(geometry);
        //renderMesh->setMeshScale(geometry.scale);
    }

    // NUOVO Rendering fatto buildTest
    try_data1->userData = renderMesh1;
    {
        PxSceneReadLock scopedLock(getActiveScene());

        renderMesh1->setPhysicsShape(try_data1, actordyn);
        //renderMesh1->setEnableCameraCull(true);

        //PxTriangleMeshGeometry geometry1;

        //try_data1->getTriangleMeshGeometry(geometry1);
        //renderMesh1->setMeshScale(provageo.scale);
    }
	PhysXSample::onTickPostRender(dtime);
}

void SampleHelloWorld::customizeSceneDesc(PxSceneDesc& sceneDesc)
{
	sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;
}

void SampleHelloWorld::newMesh(const RAWMesh& data)
{
}

//Metodo per creare le mesh da un file .obj
RAWMesh* SampleHelloWorld::createRAWMeshFromObjMesh(const char* inObjFileName, const PxTransform& inPos, PxU32 inMaterialID, RAWMesh &outRawMesh)
{

	WavefrontObj wfo;
	if (!wfo.loadObj(getSampleMediaFilename(inObjFileName), true))
		return NULL;

	PxU32 nbTris = wfo.mTriCount;
	PxU32 nbVerts = wfo.mVertexCount;

	PxVec3*	verts = (PxVec3*)SAMPLE_ALLOC(sizeof(PxVec3)*nbVerts);
	PxMemCopy(verts, wfo.mVertices, sizeof(PxVec3)*nbVerts);

	PxU32* indices = (PxU32*)SAMPLE_ALLOC(sizeof(PxU32)*nbTris * 3);
	PxMemCopy(indices, wfo.mIndices, sizeof(PxU32)*nbTris * 3);

	PxReal* uvs = (PxReal*)SAMPLE_ALLOC(sizeof(PxReal)*nbVerts * 2);
	PxMemCopy(uvs, wfo.mTexCoords, sizeof(PxReal)*nbVerts * 2);

	PxVec3* normals = (PxVec3*)SAMPLE_ALLOC(sizeof(PxVec3)*nbVerts);
	PxBuildSmoothNormals(
		nbTris,
		nbVerts,
		verts,
		indices,
		NULL,
		normals,
		true
	);

	outRawMesh.mNbVerts = nbVerts;
	outRawMesh.mVerts = verts;
	outRawMesh.mNbFaces = nbTris;
	outRawMesh.mIndices = indices;
	outRawMesh.mUVs = uvs;
	outRawMesh.mVertexNormals = normals;
	outRawMesh.mMaterialID = inMaterialID;
	outRawMesh.mTransform = inPos;


	return &outRawMesh;
}


// Copia/incolla funzione di generazione di mash triangolari da LargeWorld 
PxTriangleMesh* SampleHelloWorld::generateTriMesh(PxU32 vertCount, PxU32 triCount, const PxVec3* vers, const PxU32* indices)
{
	//Cooking mesh

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = vertCount;
	meshDesc.triangles.count = triCount;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.triangles.stride = sizeof(PxU32) * 3;
	meshDesc.points.data = vers;
	meshDesc.triangles.data = indices;
	meshDesc.flags = PxMeshFlags(0);

	PxDefaultMemoryOutputStream stream;
	bool ok = getCooking().cookTriangleMesh(meshDesc, stream);
	if (!ok)
		return NULL;

	PxDefaultMemoryInputData rb(stream.getData(), stream.getSize());

	return getPhysics().createTriangleMesh(rb);
}

//Metodo per la creazione di un rigidActor
PxRigidActor* SampleHelloWorld::createRigidActor(PxScene& scene, PxPhysics& physics, PxShape* & shape,
	const PxTransform& pose, const PxGeometry& geometry, PxMaterial& material)
{

	PxRigidActor* _actor =  static_cast<PxRigidActor*>(physics.createRigidStatic(pose));
	if (!_actor)
		return NULL;

	shape = PxRigidActorExt::createExclusiveShape(*_actor, geometry, material);
	if (!shape)
	{
		_actor->release();
		return NULL;
	}
	scene.addActor(*_actor);

	return _actor;
}

//Metodo per la creazione di un oggetto dinamico
PxRigidDynamic* SampleHelloWorld::createDynamicActor
(PxScene& scene, PxPhysics& physics, PxShape* & shape, const PxTransform& pose, const PxGeometry& geometry, PxMaterial& material)
{

	PxRigidDynamic* dyn = static_cast<PxRigidDynamic*> (physics.createRigidDynamic(pose));

	shape = PxRigidActorExt::createExclusiveShape(*dyn, geometry, material);
	if (!shape)
	{
		dyn->release();
		return NULL;
	}
    // Commento per ora, lascio le configurazioni di default
	/*dyn->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	dyn->setLinearVelocity(PxVec3(0.1, 0.0, 0.0), true);
	dyn->setMass(0.03f);
	dyn->setMassSpaceInertiaTensor(PxVec3(0.f, 0.f, 10.f));*/
	scene.addActor(*dyn);

	return dyn;
}

//Metodo che crea le 2 mesh triangolari e salva i dati in maniera globale
PxRigidStatic* SampleHelloWorld::buildTest()
{

	// Creazione mesh con relativa posizione iniziale
	PxTransform pos = PxTransform(PxVec3(10, 0, 10));
	createRAWMeshFromObjMesh("Monkey.obj", pos, 101, data);

	PxTransform pos1 = PxTransform(PxVec3(0, 1, 0));
	createRAWMeshFromObjMesh("Monkey.obj", pos1, 102, data1);

	// Creazione mesh triangolari a partire dalle RAWMesh ottenute precedentemente + controllo errori
	PxTriangleMesh* triMesh = generateTriMesh(data.mNbVerts, data.mNbFaces, data.mVerts, data.mIndices);
	if (!triMesh)
		fatalError("creating the triangle mesh failed");
 
    // Codice vecchio usava le triangle mesh, non supportate nella creazione/utilizzo di attori dinamici
    //PxTriangleMesh* triMesh1 = generateTriMesh(data1.mNbVerts, data1.mNbFaces, data1.mVerts, data1.mIndices);
    PxConvexMesh *conMesh = PxToolkit::createConvexMesh(getPhysics(), getCooking(), data1.mVerts, data1.mNbVerts, PxConvexFlag::eCOMPUTE_CONVEX);
    if (!conMesh)
		fatalError("creating the triangle mesh failed");

	// Creazione attori nel punto dello spazio ricevuto in input + controllo errori
	// Creazione shapes + controllo errori

	PxTriangleMeshGeometry geom(triMesh);
    
    // Codice vecchio usava le triangle mesh, non supportate nella creazione/utilizzo di attori dinamici
    //PxTriangleMeshGeometry geom1(triMesh1);
    PxConvexMeshGeometry geom1(conMesh);
    //geomtry = geom;
    //provageo = geom1;
	actor = createRigidActor(getActiveScene(), getPhysics(), try_data, pos, geom, getDefaultMaterial());

    actordyn = createDynamicActor(getActiveScene(), getPhysics(), try_data1, pos1, geom1, getDefaultMaterial());//static_cast<PxRigidDynamic*> (getPhysics().createRigidDynamic(pos1));

    // NUOVO: rendering fatto in buildTest invece che su onTickPre/PostRendering
    renderMesh1 = createRenderMeshFromRawMesh(data1);

	// SERVE SEMPRE - LASCIO PER FUTURI USI
	//createRenderMeshFromRawMesh(data);
	/*RenderBaseActor* renderMesh = createRenderMeshFromRawMesh(data);
	iglooShape->userData = renderMesh;
	{
	PxSceneReadLock scopedLock(getActiveScene());

	renderMesh->setPhysicsShape(iglooShape, iglooShape->getActor());
	renderMesh->setEnableCameraCull(true);

	PxTriangleMeshGeometry geometry;
	iglooShape->getTriangleMeshGeometry(geometry);
	renderMesh->setMeshScale(geometry.scale);
	}*/
	//setCCDActive(*iglooShape);

	//createRenderObjectsFromActor(iglooActor, roadGravelMaterial);

	//createRenderObjectFromShape(iglooActor, try_data, mSnowMaterial)
    // Start rendering mesh 2
    //PxTransform pos1 = PxTransform(PxVec3(0, 1, 0));

    //// Non dovrebbe essere necessaria, ho preso tutto da onTick su chuckLoader
  
	//createRenderObjectFromShape(iglooActor, iglooShape, roadGravelMaterial);

	//mPhysicsActors.push_back(iglooActor);

	return NULL;
}


static void gValue(Console* console, const char* text, void* userData)
{
	if (!text)
	{
		console->out("Usage: value <float>");
		return;
	}

	const float val = (float)::atof(text);
	shdfnd::printFormatted("value: %f\n", val);
}

static void gExport(Console* console, const char* text, void* userData)
{
	if (!text)
	{
		console->out("Usage: export <filename>");
		return;
	}
}

static void gImport(Console* console, const char* text, void* userData)
{
	if (!text)
	{
		console->out("Usage: import <filename>");
		return;
	}
}

void SampleHelloWorld::onInit()
{
	if (getConsole())
	{
		getConsole()->addCmd("value", gValue);
		getConsole()->addCmd("export", gExport);
		getConsole()->addCmd("import", gImport);
	}

	PhysXSample::onInit();
    PxSceneWriteLock scopedLock(*mScene);

	//Importo materiali/texture
	RAWTexture Texturedata;
	Texturedata.mName = "DiffuseMap.bmp";
	RenderTexture* tryTexture = createRenderTextureFromRawTexture(Texturedata);

	tryMaterial = SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 101, tryTexture);
	mRenderMaterials.push_back(tryMaterial);

	RAWTexture Texturedata1;
	Texturedata1.mName = "nvidia.bmp";
	RenderTexture* tryTexture1 = createRenderTextureFromRawTexture(Texturedata1);

	tryMaterial1 = SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 102, tryTexture1);
	mRenderMaterials.push_back(tryMaterial1);

	// Creazione mesh
	buildTest();

	mApplication.setMouseCursorHiding(true);
	mApplication.setMouseCursorRecentering(true);
	mCameraController.init(PxVec3(0.0f, 10.0f, 0.0f), PxVec3(0.0f, 0.0f, 0.0f));
	mCameraController.setMouseSensitivity(0.5f);
}

void SampleHelloWorld::collectInputEvents(std::vector<const SampleFramework::InputEvent*>& inputEvents)
{
	PhysXSample::collectInputEvents(inputEvents);
	getApplication().getPlatform()->getSampleUserInput()->unregisterInputEvent(CAMERA_SPEED_INCREASE);
	getApplication().getPlatform()->getSampleUserInput()->unregisterInputEvent(CAMERA_SPEED_DECREASE);

	//touch events
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT, "Throw Object", ABUTTON_5, IBUTTON_5);
}

void SampleHelloWorld::helpRender(PxU32 x, PxU32 y, PxU8 textAlpha)
{
	Renderer* renderer = getRenderer();
	const PxU32 yInc = 18;
	const PxReal scale = 0.5f;
	const PxReal shadowOffset = 6.0f;
	const RendererColor textColor(255, 255, 255, textAlpha);
	const bool isMouseSupported = getApplication().getPlatform()->getSampleUserInput()->mouseSupported();
	const bool isPadSupported = getApplication().getPlatform()->getSampleUserInput()->gamepadSupported();
	const char* msg;

	if (isMouseSupported && isPadSupported)
		renderer->print(x, y += yInc, "Use mouse or right stick to rotate", scale, shadowOffset, textColor);
	else if (isMouseSupported)
		renderer->print(x, y += yInc, "Use mouse to rotate", scale, shadowOffset, textColor);
	else if (isPadSupported)
		renderer->print(x, y += yInc, "Use right stick to rotate", scale, shadowOffset, textColor);
	if (isPadSupported)
		renderer->print(x, y += yInc, "Use left stick to move", scale, shadowOffset, textColor);
	msg = mApplication.inputMoveInfoMsg("Press ", " to move", CAMERA_MOVE_FORWARD, CAMERA_MOVE_BACKWARD, CAMERA_MOVE_LEFT, CAMERA_MOVE_RIGHT);
	if (msg)
		renderer->print(x, y += yInc, msg, scale, shadowOffset, textColor);
	msg = mApplication.inputInfoMsg("Press ", " to move fast", CAMERA_SHIFT_SPEED, -1);
	if (msg)
		renderer->print(x, y += yInc, msg, scale, shadowOffset, textColor);
	msg = mApplication.inputInfoMsg("Press ", " to throw an object", SPAWN_DEBUG_OBJECT, -1);
	if (msg)
		renderer->print(x, y += yInc, msg, scale, shadowOffset, textColor);
}

void SampleHelloWorld::descriptionRender(PxU32 x, PxU32 y, PxU8 textAlpha)
{
	bool print = (textAlpha != 0.0f);

	if (print)
	{
		Renderer* renderer = getRenderer();
		const PxU32 yInc = 18;
		const PxReal scale = 0.5f;
		const PxReal shadowOffset = 6.0f;
		const RendererColor textColor(255, 255, 255, textAlpha);

		char line1[256] = "This sample demonstrates how to set up and simulate a PhysX";
		char line2[256] = "scene.  Further, it illustrates the creation, simulation and";
		char line3[256] = "collision of simple dynamic objects.";
		renderer->print(x, y += yInc, line1, scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, line2, scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, line3, scale, shadowOffset, textColor);
	}
}

PxU32 SampleHelloWorld::getDebugObjectTypes() const
{
	return DEBUG_OBJECT_BOX | DEBUG_OBJECT_SPHERE | DEBUG_OBJECT_CAPSULE | DEBUG_OBJECT_CONVEX;
}

