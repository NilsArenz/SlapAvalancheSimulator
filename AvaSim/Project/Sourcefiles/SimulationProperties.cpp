#include "..\Headerfiles\SimulationProperties.h"

void physx::setupMaterialLibary(PxMaterial * snow)
{
	/*
	the main use of phMaterials is to explain the collision between object

	Material explanation:

	static friction = ger(statische Reibung)
	explains the friction of a rigid body if the object is not moving
	value from 0 to infinity

	dynamic friction = ger(dynamische Reibung)
	explains the friction of a rigid body if the object is moving
	value between 0 and 1

	restitution = ger(dynamische Stoß/Rückgabe)
	explains the bounciness of a rigid body
	value between 0 and 1
	*/
	snow->getDynamicFriction();
}
