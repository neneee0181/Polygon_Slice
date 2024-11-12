#pragma once
#include <vector>
#include "Model.h"
#include <gl/glm/glm/glm.hpp>

void handleModelSlice(Model& originalModel, const glm::vec3& planeNormal, float planeOffset, std::vector<Model>& models, 
    void (*addModelToPhysicsWorld)(Model& model), void (*AddModelBuffer)(const Model& model), void (*removeModelFromWorld)(std::vector<Model>& models, Model& modelToDelete),void(*InitBuffer)()) {
    Model topPart, bottomPart;

    for (const Face& face : originalModel.faces) {
        // 각 꼭짓점의 평면으로부터의 거리 계산
        float d0 = glm::dot(planeNormal, glm::vec3(originalModel.vertices[face.v1].x,
            originalModel.vertices[face.v1].y,
            originalModel.vertices[face.v1].z)) - planeOffset;

        float d1 = glm::dot(planeNormal, glm::vec3(originalModel.vertices[face.v2].x,
            originalModel.vertices[face.v2].y,
            originalModel.vertices[face.v2].z)) - planeOffset;

        float d2 = glm::dot(planeNormal, glm::vec3(originalModel.vertices[face.v3].x,
            originalModel.vertices[face.v3].y,
            originalModel.vertices[face.v3].z)) - planeOffset;


        // 평면과의 관계에 따라 분류
        if (d0 >= 0 && d1 >= 0 && d2 >= 0) {
            // 삼각형 전체가 평면 위에 있음
            topPart.faces.push_back(face);
            topPart.vertices.push_back(originalModel.vertices[face.v1]);
            topPart.vertices.push_back(originalModel.vertices[face.v2]);
            topPart.vertices.push_back(originalModel.vertices[face.v3]);
        }
        else if (d0 <= 0 && d1 <= 0 && d2 <= 0) {
            // 삼각형 전체가 평면 아래에 있음
            bottomPart.faces.push_back(face);
            bottomPart.vertices.push_back(originalModel.vertices[face.v1]);
            bottomPart.vertices.push_back(originalModel.vertices[face.v2]);
            bottomPart.vertices.push_back(originalModel.vertices[face.v3]);
        }
        else {
            // 삼각형이 평면을 교차하는 경우, 절단 처리
            // 교차점 계산 후 두 개의 파트로 분할
        }
    }

    removeModelFromWorld(models, originalModel);
    InitBuffer();

    models.push_back(topPart);
    AddModelBuffer(topPart);

    models.push_back(bottomPart);
    AddModelBuffer(bottomPart);

}