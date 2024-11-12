#pragma once
#include <vector>
#include "Model.h"
#include <gl/glm/glm/glm.hpp>

void handleModelSlice(Model& originalModel, const glm::vec3& planeNormal, float planeOffset, std::vector<Model>& models, 
    void (*addModelToPhysicsWorld)(Model& model), void (*AddModelBuffer)(const Model& model), void (*removeModelFromWorld)(std::vector<Model>& models, Model& modelToDelete),void(*InitBuffer)()) {
    Model topPart, bottomPart;

    for (const Face& face : originalModel.faces) {
        // �� �������� ������κ����� �Ÿ� ���
        float d0 = glm::dot(planeNormal, glm::vec3(originalModel.vertices[face.v1].x,
            originalModel.vertices[face.v1].y,
            originalModel.vertices[face.v1].z)) - planeOffset;

        float d1 = glm::dot(planeNormal, glm::vec3(originalModel.vertices[face.v2].x,
            originalModel.vertices[face.v2].y,
            originalModel.vertices[face.v2].z)) - planeOffset;

        float d2 = glm::dot(planeNormal, glm::vec3(originalModel.vertices[face.v3].x,
            originalModel.vertices[face.v3].y,
            originalModel.vertices[face.v3].z)) - planeOffset;


        // ������ ���迡 ���� �з�
        if (d0 >= 0 && d1 >= 0 && d2 >= 0) {
            // �ﰢ�� ��ü�� ��� ���� ����
            topPart.faces.push_back(face);
            topPart.vertices.push_back(originalModel.vertices[face.v1]);
            topPart.vertices.push_back(originalModel.vertices[face.v2]);
            topPart.vertices.push_back(originalModel.vertices[face.v3]);
        }
        else if (d0 <= 0 && d1 <= 0 && d2 <= 0) {
            // �ﰢ�� ��ü�� ��� �Ʒ��� ����
            bottomPart.faces.push_back(face);
            bottomPart.vertices.push_back(originalModel.vertices[face.v1]);
            bottomPart.vertices.push_back(originalModel.vertices[face.v2]);
            bottomPart.vertices.push_back(originalModel.vertices[face.v3]);
        }
        else {
            // �ﰢ���� ����� �����ϴ� ���, ���� ó��
            // ������ ��� �� �� ���� ��Ʈ�� ����
        }
    }

    removeModelFromWorld(models, originalModel);
    InitBuffer();

    models.push_back(topPart);
    AddModelBuffer(topPart);

    models.push_back(bottomPart);
    AddModelBuffer(bottomPart);

}