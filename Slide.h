#pragma once
#include <vector>
#include "Model.h"
#include <gl/glm/glm/glm.hpp>

void handleModelSlice1(Model& originalModel, const glm::vec3& planeNormal, float planeOffset, std::vector<Model>& models,
    void (*addModelToPhysicsWorld)(Model& model), void (*AddModelBuffer)(const Model& model), void (*removeModelFromWorld)(std::vector<Model>& models, Model& modelToDelete), void(*InitBuffer)(), glm::vec3 dragSqu[4]) {


    // 모델의 중앙을 계산
    glm::vec3 centroid(0.0f);
    for (const auto& vertex : originalModel.vertices) {
        centroid += vertex.toVec3();
    }
    centroid /= static_cast<float>(originalModel.vertices.size());

    // 분할된 모델을 위한 복사본 생성
    Model topPart = originalModel, bottomPart = originalModel;
    topPart.vertices.clear();
    topPart.faces.clear();
    topPart.rigidBody = nullptr;
    bottomPart.vertices.clear();
    bottomPart.faces.clear();
    bottomPart.rigidBody = nullptr;


    if (originalModel.name == "box") {
        read_obj_file("obj/box2.obj", topPart, "box");
        read_obj_file("obj/box3.obj", bottomPart, "box");
    }
    else if (originalModel.name == "cylinder") {
        read_obj_file("obj/Cylinder2.obj", topPart, "cylinder");
        read_obj_file("obj/Cylinder3.obj", bottomPart, "cylinder");
    }
    else if (originalModel.name == "sphere") {
        read_obj_file("obj/sphere2.obj", topPart, "sphere");
        read_obj_file("obj/sphere3.obj", bottomPart, "sphere");
    }
    else if (originalModel.name == "plane") {
        read_obj_file("obj/plane2.obj", topPart, "plane");
        read_obj_file("obj/plane3.obj", bottomPart, "plane");
    }

    else if (originalModel.name == "teapot") {
        read_obj_file("obj/teapot2.obj", topPart, "teapot");
        read_obj_file("obj/teapot3.obj", bottomPart, "teapot");
    }



    // 원래 모델을 장면에서 제거
    removeModelFromWorld(models, originalModel);
    InitBuffer();

    // 분할된 두 모델을 추가

    addModelToPhysicsWorld(topPart);
    models.push_back(topPart);
    AddModelBuffer(topPart);

    addModelToPhysicsWorld(bottomPart);
    models.push_back(bottomPart);
    AddModelBuffer(bottomPart);
}