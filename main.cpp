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
#include "LineMaker.h"
#include "BulletPhysics.h"

using namespace std; 

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void startTimer(int value);
void rotateTimer(int value);
void moveTimer(int value);
void moveBasket(int value);

void AddModelBuffer(const Model& model);
void InitLineBuffer(const Model& model);

vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;
std::vector<GLuint> lineVAOs;
std::vector<GLuint> lineVBOs;
GLuint basketVAO;
GLuint basketVBO[4];

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 200.0);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

glm::vec3 p_r = glm::vec3(200.0, 150.0, -250.0);
glm::vec3 p_l = glm::vec3(-200.0, 150.0, -250.0);

Model model_box, model_sphere, model_cylinder, model_plane, model_basket;

std::unordered_map<char, bool> keyState;

int model_speed = 120; // 모델 생성 시간
float moveSpeed = 0.25f; // 보간 속도 조절 (모델 속도)

void keyDown_s(const char& key) {
    keyState[key] = true;
}

void keyUp_s(const char& key) {
    keyState[key] = false;
}

bool isKeyPressed_s(const char& key) {
    return keyState[key];
}

// Bullet Physics 초기화
void InitPhysics() {
    initPhysics(); // Bullet 초기화 함수 호출
    initializeModelsWithPhysics(models); // 모든 모델을 물리 세계에 추가
}

void CleanupPhysics() {
    cleanupPhysics(); // Bullet 메모리 해제
}

void get3DMousePositionGLM(float mouseX, float mouseY, int screenWidth, int screenHeight, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    glm::vec4 viewport = glm::vec4(0, 0, screenWidth, screenHeight);

    // OpenGL 좌표계와 맞추기 위해 y 좌표 뒤집기
    glm::vec3 winCoords_near = glm::vec3(mouseX, screenHeight - mouseY, 0.0f); // Near plane (z = 0.0)
    glm::vec3 winCoords_far = glm::vec3(mouseX, screenHeight - mouseY, 1.0f);  // Far plane (z = 1.0)

    // Near, Far plane의 unproject 결과로 두 개의 world position 얻기
    glm::vec3 worldPos_near = glm::unProject(winCoords_near, view * model, projection, viewport);
    glm::vec3 worldPos_far = glm::unProject(winCoords_far, view * model, projection, viewport);

    std::cout << "Near Plane 3D Position: (" << worldPos_near.x << ", " << worldPos_near.y << ", " << worldPos_near.z << ")" << std::endl;
    std::cout << "Far Plane 3D Position: (" << worldPos_far.x << ", " << worldPos_far.y << ", " << worldPos_far.z << ")" << std::endl;

    // Near와 Far 사이의 방향 벡터 계산 (정규화된 방향 벡터)
    glm::vec3 direction = glm::normalize(worldPos_far - worldPos_near);

    // 예시: 특정 깊이에서의 위치를 계산 (이 경우, z = -100 위치에서의 점을 찾기)
    float targetDepth = -100.0f;
    float t = (targetDepth - worldPos_near.z) / direction.z;
    glm::vec3 targetPosition = worldPos_near + t * direction;

    std::cout << "3D Position at depth " << targetDepth << ": (" << targetPosition.x << ", " << targetPosition.y << ", " << targetPosition.z << ")" << std::endl;
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
        glutTimerFunc(0, rotateTimer, 0);
        glutTimerFunc(0, moveTimer, 0);
        glutTimerFunc(0, moveBasket, 0);

        break;
    case '+':
        if (moveSpeed < 0.02f)
            break;
        moveSpeed = moveSpeed - 0.01f;
        cout << "모델 속도 = " + to_string(moveSpeed) << endl;
        break;
    case '-':
        if (moveSpeed > 4.0f)
            break;
        moveSpeed = moveSpeed + 0.01f;
        cout << " 모델 속도 = " + to_string(moveSpeed) << endl;
        break;
    case '[':
        if (model_speed <= 60)
            break;
        --model_speed;
        cout << "폴리곤 생성 시간 ms = " + to_string(model_speed) << endl;
        break;
    case ']':
        if (model_speed >= 300)
            break;
        ++model_speed;
        cout << "폴리곤 생성 시간 ms = " + to_string(model_speed) << endl;
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
        //std::cout << "x = " << x << " y = " << y << std::endl;
    glutPostRedisplay();
} 

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis_model(0, 2);
uniform_int_distribution<> dis_rl(0, 1);

void startTimer(int value) {
    
    cout << value << endl;
    
    if (value % model_speed == 0) {
        int lr = dis_rl(gen);
        Model model;
        glm::mat4 matrix = glm::mat4(1.0f);
        switch (dis_model(gen))
        {
        case 0:
            model = model_box;
            model.lr = lr;
            model.r_r = dis_model(gen);
            matrix = glm::translate(matrix, lr == 1 ? p_l : p_r);
            model.modelMatrix = matrix * model.modelMatrix;
            break;
        case 1:
            model = model_sphere;
            model.lr = lr;
            model.r_r = dis_model(gen);
            matrix = glm::translate(matrix, lr == 1 ? p_l : p_r);
            model.modelMatrix = matrix * model.modelMatrix;
            break;
        case 2:
            model = model_cylinder;
            model.lr = lr;
            model.r_r = dis_model(gen);
            matrix = glm::translate(matrix, lr == 1 ? p_l : p_r);
            model.modelMatrix = matrix * model.modelMatrix;
            break;
        default:
            break;
        }
        model.material.hasTexture = false;

        // 물리 엔진에 새 모델을 추가
        addModelToPhysicsWorld(model);

        make_line_left(model.lr == 1 ? p_l : p_r, model.lines);
        InitLineBuffer(model);
        models.push_back(model);
        AddModelBuffer(model);  // 새 모델에 대해 VAO와 VBO 추가
    }


    glutPostRedisplay();
    glutTimerFunc(16, startTimer, ++value);
}

void rotateTimer(int value) {

    // 자전
    for (int i = 0; i < models.size(); ++i) {
        if (models[i].line_status && models[i].model_status) {
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(models[i].modelMatrix[3]));
            matrix = glm::rotate(matrix, glm::radians(0.5f), glm::vec3(models[i].r_r == 0 ? 1.0 : 0.0, models[i].r_r == 1 ? 1.0 : 0.0, models[i].r_r == 2 ? 1.0 : 0.0));
            matrix = glm::translate(matrix, glm::vec3(-models[i].modelMatrix[3]));
            models[i].modelMatrix = matrix * models[i].modelMatrix;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, rotateTimer, ++value);
}

// Catmull-Rom 보간 함수
glm::vec3 catmullRomInterpolation(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float t) {
    return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
}

void moveTimer(int value) {
    for (int i = 0; i < models.size(); ++i) {
        
        if (!models[i].line_status && models[i].model_status)
            continue;

        if (models[i].lines.size() < 4) continue;

        int segment = (int)models[i].moveT; // 현재 구간
        int nextSegment = (segment + 1) % (models[i].lines.size() - 3);

        // 4개의 점을 통해 Catmull-Rom 보간
        glm::vec3 p0 = models[i].lines[segment];
        glm::vec3 p1 = models[i].lines[segment + 1];
        glm::vec3 p2 = models[i].lines[segment + 2];
        glm::vec3 p3 = models[i].lines[segment + 3];

        glm::vec3 interpolatedPosition = catmullRomInterpolation(p0, p1, p2, p3, models[i].moveT - segment);

        // 현재 modelMatrix의 자전 부분을 유지하며 새 위치를 적용
        glm::mat4 rotationMatrix = models[i].modelMatrix;
        rotationMatrix[3] = glm::vec4(interpolatedPosition, 1.0f); // 위치 업데이트

        models[i].modelMatrix = rotationMatrix; // 자전과 위치 적용

        // 이동 속도에 따라 t 증가
        models[i].moveT += moveSpeed;
        if (models[i].moveT >= models[i].lines.size() - 3) {
            models[i].moveT = 0.0f; // 모든 구간을 다 이동했으면 초기화
        }

        if (models[i].modelMatrix[3].y <= -150) {
            models[i].model_status = false;
            models[i].line_status = false;
            models[i].basket_in = true;

            removeRigidBodyFromModel(models[i]);
        }
    }

    // 매 프레임마다 물리 시뮬레이션을 업데이트
    updatePhysics(models, model_basket); // deltaTime을 1/60초로 설정 (60 FPS 기준)

    glutPostRedisplay();
    glutTimerFunc(16, moveTimer, ++value);
}

bool basket_status = false;

void moveBasket(int value) {
    float speed = 0.5f;

    glm::mat4 basket_t_matrix = glm::mat4(1.0f);
    glm::mat4 models_t_matrix = glm::mat4(1.0f);

    if (model_basket.modelMatrix[3].x <= 110 && !basket_status) {
        basket_t_matrix = glm::translate(basket_t_matrix, glm::vec3(speed, 0.0, 0.0));

        for (auto& model : models) {
            if (model.model_status && !model.line_status) {
                model.modelMatrix = basket_t_matrix * model.modelMatrix;
            }
        }
    }
    else if (model_basket.modelMatrix[3].x >= 110) {
        basket_status = true;
    }

    if (model_basket.modelMatrix[3].x >= -110 && basket_status) {
        basket_t_matrix = glm::translate(basket_t_matrix, glm::vec3(-speed, 0.0, 0.0));

        for (auto& model : models) {
            if (model.model_status && !model.line_status) {
                model.modelMatrix = basket_t_matrix * model.modelMatrix;
            }
        }
    }
    else if (model_basket.modelMatrix[3].x <= -110) {
        basket_status = false;
    }

    model_basket.modelMatrix = basket_t_matrix * model_basket.modelMatrix;

    glutPostRedisplay();
    glutTimerFunc(16, moveBasket, 0);
}

int main(int argc, char** argv) {

    width = 800;
    height = 600;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Polygon Slice");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    cout << "명령어 리스트" << endl;
    cout << "s : start" << endl;
    cout << "1 : press = LINE || up == FILL" << endl;
    cout << "2 : press = MODEL_ROAD_LINE || up = NONE" << endl;

    make_shaderProgram();

    //bullet 초기화
    InitPhysics();

    read_obj_file("obj/box.obj", model_box, "box");
    read_obj_file("obj/sphere.obj", model_sphere, "sphere");
    read_obj_file("obj/Cylinder.obj", model_cylinder, "cylinder");
    read_obj_file("obj/plane.obj", model_plane, "plane");
    read_obj_file("obj/basket.obj", model_basket, "basket");

    // basket setting
    glm::mat4 basket_t_matrix = glm::mat4(1.0f);
    basket_t_matrix = glm::translate(basket_t_matrix, glm::vec3(0.0, -80.0, -30.0));

    glm::mat4 basket_s_matrix = glm::mat4(1.0f);
    basket_s_matrix = glm::scale(basket_s_matrix, glm::vec3(1.3, 1.3, 1.3));
    model_basket.modelMatrix = basket_t_matrix * basket_s_matrix * model_basket.modelMatrix;

    addModelToPhysicsWorld(model_basket);

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

void drawBasket(GLint modelLoc, GLint modelStatus) {

    glBindVertexArray(basketVAO);
    glLineWidth(1.0f);
    glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);
    GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
    GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
    GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
    GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");
    glUniform3fv(KaLoc, 1, glm::value_ptr(model_basket.material.Ka));
    glUniform3fv(KdLoc, 1, glm::value_ptr(model_basket.material.Kd));
    glUniform3fv(KsLoc, 1, glm::value_ptr(model_basket.material.Ks));
    glUniform1f(NsLoc, model_basket.material.Ns);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_basket.modelMatrix));
    glUniform1i(modelStatus, 0);
    if (isKeyPressed_s('1'))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, model_basket.faces.size() * 3, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
    glUseProgram(shaderProgramID);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error in glUseProgram: " << error << std::endl;
    }

    view = glm::mat4(1.0f);
    view = glm::lookAt(
        cameraPos,
        cameraDirection,
        cameraUp
    );
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 600.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(-glm::vec3(900.0, -900.0, -900.0)));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(0.6f, 0.65f, 0.6f)));

    glEnable(GL_DEPTH_TEST);

    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");

    drawBasket(modelLoc, modelStatus);

    for (size_t i = 0; i < models.size(); ++i) {

        if (models[i].model_status) {
            glBindVertexArray(vaos[i]);
            glLineWidth(1.0f);
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

            if (models[i].name == "box" || models[i].name == "cylinder" || models[i].name == "sphere" || models[i].name == "plane" || models[i].name == "basket") {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
                glUniform1i(modelStatus, 0);
                if (isKeyPressed_s('1'))
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
            }

            glBindVertexArray(0);
        }

        if (models[i].line_status) {
            // '2'키가 눌려 있을 때만 모델의 line을 그리기
            if (isKeyPressed_s('2') && i < lineVAOs.size()) {
                glBindVertexArray(lineVAOs[i]);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // 단위 행렬 적용
                glUniform1i(modelStatus, 1); // 선 렌더링 모드 활성화
                glLineWidth(1.5f);
                glDrawArrays(GL_LINE_STRIP, 0, models[i].lines.size());  // 1차원 벡터로 된 line을 그리기
                glBindVertexArray(0);
            }
        }
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

    glGenVertexArrays(1, &basketVAO);
    glBindVertexArray(basketVAO);

    glGenBuffers(4, basketVBO);

    // 정점 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, basketVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, model_basket.vertices.size() * sizeof(glm::vec3), model_basket.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0에 정점 할당
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, basketVBO[2]);  // 법선용 VBO
    glBufferData(GL_ARRAY_BUFFER, model_basket.normals.size() * sizeof(glm::vec3), model_basket.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 법선 할당
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, basketVBO[3]);  // 4번째 VBO가 텍스처 좌표용이라고 가정
    glBufferData(GL_ARRAY_BUFFER, model_basket.texCoords.size() * sizeof(glm::vec2), model_basket.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2에 텍스처 좌표 할당
    glEnableVertexAttribArray(2);

    vector<unsigned int> indices_basket;
    for (const Face& face : model_basket.faces) {
        indices_basket.push_back(face.v1);
        indices_basket.push_back(face.v2);
        indices_basket.push_back(face.v3);
    }

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_basket.size() * sizeof(unsigned int), indices_basket.data(), GL_STATIC_DRAW);

    //-----------------------------------------------------------------------------------------------------------
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

void InitLineBuffer(const Model& model) {
    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // VBO 생성
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, model.lines.size() * sizeof(glm::vec3), model.lines.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    lineVAOs.push_back(vao);
    lineVBOs.push_back(vbo);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}