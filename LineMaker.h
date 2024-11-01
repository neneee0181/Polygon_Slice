#pragma once
#include <iostream>
#include <gl/glm/glm/glm.hpp>
#include <gl/glew.h>
#include <vector>
#include <random> // ���� �Լ� ����� ���� �߰�

void make_line_left(glm::vec3 startPoint, std::vector<glm::vec3>& lines) {
    // ���� ������ �ʱ�ȭ
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> randomOffset(-1.0f, 1.0f); // -1.0f ~ 1.0f ������ ���� ��

    // �ùķ��̼� �Ķ���� ����
    float gravity = -9.8f;        // �߷� ���ӵ�
    float initialSpeed = 50.0f;   // �ʱ� �ӵ� (���� �������� �ٰ����� �ӵ�)
    float timeStep = 0.1f;        // �ùķ��̼� �ð� ����
    int numSteps = 100;           // ���� ����

    // �ʱ� ��ġ�� �ӵ� ����
    glm::vec3 position = startPoint;
    glm::vec3 velocity = glm::normalize(glm::vec3(-position.x, -position.y, -position.z)) * initialSpeed;

    for (int i = 0; i < numSteps; ++i) {
        // x�� �ӵ��� �ణ�� ���� �������� �߰��Ͽ� �ӵ� ������ ��
        velocity.x += randomOffset(gen);

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
