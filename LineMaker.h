#pragma once
#include <iostream>
#include <gl/glm/glm/glm.hpp>
#include <gl/glew.h>

void make_line_left(glm::vec3 startPoint, vector<vector<glm::vec3>>& lines) {
    // 선을 구성할 점들을 저장할 벡터
    std::vector<glm::vec3> linePoints;

    // 시뮬레이션 파라미터 설정
    float gravity = -9.8f;        // 중력 가속도
    float initialSpeed = 50.0f;   // 초기 속도 (원점 방향으로 다가가는 속도)
    float timeStep = 0.1f;        // 시뮬레이션 시간 간격
    int numSteps = 100;           // 점의 개수

    // 초기 위치와 속도 설정
    glm::vec3 position = startPoint;
    glm::vec3 velocity = glm::normalize(glm::vec3(-position.x, -position.y, -position.z)) * initialSpeed;

    for (int i = 0; i < numSteps; ++i) {
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
        linePoints.push_back(position);

        // z축이 0에 도달했으면 종료
        if (position.z == 0.0f) break;
    }

    // 완성된 선을 lines에 추가
    lines.push_back(linePoints);
}