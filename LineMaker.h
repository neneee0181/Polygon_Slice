#pragma once
#include <iostream>
#include <gl/glm/glm/glm.hpp>
#include <gl/glew.h>

void make_line_left(glm::vec3 startPoint, vector<glm::vec3>& lines) {

    // �ùķ��̼� �Ķ���� ����
    float gravity = -9.8f;        // �߷� ���ӵ�
    float initialSpeed = 50.0f;   // �ʱ� �ӵ� (���� �������� �ٰ����� �ӵ�)
    float timeStep = 0.1f;        // �ùķ��̼� �ð� ����
    int numSteps = 100;           // ���� ����

    // �ʱ� ��ġ�� �ӵ� ����
    glm::vec3 position = startPoint;
    glm::vec3 velocity = glm::normalize(glm::vec3(-position.x, -position.y, -position.z)) * initialSpeed;

    for (int i = 0; i < numSteps; ++i) {
        // ������ ������Ʈ
        position += velocity * timeStep;

        // y�࿡ �߷� ����
        velocity.y += gravity * timeStep;

        // z���� 0�� �����ϸ� ���߱�
        if (position.z > 0.0f) {
            position.z = 0.0f;
            velocity.z = 0.0f;
        }

        // ������Ʈ�� ��ġ�� ���� �߰�
        lines.push_back(position);

        // z���� 0�� ���������� ����
        if (position.z == 0.0f) break;
    }

}