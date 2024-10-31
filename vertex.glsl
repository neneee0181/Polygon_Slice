#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Normal;    // 정점 법선

out vec3 FragPos;      // 프래그먼트 위치 (world space)
out vec3 Normal;       // 프래그먼트의 법선 벡터 (world space)

uniform mat4 model;              // 모델 변환 행렬
uniform mat4 viewTransform;      // 뷰 변환 행렬
uniform mat4 projectionTransform; // 투영 변환 행렬

void main() {
    FragPos = vec3(model * vec4(in_Position, 1.0));      // 모델 변환 후의 위치
    Normal = mat3(transpose(inverse(model))) * in_Normal; // 법선 벡터 변환
    gl_Position = projectionTransform * viewTransform * vec4(FragPos, 1.0);
}
