#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Normal;    // 정점 법선
layout(location = 2) in vec2 aTexCoords; // 텍스처 좌표

out vec3 FragPos;      // 프래그먼트 위치 (world space)
out vec3 Normal;       // 프래그먼트의 법선 벡터 (world space)
out vec2 TexCoords;    // 텍스처 좌표를 프래그먼트 셰이더로 전달

uniform mat4 model;              // 모델 변환 행렬
uniform mat4 viewTransform;      // 뷰 변환 행렬
uniform mat4 projectionTransform; // 투영 변환 행렬

void main() {
    FragPos = vec3(model * vec4(in_Position, 1.0));      // 모델 변환 후의 위치
    Normal = mat3(transpose(inverse(model))) * in_Normal; // 법선 벡터 변환
    TexCoords = aTexCoords;                               // 텍스처 좌표 전달
    gl_Position = projectionTransform * viewTransform * vec4(FragPos, 1.0);
}
