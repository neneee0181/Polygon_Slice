#pragma once
#include <gl/glm/glm/glm.hpp>
#include <vector>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;
};

void renderParticles(std::vector<Particle> particles) {
    glPointSize(5.0f);  // 파티클의 크기 설정
    glBegin(GL_POINTS);  // 파티클을 점으로 그릴 예정
    glEnd();
}