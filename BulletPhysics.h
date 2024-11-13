#pragma once

#include <limits>  // std::numeric_limits 사용을 위한 헤더 추가
#include <gl/glm/glm/glm.hpp>

#include "Model.h"
#include "CustomContactResultCallback.h"

#include"include/btBulletCollisionCommon.h"
#include"include/btBulletDynamicsCommon.h"

void removeRigidBodyFromModel(Model& model);

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

// Model의 AABB를 계산하여 크기를 반환하는 함수
glm::vec3 calculateModelSize(const Model& model) {
    // 초기 최소, 최대값을 설정
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    float maxZ = std::numeric_limits<float>::lowest();

    // 모든 정점을 순회하며 최소, 최대값 업데이트
    for (const Vertex& vertex : model.vertices) {
        if (vertex.x < minX) minX = vertex.x;
        if (vertex.y < minY) minY = vertex.y;
        if (vertex.z < minZ) minZ = vertex.z;
        if (vertex.x > maxX) maxX = vertex.x;
        if (vertex.y > maxY) maxY = vertex.y;
        if (vertex.z > maxZ) maxZ = vertex.z;
    }

    // 폭, 높이, 깊이를 계산
    float width = maxX - minX;
    float height = maxY - minY;
    float depth = maxZ - minZ;

    return glm::vec3(width, height, depth);
}

// 모델에 대한 충돌 객체와 강체 생성 및 물리 세계에 추가
void addModelToPhysicsWorld(Model& model) {
    // 모델의 크기를 계산
    glm::vec3 size = calculateModelSize(model);

    btCollisionShape* shape = nullptr;

    // 모델의 이름에 따라 충돌 경계 설정
    if (model.name == "box") {
        // Box 형태의 충돌 경계 생성
        shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
    else if (model.name == "sphere") {
        // Sphere 형태의 충돌 경계 생성 (구의 반지름을 x, y, z 중 최소값의 절반으로 설정)
        float radius = std::min({ size.x, size.y, size.z }) * 0.5f;
        shape = new btSphereShape(radius);
    }
    else if (model.name == "cylinder") {
        // Cylinder 형태의 충돌 경계 생성 (x와 z의 평균값을 반지름으로, y를 높이로 사용)
        shape = new btCylinderShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
    else if (model.name == "plane") {
        shape = new btBoxShape(btVector3(size.x * 0.5f, 0.1f, size.z * 0.5f));
    }
    else {
        // 기본값으로 Box 형태 사용 (예외 처리를 위해)
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


// 모든 모델에 대한 물리 세계 충돌 객체 초기화
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

    // 각 모델에 대해 애니메이션된 위치와 회전 상태를 Bullet Physics에 동기화
    for (auto& model : models) {
        if (model.rigidBody) {
            btTransform transform;
            transform.setFromOpenGLMatrix(glm::value_ptr(model.modelMatrix)); // modelMatrix에 회전 포함
            model.rigidBody->setWorldTransform(transform);
            model.rigidBody->getMotionState()->setWorldTransform(transform);
        }
    }

    // 바구니 모델 위치 및 회전 동기화
    if (model_basket.rigidBody) {
        btTransform basketTransform;
        basketTransform.setFromOpenGLMatrix(glm::value_ptr(model_basket.modelMatrix)); // modelMatrix에 회전 포함
        model_basket.rigidBody->setWorldTransform(basketTransform);
        model_basket.rigidBody->getMotionState()->setWorldTransform(basketTransform);
    }

    // 모델들 각각과 바구니 모델 사이의 충돌 검사 수행
    for (auto& model : models) {
        if (model.rigidBody && model_basket.rigidBody) {
            resultCallback.reset();  // 이전 충돌 상태를 초기화
            dynamicsWorld->contactPairTest(model.rigidBody, model_basket.rigidBody, resultCallback);

            // 충돌이 감지되었는지 확인
            if (resultCallback.hitDetected) {
                // 모델을 비활성화하거나 상태를 업데이트
                model.line_status = false;
                model.basket_in = true;
                // 모델의 현재 회전 상태를 유지
                glm::mat4 rotationMatrix = glm::mat4(1.0f);
                rotationMatrix[0] = glm::vec4(model.modelMatrix[0]); // x축 회전
                rotationMatrix[1] = glm::vec4(model.modelMatrix[1]); // y축 회전
                rotationMatrix[2] = glm::vec4(model.modelMatrix[2]); // z축 회전

                // 바구니 위치로 이동 (바구니의 중심으로 모델을 이동)
                glm::vec3 basketPosition(
                    model_basket.modelMatrix[3].x + basket_r_dis(gen_b),
                    model_basket.modelMatrix[3].y + basket_r_y_dis(gen_b),
                    model_basket.modelMatrix[3].z + basket_r_dis(gen_b)
                );
                glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), basketPosition + glm::vec3(0, 10, 0));

                // 모델을 바구니 안에 작게 배치하도록 스케일 적용
                glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

                // 모델 행렬 업데이트 (스케일, 위치, 마지막 회전 적용)
                model.modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;

                // 모델의 RigidBody만 제거하여 물리 엔진의 영향을 받지 않도록 함
                removeRigidBodyFromModel(model);
            }
        }
    }

    // 모델들끼리의 충돌 검사 (선택적으로 사용 가능)
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

// 특정 모델을 삭제하는 함수
void removeModelFromWorld(std::vector<Model>& models, Model& modelToDelete) {
    // 물리 세계에서 모델의 RigidBody 제거
    if (modelToDelete.rigidBody) {
        dynamicsWorld->removeRigidBody(modelToDelete.rigidBody);

        // RigidBody에 할당된 리소스 해제
        delete modelToDelete.rigidBody->getMotionState();
        delete modelToDelete.rigidBody;
        modelToDelete.rigidBody = nullptr;
    }

    // models 벡터에서 해당 모델 제거
    auto it = std::find_if(models.begin(), models.end(), [&](const Model& model) {
        return &model == &modelToDelete;
        });

    if (it != models.end()) {
        models.erase(it);
    }
}

// 특정 모델의 RigidBody만 제거하는 함수
void removeRigidBodyFromModel(Model& model) {
    if (model.rigidBody) {
        // 물리 세계에서 RigidBody 제거
        dynamicsWorld->removeRigidBody(model.rigidBody);

        // RigidBody에 할당된 리소스 해제
        delete model.rigidBody->getMotionState();
        delete model.rigidBody;
        model.rigidBody = nullptr;
    }
}

btCollisionObject* dragPlaneObject = nullptr;

//// 드래그 평면 생성
//void createDragPlane(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& pointC, const glm::vec3& pointD) {
//    if (dragPlaneObject) {
//        // 기존 평면 삭제
//        dynamicsWorld->removeCollisionObject(dragPlaneObject);
//        delete dragPlaneObject;
//    }
//
//    // 네 개의 점을 이용해 평면 정의
//    glm::vec3 normal = glm::normalize(glm::cross(pointB - pointA, pointD - pointA));
//    btVector3 planeOrigin(pointA.x, pointA.y, pointA.z);
//    btVector3 planeNormal(normal.x, normal.y, normal.z);
//
//    // Bullet의 평면 충돌체 생성
//    btStaticPlaneShape* planeShape = new btStaticPlaneShape(planeNormal, planeOrigin.dot(planeNormal));
//    dragPlaneObject = new btCollisionObject();
//    dragPlaneObject->setCollisionShape(planeShape);
//    dragPlaneObject->setUserPointer((void*)"dragPlane");
//
//    dynamicsWorld->addCollisionObject(dragPlaneObject);
//}

void createDragBox(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& pointC, const glm::vec3& pointD) {
    if (dragPlaneObject) {
        // 기존 평면 삭제
        dynamicsWorld->removeCollisionObject(dragPlaneObject);
        delete dragPlaneObject;
    }

    // 사각형 중심 계산
    glm::vec3 center = (pointA + pointB + pointC + pointD) / 4.0f;
    glm::vec3 halfExtents = glm::vec3(glm::distance(pointA, pointB) / 2.0f, 0.1f, glm::distance(pointA, pointD) / 2.0f);

    btBoxShape* boxShape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
    dragPlaneObject = new btCollisionObject();
    dragPlaneObject->setCollisionShape(boxShape);

    // 위치 지정
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(center.x, center.y, center.z));
    dragPlaneObject->setWorldTransform(transform);

    dynamicsWorld->addCollisionObject(dragPlaneObject);
}