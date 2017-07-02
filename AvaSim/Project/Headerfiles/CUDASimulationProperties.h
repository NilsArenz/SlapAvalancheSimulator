#include "PxPhysicsAPI.h"

namespace physx {

	//function to set up all the CUDA Simulation Properties
	void setupCUDASimulationProperties(PxCudaContextManager * gCudaContextManager, PxFoundation * foundation, PxScene * scene, 
		PxPhysics * physic, PxDefaultCpuDispatcher * gDispatcher);

}