#pragma once
#include <iostream>
#include <gl/glm/glm/glm.hpp>
#include <gl/glew.h>
#include <vector>
#include <random> // 랜덤 함수 사용을 위해 추가

void make_line_left(glm::vec3 startPoint, std::vector<glm::vec3>& lines) {
    // 랜덤 생성기 초기화
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> randomOffset(-1.0f, 1.0f); // -1.0f ~ 1.0f 사이의 랜덤 값

    // 시뮬레이션 파라미터 설정
    float gravity = -9.8f;        // 중력 가속도
    float initialSpeed = 50.0f;   // 초기 속도 (원점 방향으로 다가가는 속도)
    float timeStep = 0.1f;        // 시뮬레이션 시간 간격
    int numSteps = 100;           // 점의 개수

    // 초기 위치와 속도 설정
    glm::vec3 position = startPoint;
    glm::vec3 velocity = glm::normalize(glm::vec3(-position.x, -position.y, -position.z)) * initialSpeed;

    for (int i = 0; i < numSteps; ++i) {
        // x축 속도에 약간의 랜덤 오프셋을 추가하여 속도 변동을 줌
        velocity.x += randomOffset(gen);

        // 포지션 업데이트
        position += velocity * timeStep;

        // y축에 중력 적용
        velocity.y += gravity * timeStep;

        // z축이 0에 도달하면 멈추기
        if (position.z > 0.0f) {
            position.z = 0.0f;
            velocity.z = 0.0f;
        }

        // 업데이트된 위치를 선에 추가
        lines.push_back(position);

        // z축이 0에 도달했으면 종료
        if (position.z == 0.0f) break;
    }
}
