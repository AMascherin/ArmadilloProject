#include "ArmadilloGame.h"

// Import e posizionamento modelli della scena
void ArmadilloGame::BuildScene() {

	// Modello player : armadillo
	PrepareActor(PxTransform(PxVec3(-10, 1, -10)), 101, "armadillo_model.obj", false);
	
    // Import oggetti statici
	PrepareActor(PxTransform(PxVec3(0, 0, 0)), 104, "floor_model.obj", true);
	PrepareActor(PxTransform(PxVec3(0, 0, 0)), 105, "containers_model.obj", true);
    PrepareActor(PxTransform(PxVec3(0, 0, 0)), 106, "labyrint_model.obj", true);
	PrepareActor(PxTransform(PxVec3(0, 0, 0)), 107, "building1_model.obj", true);
    PrepareActor(PxTransform(PxVec3(0, 0, 0)), 108, "building2_model.obj", true);
	PrepareActor(PxTransform(PxVec3(0, 0, 0)), 109, "crane_model.obj", true);
	PrepareActor(PxTransform(PxVec3(0, 0, 0)), 110, "switch_model.obj", true);
	PrepareActor(PxTransform(PxVec3(0, -1.f, 0)), 111, "staircase_model.obj", true);
	PrepareActor(PxTransform(PxVec3(50, 0, 0)), 113, "key_model.obj", true);
	PrepareActor(PxTransform(PxVec3(0, 0, 0)), 114, "barrier_model.obj", true);
	PrepareActor(PxTransform(PxVec3(0, 0, 0)), 116, "barrel_model.obj", true);

    // Import oggetti dinamici
	PrepareActor(PxTransform(PxVec3(5.5629, 56.9997, -26.5913)), 115, BRIDGE_NAME, false);

	// Importo oggetti dinamici per il joint
	int pipeMat = 112;
	std::vector<const char *> objNames =    { "wallC1_model.obj" };
	std::vector<int> matIds =               { pipeMat };

	PrepareActor(PxTransform(PxVec3(16.5069, 0, 23.3192)), pipeMat, "cylinder1_model.obj", true);
	PrepareActor(PxTransform(PxVec3(16.5069, 0.1, 23.3192)), matIds, objNames, false, PxTransform(PxVec3(0, 0, 0)), 2.f, PxVec3(1.f, 1.f, 1.f));

	objNames = { "wallC2_model.obj" };
    PrepareActor(PxTransform(PxVec3(0.204545, 0, 35.6161)), pipeMat, "cylinder2_model.obj", true);
	PrepareActor(PxTransform(PxVec3(0.204545, 0.1, 35.6161)), matIds, objNames, false, PxTransform(PxVec3(0, 0, 0)), 2.f, PxVec3(1.f, 1.f, 1.f));

	PrepareActor(PxTransform(PxVec3(-30.5141, 0, 35.5473)), pipeMat, "cylinder2_model.obj", true);
	PrepareActor(PxTransform(PxVec3(-30.5141, 0.1, 35.5473)), matIds, objNames, false, PxTransform(PxVec3(0, 0, 0)), 2.f, PxVec3(1.f, 1.f, 1.f));

	PrepareActor(PxTransform(PxVec3(-52.9248, 0, 41.8169)), pipeMat, "cylinder2_model.obj", true);
	PrepareActor(PxTransform(PxVec3(-52.9248, 0.1, 41.8169)), matIds, objNames, false, PxTransform(PxVec3(0, 0, 0)), 2.f, PxVec3(1.f, 1.f, 1.f));

    // Importo switch
	objNames =  { SWITCH_LEVER_NAME };
    matIds =    { 110 };
	PrepareSwitchActor(PxTransform(PxVec3(-35.1447, 26.4953, -43.8493)), matIds, objNames, false, PxTransform(PxVec3(0, 0, 0)), 2.f, PxVec3(1.f, 1.f, 1.f));
}


// Funzione per creare le mesh da un file .obj
RAWMesh* ArmadilloGame::createRAWMeshFromObjMesh(const char* inObjFileName, const PxTransform& inPos, PxU32 inMaterialID, RAWMesh &outRawMesh)
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

// Funzione di generazione di mesh triangolari
PxTriangleMesh* ArmadilloGame::generateTriMesh(PxU32 vertCount, PxU32 triCount, const PxVec3* vers, const PxU32* indices)
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

//Funzione per la creazione di un rigidActor
PxRigidActor* ArmadilloGame::createRigidActor(PxScene& scene, PxPhysics& physics, PxShape* & shape,
	const PxTransform& pose, const PxGeometry& geometry, PxMaterial& material)
{
	PxRigidActor* _actor = static_cast<PxRigidActor*>(physics.createRigidStatic(pose));
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

//Funzione per la creazione di un rigidDynamic
PxRigidDynamic* ArmadilloGame::createDynamicActor
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


//Funzione per la creazione di un rigidActor con nome
PxRigidActor* ArmadilloGame::createRigidActor(PxScene& scene, PxPhysics& physics, PxShape* & shape,
	const PxTransform& pose, const PxGeometry& geometry, PxMaterial& material, const char *shapeName)
{
	PxRigidActor* _actor = static_cast<PxRigidActor*>(physics.createRigidStatic(pose));
	if (!_actor)
		return NULL;

	shape = PxRigidActorExt::createExclusiveShape(*_actor, geometry, material);
	shape->setName(shapeName);

    // L'oggetto "switch_model" non entra a far parte della simulazione fisica per motivi di semplicità 
    // nella gestione degli urti nella scena
	if (!strcmp(shapeName, "switch_model.obj"))
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);

	if (!shape)
	{
		_actor->release();
		return NULL;
	}
	scene.addActor(*_actor);
	dataShape.push_back(shape);
	return _actor;
}

//Funzione per la creazione di un rigidDynamic con nome
PxRigidDynamic* ArmadilloGame::createDynamicActor
(PxScene& scene, PxPhysics& physics, PxShape* & shape, const PxTransform& pose, const PxGeometry& geometry, PxMaterial& material, const char *shapeName)
{
	PxRigidDynamic* dyn = static_cast<PxRigidDynamic*> (physics.createRigidDynamic(pose));
	shape = PxRigidActorExt::createExclusiveShape(*dyn, geometry, material);
	shape->setName(shapeName);

	if (!shape)
	{
		dyn->release();
		return NULL;
	}

    scene.addActor(*dyn);
	dataShape.push_back(shape);
	return dyn;
}

// Funzione creata per generare un DampedD6 Joint
// Le flag per gli assi sono state modificate opportunamente
//  t0 = posizione di partenza del join
//  t1 = punto/asse/piano di rotazione, relativo alla posizione dell'oggetto 
// (ad esempio t1=(0,0,0) corrisponde all'origine dell'oggetto stesso, non della scena)
PxJoint* createDampedD6(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxD6Joint* j = PxD6JointCreate(PxGetPhysics(), a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);		// y axis constraint
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);	// z axis constraint
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);    // x axis constraint
	j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	return j;
}

// Funzione creata per generare un DampedD6 Joint
// Le flag per gli assi sono state modificate opportunamente per gestire il movimento dello switch
//  t0 = posizione di partenza del join
//  t1 = punto/asse/piano di rotazione, relativo alla posizione dell'oggetto 
// (ad esempio t1=(0,0,0) corrisponde all'origine dell'oggetto stesso, non della scena)
PxJoint* createDampedD6Switch(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxD6Joint* j = PxD6JointCreate(PxGetPhysics(), a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);	// y axis constraint
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);	// z axis constraint
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);      // x axis constraint
	j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	return j;
}

// Funzione per generare un attore, a seconda del flag "isRigid" sarà rigidDynamic o rigidActor 
void ArmadilloGame::PrepareActor(const PxTransform & pos, int MaterialID, const char * inObjFileName, bool isRigid)
{
	// Import della mesh da disco
	RAWMesh newMesh;
	createRAWMeshFromObjMesh(inObjFileName, pos, MaterialID, newMesh);
	dataMesh.insert(std::make_pair(inObjFileName, newMesh));
	dataPos.push_back(pos);

    // I rigid actor sono realizzati a partire dal calcolo di triangle Mesh
	if (isRigid)
	{
		PxTriangleMesh* triMesh = generateTriMesh(newMesh.mNbVerts, newMesh.mNbFaces, newMesh.mVerts, newMesh.mIndices);
		if (!triMesh)
			fatalError("creating the triangle mesh failed");
		PxTriangleMeshGeometry triGeom(triMesh);
		PxShape* newShape;
		
        // Creazione dell'attore rigido per la scena Physx
        // Se la mesh è della chiave, l'attore che lo controlla viene gestito esternamente
        if (!strcmp(inObjFileName, "key_model.obj"))
			mFloatingActor = createRigidActor(getActiveScene(), getPhysics(), newShape, pos, triGeom, getDefaultMaterial(), inObjFileName);
		else
			PxRigidActor* rigidActor = createRigidActor(getActiveScene(), getPhysics(), newShape, pos, triGeom, getDefaultMaterial(), inObjFileName);

		// Salvataggio dell'attore da renderizzare
		dataRender.push_back(createRenderMeshFromRawMesh(newMesh));
	}
	else 
	{
        // Gli attori dinamici necessitano invece di Convex Mesh
		PxConvexMesh *conMesh = PxToolkit::createConvexMesh(getPhysics(), getCooking(), newMesh.mVerts, newMesh.mNbVerts, PxConvexFlag::eCOMPUTE_CONVEX);
		if (!conMesh)
			fatalError("creating the triangle mesh failed");
		PxConvexMeshGeometry conGeom(conMesh);
		PxShape* newShape;

		// Creazione dell'attore dinamico per la scena Physx
        // Se la mesh è dell'armadillo (player), l'attore che lo controlla viene gestito esternamente
		if (!strcmp(inObjFileName, "armadillo_model.obj")) {
			mControllerActor = createDynamicActor(getActiveScene(), getPhysics(), newShape, pos, conGeom, getDefaultMaterial(), inObjFileName);
			
            // Parametri di simulazione fisica
            mControllerActor->setLinearDamping(0.3f);
			mControllerActor->setAngularDamping(0.3f);
			mControllerActor->setMass(0.9f);
		}
		else {
            // Creazione dell'attore dinamico per la scena Physx
            // Se la mesh è il ponte, l'attore che lo controlla viene gestito esternamente
            if (!strcmp(inObjFileName, BRIDGE_NAME)) {
				PxMaterial* test = getPhysics().createMaterial(1, 1, 1);
                mBridgeActor = createDynamicActor(getActiveScene(), getPhysics(), newShape, pos, conGeom, *test, inObjFileName);

                // Parametri di simulazione fisica
                mBridgeActor->setMass(100.1f);
				mBridgeActor->setMassSpaceInertiaTensor(PxVec3(100.1f, 100.1f, 100.f));
				mBridgeActor->setSolverIterationCounts(10, 10);
				mBridgeActor->setLinearDamping(1.0f);
				mBridgeActor->setAngularDamping(1.0f);

                // Inizialmente l'attore sarà considerato di tipo KINEAMTIC (vedi doc Physx)
				mBridgeActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			}
			else
				PxRigidDynamic* dynActor = createDynamicActor(getActiveScene(), getPhysics(), newShape, pos, conGeom, getDefaultMaterial(), inObjFileName);
		}

		// Salvataggio dell'attore da renderizzare
		dataRender.push_back(createRenderMeshFromRawMesh(newMesh));
	}
}

// Funzione per generare un attore, a seconda del flag "isRigid" sarà rigidDynamic o rigidActor 
// Se rigidDynamic sarà di tipo Joint
void ArmadilloGame::PrepareActor(const PxTransform & pos, std::vector<int> MaterialID, std::vector<const char *> inObjFileNames,
	bool isRigid, const PxTransform & jointpos, float mass, const PxVec3 & tensor) {

    // Salvataggio dati RawMesh per ogni shape dell'attore
	std::vector<RAWMesh> vecMesh;
	for (int i = 0; i < (int)inObjFileNames.size(); i++) {
		RAWMesh mesh;
		createRAWMeshFromObjMesh(inObjFileNames[i], pos, MaterialID[i], mesh);
		vecMesh.push_back(mesh);
		dataMesh.insert(std::make_pair(inObjFileNames[i], mesh));
		dataPos.push_back(pos);
	}

    // I rigid actor sono realizzati a partire dal calcolo di triangle Mesh
	if (isRigid)
	{
		// Definisco prima l'attore...
		PxRigidActor* statActor = static_cast<PxRigidActor*>(getPhysics().createRigidStatic(pos));
		if (!statActor)
			return;

		// ... poi aggiungo ad esso ogni shape
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

			shape->setName("shape");
			dataShape.push_back(shape);

			// Salvataggio dell'attore da renderizzare
			dataRender.push_back(createRenderMeshFromRawMesh(*it));
		}
		getActiveScene().addActor(*statActor);

	}
	else
	{
        // Gli attori dinamici necessitano invece di Convex Mesh
		// Creazione dell'attore dinamico per la scena Physx
		PxRigidDynamic* dynActor = getPhysics().createRigidDynamic(PxTransform(pos));

		for (std::vector<RAWMesh>::iterator it = vecMesh.begin(); it != vecMesh.end(); ++it) {
			PxConvexMesh* triMesh = PxToolkit::createConvexMesh(getPhysics(), getCooking(), it->mVerts, it->mNbVerts, PxConvexFlag::eCOMPUTE_CONVEX);
			if (!triMesh)
				fatalError("creating the triangle mesh failed");
			PxConvexMeshGeometry conGeom(triMesh);

			PxShape* shape = PxRigidActorExt::createExclusiveShape(*dynActor, conGeom, getDefaultMaterial());
			shape->setName(inObjFileNames[0]);
			if (!shape)
			{
				dynActor->release();
				return;
			}
			shape->setName("shape");
			dataShape.push_back(shape);

			// Salvataggio dell'attore da renderizzare
			dataRender.push_back(createRenderMeshFromRawMesh(*it));
		}
		getActiveScene().addActor(*dynActor);

		// Creazione del Joint
		(*createDampedD6)(NULL, pos, dynActor, jointpos);
	}
}

// Funzione per generare l'attore per lo switch
void ArmadilloGame::PrepareSwitchActor(const PxTransform & pos, std::vector<int> MaterialID, std::vector<const char *> inObjFileNames,
	bool isRigid, const PxTransform & jointpos, float mass, const PxVec3 & tensor) {

	std::vector<RAWMesh> vecMesh;
	for (int i = 0; i < (int)inObjFileNames.size(); i++) {
		RAWMesh mesh;
		createRAWMeshFromObjMesh(inObjFileNames[i], pos, MaterialID[i], mesh);
		vecMesh.push_back(mesh);
		dataMesh.insert(std::make_pair(inObjFileNames[i], mesh));
		dataPos.push_back(pos);
	}

	if (!isRigid)
	{
		//Creazione dell'attore dinamico per la scena Physx
		PxRigidDynamic* dynActor = getPhysics().createRigidDynamic(PxTransform(pos));
		dynActor->setMass(5.1f);
		dynActor->setMassSpaceInertiaTensor(PxVec3(5.1f, 5.1f, 5.f));
		dynActor->setSolverIterationCounts(200, 200);
		dynActor->setLinearDamping(10.0f);
		dynActor->setAngularDamping(10.0f);

		for (std::vector<RAWMesh>::iterator it = vecMesh.begin(); it != vecMesh.end(); ++it) {
			PxConvexMesh* triMesh = PxToolkit::createConvexMesh(getPhysics(), getCooking(), it->mVerts, it->mNbVerts, PxConvexFlag::eCOMPUTE_CONVEX);
			if (!triMesh)
				fatalError("creating the triangle mesh failed");
			PxConvexMeshGeometry conGeom(triMesh);

			PxShape* shape = PxRigidActorExt::createExclusiveShape(*dynActor, conGeom, getDefaultMaterial());
			shape->setName("switch_key_model.obj");
			
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
		(*createDampedD6Switch)(NULL, pos, dynActor, jointpos);
	}
}

// Funzione import texture
void ArmadilloGame::ImportTexture() {
    // Texture Armadillo
	RAWTexture Texturedata;
	Texturedata.mName = "armadillo_texture.bmp";
	RenderTexture* Texture = createRenderTextureFromRawTexture(Texturedata);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 101, Texture));

	// Texture Floor
	RAWTexture Texturedata3;
	Texturedata3.mName = "floor_texture_XXL.bmp";
	RenderTexture* Texture3 = createRenderTextureFromRawTexture(Texturedata3);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 104, Texture3));

	// Texture Containers
	RAWTexture Texturedata4;
	Texturedata4.mName = "container_texture_xl.bmp";
	RenderTexture* Texture4 = createRenderTextureFromRawTexture(Texturedata4);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 105, Texture4));

	// Texture Labyrint
	RAWTexture Texturedata5;
	Texturedata5.mName = "labyrint_texture_XL.bmp";
	RenderTexture* Texture5 = createRenderTextureFromRawTexture(Texturedata5);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 106, Texture5));

	// Texture Buildings_1
	RAWTexture Texturedata6;
	Texturedata6.mName = "building1_texture_XL.bmp";
	RenderTexture* Texture6 = createRenderTextureFromRawTexture(Texturedata6);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 107, Texture6));
	// Texture Buildings_2
	RAWTexture Texturedata7;
	Texturedata7.mName = "building2_texture_XL.bmp";
	RenderTexture* Texture7 = createRenderTextureFromRawTexture(Texturedata7);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 108, Texture7));

	// Texture Crane
	RAWTexture Texturedata8;
	Texturedata8.mName = "gru_texture.bmp";
	RenderTexture* Texture8 = createRenderTextureFromRawTexture(Texturedata8);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 109, Texture8));

	// Texture Switch
	RAWTexture Texturedata9;
	Texturedata9.mName = "switch_texture_XL.bmp";
	RenderTexture* Texture9 = createRenderTextureFromRawTexture(Texturedata9);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 110, Texture9));

	// Texture Staircase
	RAWTexture Texturedata10;
	Texturedata10.mName = "staircase_texture_XL.bmp";
	RenderTexture* Texture10 = createRenderTextureFromRawTexture(Texturedata10);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 111, Texture10));

	// Texture Pipes
	RAWTexture Texturedata11;
	Texturedata11.mName = "pipe_texture.bmp";
	RenderTexture* Texture11 = createRenderTextureFromRawTexture(Texturedata11);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 112, Texture11));

	// Texture Key
	RAWTexture Texturedata12;
	Texturedata12.mName = "key_texture.bmp";
	RenderTexture* Texture12 = createRenderTextureFromRawTexture(Texturedata12);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 113, Texture12));

	// Texture Barriers
	RAWTexture Texturedata13;
	Texturedata13.mName = "barriers_texture.bmp";
	RenderTexture* Texture13 = createRenderTextureFromRawTexture(Texturedata13);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 114, Texture13));

	// Texture Bridge
	RAWTexture Texturedata14;
	Texturedata14.mName = "bridge_texture.bmp";
	RenderTexture* Texture14 = createRenderTextureFromRawTexture(Texturedata14);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 115, Texture14));

	//Texture Barrel
	RAWTexture Texturedata15;
	Texturedata15.mName = "barrel_texture.bmp";
	RenderTexture* Texture15 = createRenderTextureFromRawTexture(Texturedata15);
	mRenderMaterials.push_back(SAMPLE_NEW(RenderMaterial)(*getRenderer(), PxVec3(1.0f, 1.0f, 1.0f), 1.0f, false, 116, Texture15));
}