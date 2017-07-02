#include <ctype.h>
#include <iostream>

#include "PxPhysicsAPI.h"
#include "SnowpackGenerator.h"
#include "TerrainGenerator.h"
#include "CUDASimulationProperties.h"
#include "SimulationProperties.h"

#define PVD_HOST "127.0.0.1"

using namespace physx;


PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;
PxCooking*				gCooking	= NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxGpuDispatcher*		gGPUDispatcher = NULL;

PxScene*				gScene		= NULL; 

PxMaterial*				snowMaterial	= NULL;
PxMaterial*				groundMaterial	= NULL; 
PxMaterial*				objectMaterial = NULL;


PxPvd*                  gPvd = NULL;

PxCudaContextManager* gCudaContextManager = NULL;


PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity=PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *snowMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}


void initPhysics(bool /*interactive*/)
{
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::ePROFILE);

	//will be put to Simulation properties soon
	PxTolerancesScale scale;
	scale.length = 11;																		//human sized simulation = 1; centimeter simulation = 100; 1-10 = very fluid like
	scale.mass = 100;																			//length*length*length; mass in kilogramm = 1000
	scale.speed = 9.81;																		//default 10 -> gravity

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(scale), true, gPvd);	

	PxCookingParams CookingPar(gPhysics->getTolerancesScale());
	CookingPar.buildGPUData = true;
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, CookingPar);

	PxCudaContextManagerDesc cudaContextManagerDesc;

	cudaContextManagerDesc.interopMode = PxCudaInteropMode::OGL_INTEROP;					//Choose interop mode. As the snippets use OGL, we select OGL_INTEROP		

	gCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc);	//Create the CUDA context manager, required for GRB to dispatch CUDA kernels.
	if( gCudaContextManager )
	{
		if( !gCudaContextManager->contextIsValid() )
		{
			gCudaContextManager->release();
			gCudaContextManager = NULL;
		}
	}	
						
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());					//will be put to Simulation properties soon
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);							//will be put to Simulation properties soon
	
	gDispatcher = PxDefaultCpuDispatcherCreate(8);							//create a CPU dispatcher using 4 worther threads
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.gpuMaxNumPartitions = 32;										//Defines the maximum number of partitions used by the solver. Only power-of-2 values are valid. 
	sceneDesc.gpuDispatcher = gGPUDispatcher;
	sceneDesc.gpuDispatcher = gCudaContextManager->getGpuDispatcher();

	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;

	sceneDesc.gpuDynamicsConfig.constraintBufferCapacity *= 3;
	sceneDesc.gpuDynamicsConfig.contactBufferCapacity *= 3;	
	sceneDesc.gpuDynamicsConfig.tempBufferCapacity *= 3;		
	sceneDesc.gpuDynamicsConfig.contactStreamSize *= 3;		
	sceneDesc.gpuDynamicsConfig.patchStreamSize *= 3;			
	sceneDesc.gpuDynamicsConfig.forceStreamCapacity *= 3;


	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;					//Enable GPU dynamics - without this enabled, simulation (contact gen and solver) will run on the CPU.
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;							//Enable PCM. PCM NP is supported on GPU. Legacy contact gen will fall back to CPU
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;					//Improve solver stability by enabling post-stabilization.

	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;						//Enable GPU broad phase. Without this set, broad phase will run on the CPU.								
																			//A value of 8 generally gives best balance between performance and stability.

	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, false);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, false);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, false);
	}	
		
	snowMaterial = gPhysics->createMaterial(20.0f, 0.3f, 0.05f);
	groundMaterial = gPhysics->createMaterial(20.0f, 0.5f, 0.05f);
	//objectMaterial = gPhysics->createMaterial(1.0f, 0.5f, 0.1f);

	createBorder(*gPhysics, *groundMaterial, *gScene);
	//createSnowpack(PxVec3(30.0f,30.0f,30.0f),PxVec3(0.0f,10.0f,0.0f), 1.0f, 20.0f, snowMaterial, gScene, gPhysics);

	createSnowpack(PxVec3(50.0f, 10.0f, 30.0f), PxVec3(-10.0f, 25.5f, -15.0f), 0.8f, 35.0f, snowMaterial, gScene, gPhysics);
	createScene(*gPhysics, *groundMaterial, *gScene);
	}

void stepPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	gScene->simulate(1.0f/60.0f);
	gScene->fetchResults(true);
}
	
void cleanupPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	if (gScene)
	{
		gScene->release();
		gScene = NULL;
	}
	if (gDispatcher)
	{
		gDispatcher->release();
		gDispatcher = NULL;
	}
	
	if (gPhysics)
	{
		gPhysics->release();
		gPhysics = NULL;

	}
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();
		transport->release();
	}

	if (gCudaContextManager)
	{
		gCudaContextManager->release();
		gCudaContextManager = NULL;
	}

	if (gFoundation)
	{
		gFoundation->release();
		gFoundation = NULL;
	}	
}

void keyPress(unsigned char key, const PxTransform& camera)
{
	switch (toupper(key))
	{	
	case ' ':	createDynamic(camera, PxSphereGeometry(3.0f), camera.rotate(PxVec3(0, 0, -1)) * 200);	break;
	}
}

int doRender(int, const char*const*)
{

	extern void renderLoop();
	renderLoop();

	return 0;
}
