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
#include "ArmadilloGame.h"

using namespace SampleRenderer;
using namespace SampleFramework;

REGISTER_SAMPLE(ArmadilloGame, "Start the game!")

std::vector<PxVec3> gContactPositions;
std::vector<PxVec3> gContactImpulses;

///////////////////////////////////////////////////////////////////////////////
//Game Logic Flag

float step = 0.05f;
bool flagMove = true;
float count = 0.0f;
bool flagKinematic = false;
bool leverPulled = false;
bool keyReached = false;
bool gameEnded = false;


///////////////////////////////////////////////////////////////////////////////

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    PX_UNUSED(attributes0);
    PX_UNUSED(attributes1);
    PX_UNUSED(filterData0);
    PX_UNUSED(filterData1);
    PX_UNUSED(constantBlockSize);
    PX_UNUSED(constantBlock);

    // all initial and persisting reports for everything, with per-point data
    pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
        | PxPairFlag::eNOTIFY_TOUCH_CCD
        | PxPairFlag::eNOTIFY_TOUCH_FOUND
        | PxPairFlag::eNOTIFY_TOUCH_PERSISTS
        | PxPairFlag::eNOTIFY_CONTACT_POINTS;
    return PxFilterFlag::eDEFAULT;
}

class ContactReportCallback : public PxSimulationEventCallback
{
    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
    void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
    void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
    void onTrigger(PxTriggerPair* pairs, PxU32 count) { PX_UNUSED(pairs); PX_UNUSED(count); }
    void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) {}
    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
    {
        PX_UNUSED((pairHeader));
        std::vector<PxContactPairPoint> contactPoints;

        for (PxU32 i = 0; i < nbPairs; i++)
        {
            PxU32 contactCount = pairs[i].contactCount;

            std::string SwitchleverName =   "switch_key_model.obj";
            std::string BuildingName =      "building2_model.obj";
            std::string leverName =         "switch_model.obj";
            std::string KeyName =           "key_model.obj";

            if (contactCount)
            {
                // Per ogni coppia di shapehits rilevate dalla callback, si controlla se gli le shapes corrispondono ad oggetti di interesse per il gioco
                // In questo caso la leva che tocca il pavimento ...
                if ((pairs[i].shapes[0] != NULL && !SwitchleverName.compare(pairs[i].shapes[0]->getName())) &&
                    (pairs[i].shapes[1] != NULL && !BuildingName.compare(pairs[i].shapes[1]->getName())))
                    leverPulled = true;
                else if (pairs[i].shapes[1]->getName() != NULL) {
                    // ...o che il player abbia toccato la chiave
                    if (!KeyName.compare(pairs[i].shapes[1]->getName()))
                        keyReached = true;
                }

                contactPoints.resize(contactCount);
                pairs[i].extractContacts(&contactPoints[0], contactCount);

                for (PxU32 j = 0; j < contactCount; j++)
                {
                    gContactPositions.push_back(contactPoints[j].position);
                    gContactImpulses.push_back(contactPoints[j].impulse);
                }
            }
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
ContactReportCallback gContactReportCallback;

// Funzione richiamata quando la leva tocca il terreno
void ArmadilloGame::onLeverPulled()
{
    shdfnd::printFormatted("Lever contact");
    if (!flagKinematic) {
        mBridgeActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
        mBridgeActor->setAngularVelocity(PxVec3(0.f, 0.f, 0.f), false);
        flagKinematic = true;
    }
}

// Funzione richiamata quando il player prende la chiave
void ArmadilloGame::onKeyReached()
{
    if (!gameEnded) {
        shdfnd::printFormatted("Key contact");
        std::string KeyName = "key_model.obj";

        PxSceneWriteLock scopedLock(*mScene);
        for (int j = 0; j < (int)mRenderActors.size(); j++) {
            if (mRenderActors[j]->getPhysicsShape()->getName() != NULL) {
                if (!KeyName.compare(mRenderActors[j]->getPhysicsShape()->getName())) {
                    mRenderActors.erase(mRenderActors.begin() + j);
                }
            }
        }
        for (int i = 0; i < (int)dataRender.size(); i++) {
            if (dataShape[i]->getName() != NULL)
                if (!KeyName.compare(dataShape[i]->getName())) {
                    mScene->removeActor(*mFloatingActor);
                    dataShape.erase(dataShape.begin() + i);
                    dataRender.erase(dataRender.begin() + i);
                }
        }
        gameEnded = true;
        mArmadilloCamera->resetStartGameFlag();

        audioManager->PlaySFX();
    }
}

void ArmadilloGame::customizeSceneDesc(PxSceneDesc& sceneDesc)
{
    sceneDesc.filterShader = contactReportFilterShader;
    sceneDesc.simulationEventCallback = &gContactReportCallback;
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;
    sceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_PAIRS;
    sceneDesc.ccdMaxPasses = 1;
}

ArmadilloGame::ArmadilloGame(PhysXSampleApplication& app) : PhysXSample(app)
{
    mControllerInitialPosition = PxExtendedVec3(0, 0, 10);
   
    mControllerActor =  NULL;
    mFloatingActor =    NULL;
    mBridgeActor =      NULL;
    
    count = 0.0f;
    flagKinematic = false;
    leverPulled =   false;
    isGameStarted = false;
    keyReached =    false;
    gameEnded =     false;
    mCreateGroundPlane = false;

    audioManager = new AudioManager();
}

ArmadilloGame::~ArmadilloGame()
{
}


void ArmadilloGame::onSubstep(float dtime) {
    PxSceneWriteLock scopedLock(*mScene);

    // Gestione input utente e camera
    mArmadilloCamera->handleInput(getCamera(), gContactPositions.size() > 0 ? true : false);
    
    // Pulizia contatti rilevato dalla callback
    gContactPositions.clear();
    gContactImpulses.clear();

    // Controllo stati di gioco ed eventi
    if (leverPulled && !flagKinematic) {
        onLeverPulled();
    }
    if (keyReached && !gameEnded)
        onKeyReached();

    // Movimento "fluttiante" chiave 
    if (mFloatingActor) {
        PxTransform currentpose = mFloatingActor->getGlobalPose();
        float tmpY = currentpose.p.y;

        if (currentpose.p.y >= 0.0f && currentpose.p.y <= 0.3f) flagMove = true;
        else if (currentpose.p.y >= 2.3f)                       flagMove = false;
        
        if (flagMove) tmpY = tmpY + step;
        else          tmpY = tmpY - step;

        currentpose.p.y = tmpY;

        mFloatingActor->setGlobalPose(currentpose);
    }

    // Gestione del menù e applicazione forze alla palla 
    if (mControllerActor && mArmadilloCamera->getStartGameFlag() && !getApplication().mMenuExpand) {
        if (mControllerActor->getGlobalPose().p.y < -5.f) {
            mControllerActor->setGlobalPose(PxTransform(-10, 3, -10));
        }
        else
            mControllerActor->addForce(mArmadilloCamera->getForce());
    }
    // Reset forze applicate per evitare movimenti spuri
    mArmadilloCamera->resetForce();
}

void ArmadilloGame::onTickPreRender(float dtime)
{
    if (dataShape.size() != dataRender.size())
        fatalError("Shape e posizioni relative non coincidono. Controllare la presenza di tutti i render object e shape");

    for (int i = 0; i < (int)dataRender.size(); i++) {
        dataShape[i]->userData = dataRender[i];
        {
            PxSceneReadLock scopedLock(*mScene);

            dataRender[i]->setPhysicsShape(dataShape[i], dataShape[i]->getActor());
            dataRender[i]->setEnableCameraCull(true);

            PxTriangleMeshGeometry geometry;
            dataShape[i]->getTriangleMeshGeometry(geometry);
            dataRender[i]->setMeshScale(geometry.scale);
        }
    }

    // Metto il lock sulla risorsa per ottenere la velocità lineare dell'attore in movimento
    PxSceneWriteLock scopedLock(*mScene);

    // Aggiornamento posizione della camera in base alla posizione della palla
    if (mControllerActor)
        mArmadilloCamera->updateFollowingMode(getCamera(), dtime, mControllerActor->getGlobalPose().p);

    PhysXSample::onTickPreRender(dtime);
}

void ArmadilloGame::onTickPostRender(float dtime)
{
    // Gestione musiche
    if (getApplication().mMenuExpand)   audioManager->StopMusic();
    else                                audioManager->PlayMusic();

    // Gestione GUI
    if (!mArmadilloCamera->getStartGameFlag() && !getApplication().mMenuExpand && !gameEnded) {
        printIntroMessage();
    }
    else if (!mArmadilloCamera->getStartGameFlag() && !getApplication().mMenuExpand && keyReached)
        printConclusionMessage();
    else if (gameEnded && mArmadilloCamera->getStartGameFlag())
        getApplication().mMenuExpand = true;
    
    PhysXSample::onTickPostRender(dtime);
}


void ArmadilloGame::onInit()
{
    // Inizializzazione scena
    PhysXSample::onInit();

    // Acquisizione lock
    PxSceneWriteLock scopedLock(*mScene);

    getRenderer()->setAmbientColor(RendererColor(100, 100, 100));
    getActiveScene().setGravity(PxVec3(0.0f, -9.81f, 0.0f));
    
    // Importazione texture e mesh
    ImportTexture();
    BuildScene();

    // Inizializzazione controllore telecamera
    mControllerManager = PxCreateControllerManager(getActiveScene());
    mArmadilloCamera = SAMPLE_NEW(ArmadilloCameraController)();
    setCameraController(mArmadilloCamera);
    mArmadilloCamera->init(PxTransform(PxIdentity));
    mArmadilloCamera->setFollowingMode(true);
    mArmadilloCamera->setMouseSensitivity(0.5f);
    mArmadilloCamera->setMouseLookOnMouseButton(false);

    // Importazione skybox
    importRAWFile("sky_mission_race1.raw", 2.0f);
}

// Metodo per liberare il controllerManager e la camera
void ArmadilloGame::onShutdown()
{
    {
        PxSceneWriteLock scopedLock(*mScene);
        DELETESINGLE(mArmadilloCamera);

        mControllerManager->release();
    }
    PhysXSample::onShutdown();
}


void ArmadilloGame::collectInputEvents(std::vector<const SampleFramework::InputEvent*>& inputEvents)
{
    PhysXSample::collectInputEvents(inputEvents);
    getApplication().getPlatform()->getSampleUserInput()->unregisterInputEvent(CAMERA_SPEED_INCREASE);
    getApplication().getPlatform()->getSampleUserInput()->unregisterInputEvent(CAMERA_SPEED_DECREASE);
}
