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
////////////////////////////////////////////////////////////////////////////////
/*#define CONTACT_OFFSET			0.01f
#define STEP_OFFSET				0.01f
#define SLOPE_LIMIT				0.0f
#define INVISIBLE_WALLS_HEIGHT	0.0f
#define MAX_JUMP_HEIGHT			0.0f

static const float gScaleFactor = 1.0f;// 1.5f;
static const float gStandingSize = 0.5f * gScaleFactor;
static const float gCrouchingSize = 0.25f * gScaleFactor;
static const float gControllerRadius = 0.5f * gScaleFactor;*/

#define CONTACT_OFFSET			0.01f
//	#define CONTACT_OFFSET			0.1f
//	#define STEP_OFFSET				0.01f
#define STEP_OFFSET				0.05f
//	#define STEP_OFFSET				0.1f
//	#define STEP_OFFSET				0.2f

//	#define SLOPE_LIMIT				0.8f
#define SLOPE_LIMIT				0.0f
//	#define INVISIBLE_WALLS_HEIGHT	6.0f
#define INVISIBLE_WALLS_HEIGHT	0.0f
//	#define MAX_JUMP_HEIGHT			4.0f
#define MAX_JUMP_HEIGHT			0.0f
static const float gScaleFactor = 1.0f;
static float gStandingSize = 1.0f * gScaleFactor;
static const float gCrouchingSize = 0.25f * gScaleFactor;
static float gControllerRadius = 0.5f * gScaleFactor;
extern const char* gDynamic;


///////////////////////////////////////////////////////////////////////////////

SampleHelloWorld::SampleHelloWorld(PhysXSampleApplication& app) :	PhysXSample(app)
{
	mControllerInitialPosition = PxExtendedVec3(0,0 , 10);
}

SampleHelloWorld::~SampleHelloWorld()
{
}

void SampleHelloWorld::onTickPreRender(float dtime)
{
	if (dataShape.size()!=dataRender.size())
		fatalError("Shape e posizioni relative non coincidono. Controllare la presenza di tutti i render object e shape");

	for (int i = 0; i < (int)dataRender.size(); i++) {
		dataShape[i]->userData = dataRender[i];
		{
            PxSceneReadLock scopedLock(*mScene);// getActiveScene());

			dataRender[i]->setPhysicsShape(dataShape[i], dataShape[i]->getActor());
            dataRender[i]->setEnableCameraCull(true);

			PxTriangleMeshGeometry geometry;
			dataShape[i]->getTriangleMeshGeometry(geometry);
			dataRender[i]->setMeshScale(geometry.scale);
		}	
	}

    // metto il lock sulla risorsa per ottenere la velocità lineare dell'attore in movimento
    PxSceneWriteLock scopedLock(*mScene);
    mActor->synCamera(mApplication.getCamera(), mActor->getController()->getActor()->getLinearVelocity());
    PhysXSample::onTickPreRender(dtime);
}

void SampleHelloWorld::onTickPostRender(float dtime)
{
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

//Codice preso da SnippetJoint. Le flag per gli assi sono state modificate opportunamente
PxJoint* createDampedD6(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1) 
//t0 = posizione di partenza del join
//t1 = punto/asse/piano di rotazione, relativo alla posizione dell'oggetto 
//(ad esempio t1=(0,0,0) corrisponde all'origine dell'oggetto stesso, non della scena)
{
	PxD6Joint* j = PxD6JointCreate(PxGetPhysics(), a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);		// y axis constraint
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);	// z axis constraint
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);    // x axis constraint
	j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	return j;
}



//Metodo che crea le 2 mesh triangolari e salva i dati in maniera globale
PxRigidStatic* SampleHelloWorld::buildTest()
{
	// Creazione mesh con relativa posizione iniziale
    PxTransform pos = PxTransform(PxVec3(0, 0, 0));//, PxQuat(-PxHalfPi, PxVec3(1.0f, 0.0f, 0.0f)));// PxQuat(-0.0017525638f, -0.24773766f, 0.00040674198f, -0.96882552f));
	createRAWMeshFromObjMesh("sfera_l2.obj", pos, 101, data);    
	dataPos.push_back(pos);

    PxTransform pos1 = PxTransform(PxVec3(10, 0, 0));// , PxQuat(PxHalfPi, PxVec3(1.0f, 0.0f, 0.0f)));
	createRAWMeshFromObjMesh("Staircase.obj", pos1, 102, data1);  
	dataPos.push_back(pos1);
    
	PxTransform pos2 = PxTransform(PxVec3(0, 0, -10));
	createRAWMeshFromObjMesh("JointTest2.obj", pos1, 103, data2);
	dataPos.push_back(pos2);

	// Creazione mesh triangolari a partire dalle RAWMesh ottenute precedentemente + controllo errori
	/*PxTriangleMesh* triMesh = generateTriMesh(data.mNbVerts, data.mNbFaces, data.mVerts, data.mIndices);
	if (!triMesh)
		fatalError("creating the triangle mesh failed");
        */
	PxTriangleMesh* triMesh1 = generateTriMesh(data1.mNbVerts, data1.mNbFaces, data1.mVerts, data1.mIndices);
	if (!triMesh1)
		fatalError("creating the triangle mesh failed");

    //Gli attori dinamici non supportano le triangle mesh. E' necessario utilizzare le convexMesh
    PxConvexMesh *conMesh = PxToolkit::createConvexMesh(getPhysics(), getCooking(), data.mVerts, data.mNbVerts, PxConvexFlag::eCOMPUTE_CONVEX);
    if (!conMesh)
		fatalError("creating the triangle mesh failed");

	PxConvexMesh *conMesh2 = PxToolkit::createConvexMesh(getPhysics(), getCooking(), data2.mVerts, data2.mNbVerts, PxConvexFlag::eCOMPUTE_CONVEX);
	if (!conMesh2)
		fatalError("creating the triangle mesh failed");

	// Creazione attori nel punto dello spazio ricevuto in input + controllo errori
	// Creazione shapes + controllo errori

	//PxTriangleMeshGeometry geom(triMesh);
	PxTriangleMeshGeometry geom1(triMesh1);
	PxConvexMeshGeometry   geom(conMesh);
	PxConvexMeshGeometry   geom2(conMesh2);

    
    
    PxShape* try_data, *try_data1, *try_data2;
	//PxRigidActor* actor = createRigidActor(getActiveScene(), getPhysics(), try_data, pos, geom, getDefaultMaterial());
    PxRigidDynamic* actor = createDynamicActor(getActiveScene(), getPhysics(), try_data, pos, geom, getDefaultMaterial());

	PxRigidActor* staircase = createRigidActor(getActiveScene(), getPhysics(), try_data1, pos1, geom1, getDefaultMaterial());
	PxRigidDynamic* jointTest = createDynamicActor(getActiveScene(), getPhysics(), try_data2, pos2, geom2, getDefaultMaterial());
	//Posizione rispetto al quale l'oggetto è vincolato a muoversi
	//NOTA: La scimmia nera è posizionata sopra l'origine come riferimento

	// PxTransform pos2 = PxTransform(PxVec3(0, 0, -10));
	PxTransform jointpos = PxTransform(PxVec3(0, 0, 2));

    //TEST JOINT:
	(*createDampedD6)(NULL, pos2, jointTest, jointpos); //-->in questo caso l'oggetto è posizionato nel punto pos2 e ruota attorno al proprio asse y
	//(*createDampedD6)(NULL, pos2, jointTest, pos2);    //-->in questo caso l'oggetto è posizionato nell'origine e ruota attorno a pos2
	//(*createDampedD6)(NULL, jointpos, jointTest, PxTransform(PxVec3(0,0,10)));  //-->oggetto posizionato in pos2 e ruota attorno all'origine
	//(*createDampedD6)(NULL, jointpos, jointTest, pos2);  //-->oggetto posizionato in -pos2 e ruota attorno all'origine
	// (*createDampedD6)(NULL, jointpos, jointTest, jointpos); //-->oggetto posizionato nell'origine e ruota attorno al proprio asse y
    
	dataShape.push_back(try_data);
	dataShape.push_back(try_data1);
	dataShape.push_back(try_data2);

	//Creazione delle RenderMesh
	RenderBaseActor* renderMesh = createRenderMeshFromRawMesh(data);
	dataRender.push_back(renderMesh);
	RenderBaseActor* renderMesh1 = createRenderMeshFromRawMesh(data1);
	dataRender.push_back(renderMesh1);
	RenderBaseActor* renderMesh2 = createRenderMeshFromRawMesh(data2);
	dataRender.push_back(renderMesh2);	
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

    getActiveScene().setGravity(PxVec3(0.0f, -9.81f, 0.0f));
	//Importo materiali/texture
	{
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

		RAWTexture Texturedata2;
		Texturedata2.mName = "Wood_Tileable.jpg";
		RenderTexture* tryTexture2 = createRenderTextureFromRawTexture(Texturedata2);

		tryMaterial2 = SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 103, tryTexture2);
		mRenderMaterials.push_back(tryMaterial2);
	}
	// Creazione mesh
	buildTest();

	//Setting application
	//mApplication.setMouseCursorHiding(true);
	//mApplication.setMouseCursorRecentering(true);
    
	//mCameraController.init(PxVec3(0.0f, 10.0f, 0.0f), PxVec3(0.0f, 0.0f, 0.0f));
	//mCameraController.setMouseSensitivity(0.5f);

	//-----------------Codice preso da SampleBridge----------------
	mControllerManager = PxCreateControllerManager(getActiveScene());
	ControlledActorDesc desc;
	{
        // Caso controllore sia una capsula
		/*desc.mType = PxControllerShapeType::eCAPSULE;
        desc.mPosition = mControllerInitialPosition;
		desc.mSlopeLimit = SLOPE_LIMIT;
		desc.mContactOffset = CONTACT_OFFSET;
		desc.mStepOffset = STEP_OFFSET;
		desc.mInvisibleWallHeight = INVISIBLE_WALLS_HEIGHT;
		desc.mMaxJumpHeight = MAX_JUMP_HEIGHT;
		desc.mRadius = gControllerRadius;
        desc.mHeight = gStandingSize;
		desc.mCrouchHeight = gCrouchingSize;
		//	desc.mReportCallback = this;
		//	desc.mBehaviorCallback = this;*/

        // Caso controllore sia un cubo
        desc.mType = PxControllerShapeType::eBOX;
        desc.mPosition = mControllerInitialPosition;
        desc.mSlopeLimit = SLOPE_LIMIT;
        desc.mContactOffset = CONTACT_OFFSET;
        desc.mStepOffset = STEP_OFFSET;
        desc.mInvisibleWallHeight = INVISIBLE_WALLS_HEIGHT;
        desc.mMaxJumpHeight = MAX_JUMP_HEIGHT;
        desc.mRadius = 0.5f;// gControllerRadius; // IMPORTANTE NON MODIFICARE SE NON IN RELAZIONE ALLA MESH
        desc.mHeight = 0.01f; //gStandingSize;    // IMPORTANTE NON MODIFICARE SE NON IN RELAZIONE ALLA MESH
        desc.mCrouchHeight = gCrouchingSize;
        desc.mReportCallback = this;
        desc.mBehaviorCallback = this;
	}
	{

		mActor = SAMPLE_NEW(ControlledActor)(*this);
       
        mActor->init2(desc, mControllerManager, data);

        // Lasciata in caso di controllo
        //mActor->init(desc, mControllerManager);// , data2);

		RenderBaseActor* actor0 = mActor->getRenderActorStanding();
        if (actor0) {
            actor0->setRenderMaterial(mRenderMaterials[0]);
            mRenderActors.push_back(actor0);
        }
		
        // Codice vecchio
        //PxMeshScale* sc = new PxMeshScale(PxVec3(1.2f, 1.2f, 1.2f));
		//actor0->setMeshScale(*sc);
        //RenderBaseActor* actor1 = mActor->getRenderActorCrouching();
		/*if (actor0) {
            //actor0->setTransform(PxTransform(PxVec3(0, 0, 0), PxQuat(2 * PxHalfPi, PxVec3(1.0f, 0.0f, 0.0f))));
			actor0->setRenderMaterial(mRenderMaterials[0]);
			mRenderActors.push_back(actor0);
		}*/
//		if (actor1)
//			mRenderActors.push_back(actor1);
	}
    
	mCCTCamera = SAMPLE_NEW(SampleCCTCameraController)(*this);
	mCCTCamera->setControlled(&mActor, 0, 1);
	//	mCCTCamera->setFilterData();
	mCCTCamera->setFilterCallback(this);
	mCCTCamera->setGravity(-20.0f);

	setCameraController(mCCTCamera);
	mCCTCamera->setView(0, 0);
	
	//-----------------Fine Codice preso da SampleBridge----------------
}

// Metodo per liberare il controllerManager e l'oggetto mCCTCamera
void SampleHelloWorld::onShutdown()
{
    {
        PxSceneWriteLock scopedLock(*mScene);
        DELETESINGLE(mCCTCamera);

        mControllerManager->release();
    }

    PhysXSample::onShutdown();
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




extern const char* gDynamic;

void SampleHelloWorld::onShapeHit(const PxControllerShapeHit& hit)
{
    defaultCCTInteraction(hit);
}


PxControllerBehaviorFlags SampleHelloWorld::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
    return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags SampleHelloWorld::getBehaviorFlags(const PxController&)
{
    return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags SampleHelloWorld::getBehaviorFlags(const PxObstacle&)
{
    return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
}

PxQueryHitType::Enum SampleHelloWorld::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
    PX_UNUSED(actor);
    const char* actorName = shape->getActor()->getName();
    if (actorName == gDynamic)
        return PxQueryHitType::eNONE;

    return PxQueryHitType::eBLOCK;
}

PxQueryHitType::Enum SampleHelloWorld::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
    return PxQueryHitType::eBLOCK;
}