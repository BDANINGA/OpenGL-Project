//--- ���� �Լ�
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <tuple>
// --- ����ü
struct VertexNormal {
	float x, y, z;     // Vertex coordinates
	float nx, ny, nz;  // Normal coordinates
};
struct Vertex {
	float x, y, z;
};
struct TextureCoord {
	float u, v;
};
struct Normal {
	float nx, ny, nz;
};
struct Face {
	std::vector<std::tuple<int, int, int>> vertices; // (vertex_index, tex_coord_index, normal_index)
};
struct ObjData {
	std::vector<Vertex> vertices;
	std::vector<TextureCoord> texCoords;
	std::vector<Normal> normals;
	std::vector<Face> faces;
};
//--- �ʿ� �Լ�
void windowToOpenGL(int window_x, int window_y, int window_width, int window_height, float& gl_x, float& gl_y);
void TimerFunction(int value);
void MakeShape(GLfloat Shape[][3], GLfloat normal[][3], GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, int first_index, std::string shape);
void MakeColor(GLfloat arr[][3], int first_index, int index_count, GLfloat color[3]);
void convertToGLArrays(const ObjData& objData, std::vector<GLfloat>& vertexArray, std::vector<GLfloat>& normalArray);
ObjData parseObj(const std::string& filePath);
//--- �ʿ��� ���� ����
extern GLuint vao, vbo[3];
extern GLint width, height;
extern GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
extern GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
extern GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

//------------------------------------------------------------------------------------------------------

GLfloat colors[400000][3]{};
std::vector<GLfloat> vertexArray;
std::vector<GLfloat> normalArray;
ObjData data{};
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 light = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightp = glm::vec3(0.0f, 2.0f, 3.0f);
//-----------------------------------------------------------------------
// 241207
void MoveBall();
// Ű���� ����
bool keyStates[256] = { false };

// ��ġ �̵�
glm::vec3 playerPos = glm::vec3(0.0f, 0.0f, 0.0f);  // ��ü ��ġ
glm::vec3 ballPos = glm::vec3(0.0f, 0.0f, 0.0f);  // �� ��° ��ü ��ġ

// ���� �ӵ� �� ���ӵ� ���� �߰�
glm::vec3 ballVelocity = glm::vec3(0.0f, 0.0f, 0.0f);  // ���� �ӵ�
glm::vec3 ballAcceleration = glm::vec3(0.0f, 0.0f, 0.0f); // ���� ���ӵ�

const float MAX_SPEED = 20.0f; // ���� �ִ� �ӵ�
const float ACCELERATION = 0.02f; // ���ӵ�
const float FRICTION = 0.98f; // ������ (�ӵ� ���� ����)
const float DECELERATION = 0.001f; // ���ӵ� ���� ����
const float GRAVITY = -0.05f;  // �߷� ��, ������ �����Ͽ� �Ʒ��� ����������
const float JUMP_STRENGTH = 0.5f;  // z Ű�� ������ �� ���� ���� ��
const float BALL_BOUNCE_DAMPING = 0.8f;  // �ٿ ���� (0~1 ���̷� ����, 1�̸� �ݻ� �� �ӵ� �״��)

glm::vec3 lastBallDirection = glm::vec3(0.0f, 0.0f, 0.0f);  // ���� ������ ���� ����
float shootingPower = 0.0f;  // ���� �Ŀ� ���� (0.0 ~ MAX_SHOOTING_POWER ����)
bool shootingInProgress = false;  // ���� ���� �� ���� (d Ű�� ���� ��������)
const float MAX_SHOOTING_POWER = 20.0f;  // �ִ� ���� �Ŀ�
const float SHOOTING_INCREMENT = 0.1f;  // ���� �Ŀ� 
const float SHOOTING_DECAY = 0.1f;  // ���� �Ŀ� ���ҷ� (d Ű�� ������ ��)

void drawGrass();
void drawPlayer();
void drawBall();

//------------------------------------------------------------------------
void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�

	glGenBuffers(3, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	//--- triShape �迭�� ������: 9 * float
	glBufferData(GL_ARRAY_BUFFER, vertexArray.size() * sizeof(GLfloat), vertexArray.data(), GL_STATIC_DRAW);
	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 0���� ��밡���ϰ� ��
	glEnableVertexAttribArray(0);

	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- colors �迭�� ������: 9 *float
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 1���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(1);

	//--- 3��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- normal �迭�� ������: 9 *float
	glBufferData(GL_ARRAY_BUFFER, normalArray.size() * sizeof(GLfloat), normalArray.data(), GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 2���� ����Ѵ�: ���ؽ� �� 3*float
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 2���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(2);
}

int once = 0;
GLfloat red = 0.0f;
GLfloat blue = 0.0f;
GLfloat green = 0.0f;
bool left_button = 0;
GLfloat redcolor[3] = { 1.0f, 0.0f, 0.0f };
GLfloat bluecolor[3] = { 0.0f, 0.0f, 1.0f };
GLfloat greencolor[3] = { 0.0f, 1.0f, 0.0f };
GLfloat pinkcolor[3] = { 1.0f, 0.0f, 1.0f };
GLfloat skycolor[3] = { 0.0f, 1.0f, 1.0f };
GLfloat yellowcolor[3] = { 1.0f, 1.0f, 0.0f };
GLfloat Long{ 2.0f };

int firstObjectVertexCount{}, secondObjectVertexCount;
//cube 36
//pyramid 16
//cone 1263
//sphere 1008

GLvoid drawScene() {
	if (once == 0) {
		once = 1;

		std::cout << "----- obj ������ �Ľ� �� -----" << std::endl;

		data = parseObj("player.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		firstObjectVertexCount = vertexArray.size() / 3;

		std::cout << "player �Ϸ�" << std::endl;
		std::cout << "vertexcount - " << firstObjectVertexCount << std::endl;

		data = parseObj("ball.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		secondObjectVertexCount = vertexArray.size() / 3 - firstObjectVertexCount;

		std::cout << "ball �Ϸ�" << std::endl;
		std::cout << "vertexcount - " << secondObjectVertexCount << std::endl;

		std::cout << "----- obj ������ �Ľ� �Ϸ� -----" << std::endl;

		for (int i = 0; i < firstObjectVertexCount; ++i) {
			for (int j = 0; j < 3; ++j)
				colors[i][j] = 0.5f;
		}

		for (int i = firstObjectVertexCount; i < firstObjectVertexCount + secondObjectVertexCount; ++i) {
			for (int j = 0; j < 3; ++j)
				colors[i][j] = 1.0f;
		}

		glutTimerFunc(25, TimerFunction, 1);
		InitBuffer();
	}

	//--- ����� ���� ����
	glClearColor(1, 1, 1, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glEnable(GL_DEPTH_TEST);   // ���� �׽�Ʈ Ȱ��ȭ
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(vao);


	
	// ù ��° ��ü (�÷��̾�) �׸���
	drawPlayer();
	drawGrass();
	// �� ��° ��ü (��) �׸���
	drawBall();


	// ���� ��ȯ
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// ���� ��ȯ
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -4.0));
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	// ����
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, lightp.x, lightp.y, lightp.z);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, light.x, light.y, light.z);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}



//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}
void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'z':
		if (ballPos.y == 0.0f) {
			ballVelocity.y = JUMP_STRENGTH;
		}
		break;
	case 'd':
		// d Ű�� ������ ���� �Ŀ� ����
		shootingInProgress = true;
		break;
	case 'D':
		// d Ű�� ��ҹ��ڷ� ó��, ��ҹ��� ����
		shootingInProgress = true;
		break;
	case 'Z':
		cameraPos.z -= 0.1f;
		break;
	case 'y':
		cameraPos.y += 0.1f;
		break;
	case 'Y':
		cameraPos.y -= 0.1f;
		break;
	case 'r':
		cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
		cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void KeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'd':
	case 'D':
		// d Ű�� �������� �� �߻�
		if (shootingInProgress) {
			if (ballPos.y == 0.0f) {  // ���� �ٴڿ� ���� ���� �߻�
				ballVelocity = glm::normalize(ballVelocity) * shootingPower;  // ���� �Ŀ� ����
				ballVelocity.y = shootingPower/2.0f;  // ��¦ ���� ƨ��� �� ���� ����
			}
			shootingPower = 0.0f;  // ���� �Ŀ� �ʱ�ȭ
			shootingInProgress = false;  // ���� ���� �� ���� �ʱ�ȭ
		}
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
void Mouse(int button, int state, int x, int y)
{
	float gl_x, gl_y;
	windowToOpenGL(x, y, width, height, gl_x, gl_y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

	}
}
void TimerFunction(int value)
{
	glutTimerFunc(25, TimerFunction, 1);
	glutPostRedisplay();
}
void Motion(int x, int y)
{
	float gl_x, gl_y;
	windowToOpenGL(x, y, width, height, gl_x, gl_y);
	if (left_button == true)
	{
	}
	glutPostRedisplay();
}
GLvoid SpecialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		keyStates[GLUT_KEY_UP] = true;
		break;
	case GLUT_KEY_DOWN:
		keyStates[GLUT_KEY_DOWN] = true;
		break;
	case GLUT_KEY_LEFT:
		keyStates[GLUT_KEY_LEFT] = true;
		break;
	case GLUT_KEY_RIGHT:
		keyStates[GLUT_KEY_RIGHT] = true;
		break;
	}
	glutPostRedisplay();  // ȭ�� ����
}

GLvoid SpecialKeysUp(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		keyStates[GLUT_KEY_UP] = false;
		break;
	case GLUT_KEY_DOWN:
		keyStates[GLUT_KEY_DOWN] = false;
		break;
	case GLUT_KEY_LEFT:
		keyStates[GLUT_KEY_LEFT] = false;
		break;
	case GLUT_KEY_RIGHT:
		keyStates[GLUT_KEY_RIGHT] = false;
		break;
	}
	glutPostRedisplay();  // ȭ�� ����
}

void MoveBall() {
	// d Ű�� ������ �� ���� �Ŀ��� ������Ŵ
	if (shootingInProgress) {
		shootingPower += SHOOTING_INCREMENT;
		if (shootingPower > MAX_SHOOTING_POWER) {
			shootingPower = MAX_SHOOTING_POWER;  // �ִ� ���� �Ŀ� ����
		}
	}

	// ���� ���� ���ӵ� ���� (����Ű �Է¿� ���� ���ӵ�)
	if (keyStates[GLUT_KEY_UP] && keyStates[GLUT_KEY_LEFT]) {
		ballAcceleration.x = -ACCELERATION;
		ballAcceleration.z = -ACCELERATION;
	}
	else if (keyStates[GLUT_KEY_UP] && keyStates[GLUT_KEY_RIGHT]) {
		ballAcceleration.x = ACCELERATION;
		ballAcceleration.z = -ACCELERATION;
	}
	else if (keyStates[GLUT_KEY_DOWN] && keyStates[GLUT_KEY_LEFT]) {
		ballAcceleration.x = -ACCELERATION;
		ballAcceleration.z = ACCELERATION;
	}
	else if (keyStates[GLUT_KEY_DOWN] && keyStates[GLUT_KEY_RIGHT]) {
		ballAcceleration.x = ACCELERATION;
		ballAcceleration.z = ACCELERATION;
	}
	else if (keyStates[GLUT_KEY_UP]) {
		ballAcceleration.z = -ACCELERATION;
	}
	else if (keyStates[GLUT_KEY_DOWN]) {
		ballAcceleration.z = ACCELERATION;
	}
	else if (keyStates[GLUT_KEY_LEFT]) {
		ballAcceleration.x = -ACCELERATION;
	}
	else if (keyStates[GLUT_KEY_RIGHT]) {
		ballAcceleration.x = ACCELERATION;
	}
	else {
		ballAcceleration = glm::vec3(0.0f, 0.0f, 0.0f); // ����Ű �Է��� ���� ��� ���ӵ� 0���� �ʱ�ȭ
	}

	// �߷� ����: y �������� ���ӵ� ����
	ballVelocity.y += GRAVITY;  // ���� �߷� ���ӵ� ����

	// ���� �ӵ� ������Ʈ
	ballVelocity += ballAcceleration;  // ���ӵ��� �ӵ��� �߰�

	// �ִ� �ӵ� ����
	float speed = glm::length(ballVelocity); // �ӵ� ũ��
	if (speed > MAX_SPEED) {
		ballVelocity = glm::normalize(ballVelocity) * MAX_SPEED;  // �ִ� �ӵ� ����
	}

	// ���� y ��ġ�� 0�� ���� ������ ����
	if (ballPos.y == 0.0f) {
		ballVelocity *= FRICTION;  // ������ ����
	}

	// ���� ���ߵ���: ���� �ӵ� ���Ϸ� �������� ���� ����
	if (glm::length(ballVelocity) < DECELERATION) {
		ballVelocity = glm::vec3(0.0f, 0.0f, 0.0f);  // �ӵ��� �ſ� ������ ���� ����
	}

	// ���� ��ġ ������Ʈ
	ballPos += ballVelocity;  // �ӵ��� ���� ��ġ ����

	// ���� �ٴڿ� ������ �ݻ�
	if (ballPos.y < 0.0f) {
		ballPos.y = 0.0f;  // ���� y ��ġ�� 0���� ���� (�ٴ�)
		ballVelocity.y = -ballVelocity.y * BALL_BOUNCE_DAMPING;  // y ���� �ӵ��� ������Ű�� ���� ����
	}

	// x�� z ��迡 ������ �ݻ� ó�� (Grass ���: -50.0f ~ 50.0f)
	if (ballPos.x < -50.0f) {  // x �� ���� ���
		ballPos.x = -50.0f;  // ���� ��迡 ���� ��ġ ����
		ballVelocity.x = -ballVelocity.x * BALL_BOUNCE_DAMPING;  // x ���� �ӵ� ���� (ƨ���)
	}
	else if (ballPos.x > 50.0f) {  // x �� ������ ���
		ballPos.x = 50.0f;  // ���� ��迡 ���� ��ġ ����
		ballVelocity.x = -ballVelocity.x * BALL_BOUNCE_DAMPING;  // x ���� �ӵ� ���� (ƨ���)
	}

	if (ballPos.z < -50.0f) {  // z �� ���� ���
		ballPos.z = -50.0f;  // ���� ��迡 ���� ��ġ ����
		ballVelocity.z = -ballVelocity.z * BALL_BOUNCE_DAMPING;  // z ���� �ӵ� ���� (ƨ���)
	}
	else if (ballPos.z > 50.0f) {  // z �� ���� ���
		ballPos.z = 50.0f;  // ���� ��迡 ���� ��ġ ����
		ballVelocity.z = -ballVelocity.z * BALL_BOUNCE_DAMPING;  // z ���� �ӵ� ���� (ƨ���)
	}

	// ī�޶� ���� ���󰡵��� �ϱ� ���� �ڵ� �߰�
	cameraPos = ballPos + glm::vec3(0.0f, 1.0f, 5.0f);  // ���� ��ġ���� ī�޶��� ����� ��ġ ���� (�� 2, �� 5)
	cameraDirection = ballPos;  // ī�޶�� ���� ���ϵ���

	// ī�޶��� ��ġ�� ������ �������� �� ��ȯ ����� ����մϴ�.
	glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
}

void drawGrass() {
	// xz ����� ������ ������ �׸���
	GLfloat grassVertices[] = {
		// x, y, z
		-50.0f, -0.3f, -50.0f,  // ���� �ϴ�
		50.0f, -0.3f, -50.0f,   // ������ �ϴ�
		50.0f, -0.3f, 50.0f,    // ������ ���
		-50.0f, -0.3f, 50.0f    // ���� ���
	};

	GLfloat grassColor[] = { 1.0f, 1.0f, 1.0f };  // �ʷϻ�

	// xz ��� �׸���
	glBegin(GL_QUADS);
	glColor3fv(grassColor);  // ���� ���� (�ʷϻ�)
	for (int i = 0; i < 4; ++i) {
		glVertex3f(grassVertices[i * 3], grassVertices[i * 3 + 1], grassVertices[i * 3 + 2]);
	}
	glEnd();
}

void drawPlayer() {
	glm::mat4 Trans = glm::mat4(1.0f);

	// ù ��° ��ü(�÷��̾�) �̵��� ���� ��ġ ������Ʈ
	Trans = glm::translate(Trans, playerPos);
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	// ù ��° ��ü(�÷��̾�) �׸���
	glDrawArrays(GL_TRIANGLES, 0, firstObjectVertexCount);
}

void drawBall() {
	MoveBall();
	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 S = glm::mat4(1.0f);
	glm::mat4 Trans = glm::mat4(1.0f);

	// �� ��° ��ü(��) �̵��� ���� ��ġ ������Ʈ
	T = glm::translate(T, ballPos);  // �� ��° ��ü�� ��ġ ����
	S = glm::scale(S, glm::vec3(0.01f, 0.01f, 0.01f));  // ũ�� ����
	Trans = T * S;

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	// �� ��° ��ü(��) �׸���
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount, secondObjectVertexCount);
}


