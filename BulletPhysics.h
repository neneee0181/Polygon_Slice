#pragma once


#include "Model.h"
#include "CustomContactResultCallback.h"

#include"include/btBulletCollisionCommon.h"
#include"include/btBulletDynamicsCommon.h"

// Bullet Physics ���� ������
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

// Bullet Physics �ʱ�ȭ �Լ�
void initPhysics() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

// �𵨿� ���� �浹 ��ü�� ��ü ���� �� ���� ���迡 �߰�
void addModelToPhysicsWorld(Model& model) {
    btCollisionShape* shape = new btBoxShape(btVector3(10, 10, 10));

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

// ��� �𵨿� ���� ���� ���� �浹 ��ü �ʱ�ȭ
void initializeModelsWithPhysics(std::vector<Model>& models) {
    for (auto& model : models) {
        addModelToPhysicsWorld(model);
    }
}

void updatePhysics(std::vector<Model>& models, Model& model_basket) {
    CustomContactResultCallback resultCallback;

    // �� �𵨿� ���� �ִϸ��̼ǵ� ��ġ�� Bullet Physics ��ġ ����ȭ
    for (auto& model : models) {
        if (model.rigidBody) {
            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(btVector3(model.modelMatrix[3].x, model.modelMatrix[3].y, model.modelMatrix[3].z));
            model.rigidBody->setWorldTransform(transform);
            model.rigidBody->getMotionState()->setWorldTransform(transform);
        }
    }

    // �ٱ��� �� ��ġ ����ȭ
    if (model_basket.rigidBody) {
        btTransform basketTransform;
        basketTransform.setIdentity();
        basketTransform.setOrigin(btVector3(model_basket.modelMatrix[3].x, model_basket.modelMatrix[3].y, model_basket.modelMatrix[3].z));
        model_basket.rigidBody->setWorldTransform(basketTransform);
        model_basket.rigidBody->getMotionState()->setWorldTransform(basketTransform);
    }

    // �𵨵� ������ �ٱ��� �� ������ �浹 �˻� ����
    for (auto& model : models) {
        if (model.rigidBody && model_basket.rigidBody) {
            dynamicsWorld->contactPairTest(model.rigidBody, model_basket.rigidBody, resultCallback);
        }
    }

    // �𵨵鳢���� �浹 �˻�
    //for (size_t i = 0; i < models.size(); ++i) {
    //    for (size_t j = i + 1; j < models.size(); ++j) {  // (i+1)���� �����Ͽ� �ߺ� �˻� ����
    //        if (models[i].rigidBody && models[j].rigidBody) {
    //            dynamicsWorld->contactPairTest(models[i].rigidBody, models[j].rigidBody, resultCallback);
    //        }
    //    }
    //}
}

void cleanupPhysics() {
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}