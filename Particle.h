#pragma once
#include <gl/glm/glm/glm.hpp>
#include <vector>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;
};

void renderParticles(std::vector<Particle> particles) {
    glPointSize(5.0f);  // ��ƼŬ�� ũ�� ����
    glBegin(GL_POINTS);  // ��ƼŬ�� ������ �׸� ����
    glEnd();
}