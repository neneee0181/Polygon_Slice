#pragma once


#include "Model.h"

#include"include/btBulletCollisionCommon.h"
#include"include/btBulletDynamicsCommon.h"

// Bullet Physics 관련 변수들
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

// Bullet Physics 초기화 함수
void initPhysics() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

// 모델에 대한 충돌 객체와 강체 생성 및 물리 세계에 추가
void addModelToPhysicsWorld(Model& model) {
    btCollisionShape* shape = new btBoxShape(btVector3(1, 1, 1));

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(model.modelMatrix[3].x, model.modelMatrix[3].y, model.modelMatrix[3].z));

    btScalar mass = 1.0f;
    bool isDynamic = (mass != 0.0f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
    model.rigidBody = body;
}

// 모든 모델에 대한 물리 세계 충돌 객체 초기화
void initializeModelsWithPhysics(std::vector<Model>& models) {
    for (auto& model : models) {
        addModelToPhysicsWorld(model);
    }
}

// 물리 세계 업데이트 함수
void updatePhysics(float deltaTime, std::vector<Model>& models) {
    dynamicsWorld->stepSimulation(deltaTime, 10);

    for (auto& model : models) {
        btTransform trans;
        model.rigidBody->getMotionState()->getWorldTransform(trans);

        model.modelMatrix[3].x = trans.getOrigin().getX();
        model.modelMatrix[3].y = trans.getOrigin().getY();
        model.modelMatrix[3].z = trans.getOrigin().getZ();
    }
}