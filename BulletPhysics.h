#pragma once

#include <limits>  // std::numeric_limits ����� ���� ��� �߰�

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

// Model�� AABB�� ����Ͽ� ũ�⸦ ��ȯ�ϴ� �Լ�
glm::vec3 calculateModelSize(const Model& model) {
    // �ʱ� �ּ�, �ִ밪�� ����
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    float maxZ = std::numeric_limits<float>::lowest();

    // ��� ������ ��ȸ�ϸ� �ּ�, �ִ밪 ������Ʈ
    for (const Vertex& vertex : model.vertices) {
        if (vertex.x < minX) minX = vertex.x;
        if (vertex.y < minY) minY = vertex.y;
        if (vertex.z < minZ) minZ = vertex.z;
        if (vertex.x > maxX) maxX = vertex.x;
        if (vertex.y > maxY) maxY = vertex.y;
        if (vertex.z > maxZ) maxZ = vertex.z;
    }

    // ��, ����, ���̸� ���
    float width = maxX - minX;
    float height = maxY - minY;
    float depth = maxZ - minZ;

    return glm::vec3(width, height, depth);
}

// �𵨿� ���� �浹 ��ü�� ��ü ���� �� ���� ���迡 �߰�
void addModelToPhysicsWorld(Model& model) {
    // ���� ũ�⸦ ���
    glm::vec3 size = calculateModelSize(model);

    btCollisionShape* shape = nullptr;

    // ���� �̸��� ���� �浹 ��� ����
    if (model.name == "box") {
        // Box ������ �浹 ��� ����
        shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
    else if (model.name == "sphere") {
        // Sphere ������ �浹 ��� ���� (���� �������� x, y, z �� �ּҰ��� �������� ����)
        float radius = std::min({ size.x, size.y, size.z }) * 0.5f;
        shape = new btSphereShape(radius);
    }
    else if (model.name == "cylinder") {
        // Cylinder ������ �浹 ��� ���� (x�� z�� ��հ��� ����������, y�� ���̷� ���)
        shape = new btCylinderShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
    else {
        // �⺻������ Box ���� ��� (���� ó���� ����)
        shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }

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

    // �� �𵨿� ���� �ִϸ��̼ǵ� ��ġ�� ȸ�� ���¸� Bullet Physics�� ����ȭ
    for (auto& model : models) {
        if (model.rigidBody) {
            btTransform transform;
            transform.setFromOpenGLMatrix(glm::value_ptr(model.modelMatrix)); // modelMatrix�� ȸ�� ����
            model.rigidBody->setWorldTransform(transform);
            model.rigidBody->getMotionState()->setWorldTransform(transform);
        }
    }

    // �ٱ��� �� ��ġ �� ȸ�� ����ȭ
    if (model_basket.rigidBody) {
        btTransform basketTransform;
        basketTransform.setFromOpenGLMatrix(glm::value_ptr(model_basket.modelMatrix)); // modelMatrix�� ȸ�� ����
        model_basket.rigidBody->setWorldTransform(basketTransform);
        model_basket.rigidBody->getMotionState()->setWorldTransform(basketTransform);
    }

    // �𵨵� ������ �ٱ��� �� ������ �浹 �˻� ����
    for (auto& model : models) {
        if (model.rigidBody && model_basket.rigidBody) {
            dynamicsWorld->contactPairTest(model.rigidBody, model_basket.rigidBody, resultCallback);
        }
    }

    // �𵨵鳢���� �浹 �˻� (���������� ��� ����)
    // for (size_t i = 0; i < models.size(); ++i) {
    //     for (size_t j = i + 1; j < models.size(); ++j) {  
    //         if (models[i].rigidBody && models[j].rigidBody) {
    //             dynamicsWorld->contactPairTest(models[i].rigidBody, models[j].rigidBody, resultCallback);
    //         }
    //     }
    // }
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