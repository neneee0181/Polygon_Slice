#pragma once
#include <vector>
#include "Model.h"
#include <gl/glm/glm/glm.hpp>
#include "LineMaker.h"

void handleModelSlice1(Model& originalModel, const glm::vec3& planeNormal, float planeOffset, std::vector<Model>& models,
    void (*addModelToPhysicsWorld)(Model& model), void (*AddModelBuffer)(const Model& model),
    void (*removeModelFromWorld)(std::vector<Model>& models, Model& modelToDelete), 
    void(*InitBuffer)(), void(*InitLineBuffer)(const std::vector<Model>& models), glm::vec3 dragSqu[4]) {

    // ���ҵ� ���� ���� ���纻 ����
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

    // line_status ����
    topPart.slide_status = true;
    bottomPart.slide_status = true;
    topPart.lines.clear();
    bottomPart.lines.clear();

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

    InitLineBuffer(models);
}

void handleModelSlice2(Model& originalModel, const glm::vec3& planeNormal, float planeOffset, std::vector<Model>& models,
    void (*addModelToPhysicsWorld)(Model& model), void (*AddModelBuffer)(const Model& model), void (*removeModelFromWorld)(std::vector<Model>& models, Model& modelToDelete), void(*InitBuffer)(), glm::vec3 dragSqu[4]) {
    Model topPart = originalModel, bottomPart = originalModel;
    topPart.vertices.clear();
    topPart.faces.clear();
    topPart.rigidBody = nullptr;
    bottomPart.vertices.clear();
    bottomPart.faces.clear();
    bottomPart.rigidBody = nullptr;
    // dragSqu[0]�� dragSqu[2]�� �̿��Ͽ� ������ ����� ���� ���
    float slope = (dragSqu[2].y - dragSqu[0].y) / (dragSqu[2].x - dragSqu[0].x);  // ���� m
    float intercept = dragSqu[0].y - slope * dragSqu[0].x;                        // ���� b

    for (const Face& face : originalModel.faces) {
        // �� face�� �� ������ ��������
        Vertex v0 = originalModel.vertices[face.v1];
        Vertex v1 = originalModel.vertices[face.v2];
        Vertex v2 = originalModel.vertices[face.v3];

        // �ﰢ���� �߽����� ���
        float centerX = (v0.x + v1.x + v2.x) / 3.0f;
        float centerY = (v0.y + v1.y + v2.y) / 3.0f;

        // �߽����� ���� ���� �ִ��� �Ʒ��� �ִ��� Ȯ��
        // ������ ������: y = slope * x + intercept
        float lineY = slope * centerX + intercept;

        if (centerY >= lineY) {
            // �ﰢ���� �߽��� ���� ���ʿ� �ִ� ��� topPart�� �з�
            topPart.faces.push_back(face);
            topPart.vertices.push_back(v0);
            topPart.vertices.push_back(v1);
            topPart.vertices.push_back(v2);
        }
        else {
            // �ﰢ���� �߽��� ���� �Ʒ��ʿ� �ִ� ��� bottomPart�� �з�
            bottomPart.faces.push_back(face);
            bottomPart.vertices.push_back(v0);
            bottomPart.vertices.push_back(v1);
            bottomPart.vertices.push_back(v2);
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