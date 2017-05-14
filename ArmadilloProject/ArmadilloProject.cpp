#include <ctype.h>
#include <vector>

#include "PxPhysicsAPI.h"
#include "glut.h"

#include "PVD.h" //Visual debugger
#include "Camera.h"
#include "Renderer.h"

using namespace physx;


extern void initPhysics(bool interactive);
extern void stepPhysics(bool interactive);
extern void cleanupPhysics(bool interactive);
extern void keyPress(const char key, const PxTransform& camera);


namespace
{
	ArmadilloProject::Camera*	sCamera;

	void motionCallback(int x, int y)
	{
		sCamera->handleMotion(x, y);
	}

	void keyboardCallback(unsigned char key, int x, int y)
	{
		if (key == 27)
			exit(0);

		if (!sCamera->handleKey(key, x, y))
			keyPress(key, sCamera->getTransform());
	}

	void mouseCallback(int button, int state, int x, int y)
	{
		sCamera->handleMouse(button, state, x, y);
	}

	void idleCallback()
	{
		glutPostRedisplay();
	}

	void renderCallback()
	{
		stepPhysics(true);

		ArmadilloProject::startRender(sCamera->getEye(), sCamera->getDir());

		PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);
		PxU32 nbActors = scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC);
		if (nbActors)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);
			ArmadilloProject::renderActors(&actors[0], (PxU32)actors.size(), true);
		}

		ArmadilloProject::finishRender();
	}

	void exitCallback(void)
	{
		delete sCamera;
		cleanupPhysics(true);
	}
}

void renderLoop()
{
	sCamera = new ArmadilloProject::Camera(PxVec3(50.0f, 50.0f, 50.0f), PxVec3(-0.6f, -0.2f, -0.7f));

	ArmadilloProject::setupDefaultWindow("PhysX ArmadilloProject");
	ArmadilloProject::setupDefaultRenderState();

	glutIdleFunc(idleCallback);
	glutDisplayFunc(renderCallback);
	glutKeyboardFunc(keyboardCallback);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(motionCallback);
	motionCallback(0, 0);

	atexit(exitCallback);

	initPhysics(true);
	glutMainLoop();
}

int ArmadilloMain(int, const char*const*)
{
	extern void renderLoop();
	renderLoop();
	return 0;
}
