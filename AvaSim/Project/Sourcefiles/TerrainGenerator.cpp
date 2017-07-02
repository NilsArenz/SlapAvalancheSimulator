#include "PxPhysicsAPI.h"
#include "TerrainGenerator.h"
//#include "MeshData.h"
#include <iostream>

using namespace physx;

//not working at the moment
void physx::createTerain(PxCooking & cooking, PxPhysics & physics, PxMaterial & mat, PxScene & scene)
{
	PxTransform pos = PxTransform(PxVec3(0.0f, 5.0f, 0.0f));
	const PxReal heightScale = 0.01f;
	PxReal hfScale = 10.0f;

	PxU32 numRows = 5;
	PxU32 numCols = 5;

	//PxU32 numVerts = numRows*numCols;

	PxHeightFieldSample* hfSample = new PxHeightFieldSample[25];
	for (PxU32 i = 0; i < 25; i++) {
		hfSample[i].height = 20;
		hfSample[i].materialIndex0 = 2;
		hfSample[i].materialIndex1 = 3;
		}

	PxHeightFieldDesc hfDesc;
	hfDesc.nbColumns = numCols;
	hfDesc.nbRows = numRows;
	hfDesc.thickness = -10;
	hfDesc.convexEdgeThreshold = 3;
	hfDesc.samples.data = hfSample;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	PxRigidActor* hfActor = physics.createRigidStatic(pos);

	PxDefaultMemoryOutputStream buf;
	//PxConvexMeshCookingResult::Enum result;
	cooking.cookHeightField(hfDesc,buf);
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	//PxHeightField* hfMesh = physics.createHeightField(input);
	PxHeightField* heightField = cooking.createHeightField(hfDesc, physics.getPhysicsInsertionCallback());
	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, hfScale, hfScale);
	PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, mat);

	hfShape->setLocalPose(pos);
	scene.addActor(*hfActor);
}


void physx::createObject(PxCooking & cooking, PxPhysics & physics, PxMaterial & mat, PxScene & scene)
{
	//static const PxU32 avalancheControlStructure_Verts_count = 3;
	//static const PxU32 avalancheControlStructure_Tris_count = 3;

	PxVec3 avalancheControlStructure_Verts[] = {
		PxVec3(0.0f,1.0f,0.0f),
		PxVec3(1.0f,0.0f,0.0f),
		PxVec3(-1.0f,0.0f,0.0f),
	};
	PxU32 avalancheControlStructure_Tris[] = {
		1,
		2,
		3
	};

	PxTransform pos = PxTransform(PxVec3(0.0f, 20.0f, 0.0f));

	PxCookingParams params = cooking.getParams();
	params.buildGPUData = true;

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = 3;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = avalancheControlStructure_Verts;

	meshDesc.triangles.count = 3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = avalancheControlStructure_Tris;

	/////////////////
	std::cout << "Test" << std::endl;
	/////////////////

	meshDesc.flags = PxMeshFlags();

	PxRigidActor* triangleMeshActor = physics.createRigidStatic(pos);
	PxTriangleMeshGeometry geom;

	PxDefaultMemoryOutputStream writebuffer;
	PxTriangleMeshCookingResult::Enum result;
	cooking.cookTriangleMesh(meshDesc, writebuffer, &result);

	//cooking.cookTriangleMesh(meshDesc, writebuffer);	

	PxDefaultMemoryInputData input(writebuffer.getData(), writebuffer.getSize());
	physics.createTriangleMesh(input);
	PxTriangleMesh* triangleMeshGeo = physics.createTriangleMesh(input);
	//PxShape* triangleMeshShape = PxRigidActorExt::createExclusiveShape(*triangleMeshActor, PxTriangleMeshGeometry(triangleMeshGeo), mat);

	geom.triangleMesh = triangleMeshGeo;

	triangleMeshActor = physics.createRigidStatic(pos);

	triangleMeshActor->createShape(geom, mat);

	scene.addActor(*triangleMeshActor);
}


void physx::createStructure(PxCooking & cooking, PxPhysics & physics, PxMaterial & mat, PxScene & scene)
{

	PxRigidActor *hfActor;
	hfActor = physics.createRigidStatic(PxTransform(PxIdentity));
	hfActor->release();

	PxFilterData simulationFilterData;
	simulationFilterData.word0 = 1 << 0;

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

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.data = avalancheControlStructure_Verts;
	meshDesc.points.count = avalancheControlStructure_Verts_count;
	meshDesc.points.stride = sizeof(PxVec3);

	meshDesc.triangles.stride = sizeof(PxU32) * 3;
	meshDesc.triangles.count = avalancheControlStructure_Tris_count;
	meshDesc.triangles.data = avalancheControlStructure_Tris;

	meshDesc.flags = PxMeshFlags();	

	PxTriangleMeshGeometry geom;

	PxCookingParams params = cooking.getParams();	
	params.buildGPUData = true;

	cooking.setParams(params);

	PxDefaultMemoryOutputStream writeBuffer;
	bool status = cooking.cookTriangleMesh(meshDesc, writeBuffer);
	PX_ASSERT(status);
	if (!status)
		std::cout << "works" << std::endl;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* triangleMesh = physics.createTriangleMesh(readBuffer);

	geom.triangleMesh = triangleMesh;



	PxShape* meshShape = hfActor->createShape(geom, mat);

	//meshShape->setQueryFilterData(queryFilterData);
	meshShape->setSimulationFilterData(simulationFilterData);

	scene.addActor(*hfActor);

}


void physx::createScene(PxPhysics & physics, PxMaterial & mat, PxScene & scene)
{
	//variables
	PxReal angle = 35.0f;
	PxReal realangle = (angle * PxPi) / 180.0f;

	//ground Box
	PxTransform groundActorPos = PxTransform(PxVec3(0.0f, 30.0f, 0.0f), PxQuat(realangle, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* groundBox = physics.createRigidStatic(groundActorPos);
	groundBox->createShape(PxBoxGeometry(50,1,40),mat);
	scene.addActor(*groundBox);

	//Wall bottom
	PxTransform wallActorPos_bottom = PxTransform(PxVec3(-47.0f, 7.0f, 0.0f), PxQuat(realangle, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* groundWall_I = physics.createRigidStatic(wallActorPos_bottom);
	groundWall_I->createShape(PxBoxGeometry(1, 10, 40), mat);
	scene.addActor(*groundWall_I);

	//Wall side_right
	PxTransform wallActorPos_right = PxTransform(PxVec3(-5.0f, 35.0f, 40.0f), PxQuat(realangle, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* groundWall_II = physics.createRigidStatic(wallActorPos_right);
	groundWall_II->createShape(PxBoxGeometry(50,10, 1), mat);
	scene.addActor(*groundWall_II);

	//Wall side_left
	PxTransform wallActorPos_left = PxTransform(PxVec3(-5.0f, 35.0f, -40.0f), PxQuat(realangle, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* groundWall_III = physics.createRigidStatic(wallActorPos_left);
	groundWall_III->createShape(PxBoxGeometry(50, 10, 1), mat);
	scene.addActor(*groundWall_III);


}

void physx::createBorder(PxPhysics & physics, PxMaterial & mat, PxScene & scene)
{

	//50 50
	PxTransform boxActor_I = PxTransform(PxVec3(50.0f, 50.0f, 50.0f));
	PxRigidStatic* boxObject_I = physics.createRigidStatic(boxActor_I);
	boxObject_I->createShape(PxBoxGeometry(1, 50, 1), mat);
	scene.addActor(*boxObject_I);

	//-50 -50
	PxTransform boxActor_II = PxTransform(PxVec3(-50.0f, 50.0f, -50.0f));
	PxRigidStatic* boxObject_II = physics.createRigidStatic(boxActor_II);
	boxObject_II->createShape(PxBoxGeometry(1, 50, 1), mat);
	scene.addActor(*boxObject_II);

	//50 -50
	PxTransform boxActor_III = PxTransform(PxVec3(50.0f, 50.0f, -50.0f));
	PxRigidStatic* boxObject_III = physics.createRigidStatic(boxActor_III);
	boxObject_III->createShape(PxBoxGeometry(1, 50, 1), mat);
	scene.addActor(*boxObject_III);

	//-50 50
	PxTransform boxActor_IV = PxTransform(PxVec3(-50.0f, 50.0f, 50.0f));
	PxRigidStatic* boxObject_IV = physics.createRigidStatic(boxActor_IV);
	boxObject_IV->createShape(PxBoxGeometry(1, 50, 1), mat);
	scene.addActor(*boxObject_IV);

	//mid cube
	PxTransform boxActor_V = PxTransform(PxVec3(0.0f, 1.0f, 0.0f));
	PxRigidStatic* boxObject_V = physics.createRigidStatic(boxActor_V);
	boxObject_V->createShape(PxBoxGeometry(1, 1, 1), mat);
	scene.addActor(*boxObject_V);

	//mid plane
	PxTransform planeActor = PxTransform(PxVec3(0.0f, 0.0f, 0.0f));
	PxRigidStatic* planeObject = physics.createRigidStatic(planeActor);
	planeObject->createShape(PxBoxGeometry(50,1,50), mat);
	scene.addActor(*planeObject);
}
