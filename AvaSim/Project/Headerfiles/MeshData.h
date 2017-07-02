#include "PxPhysicsAPI.h"

//collection of vertex Data used for triangle Mesh structures

namespace physx {


		PxU32 avalancheControlStructure_Verts_count = 3;
		PxU32 avalancheControlStructure_Tris_count = 3;

		static const PxVec3 avalancheControlStructure_Verts[] = {
			PxVec3(0,1,0),
			PxVec3(1,0,0),
			PxVec3(-1,0,0),
		};
		static const PxU32 avalancheControlStructure_Tris[] = {
			1,
			2,
			3
		};	

}