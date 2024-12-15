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
void drawGoal();
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
glm::vec3 lightp = glm::vec3(0.0f, 50.0f, 50.0f);
//-----------------------------------------------------------------------
// 241207
void MoveBall(glm::vec3 playerPos);
// Ű���� ����
bool keyStates[256] = { false };

// ��ġ �̵�
glm::vec3 playerPos = glm::vec3(0.0f, 0.0f, 0.0f);  // ��ü ��ġ
glm::vec3 ballPos = glm::vec3(0.0f, 0.0f, 0.0f);  // �� ��° ��ü ��ġ

// ���� �ӵ� �� ���ӵ� ���� �߰�
glm::vec3 ballVelocity = glm::vec3(0.0f, 0.0f, 0.0f);  // ���� �ӵ�
glm::vec3 ballAcceleration = glm::vec3(0.0f, 0.0f, 0.0f); // ���� ���ӵ�

const float MAX_SPEED = 1.0f; // ���� �ִ� �ӵ�
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
void drawPlayer(glm::vec3 ballPos);
void drawBall();
bool player_has_ball = 0;
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

int firstObjectVertexCount{}, secondObjectVertexCount{}, thirdObjectVertexCount[4]{};
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

		data = parseObj("cube.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		thirdObjectVertexCount[0] = vertexArray.size() / 3 - (firstObjectVertexCount + secondObjectVertexCount);
		data = parseObj("cube.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		thirdObjectVertexCount[1] = vertexArray.size() / 3 - (firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0]);
		data = parseObj("cube.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		thirdObjectVertexCount[2] = vertexArray.size() / 3 - (firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0] + thirdObjectVertexCount[1]);
		data = parseObj("cube.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		thirdObjectVertexCount[3] = vertexArray.size() / 3 - (firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0] + thirdObjectVertexCount[1] + thirdObjectVertexCount[2]);

		std::cout << "��� �Ϸ�" << std::endl;

		std::cout << "----- obj ������ �Ľ� �Ϸ� -----" << std::endl;

		for (int i = 0; i < firstObjectVertexCount; ++i) {
			for (int j = 0; j < 3; ++j)
				colors[i][j] = 0.5f;
		}

		for (int i = firstObjectVertexCount; i < firstObjectVertexCount + secondObjectVertexCount; ++i) {
			for (int j = 0; j < 3; ++j)
				colors[i][j] = 1.0f;
		}

		for (int i = firstObjectVertexCount + secondObjectVertexCount; i < firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0] + thirdObjectVertexCount[1] + thirdObjectVertexCount[2] + thirdObjectVertexCount[3]; ++i) {
			for (int j = 0; j < 3; ++j)
				colors[i][j] = 1.0f;
		}

		glutTimerFunc(25, TimerFunction, 1);
		InitBuffer();
	}

	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glEnable(GL_DEPTH_TEST);   // ���� �׽�Ʈ Ȱ��ȭ
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(vao);


	drawBall();
	drawPlayer(ballPos);
	drawGoal();
	drawGrass();


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

bool sprint = 0;
bool curve = 0;
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}
void Keyboard(unsigned char key, int x, int y) {
	switch (key) {

	case 'd':
		// d Ű�� ������ ���� �Ŀ� ����
		shootingInProgress = true;
		break;
	case 'D':
		// d Ű�� ��ҹ��ڷ� ó��, ��ҹ��� ����
		shootingInProgress = true;
		break;

	case 'y':
		cameraPos.y += 0.1f;
		break;
	case 'Y':
		cameraPos.y -= 0.1f;
		break;
	case 'r':
		//cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
		//cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		ballPos = playerPos;
		ballVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
		break;
	case 'e':
	case 'E':
		sprint = 1;
		break;
	case 'z':
	case 'Z':
		curve = 1;
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
				ballVelocity.y = shootingPower / 2.0f;  // ��¦ ���� ƨ��� �� ���� ����
			}
			shootingPower = 0.0f;  // ���� �Ŀ� �ʱ�ȭ
			shootingInProgress = false;  // ���� ���� �� ���� �ʱ�ȭ
		}
		player_has_ball = 0;
		break;
	case 'e':
	case 'E':
		sprint = 0;
		break;
	case 'z':
	case 'Z':
		curve = 0;
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
bool checkSegmentCollision(glm::vec3 start, glm::vec3 end, glm::vec3 goalPos, glm::vec3 goalScale) {
	glm::vec3 boxMin = goalPos - goalScale;
	glm::vec3 boxMax = goalPos + goalScale;

	for (int i = 0; i < 3; ++i) {
		float boxMinCoord = boxMin[i];
		float boxMaxCoord = boxMax[i];

		if (start[i] < boxMinCoord && end[i] < boxMinCoord || start[i] > boxMaxCoord && end[i] > boxMaxCoord)
			return false;

		if (start[i] < boxMinCoord || end[i] > boxMaxCoord) {
			float t = (boxMinCoord - start[i]) / (end[i] - start[i]);
			if (t > 0.0f && t < 1.0f) {
				return true;
			}
		}
	}

	return true;
}

void MoveBall(glm::vec3 playerPos) {
	const float maxDistance = 0.2f;
	glm::vec3 distanceVec = playerPos - ballPos;
	float distance = glm::distance(glm::vec2(playerPos.x, playerPos.z), glm::vec2(ballPos.x, ballPos.z));
	float CURVE_TURN_SPEED = 0.03f;
	std::cout << distance << std::endl;
	if (player_has_ball) {
		ballAcceleration.x = 0;
		ballAcceleration.z = 0;
		
		if (shootingInProgress && distance <= 1.5f) {
			shootingPower += SHOOTING_INCREMENT;
			if (shootingPower > MAX_SHOOTING_POWER) {
				shootingPower = MAX_SHOOTING_POWER;
			}
		}

		// ����Ű �Է¿� ���� ���� ���ӵ�
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
			ballAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
	if (distance >= 1.0f) {
		ballAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	// ����� �� ��ǰ ��ǥ �� ũ��
	glm::vec3 goalBarPos = glm::vec3(0.0f, 2.0f, -30.0f);
	glm::vec3 goalBarScale = glm::vec3(2.0f, 0.05f, 1.0f);
	glm::vec3 leftPostPos = glm::vec3(-2.0f, 1.0f, -30.0f);
	glm::vec3 leftPostScale = glm::vec3(0.05f, 1.0f, 1.0f);
	glm::vec3 rightPostPos = glm::vec3(2.0f, 1.0f, -30.0f);
	glm::vec3 rightPostScale = glm::vec3(0.05f, 1.0f, 1.0f);
	glm::vec3 bottomBarPos = glm::vec3(0.0f, 1.0f, -31.0f);
	glm::vec3 bottomBarScale = glm::vec3(2.0f, 1.0f, 0.05f);

	glm::vec3 startPos = ballPos;
	glm::vec3 endPos = ballPos + ballVelocity; // ���� �̵� ����

	// �浹 üũ 
	if (
		(checkSegmentCollision(startPos, endPos, rightPostPos, rightPostScale) && ballPos.z <= -28) ||
		(checkSegmentCollision(startPos, endPos, leftPostPos, leftPostScale) && ballPos.z <= -28) ||
		(checkSegmentCollision(startPos, endPos, goalBarPos, goalBarScale) && ballPos.z <= -30) ||
		(checkSegmentCollision(startPos, endPos, bottomBarPos, bottomBarScale) && ballPos.z <= -30)
		)
	{
		// �浹 ó��
		ballVelocity = -ballVelocity * BALL_BOUNCE_DAMPING;
		std::cout << "�浹 ���" << std::endl;

		// ��뿡 ��
		if (checkSegmentCollision(startPos, endPos, bottomBarPos, bottomBarScale) && ballPos.z <= -30) {
			std::cout << "��" << std::endl;
		}
	}

	// �߷� ����
	ballVelocity.y += GRAVITY;

	// ���� �ӵ� ������Ʈ
	ballVelocity += ballAcceleration;

	// �ִ� �ӵ� ����
	float speed = glm::length(ballVelocity);
	if (speed > MAX_SPEED) {
		ballVelocity = glm::normalize(ballVelocity) * MAX_SPEED;
	}

	// ������ ����
	if (ballPos.y == 0.0f) {
		ballVelocity *= FRICTION;
	}

	// ���� ���ߵ���: ���� �ӵ� ���Ϸ� �������� ���� ����
	if (glm::length(ballVelocity) < DECELERATION) {
		ballVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	// **Curve ȿ��** (Z�� ���Ȱ�, curve ������ ����)
	if (curve && !player_has_ball && distance <= 15) {
		// X ��ǥ�� 0���� ũ�� ���ҽ�Ű��, ������ �������� 0���� �������� ȿ��
		if (ballPos.x > 0.1f) {
			ballVelocity.x -= CURVE_TURN_SPEED;  // ���������� X ���� 0���� ��������
		}
		else if (ballPos.x < -0.1f) {
			ballVelocity.x += CURVE_TURN_SPEED;  // �ݴ�� X ���� 0���� ��������
		}
		// X ��ǥ�� ���� 0�� �����ϸ�, �̼��ϰ� �̵����� �ʵ���
		if (glm::abs(ballPos.x) < 0.05f) {
			ballVelocity.x = 0.0f;  // �������� �� X �ӵ� 0����
		}
	}

	// ���� ��ġ ������Ʈ
	ballPos += ballVelocity;

	// ���� �ٴڿ� ������ �ݻ�
	if (ballPos.y < 0.0f) {
		ballPos.y = 0.0f;
		ballVelocity.y = -ballVelocity.y * BALL_BOUNCE_DAMPING;
	}

	// x�� z ��迡 ������ �ݻ� ó��
	if (ballPos.x < -50.0f) {
		ballPos.x = -50.0f;
		ballVelocity.x = -ballVelocity.x * BALL_BOUNCE_DAMPING;
	}
	else if (ballPos.x > 50.0f) {
		ballPos.x = 50.0f;
		ballVelocity.x = -ballVelocity.x * BALL_BOUNCE_DAMPING;
	}

	if (ballPos.z < -50.0f) {
		ballPos.z = -50.0f;
		ballVelocity.z = -ballVelocity.z * BALL_BOUNCE_DAMPING;
	}
	else if (ballPos.z > 50.0f) {
		ballPos.z = 50.0f;
		ballVelocity.z = -ballVelocity.z * BALL_BOUNCE_DAMPING;
	}
}





float playerRotation = 0.0f;  // �ʱ� ���� 0��, z���� ���ϵ��� ����

void MovePlayer(glm::vec3 ballPos) {
	float moveSpeed = 1.0f;  // �÷��̾� �⺻ �̵� �ӵ�
	float acceleration = 0.05f;  // �÷��̾��� ���ӵ�
	const float deceleration = 0.01f; // ���� (���ӵ��� �ݴ�)
	float MAX_PLAYER_SPEED = 10.0f;
	glm::vec3 playerVelocity = glm::vec3(0.0f, 0.0f, 0.0f);  // �ӵ�
	glm::vec3 moveDirection(0.0f);  // �̵� ���� �ʱ�ȭ

	glm::vec3 distanceVec;
	float distance = glm::distance(glm::vec2(playerPos.x, playerPos.z), glm::vec2(ballPos.x, ballPos.z));
	const float maxDistance = 0.5f;  // ���� �÷��̾� ������ �ִ� �Ÿ�

	playerPos.y = 0;
	if (sprint) {
		std::cout << "!!!";
		moveSpeed = 10.0f;
		MAX_PLAYER_SPEED = 50.0f;
		acceleration = 0.2f;
	}
		
	if (player_has_ball) {
		// �÷��̾�� �� ������ ���� ���� ���
		distanceVec = playerPos - ballPos;

		// �Ÿ��� maxDistance�� �ʰ��ϴ� ���
		if (glm::length(distanceVec) > maxDistance) {
			// ���͸� ����ȭ
			distanceVec = glm::normalize(distanceVec);

			// �÷��̾ ������ ���������� �ٰ������� �̵�
			playerPos.x -= distanceVec.x * 0.07f;  // �̵� �ӵ���ŭ �÷��̾� ��ġ ����
			playerPos.z -= distanceVec.z * 0.07f;  // �̵� �ӵ���ŭ �÷��̾� ��ġ ����
		}
	}
	else {
		// ����Ű�� ���� �÷��̾� �̵� ���� ����
		if (keyStates[GLUT_KEY_UP]) {
			moveDirection.z -= moveSpeed;  // �������� �̵�
		}
		if (keyStates[GLUT_KEY_DOWN]) {
			moveDirection.z += moveSpeed;  // �������� �̵�
		}
		if (keyStates[GLUT_KEY_LEFT]) {
			moveDirection.x -= moveSpeed;  // �������� �̵�
		}
		if (keyStates[GLUT_KEY_RIGHT]) {
			moveDirection.x += moveSpeed;  // ���������� �̵�
		}

		// 8�������� �̵� �����ϵ��� ����
		if (keyStates[GLUT_KEY_UP] && keyStates[GLUT_KEY_LEFT]) {
			moveDirection.x -= moveSpeed;
			moveDirection.z -= moveSpeed;
		}
		if (keyStates[GLUT_KEY_UP] && keyStates[GLUT_KEY_RIGHT]) {
			moveDirection.x += moveSpeed;
			moveDirection.z -= moveSpeed;
		}
		if (keyStates[GLUT_KEY_DOWN] && keyStates[GLUT_KEY_LEFT]) {
			moveDirection.x -= moveSpeed;
			moveDirection.z += moveSpeed;
		}
		if (keyStates[GLUT_KEY_DOWN] && keyStates[GLUT_KEY_RIGHT]) {
			moveDirection.x += moveSpeed;
			moveDirection.z += moveSpeed;
		}
	}
	// ȸ�� ó��
	if (keyStates[GLUT_KEY_LEFT]) {
		playerRotation = glm::radians(-90.0f);
	}
	else if (keyStates[GLUT_KEY_RIGHT]) {
		playerRotation = glm::radians(90.0f);
	}
	else if (keyStates[GLUT_KEY_UP]) {
		playerRotation = glm::radians(180.0f);
	}
	else if (keyStates[GLUT_KEY_DOWN]) {
		playerRotation = glm::radians(0.0f);
	}
	// ���ӵ��� �����ϱ� ���� �̵� ������ 0�� �ƴ��� Ȯ��
	if (glm::length(moveDirection) > 0.0f) {
		// �̵� ������ ����ȭ�Ͽ� ���ӵ��� ����
		moveDirection = glm::normalize(moveDirection);

		// ���� �ӵ��� ���ӵ��� �����Ͽ� �ӵ� ����
		playerVelocity += moveDirection * acceleration;  // ���ӵ� ����

		// �ִ� �ӵ��� ����
		if (glm::length(playerVelocity) > MAX_PLAYER_SPEED) {
			playerVelocity = glm::normalize(playerVelocity) * MAX_PLAYER_SPEED;  // �ִ� �ӵ� ����
		}
	}
	else {
		// �̵����� ������ ������ ����
		if (glm::length(playerVelocity) > 0.0f) {
			playerVelocity -= glm::normalize(playerVelocity) * deceleration;  // ����
		}

		// ���� �� �ӵ��� �ʹ� �������� �ӵ��� 0���� ����
		if (glm::length(playerVelocity) < 0.001f) {
			playerVelocity = glm::vec3(0.0f);
		}
	}

	// �ӵ��� �������� �÷��̾� ��ġ ������Ʈ
	playerPos += playerVelocity;  // ���� �ӵ��� �ݿ��Ͽ� �÷��̾� ��ġ �̵�

	// �� ��ȯ ��� ������Ʈ
	glm::mat4 modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, playerPos);  // ��ġ �̵�

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));

	//if (!player_has_ball) {
		// ī�޶� ����: �÷��̾ ���󰡴� ī�޶�
	cameraPos = playerPos + glm::vec3(0.0f, 1.0f, 5.0f);  // �÷��̾� ��ġ �������� ī�޶� ��ġ ���� (�� 2, �� 5)
	cameraDirection = playerPos;  // ī�޶�� �÷��̾ ���ϵ��� ����

	// ī�޶��� ��ġ�� ������ �������� �� ��ȯ ����� ���
	glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	//}

	//std::cout << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << ", " << std::endl;
	//std::cout << ballPos.x << ", " << ballPos.y << ", " << ballPos.z << ", " << std::endl;
	//std::cout << glm::length(distanceVec) << std::endl;
	//std::cout << distance << std::endl;
	if (ballPos.y <= 2.0f) {
		float distance = glm::distance(glm::vec2(playerPos.x, playerPos.z), glm::vec2(ballPos.x, ballPos.z));

		if (distance <= 0.75f) {
			std::cout << "Catch Ball!" << std::endl;
			if (player_has_ball == 0) {
				ballVelocity.x = 0.0f;
				ballVelocity.z = 0.0f;
				playerPos = ballPos;
			}

			player_has_ball = 1;

		}
	}
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

	GLfloat grassColor[] = {
		0.0f, 1.0f, 0.0f,  // �ʷϻ�
		0.0f, 1.0f, 0.0f,  // �ʷϻ�
		0.0f, 1.0f, 0.0f,  // �ʷϻ�
		0.0f, 1.0f, 0.0f   // �ʷϻ�
	};

	GLfloat grassNormal[] = {
		// x, y, z
		-50.0f, -0.3f, -50.0f,  // ���� �ϴ�
		50.0f, -0.3f, -50.0f,   // ������ �ϴ�
		50.0f, -0.3f, 50.0f,    // ������ ���
		-50.0f, -0.3f, 50.0f    // ���� ���
	};

	GLuint vao_grass, vbo_grass[3];

	glGenVertexArrays(1, &vao_grass); // VAO ����
	glBindVertexArray(vao_grass); // VAO ���ε�

	glGenBuffers(3, vbo_grass); // VBO 3�� ����

	// 1��° VBO: Grass vertices (��ǥ)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_grass[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 2��° VBO: Grass color (����)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_grass[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassColor), grassColor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 3��° VBO: Grass color (����)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_grass[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassNormal), grassNormal, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// glDrawArrays�� �̿��Ͽ� xz ����� �׸���
	glDrawArrays(GL_QUADS, 0, 4); // 4���� �������� �簢�� �׸���

	glBindVertexArray(0); // VAO ���ε� ����
}

void drawPlayer(glm::vec3 ballPos) {
	MovePlayer(ballPos);

	glm::mat4 Trans = glm::mat4(1.0f);
	// �÷��̾� �̵��� ���� ��ġ ������Ʈ
	Trans = glm::translate(Trans, playerPos);
	// ȸ�� ����
	Trans = glm::rotate(Trans, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));  // ȸ�� ����

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	// �÷��̾� �׸���
	glDrawArrays(GL_TRIANGLES, 0, firstObjectVertexCount);
}


void drawBall() {
	MoveBall(playerPos);
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

void drawGoal() {
	glm::mat4 Trans = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Transform = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");


	Scale = glm::scale(Scale, glm::vec3(2.0f, 0.05f, 1.0f));
	Trans = glm::translate(Trans, glm::vec3(0.0f, 2.0f, -30.0f));
	Transform = Trans * Scale;

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount + secondObjectVertexCount, thirdObjectVertexCount[0]);

	Trans = glm::mat4(1.0f);
	Scale = glm::mat4(1.0f);
	Transform = glm::mat4(1.0f);

	Scale = glm::scale(Scale, glm::vec3(0.05f, 1.0f, 1.0f));
	Trans = glm::translate(Trans, glm::vec3(-2.0f, 1.0f, -30.0f));
	Transform = Trans * Scale;

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0], thirdObjectVertexCount[1]);

	Trans = glm::mat4(1.0f);
	Scale = glm::mat4(1.0f);
	Transform = glm::mat4(1.0f);

	Scale = glm::scale(Scale, glm::vec3(0.05f, 1.0f, 1.0f));
	Trans = glm::translate(Trans, glm::vec3(2.0f, 1.0f, -30.0f));
	Transform = Trans * Scale;

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0] + thirdObjectVertexCount[1], thirdObjectVertexCount[2]);

	Trans = glm::mat4(1.0f);
	Scale = glm::mat4(1.0f);
	Transform = glm::mat4(1.0f);

	Scale = glm::scale(Scale, glm::vec3(2.0f, 1.0f, 0.05f));
	Trans = glm::translate(Trans, glm::vec3(0.0f, 1.0f, -31.0f));
	Transform = Trans * Scale;

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0] + thirdObjectVertexCount[1] + thirdObjectVertexCount[2], thirdObjectVertexCount[3]);

	Transform = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));
}
