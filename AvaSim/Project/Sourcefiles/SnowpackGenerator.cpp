#include "PxPhysicsAPI.h"
#include "SnowpackGenerator.h"
#include <iostream>
#include <cstdlib>

/*TODO:
	- better create Algorithm
		- initialising the grains like that: o o o o o
											  o o o o 
											 o o o o o 
		- odd/even height
	- mixture of capusles & sphears
*/
////////////////////////////////////////////////////////////////////////////////////////
/*DONE:

	- output after creation how much elements were spawned
	- DONE snowgrainsize DONE
	- better size
	- more than 6000 elements pls
	- random rotation in every sphear
*/

void physx::createSnowpack(PxVec3 size, PxVec3 pos, PxReal snowgrainsize, PxReal ang, PxMaterial * mat, PxScene * scene, PxPhysics * physics)
{
	//better exeptions comming soon
	if (!mat)
		std::cout << "Material Error" << std::endl;
	if(!scene)
		std::cout << "Scene Error" << std::endl;
	if(!physics)
		std::cout << "Physic Error" << std::endl;
	if (ang < 0 || ang > 70)
		std::cout << "It's recoment to use a angle between 0 - 60." << std::endl;

	//PxReal heighthelper = snowgrainsize + snowgrainsize/2;					//minimum distance between object: sqrt(3) * radius * cos(30°) = radius/2
	PxReal angle = (PxPi*ang) / 180;

	for (PxReal width = 0; width < size.x; width += (2 * snowgrainsize)) {	

		PxReal a = width * PxTan(angle);

		for (PxReal height = 0; height < size.y; height += 2 * snowgrainsize) {

			for (PxReal depth = 0; depth < size.z; depth += (2 * snowgrainsize)) {

				//create randow angle
				PxReal r_angle = (PxPi*PxReal(rand() % 360)) / 180;

				//even height
				if (PxU32(height) % 2 == 0) {

					PxTransform snowpack_pos = PxTransform(PxVec3(width + pos.x, (height + a + pos.y), depth + pos.z),
						PxQuat(r_angle, PxVec3(1.0f,0.0f,0.0f)));
				//	PxRigidStatic* snowgrain = physics->createRigidStatic(snowpack_pos);
					PxRigidDynamic* snowgrain = physics->createRigidDynamic(snowpack_pos);
					snowgrain->createShape(PxSphereGeometry(snowgrainsize), *mat);
					//snowgrain->createShape(PxBoxGeometry(snowgrainsize, snowgrainsize, snowgrainsize), *mat);
					scene->addActor(*snowgrain);
				}
				//odd height
				else
				{
					PxTransform snowpack_pos = PxTransform(PxVec3(width + pos.x, (height + a + pos.y), depth + pos.z), 
						PxQuat(r_angle, PxVec3(1.0f, 0.0f, 0.0f)));
				//	PxRigidStatic* snowgrain = physics->createRigidStatic(snowpack_pos);
					PxRigidDynamic* snowgrain = physics->createRigidDynamic(snowpack_pos);
					//snowgrain->createShape(PxBoxGeometry(snowgrainsize, snowgrainsize, snowgrainsize), *mat);
					snowgrain->createShape(PxSphereGeometry(snowgrainsize), *mat);
					scene->addActor(*snowgrain);
				}					
			}
		}
	}
	//total spawned element number output
	PxReal totalElementNumber = ((size.x / (2*snowgrainsize)) * (size.z / (2 * snowgrainsize))) * (size.y / (2 * snowgrainsize));
	std::cout <<"successfully created: "<< totalElementNumber <<" objects"<< std::endl;
}
