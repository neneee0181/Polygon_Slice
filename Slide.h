#pragma once
#include <vector>
#include "Model.h"
#include <gl/glm/glm/glm.hpp>

void handleModelSlice(Model& originalModel, const glm::vec3& planeNormal, float planeOffset, std::vector<Model>& models, 
    void (*addModelToPhysicsWorld)(Model& model), void (*AddModelBuffer)(const Model& model), void (*removeModelFromWorld)(std::vector<Model>& models, Model& modelToDelete),void(*InitBuffer)(), glm::vec3 dragSqu[4]) {
    Model topPart, bottomPart;

    for (const Face& face : originalModel.faces) {
        // 각 face의 세 꼭짓점 가져오기
        Vertex v0 = originalModel.vertices[face.v1];
        Vertex v1 = originalModel.vertices[face.v2];
        Vertex v2 = originalModel.vertices[face.v3];

        // dragSqu[0]과 dragSqu[2]를 이용해 x, y 범위 계산
        float minX = std::min(dragSqu[0].x, dragSqu[2].x);
        float maxX = std::max(dragSqu[0].x, dragSqu[2].x);
        float minY = std::min(dragSqu[0].y, dragSqu[2].y);
        float maxY = std::max(dragSqu[0].y, dragSqu[2].y);

        // 각 꼭짓점이 드래그된 사각형 내에 있는지 여부 확인
        bool v0_in = (v0.x >= minX && v0.x <= maxX && v0.y >= minY && v0.y <= maxY);
        bool v1_in = (v1.x >= minX && v1.x <= maxX && v1.y >= minY && v1.y <= maxY);
        bool v2_in = (v2.x >= minX && v2.x <= maxX && v2.y >= minY && v2.y <= maxY);

        if (v0_in && v1_in && v2_in) {
            // 모든 꼭짓점이 사각형 내부에 있는 경우 topPart로 분류
            topPart.faces.push_back(face);
            topPart.vertices.push_back(v0);
            topPart.vertices.push_back(v1);
            topPart.vertices.push_back(v2);
        }
        else {
            // 모든 꼭짓점이 사각형 밖에 있는 경우 bottomPart로 분류
            bottomPart.faces.push_back(face);
            bottomPart.vertices.push_back(v0);
            bottomPart.vertices.push_back(v1);
            bottomPart.vertices.push_back(v2);
        }
    }

    // 원래 모델을 장면에서 제거
    removeModelFromWorld(models, originalModel);
    InitBuffer();

    // 새로운 두 모델을 추가하고 OpenGL 버퍼 초기화
    models.push_back(topPart);
    AddModelBuffer(topPart);

    models.push_back(bottomPart);
    AddModelBuffer(bottomPart);

}