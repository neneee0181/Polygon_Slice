#pragma once
#include <vector>
#include "Model.h"
#include <gl/glm/glm/glm.hpp>

void handleModelSlice(Model& originalModel, const glm::vec3& planeNormal, float planeOffset, std::vector<Model>& models, 
    void (*addModelToPhysicsWorld)(Model& model), void (*AddModelBuffer)(const Model& model), void (*removeModelFromWorld)(std::vector<Model>& models, Model& modelToDelete),void(*InitBuffer)(), glm::vec3 dragSqu[4]) {
    Model topPart, bottomPart;

    for (const Face& face : originalModel.faces) {
        // �� face�� �� ������ ��������
        Vertex v0 = originalModel.vertices[face.v1];
        Vertex v1 = originalModel.vertices[face.v2];
        Vertex v2 = originalModel.vertices[face.v3];

        // dragSqu[0]�� dragSqu[2]�� �̿��� x, y ���� ���
        float minX = std::min(dragSqu[0].x, dragSqu[2].x);
        float maxX = std::max(dragSqu[0].x, dragSqu[2].x);
        float minY = std::min(dragSqu[0].y, dragSqu[2].y);
        float maxY = std::max(dragSqu[0].y, dragSqu[2].y);

        // �� �������� �巡�׵� �簢�� ���� �ִ��� ���� Ȯ��
        bool v0_in = (v0.x >= minX && v0.x <= maxX && v0.y >= minY && v0.y <= maxY);
        bool v1_in = (v1.x >= minX && v1.x <= maxX && v1.y >= minY && v1.y <= maxY);
        bool v2_in = (v2.x >= minX && v2.x <= maxX && v2.y >= minY && v2.y <= maxY);

        if (v0_in && v1_in && v2_in) {
            // ��� �������� �簢�� ���ο� �ִ� ��� topPart�� �з�
            topPart.faces.push_back(face);
            topPart.vertices.push_back(v0);
            topPart.vertices.push_back(v1);
            topPart.vertices.push_back(v2);
        }
        else {
            // ��� �������� �簢�� �ۿ� �ִ� ��� bottomPart�� �з�
            bottomPart.faces.push_back(face);
            bottomPart.vertices.push_back(v0);
            bottomPart.vertices.push_back(v1);
            bottomPart.vertices.push_back(v2);
        }
    }

    // ���� ���� ��鿡�� ����
    removeModelFromWorld(models, originalModel);
    InitBuffer();

    // ���ο� �� ���� �߰��ϰ� OpenGL ���� �ʱ�ȭ
    models.push_back(topPart);
    AddModelBuffer(topPart);

    models.push_back(bottomPart);
    AddModelBuffer(bottomPart);

}