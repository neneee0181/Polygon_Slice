#pragma once
#include <vector>
#include "Model.h"
#include <gl/glm/glm/glm.hpp>

void handleModelSlice1(Model& originalModel, const glm::vec3& planeNormal, float planeOffset, std::vector<Model>& models,
    void (*addModelToPhysicsWorld)(Model& model), void (*AddModelBuffer)(const Model& model), void (*removeModelFromWorld)(std::vector<Model>& models, Model& modelToDelete), void(*InitBuffer)(), glm::vec3 dragSqu[4]) {

    Model topPart = originalModel, bottomPart = originalModel;
    topPart.vertices.clear();
    topPart.faces.clear();
    topPart.rigidBody = nullptr;
    bottomPart.vertices.clear();
    bottomPart.faces.clear();
    bottomPart.rigidBody = nullptr;

    // �� �߽� ��ǥ ���
    glm::vec3 modelCenter(0.0f);
    for (const auto& vertex : originalModel.vertices) {
        modelCenter += glm::vec3(vertex.x, vertex.y, vertex.z);
    }
    modelCenter /= originalModel.vertices.size();

    // ���� �߽��� �������� �и��� ���ؽ� �ε����� ����
    std::unordered_map<int, int> topVertexMap, bottomVertexMap;
    for (int i = 0; i < originalModel.vertices.size(); ++i) {
        const auto& vertex = originalModel.vertices[i];
        if (vertex.y > modelCenter.y) {
            topVertexMap[i] = topPart.vertices.size();
            topPart.vertices.push_back(vertex);
        }
        else {
            bottomVertexMap[i] = bottomPart.vertices.size();
            bottomPart.vertices.push_back(vertex);
        }
    }

    // faces�� �������� �� ��Ʈ�� �ε��� ����Ʈ�� ����
    for (const auto& face : originalModel.faces) {
        bool inTop = topVertexMap.count(face.v1) && topVertexMap.count(face.v2) && topVertexMap.count(face.v3);
        bool inBottom = bottomVertexMap.count(face.v1) && bottomVertexMap.count(face.v2) && bottomVertexMap.count(face.v3);

        if (inTop) {
            Face newFace = { topVertexMap[face.v1], topVertexMap[face.v2], topVertexMap[face.v3] };
            topPart.faces.push_back(newFace);
        }
        else if (inBottom) {
            Face newFace = { bottomVertexMap[face.v1], bottomVertexMap[face.v2], bottomVertexMap[face.v3] };
            bottomPart.faces.push_back(newFace);
        }
    }


    // ���� ���� ��鿡�� ����
    removeModelFromWorld(models, originalModel);
    InitBuffer();

    // ���ҵ� �� ���� �߰�

    addModelToPhysicsWorld(topPart);
    models.push_back(topPart);
    AddModelBuffer(topPart);

    addModelToPhysicsWorld(bottomPart);
    models.push_back(bottomPart);
    AddModelBuffer(bottomPart);
}