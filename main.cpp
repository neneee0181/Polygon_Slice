#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector> 
#include <unordered_map>
#include <random>

#include "LoadObj.h"
#include "shaderMaker.h"

using namespace std; 

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void startTimer(int value);
void AddModelBuffer(const Model& model);

vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 200.0);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

glm::vec3 p_r = glm::vec3(200.0, 150.0, -250.0);
glm::vec3 p_l = glm::vec3(-200.0, 150.0, -250.0);

Model model_box, model_sphere, model_cylinder;

std::unordered_map<char, bool> keyState;

void keyDown_s(const char& key) {
    keyState[key] = true;
}

void keyUp_s(const char& key) {
    keyState[key] = false;
}

bool isKeyPressed_s(const char& key) {
    return keyState[key];
}

void get3DMousePositionGLM(float mouseX, float mouseY, int screenWidth, int screenHeight, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    glm::vec4 viewport = glm::vec4(0, 0, screenWidth, screenHeight);

    // OpenGL 좌표계를 맞추기 위해 y를 뒤집음
    glm::vec3 winCoords = glm::vec3(mouseX, screenHeight - mouseY, 0.0f); // Near plane (0.0 -> near, 1.0 -> far)

    // Unproject
    glm::vec3 worldPos = glm::unProject(winCoords, view * model, projection, viewport);

    std::cout << "3D Position: (" << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ")" << std::endl;
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;
}

void keyUp(unsigned char key, int x, int y) {
    keyUp_s(key);
}

void keyDown(unsigned char key, int x, int y) {

    keyDown_s(key);

    switch (key)
    {
    case 'q':
        cout << " 프로그램 종료 " << endl;
        exit(0);
        break;
    case 's':
        cout << "-------- START --------" << endl;
        glutTimerFunc(0, startTimer, 0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void keySpecial(int key, int x, int y) {

    glm::mat4 rotationMatrix;
    if (key == GLUT_KEY_LEFT) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (key == GLUT_KEY_RIGHT) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (key == GLUT_KEY_UP) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else if (key == GLUT_KEY_DOWN) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else {
        return;
    }

    cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos, 1.0f));

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        get3DMousePositionGLM(x, y, width, height, glm::mat4(1.0f), view, projection);
    }
        std::cout << "x = " << x << " y = " << y << std::endl;
    glutPostRedisplay();
} 

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis_model(0, 2);
uniform_int_distribution<> dis_rl(0, 1);

void startTimer(int value) {
    
    cout << value << endl;
    
    if (value % 120 == 0) {
        Model model;
        switch (dis_model(gen))
        {
        case 0:
            model = model_box;
            break;
        case 1:
            model = model_sphere;
            break;
        case 2:
            model = model_cylinder;
            break;
        default:
            break;
        }
        models.push_back(model);
        AddModelBuffer(model);  // 새 모델에 대해 VAO와 VBO 추가
    }

    glutTimerFunc(16, startTimer, ++value);
}

int main(int argc, char** argv) {

    width = 800;
    height = 600;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("template");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    cout << "명령어 리스트" << endl;
    cout << "1 : LINE" << endl;
    cout << "2 : FILL" << endl;
    cout << "s : start" << endl;

    make_shaderProgram();

    read_obj_file("obj/box.obj", model_box, "box");
    model_box.translateMatrix = glm::translate(glm::mat4(1.0f), p_l);
    model_box.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    model_box.modelMatrix = model_box.translateMatrix * model_box.rotateMatrix *model_box.modelMatrix;
    model_box.lr = 1;
    models.push_back(model_box);

    read_obj_file("obj/sphere.obj", model_sphere, "sphere");
    model_sphere.translateMatrix = glm::translate(glm::mat4(1.0f), p_r);
    model_sphere.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    model_sphere.modelMatrix = model_sphere.translateMatrix * model_sphere.rotateMatrix * model_sphere.modelMatrix;
    model_sphere.lr = 2;
    models.push_back(model_sphere);

    read_obj_file("obj/Cylinder.obj", model_cylinder, "cylinder");
    model_cylinder.translateMatrix = glm::translate(glm::mat4(1.0f), p_l);
    model_cylinder.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    model_cylinder.modelMatrix = model_cylinder.translateMatrix * model_cylinder.rotateMatrix * model_cylinder.modelMatrix;
    model_sphere.lr = 2;
    models.push_back(model_cylinder);

    for (auto& model : models) {
        if (!model.material.map_Kd.empty()) {
            model.material.textureID = loadTexture(model.material.map_Kd);
            model.material.hasTexture = true; // 텍스처가 있음을 표시
        }
        else {
            model.material.hasTexture = false;
        }
    }

    InitBuffer();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(keySpecial);
    glutMouseFunc(mouse);
    glutMainLoop();

    return 0;
}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    view = glm::mat4(1.0f);
    view = glm::lookAt(
        cameraPos,
        cameraDirection,
        cameraUp
    );
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 500.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(-glm::vec3(0.0, 0.0, 700.0)));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.95f, 0.9f)));

    glEnable(GL_DEPTH_TEST);

    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");

    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);

        if (models[i].material.hasTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, models[i].material.textureID);
            glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
            glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 1);
        }
        else {
            glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);

            GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
            GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
            GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
            GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

            glUniform3fv(KaLoc, 1, glm::value_ptr(models[i].material.Ka));
            glUniform3fv(KdLoc, 1, glm::value_ptr(models[i].material.Kd));
            glUniform3fv(KsLoc, 1, glm::value_ptr(models[i].material.Ks));
            glUniform1f(NsLoc, models[i].material.Ns);
        }

        if (models[i].name == "box" || models[i].name == "cylinder" || models[i].name == "sphere") {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            glUniform1i(modelStatus, 0);
            if (isKeyPressed_s('1'))
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else if (isKeyPressed_s('2'))
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);
    }

    glDisable(GL_DEPTH_TEST);

    glutSwapBuffers();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
    }
}

// 버퍼 초기화 함수
void InitBuffer() {

    vaos.resize(models.size());
    vbos.resize(models.size(), vector<GLuint>(4)); // 모델마다 4개의 VBO가 필요 (정점, 색상, 법선, 텍스처 좌표)

    for (size_t i = 0; i < models.size(); ++i) {
        glGenVertexArrays(1, &vaos[i]);
        glBindVertexArray(vaos[i]);

        // VBO 생성
        glGenBuffers(4, vbos[i].data());

        // 정점 버퍼 설정
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][0]);
        glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size() * sizeof(glm::vec3), models[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0에 정점 할당
        glEnableVertexAttribArray(0);

        // 색상 버퍼 설정
        //glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
        //glBufferData(GL_ARRAY_BUFFER, models[i].colors.size() * sizeof(glm::vec3), models[i].colors.data(), GL_STATIC_DRAW);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 색상 할당
        //glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][2]);  // 법선용 VBO
        glBufferData(GL_ARRAY_BUFFER, models[i].normals.size() * sizeof(glm::vec3), models[i].normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 법선 할당
        glEnableVertexAttribArray(1);

        // 텍스처 좌표 VBO 설정
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][3]);  // 4번째 VBO가 텍스처 좌표용이라고 가정
        glBufferData(GL_ARRAY_BUFFER, models[i].texCoords.size() * sizeof(glm::vec2), models[i].texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2에 텍스처 좌표 할당
        glEnableVertexAttribArray(2);

        // 면 인덱스 데이터 (EBO) 설정
        vector<unsigned int> indices;
        for (const Face& face : models[i].faces) {
            indices.push_back(face.v1);
            indices.push_back(face.v2);
            indices.push_back(face.v3);
        }

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }
}


void AddModelBuffer(const Model& model) {
    GLuint vao;
    vector<GLuint> vbo(4);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // VBO 생성
    glGenBuffers(4, vbo.data());

    // 정점 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), model.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0에 정점 할당
    glEnableVertexAttribArray(0);

    // 법선 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3), model.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 법선 할당
    glEnableVertexAttribArray(1);

    // 텍스처 좌표 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(glm::vec2), model.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2에 텍스처 좌표 할당
    glEnableVertexAttribArray(2);

    // 면 인덱스 데이터 (EBO) 설정
    vector<unsigned int> indices;
    for (const Face& face : model.faces) {
        indices.push_back(face.v1);
        indices.push_back(face.v2);
        indices.push_back(face.v3);
    }

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 설정이 끝난 VAO와 VBO를 저장
    vaos.push_back(vao);
    vbos.push_back(vbo);

    glBindVertexArray(0); // VAO unbind
}