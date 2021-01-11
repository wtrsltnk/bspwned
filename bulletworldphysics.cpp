/* 
 * File:   bulletworldphysics.cpp
 * Author: wouter
 * 
 * Created on May 28, 2009, 5:09 PM
 */

#include "bulletworldphysics.h"

BulletWorldPhysics::BulletWorldPhysics()
{
    btVector3 worldAabbMin(-10000,-10000,-10000);
    btVector3 worldAabbMax(10000,10000,10000);
    int maxProxies = 1024;

    broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);

    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    solver = new btSequentialImpulseConstraintSolver();

    // Initialize dynamic word
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0,-10,0));
}

BulletWorldPhysics::~BulletWorldPhysics()
{
    delete dynamicsWorld;
    delete solver;
    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
}

void BulletWorldPhysics::setLeafSpacesCount(int count)
{
}

void BulletWorldPhysics::addToLeafSpace(int leafSpaceIndex, float* vertexData, int vertexCount, int* indexData, int indexCount)
{
}

void BulletWorldPhysics::createPlane(const float plane[4])
{
    btVector3 position(plane[0], plane[1], plane[2]);
    btStaticPlaneShape* shape = new btStaticPlaneShape(position, 1);

    // Floor motion state initialization
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,shape,btVector3(0,0,0));

    btRigidBody* rigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(rigidBody);
}

void* BulletWorldPhysics::createSphere(float radius, float mass, const float position[3], const float force[3], const float ang[3])
{
    btVector3 shapePosition(position[0], position[1], position[2]);
    btSphereShape* shape = new btSphereShape(radius);
    btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), shapePosition));
    btVector3 fallInertia(0, 0, 0);
    shape->calculateLocalInertia(mass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, shape, fallInertia);
    btRigidBody* rigidBody = new btRigidBody(fallRigidBodyCI);
    dynamicsWorld->addRigidBody(rigidBody);
    return rigidBody;
}

void BulletWorldPhysics::getGeomPosition(void* geom, float position[3])
{
    btRigidBody* rigidBody = (btRigidBody*)geom;
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);

    position[0] = trans.getOrigin().getX();
    position[1] = trans.getOrigin().getY();
    position[2] = trans.getOrigin().getZ();
}

void BulletWorldPhysics::step(double time)
{
    dynamicsWorld->stepSimulation(1/60.f, 10);
}
