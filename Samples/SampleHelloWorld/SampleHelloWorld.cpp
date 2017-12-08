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

#define STEP_OFFSET				0.05f

#define SLOPE_LIMIT				0.0f

#define INVISIBLE_WALLS_HEIGHT	0.0f

#define MAX_JUMP_HEIGHT			0.0f
static const float gScaleFactor = 1.0f;
static float gStandingSize = 1.0f * gScaleFactor;
static const float gCrouchingSize = 0.25f * gScaleFactor;
static float gControllerRadius = 0.5f * gScaleFactor;

//static const char* gDynamic = "Dynamic";

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
	dataShape.push_back(shape);
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
	scene.addActor(*dyn);
	dataShape.push_back(shape);
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



//Vecchio metodo per la generazione della scena. Lascio come riferimento (da rimuovere in futuri commit)
//void SampleHelloWorld::buildTest()
/*{

	PxTransform pos = PxTransform(PxVec3(0, 0, 0));//, PxQuat(-PxHalfPi, PxVec3(1.0f, 0.0f, 0.0f)));// PxQuat(-0.0017525638f, -0.24773766f, 0.00040674198f, -0.96882552f));
	createRAWMeshFromObjMesh("sfera_l2.obj", pos, 101, data);
	dataPos.push_back(pos);

    PxTransform pos1 = PxTransform(PxVec3(10, 0, 0));// , PxQuat(PxHalfPi, PxVec3(1.0f, 0.0f, 0.0f)));
	createRAWMeshFromObjMesh("Staircase.obj", pos1, 102, data1);  
	dataPos.push_back(pos1);
    
	PxTransform pos2 = PxTransform(PxVec3(0, 0, -10));
	createRAWMeshFromObjMesh("Pala.obj", pos2, 103, data2);
	dataPos.push_back(pos2);

	PxTransform pos3 = PxTransform(PxVec3(0, 0, -10));
	createRAWMeshFromObjMesh("Cilinder.obj", pos2, 103, data3);
	dataPos.push_back(pos2);
	
	// Creazione mesh triangolari a partire dalle RAWMesh ottenute precedentemente + controllo errori
	//PxTriangleMesh* triMesh = generateTriMesh(data.mNbVerts, data.mNbFaces, data.mVerts, data.mIndices);
	//if (!triMesh)
	//	fatalError("creating the triangle mesh failed");
    //
	PxTriangleMesh* triMesh1 = generateTriMesh(data1.mNbVerts, data1.mNbFaces, data1.mVerts, data1.mIndices);
	if (!triMesh1)
		fatalError("creating the triangle mesh failed");

	PxTriangleMesh* triMesh3 = generateTriMesh(data3.mNbVerts, data3.mNbFaces, data3.mVerts, data3.mIndices);
	if (!triMesh3)
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
	PxTriangleMeshGeometry geom3(triMesh3);
	PxConvexMeshGeometry   geom(conMesh);
	PxConvexMeshGeometry   geom2(conMesh2);  
	
    
	PxShape* try_data, *try_data1, *try_data2, *try_data3;
	
    PxRigidDynamic* actor =		createDynamicActor	(getActiveScene(), getPhysics(), try_data,	pos,  geom,  getDefaultMaterial());
	PxRigidActor* staircase =	createRigidActor	(getActiveScene(), getPhysics(), try_data1, pos1, geom1, getDefaultMaterial());
	PxRigidDynamic* jointTest = createDynamicActor	(getActiveScene(), getPhysics(), try_data2, pos2, geom2, getDefaultMaterial());
	PxRigidActor* column =		createRigidActor	(getActiveScene(), getPhysics(), try_data3, pos3, geom3, getDefaultMaterial());

	//Creazione dei Joint

	//Posizione rispetto al quale l'oggetto è vincolato a muoversi
	// PxTransform pos2 = PxTransform(PxVec3(0, 0, -10));
	PxTransform jointpos = PxTransform(PxVec3(0, 0,0.4f));
	jointTest->setMass(2.f);
	jointTest->setMassSpaceInertiaTensor(PxVec3(1.f, 1.f, 1.f));
    //TEST JOINT:
	(*createDampedD6)(NULL, pos2, jointTest, jointpos); //-->in questo caso l'oggetto è posizionato nel punto pos2 e ruota attorno al proprio asse y
	//(*createDampedD6)(NULL, pos2, jointTest, pos2);    //-->in questo caso l'oggetto è posizionato nell'origine e ruota attorno a pos2
	//(*createDampedD6)(NULL, jointpos, jointTest, PxTransform(PxVec3(0,0,10)));  //-->oggetto posizionato in pos2 e ruota attorno all'origine
	//(*createDampedD6)(NULL, jointpos, jointTest, pos2);  //-->oggetto posizionato in -pos2 e ruota attorno all'origine
	// (*createDampedD6)(NULL, jointpos, jointTest, jointpos); //-->oggetto posizionato nell'origine e ruota attorno al proprio asse y

	//Creazione delle RenderMesh
	dataRender.push_back(createRenderMeshFromRawMesh(data));
	dataRender.push_back(createRenderMeshFromRawMesh(data1));
	dataRender.push_back(createRenderMeshFromRawMesh(data2));
	dataRender.push_back(createRenderMeshFromRawMesh(data3));
	return;
}
*/

void SampleHelloWorld::buildScene() {

	PrepareActor(PxTransform(PxVec3(0, 0, 0)),  101, "sfera_l2.obj", false);
	PrepareActor(PxTransform(PxVec3(10, 0, 0)), 102, "Staircase.obj", true);
	PrepareActor(PxTransform(PxVec3(0, 0, -10)), 103, "cilinder.obj", true );
	//PrepareActor(PxTransform(PxVec3(0, 0, -10)),103,"wall.obj",false, PxTransform(PxVec3(0, 0, 0.4f)),2.f, PxVec3(1.f, 1.f, 1.f));

    std::vector<const char *> objNames = { "wallp.obj", "wallp1.obj" };
    std::vector<int> matIds = { 103, 103 };

    PrepareActor(PxTransform(PxVec3(0, 0, -10)), matIds, objNames, false, PxTransform(PxVec3(0, 0, 0.4f)), 2.f, PxVec3(1.f, 1.f, 1.f));
}

void SampleHelloWorld::PrepareActor(const PxTransform & pos, int MaterialID, const char * inObjFileName, bool isRigid)
{
	//Import della mesh da disco
	RAWMesh newMesh;
	createRAWMeshFromObjMesh(inObjFileName, pos, MaterialID, newMesh);
	dataMesh.insert(std::make_pair(inObjFileName, newMesh));
	dataPos.push_back(pos);

	if (isRigid) //I rigid actor sono realizzati a partire dal calcolo di triangle Mesh
	{
        PxTriangleMesh* triMesh = generateTriMesh(newMesh.mNbVerts, newMesh.mNbFaces, newMesh.mVerts, newMesh.mIndices);
		if (!triMesh)
			fatalError("creating the triangle mesh failed");
		PxTriangleMeshGeometry triGeom(triMesh);
		PxShape* newShape;

		//Creazione dell'attore rigido per la scena Physx
		PxRigidActor* rigidActor = createRigidActor(getActiveScene(), getPhysics(), newShape, pos, triGeom, getDefaultMaterial());

		//Creazione dell'attore da renderizzare
		dataRender.push_back(createRenderMeshFromRawMesh(newMesh));
	}
	else //Gli attori dinamici necessitano invece di Convex Mesh
	{
		PxConvexMesh *conMesh = PxToolkit::createConvexMesh(getPhysics(), getCooking(), newMesh.mVerts, newMesh.mNbVerts, PxConvexFlag::eCOMPUTE_CONVEX);
		if (!conMesh)
			fatalError("creating the triangle mesh failed");
		PxConvexMeshGeometry   conGeom(conMesh);
		PxShape* newShape;

		//Creazione dell'attore dinamico per la scena Physx
		PxRigidDynamic* dynActor = createDynamicActor(getActiveScene(), getPhysics(), newShape, pos, conGeom, getDefaultMaterial());

		//Creazione dell'attore da renderizzare
		dataRender.push_back(createRenderMeshFromRawMesh(newMesh));
	}
}



//TODO: Controllare la presenza di tutti i parametri con assert
void SampleHelloWorld::PrepareActor(const PxTransform & pos, int MaterialID, const char * inObjFileName, 
									bool isRigid, const PxTransform & jointpos, float mass, const PxVec3 & tensor)
{
	if (isRigid) //I rigid actor sono realizzati a partire dal calcolo di triangle Mesh
	{
		PrepareActor(pos, MaterialID, inObjFileName, true);
	}
	else //Gli attori dinamici necessitano invece di Convex Mesh
	{
        //Import della mesh da disco
        RAWMesh newMesh;
        createRAWMeshFromObjMesh(inObjFileName, pos, MaterialID, newMesh);
        dataMesh.insert(std::make_pair(inObjFileName, newMesh));
        dataPos.push_back(pos);

		PxConvexMesh *conMesh = PxToolkit::createConvexMesh(getPhysics(), getCooking(), newMesh.mVerts, newMesh.mNbVerts, PxConvexFlag::eCOMPUTE_CONVEX);
		if (!conMesh)
			fatalError("creating the triangle mesh failed");
		PxConvexMeshGeometry   conGeom(conMesh);
		PxShape* newShape;

		//Creazione dell'attore dinamico per la scena Physx
		PxRigidDynamic* dynActor = createDynamicActor(getActiveScene(), getPhysics(), newShape, pos, conGeom, getDefaultMaterial());


		//Creazione del Joint
		(*createDampedD6)(NULL, pos, dynActor, jointpos);
		dynActor->setMass(mass);
		dynActor->setMassSpaceInertiaTensor(tensor);


		//Creazione dell'attore da renderizzare
		dataRender.push_back(createRenderMeshFromRawMesh(newMesh));
	}
}

//TODO: Implementare questo overload
void SampleHelloWorld::PrepareActor(const PxTransform & pos, std::vector<int> MaterialID, std::vector<const char *> inObjFileNames,
	bool isRigid, const PxTransform & jointpos, float mass, const PxVec3 & tensor) {
    
    std::vector<RAWMesh> vecMesh;
    for (int i = 0; i < (int)inObjFileNames.size(); i++) {
        RAWMesh mesh;
        createRAWMeshFromObjMesh(inObjFileNames[i], pos, MaterialID[i], mesh);
        vecMesh.push_back(mesh);
        dataMesh.insert(std::make_pair(inObjFileNames[i], mesh));
        dataPos.push_back(pos);
    }

    if (isRigid) //I rigid actor sono realizzati a partire dal calcolo di triangle Mesh
    {
        // Definisco prima attore
        PxRigidActor* statActor = static_cast<PxRigidActor*>(getPhysics().createRigidStatic(pos));
        if (!statActor)
            return;
        
        //... poi aggiungo ad esso ogni shape
        for (std::vector<RAWMesh>::iterator it = vecMesh.begin(); it != vecMesh.end(); ++it) {
            PxTriangleMesh* triMesh = generateTriMesh(it->mNbVerts, it->mNbFaces, it->mVerts, it->mIndices);
            if (!triMesh)
                fatalError("creating the triangle mesh failed");
            PxTriangleMeshGeometry triGeom(triMesh);
            
            PxShape* shape = PxRigidActorExt::createExclusiveShape(*statActor, triGeom, getDefaultMaterial());
            if (!shape)
            {
                statActor->release();
                return;
            }

            dataShape.push_back(shape);

            //Creazione dell'attore da renderizzare
            dataRender.push_back(createRenderMeshFromRawMesh(*it));
        }
        getActiveScene().addActor(*statActor);

    }
    else //Gli attori dinamici necessitano invece di Convex Mesh
    {
        //Creazione dell'attore dinamico per la scena Physx
        PxRigidDynamic* dynActor = getPhysics().createRigidDynamic(PxTransform(pos));

        for (std::vector<RAWMesh>::iterator it = vecMesh.begin(); it != vecMesh.end(); ++it) {
            PxConvexMesh* triMesh = PxToolkit::createConvexMesh(getPhysics(), getCooking(), it->mVerts, it->mNbVerts, PxConvexFlag::eCOMPUTE_CONVEX);
            if (!triMesh)
                fatalError("creating the triangle mesh failed");
            PxConvexMeshGeometry conGeom(triMesh);

            PxShape* shape = PxRigidActorExt::createExclusiveShape(*dynActor, conGeom, getDefaultMaterial());
            if (!shape)
            {
                dynActor->release();
                return;
            }

            dataShape.push_back(shape);

            //Creazione dell'attore da renderizzare
            dataRender.push_back(createRenderMeshFromRawMesh(*it));
        }
        getActiveScene().addActor(*dynActor);


        //Creazione del Joint
        (*createDampedD6)(NULL, pos, dynActor, jointpos);
        dynActor->setMass(mass);
        dynActor->setMassSpaceInertiaTensor(tensor);
    }
}

void SampleHelloWorld::onInit()
{

	PhysXSample::onInit();
    PxSceneWriteLock scopedLock(*mScene);

    getActiveScene().setGravity(PxVec3(0.0f, -9.81f, 0.0f));
	//Importo materiali/texture
	{
		RAWTexture Texturedata;
		Texturedata.mName = "DiffuseMap.bmp";
		RenderTexture* Texture = createRenderTextureFromRawTexture(Texturedata);
		mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 101, Texture));

		RAWTexture Texturedata1;
		Texturedata1.mName = "nvidia.bmp";
		RenderTexture* Texture1 = createRenderTextureFromRawTexture(Texturedata1);
		mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 102, Texture1));

		RAWTexture Texturedata2;
		Texturedata2.mName = "Wood_Tileable.jpg";
		RenderTexture* Texture2 = createRenderTextureFromRawTexture(Texturedata2);
		mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 103, Texture2));
	}


	// Creazione mesh
	buildScene();

	//Setting application
	//mApplication.setMouseCursorHiding(true);
	//mApplication.setMouseCursorRecentering(true);
    
	//-----------------Codice preso da SampleBridge----------------
	mControllerManager = PxCreateControllerManager(getActiveScene());
	ControlledActorDesc desc;
	{

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
       
        mActor->init2(desc, mControllerManager, dataMesh["sfera_l2.obj"]);

        // Lasciata in caso di controllo
        //mActor->init(desc, mControllerManager);// , data2);

		RenderBaseActor* actor0 = mActor->getRenderActorStanding();
        if (actor0) {
            actor0->setRenderMaterial(mRenderMaterials[0]);
            mRenderActors.push_back(actor0);
        }
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
    if (actorName == "Dynamic")
        return PxQueryHitType::eNONE;

    return PxQueryHitType::eBLOCK;
}

PxQueryHitType::Enum SampleHelloWorld::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
    return PxQueryHitType::eBLOCK;
}