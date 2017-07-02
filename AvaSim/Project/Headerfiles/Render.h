#include "PxPhysicsAPI.h"
#include "foundation/PxPreprocessor.h"

#include <windows.h>
#pragma warning(disable: 4505)
#include <glut.h>


namespace Render
{
void setupDefaultWindow(const char* name);
void setupDefaultRenderState();

void startRender(const physx::PxVec3& cameraEye, const physx::PxVec3& cameraDir, physx::PxReal nearClip = 1.f, physx::PxReal farClip = 10000.f);
void renderActors(physx::PxRigidActor** actors, const physx::PxU32 numActors, bool shadows = true, const physx::PxVec3 & color = physx::PxVec3(0.9f, 0.9f, 0.9f));
void finishRender();
}

#define MAX_NUM_ACTOR_SHAPES 128
