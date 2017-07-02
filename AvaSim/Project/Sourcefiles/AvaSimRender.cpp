#include <vector>

#include "PxPhysicsAPI.h"

#include "../Headerfiles/Camera.h"
#include "../Headerfiles/Render.h"

using namespace physx;

extern void initPhysics(bool interactive);
extern void stepPhysics(bool interactive);	
extern void cleanupPhysics(bool interactive);
extern void keyPress(unsigned char key, const PxTransform& camera);

namespace
{
Render::Camera*	sCamera;

void motionCallback(int x, int y)
{
	sCamera->handleMotion(x, y);
}

void mouseCallback(int button, int state, int x, int y)
{
	sCamera->handleMouse(button, state, x, y);
}

void keyboardCallback(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);

	if (!sCamera->handleKey(key, x, y))
		keyPress(key, sCamera->getTransform());
}

void idleCallback()
{
	glutPostRedisplay();
}

void renderCallback()
{
	stepPhysics(true);

	Render::startRender(sCamera->getEye(), sCamera->getDir());

	PxScene* scene;
	PxGetPhysics().getScenes(&scene,1);
	PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if(nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);

		Render::renderActors(&actors[0], (PxU32)actors.size(), true);
	}

	Render::finishRender();
}

void exitCallback(void)
{
	delete sCamera;
	cleanupPhysics(true);
}
}

void renderLoop()
{
	sCamera = new Render::Camera(PxVec3(-90.0f, 50.0f, 90.0f), PxVec3(0.65f,-0.2f,-0.7f));
	

	Render::setupDefaultWindow("Slap Avalanche Simulator");
	Render::setupDefaultRenderState();

	glutIdleFunc(idleCallback);
	glutDisplayFunc(renderCallback);
	glutKeyboardFunc(keyboardCallback);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(motionCallback);
	motionCallback(0,0);

	atexit(exitCallback);

	initPhysics(true);
	glutMainLoop();
}

