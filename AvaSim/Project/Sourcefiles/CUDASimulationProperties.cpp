#include "..\Headerfiles\CUDASimulationProperties.h"

void physx::setupCUDASimulationProperties(PxCudaContextManager * gCudaContextManager, PxFoundation * foundation, PxScene * scene, PxPhysics * physic, PxDefaultCpuDispatcher * gDispatcher)
{
	//enable OpenGL
	PxCudaContextManagerDesc cudaContextManagerDesc;
	cudaContextManagerDesc.interopMode = PxCudaInteropMode::OGL_INTEROP;

	//create CUDA
	gCudaContextManager = PxCreateCudaContextManager(*foundation, cudaContextManagerDesc);
	if (gCudaContextManager)
	{
		if (!gCudaContextManager->contextIsValid())
		{
			gCudaContextManager->release();
			gCudaContextManager = NULL;
		}
	}

	//setup Scene Parameters
	PxSceneDesc sceneDesc(physic->getTolerancesScale());					//sould maybe put into "SimulationProperties"
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);							//sould maybe put into "SimulationProperties"
	gDispatcher = PxDefaultCpuDispatcherCreate(8);							//only if CUDA is not active
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;				//own Shader files comming

																			//setup GPU Parameters
	sceneDesc.gpuDynamicsConfig.constraintBufferCapacity *= 2;				//doku comming
	sceneDesc.gpuDynamicsConfig.contactBufferCapacity *= 2;					//doku comming
	sceneDesc.gpuDynamicsConfig.tempBufferCapacity *= 2;					//doku comming
	sceneDesc.gpuDynamicsConfig.contactStreamSize *= 2;						//doku comming
	sceneDesc.gpuDynamicsConfig.patchStreamSize *= 2;						//doku comming
	sceneDesc.gpuDynamicsConfig.forceStreamCapacity *= 2;					//doku comming

	sceneDesc.gpuDispatcher = gCudaContextManager->getGpuDispatcher();		//activat CUDA on Scene

	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;					//Enable GPU dynamics - without this enabled, simulation (contact gen and solver) will run on the CPU.
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;							//Enable PCM. PCM NP is supported on GPU. Legacy contact gen will fall back to CPU
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;					//Improve solver stability by enabling post-stabilization.

	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;						//Enable GPU broad phase. Without this set, broad phase will run on the CPU.
	sceneDesc.gpuMaxNumPartitions = 16;										//Defines the maximum number of partitions used by the solver. Only power-of-2 values are valid. 
																			//A value of 8 generally gives best balance between performance and stability.
																				
	scene = physic->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, false);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, false);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, false);
	}
}
