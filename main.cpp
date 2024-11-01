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

using namespace std; 

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void startTimer(int value);
void AddModelBuffer(const Model& model);
void InitLineBuffer(const Model& model);

vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;
std::vector<GLuint> lineVAOs;
std::vector<GLuint> lineVBOs;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 200.0);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

glm::vec3 p_r = glm::vec3(200.0, 150.0, -250.0);
glm::vec3 p_l = glm::vec3(-200.0, 150.0, -250.0);

Model model_box, model_sphere, model_cylinder;

std::unordered_map<char, bool> keyState;

int model_speed = 120;

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

    // OpenGL ��ǥ�踦 ���߱� ���� y�� ������
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
        cout << " ���α׷� ���� " << endl;
        exit(0);
        break;
    case 's':
        cout << "-------- START --------" << endl;
        glutTimerFunc(0, startTimer, 0);
        break;
    case '+':
        if (model_speed <= 60)
            break;
        --model_speed;
        cout << "������ ���� �ð� ms = " + to_string(model_speed) << endl;
        break;
    case '-':
        if (model_speed >= 300)
            break;
        ++model_speed;
        cout << "������ ���� �ð� ms = " + to_string(model_speed) << endl;
        
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
    
    if (value % model_speed == 0) {
        int lr = dis_rl(gen);
        Model model;
        glm::mat4 matrix = glm::mat4(1.0f);
        switch (dis_model(gen))
        {
        case 0:
            model = model_box;
            model.lr = lr;
            matrix = glm::translate(matrix, lr == 1 ? p_l : p_r);
            model.modelMatrix = matrix * model.modelMatrix;
            break;
        case 1:
            model = model_sphere;
            model.lr = lr;
            matrix = glm::translate(matrix, lr == 1 ? p_l : p_r);
            model.modelMatrix = matrix * model.modelMatrix;
            break;
        case 2:
            model = model_cylinder;
            model.lr = lr;
            matrix = glm::translate(matrix, lr == 1 ? p_l : p_r);
            model.modelMatrix = matrix * model.modelMatrix;
            break;
        default:
            break;
        }

        make_line_left(model.lr == 1 ? p_l : p_r, model.lines);
        InitLineBuffer(model);

        models.push_back(model);
        AddModelBuffer(model);  // �� �𵨿� ���� VAO�� VBO �߰�
    }
    glutPostRedisplay();
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

    cout << "��ɾ� ����Ʈ" << endl;
    cout << "s : start" << endl;
    cout << "1 : press = LINE || up == FILL" << endl;
    cout << "2 : press = MODEL_ROAD_LINE || up = NONE" << endl;

    make_shaderProgram();

    read_obj_file("obj/box.obj", model_box, "box");
    read_obj_file("obj/sphere.obj", model_sphere, "sphere");
    read_obj_file("obj/Cylinder.obj", model_cylinder, "cylinder");

    for (auto& model : models) {
        if (!model.material.map_Kd.empty()) {
            model.material.textureID = loadTexture(model.material.map_Kd);
            model.material.hasTexture = true; // �ؽ�ó�� ������ ǥ��
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

        if (models[i].name == "box" || models[i].name == "cylinder" || models[i].name == "sphere") {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            glUniform1i(modelStatus, 0);
            if (isKeyPressed_s('1'))
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);


        // '2'Ű�� ���� ���� ���� ���� line�� �׸���
        if (isKeyPressed_s('2') && i < lineVAOs.size()) {
            glBindVertexArray(lineVAOs[i]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // ���� ��� ����
            glUniform1i(modelStatus, 1); // �� ������ ��� Ȱ��ȭ
            glLineWidth(1.5f);
            glDrawArrays(GL_LINE_STRIP, 0, models[i].lines.size());  // 1���� ���ͷ� �� line�� �׸���
            glBindVertexArray(0);
        }

    }


  
    glDisable(GL_DEPTH_TEST);
    glutSwapBuffers();
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
    }
}

// ���� �ʱ�ȭ �Լ�
void InitBuffer() {

    vaos.resize(models.size());
    vbos.resize(models.size(), vector<GLuint>(4)); // �𵨸��� 4���� VBO�� �ʿ� (����, ����, ����, �ؽ�ó ��ǥ)

    for (size_t i = 0; i < models.size(); ++i) {
        glGenVertexArrays(1, &vaos[i]);
        glBindVertexArray(vaos[i]);

        // VBO ����
        glGenBuffers(4, vbos[i].data());

        // ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][0]);
        glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size() * sizeof(glm::vec3), models[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0�� ���� �Ҵ�
        glEnableVertexAttribArray(0);

        // ���� ���� ����
        //glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
        //glBufferData(GL_ARRAY_BUFFER, models[i].colors.size() * sizeof(glm::vec3), models[i].colors.data(), GL_STATIC_DRAW);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1�� ���� �Ҵ�
        //glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][2]);  // ������ VBO
        glBufferData(GL_ARRAY_BUFFER, models[i].normals.size() * sizeof(glm::vec3), models[i].normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1�� ���� �Ҵ�
        glEnableVertexAttribArray(1);

        // �ؽ�ó ��ǥ VBO ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][3]);  // 4��° VBO�� �ؽ�ó ��ǥ���̶�� ����
        glBufferData(GL_ARRAY_BUFFER, models[i].texCoords.size() * sizeof(glm::vec2), models[i].texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2�� �ؽ�ó ��ǥ �Ҵ�
        glEnableVertexAttribArray(2);

        // �� �ε��� ������ (EBO) ����
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

    // VBO ����
    glGenBuffers(4, vbo.data());

    // ���� ���� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), model.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0�� ���� �Ҵ�
    glEnableVertexAttribArray(0);

    // ���� ���� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3), model.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1�� ���� �Ҵ�
    glEnableVertexAttribArray(1);

    // �ؽ�ó ��ǥ ���� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(glm::vec2), model.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2�� �ؽ�ó ��ǥ �Ҵ�
    glEnableVertexAttribArray(2);

    // �� �ε��� ������ (EBO) ����
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

    // ������ ���� VAO�� VBO�� ����
    vaos.push_back(vao);
    vbos.push_back(vbo);

    glBindVertexArray(0); // VAO unbind
}

void InitLineBuffer(const Model& model) {
    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // VBO ����
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