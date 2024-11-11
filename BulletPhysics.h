#pragma once

#include <limits>  // std::numeric_limits ����� ���� ��� �߰�
#include <gl/glm/glm/glm.hpp>

#include "Model.h"
#include "CustomContactResultCallback.h"

#include"include/btBulletCollisionCommon.h"
#include"include/btBulletDynamicsCommon.h"

void removeRigidBodyFromModel(Model& model);

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

random_device rd_b;
mt19937 gen_b(rd_b());
uniform_real_distribution<> basket_r_dis(-10.0, 10.0);
uniform_real_distribution<> basket_r_y_dis(-8.0, -2.0);

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
            resultCallback.reset();  // ���� �浹 ���¸� �ʱ�ȭ
            dynamicsWorld->contactPairTest(model.rigidBody, model_basket.rigidBody, resultCallback);

            // �浹�� �����Ǿ����� Ȯ��
            if (resultCallback.hitDetected) {
                // ���� ��Ȱ��ȭ�ϰų� ���¸� ������Ʈ
                model.line_status = false;

                // ���� ���� ȸ�� ���¸� ����
                glm::mat4 rotationMatrix = glm::mat4(1.0f);
                rotationMatrix[0] = glm::vec4(model.modelMatrix[0]); // x�� ȸ��
                rotationMatrix[1] = glm::vec4(model.modelMatrix[1]); // y�� ȸ��
                rotationMatrix[2] = glm::vec4(model.modelMatrix[2]); // z�� ȸ��

                // �ٱ��� ��ġ�� �̵� (�ٱ����� �߽����� ���� �̵�)
                glm::vec3 basketPosition(
                    model_basket.modelMatrix[3].x + basket_r_dis(gen_b),
                    model_basket.modelMatrix[3].y + basket_r_y_dis(gen_b),
                    model_basket.modelMatrix[3].z + basket_r_dis(gen_b)
                );
                glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), basketPosition + glm::vec3(0, 10, 0));

                // ���� �ٱ��� �ȿ� �۰� ��ġ�ϵ��� ������ ����
                glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

                // �� ��� ������Ʈ (������, ��ġ, ������ ȸ�� ����)
                model.modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;

                // ���� RigidBody�� �����Ͽ� ���� ������ ������ ���� �ʵ��� ��
                removeRigidBodyFromModel(model);
            }
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

// Ư�� ���� �����ϴ� �Լ�
void removeModelFromWorld(std::vector<Model>& models, Model& modelToDelete) {
    // ���� ���迡�� ���� RigidBody ����
    if (modelToDelete.rigidBody) {
        dynamicsWorld->removeRigidBody(modelToDelete.rigidBody);

        // RigidBody�� �Ҵ�� ���ҽ� ����
        delete modelToDelete.rigidBody->getMotionState();
        delete modelToDelete.rigidBody;
        modelToDelete.rigidBody = nullptr;
    }

    // models ���Ϳ��� �ش� �� ����
    auto it = std::find_if(models.begin(), models.end(), [&](const Model& model) {
        return &model == &modelToDelete;
        });

    if (it != models.end()) {
        models.erase(it);
    }
}

// Ư�� ���� RigidBody�� �����ϴ� �Լ�
void removeRigidBodyFromModel(Model& model) {
    if (model.rigidBody) {
        // ���� ���迡�� RigidBody ����
        dynamicsWorld->removeRigidBody(model.rigidBody);

        // RigidBody�� �Ҵ�� ���ҽ� ����
        delete model.rigidBody->getMotionState();
        delete model.rigidBody;
        model.rigidBody = nullptr;
    }
}