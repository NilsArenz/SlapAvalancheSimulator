#include "PxPhysicsAPI.h"

//create a mountain Terain

namespace physx {


	//creating a Terain
	void createTerain(PxCooking& cooking, PxPhysics& physics, PxMaterial& mat, PxScene& scene);
	
	//creating a structure element using a Triangular mesh
	void createObject(PxCooking& cooking, PxPhysics& physics, PxMaterial& mat, PxScene& scene);

	//try nr.3
	void createStructure(PxCooking& cooking, PxPhysics& physics, PxMaterial& mat, PxScene& scene);

	//create a basic scene
	void createScene(PxPhysics& physics, PxMaterial& mat, PxScene& scene);

	//will create border depending on the 
	void createBorder(PxPhysics& physics, PxMaterial& mat, PxScene& scene);
}