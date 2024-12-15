//--- 메인 함수
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <tuple>
// --- 구조체
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
//--- 필요 함수
void windowToOpenGL(int window_x, int window_y, int window_width, int window_height, float& gl_x, float& gl_y);
void TimerFunction(int value);
void MakeShape(GLfloat Shape[][3], GLfloat normal[][3], GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, int first_index, std::string shape);
void MakeColor(GLfloat arr[][3], int first_index, int index_count, GLfloat color[3]);
void convertToGLArrays(const ObjData& objData, std::vector<GLfloat>& vertexArray, std::vector<GLfloat>& normalArray);
ObjData parseObj(const std::string& filePath);
void drawGoal();
//--- 필요한 변수 선언
extern GLuint vao, vbo[3];
extern GLint width, height;
extern GLuint shaderProgramID; //--- 세이더 프로그램 이름
extern GLuint vertexShader; //--- 버텍스 세이더 객체
extern GLuint fragmentShader; //--- 프래그먼트 세이더 객체

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
// 키보드 상태
bool keyStates[256] = { false };

// 위치 이동
glm::vec3 playerPos = glm::vec3(0.0f, 0.0f, 0.0f);  // 객체 위치
glm::vec3 ballPos = glm::vec3(0.0f, 0.0f, 0.0f);  // 두 번째 객체 위치

// 공의 속도 및 가속도 변수 추가
glm::vec3 ballVelocity = glm::vec3(0.0f, 0.0f, 0.0f);  // 공의 속도
glm::vec3 ballAcceleration = glm::vec3(0.0f, 0.0f, 0.0f); // 공의 가속도

const float MAX_SPEED = 1.0f; // 공의 최대 속도
const float ACCELERATION = 0.02f; // 가속도
const float FRICTION = 0.98f; // 마찰력 (속도 감소 비율)
const float DECELERATION = 0.001f; // 가속도 제거 비율
const float GRAVITY = -0.05f;  // 중력 값, 음수로 설정하여 아래로 떨어지도록
const float JUMP_STRENGTH = 0.5f;  // z 키를 눌렀을 때 공을 띄우는 힘
const float BALL_BOUNCE_DAMPING = 0.8f;  // 바운스 감쇠 (0~1 사이로 설정, 1이면 반사 후 속도 그대로)

glm::vec3 lastBallDirection = glm::vec3(0.0f, 0.0f, 0.0f);  // 공의 마지막 방향 추적
float shootingPower = 0.0f;  // 슈팅 파워 변수 (0.0 ~ MAX_SHOOTING_POWER 범위)
bool shootingInProgress = false;  // 슈팅 진행 중 여부 (d 키가 눌린 상태인지)
const float MAX_SHOOTING_POWER = 20.0f;  // 최대 슈팅 파워
const float SHOOTING_INCREMENT = 0.1f;  // 슈팅 파워 
const float SHOOTING_DECAY = 0.1f;  // 슈팅 파워 감소량 (d 키를 떼었을 때)

void drawGrass();
void drawPlayer(glm::vec3 ballPos);
void drawBall();
bool player_has_ball = 0;
//------------------------------------------------------------------------
void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao); //--- VAO를 바인드하기

	glGenBuffers(3, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	//--- triShape 배열의 사이즈: 9 * float
	glBufferData(GL_ARRAY_BUFFER, vertexArray.size() * sizeof(GLfloat), vertexArray.data(), GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);

	//--- 3번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- normal 배열의 사이즈: 9 *float
	glBufferData(GL_ARRAY_BUFFER, normalArray.size() * sizeof(GLfloat), normalArray.data(), GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 2번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 2번을 사용 가능하게 함.
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

		std::cout << "----- obj 데이터 파싱 중 -----" << std::endl;

		data = parseObj("player.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		firstObjectVertexCount = vertexArray.size() / 3;

		std::cout << "player 완료" << std::endl;
		std::cout << "vertexcount - " << firstObjectVertexCount << std::endl;

		data = parseObj("ball.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		secondObjectVertexCount = vertexArray.size() / 3 - firstObjectVertexCount;

		std::cout << "ball 완료" << std::endl;
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

		std::cout << "골대 완료" << std::endl;

		std::cout << "----- obj 데이터 파싱 완료 -----" << std::endl;

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

	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glEnable(GL_DEPTH_TEST);   // 깊이 테스트 활성화
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(vao);


	drawBall();
	drawPlayer(ballPos);
	drawGoal();
	drawGrass();


	// 뷰잉 변환
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// 투영 변환
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -4.0));
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	// 조명
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, lightp.x, lightp.y, lightp.z);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, light.x, light.y, light.z);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

	glutSwapBuffers(); // 화면에 출력하기
}

bool sprint = 0;
bool curve = 0;
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
void Keyboard(unsigned char key, int x, int y) {
	switch (key) {

	case 'd':
		// d 키가 눌리면 슈팅 파워 증가
		shootingInProgress = true;
		break;
	case 'D':
		// d 키를 대소문자로 처리, 대소문자 구분
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
		// d 키가 떼어지면 공 발사
		if (shootingInProgress) {
			if (ballPos.y == 0.0f) {  // 공이 바닥에 있을 때만 발사
				ballVelocity = glm::normalize(ballVelocity) * shootingPower;  // 슈팅 파워 적용
				ballVelocity.y = shootingPower / 2.0f;  // 살짝 위로 튕기게 할 수도 있음
			}
			shootingPower = 0.0f;  // 슈팅 파워 초기화
			shootingInProgress = false;  // 슈팅 진행 중 상태 초기화
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
	glutPostRedisplay();  // 화면 갱신
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
	glutPostRedisplay();  // 화면 갱신
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

		// 방향키 입력에 따른 공의 가속도
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

	// 골대의 각 부품 좌표 및 크기
	glm::vec3 goalBarPos = glm::vec3(0.0f, 2.0f, -30.0f);
	glm::vec3 goalBarScale = glm::vec3(2.0f, 0.05f, 1.0f);
	glm::vec3 leftPostPos = glm::vec3(-2.0f, 1.0f, -30.0f);
	glm::vec3 leftPostScale = glm::vec3(0.05f, 1.0f, 1.0f);
	glm::vec3 rightPostPos = glm::vec3(2.0f, 1.0f, -30.0f);
	glm::vec3 rightPostScale = glm::vec3(0.05f, 1.0f, 1.0f);
	glm::vec3 bottomBarPos = glm::vec3(0.0f, 1.0f, -31.0f);
	glm::vec3 bottomBarScale = glm::vec3(2.0f, 1.0f, 0.05f);

	glm::vec3 startPos = ballPos;
	glm::vec3 endPos = ballPos + ballVelocity; // 공의 이동 방향

	// 충돌 체크 
	if (
		(checkSegmentCollision(startPos, endPos, rightPostPos, rightPostScale) && ballPos.z <= -28) ||
		(checkSegmentCollision(startPos, endPos, leftPostPos, leftPostScale) && ballPos.z <= -28) ||
		(checkSegmentCollision(startPos, endPos, goalBarPos, goalBarScale) && ballPos.z <= -30) ||
		(checkSegmentCollision(startPos, endPos, bottomBarPos, bottomBarScale) && ballPos.z <= -30)
		)
	{
		// 충돌 처리
		ballVelocity = -ballVelocity * BALL_BOUNCE_DAMPING;
		std::cout << "충돌 골대" << std::endl;

		// 골대에 들어감
		if (checkSegmentCollision(startPos, endPos, bottomBarPos, bottomBarScale) && ballPos.z <= -30) {
			std::cout << "골" << std::endl;
		}
	}

	// 중력 적용
	ballVelocity.y += GRAVITY;

	// 공의 속도 업데이트
	ballVelocity += ballAcceleration;

	// 최대 속도 제한
	float speed = glm::length(ballVelocity);
	if (speed > MAX_SPEED) {
		ballVelocity = glm::normalize(ballVelocity) * MAX_SPEED;
	}

	// 마찰력 적용
	if (ballPos.y == 0.0f) {
		ballVelocity *= FRICTION;
	}

	// 공이 멈추도록: 일정 속도 이하로 떨어지면 공을 멈춤
	if (glm::length(ballVelocity) < DECELERATION) {
		ballVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	// **Curve 효과** (Z가 눌렸고, curve 상태일 때만)
	if (curve && !player_has_ball && distance <= 15) {
		// X 좌표가 0보다 크면 감소시키고, 작으면 증가시켜 0으로 감아차기 효과
		if (ballPos.x > 0.1f) {
			ballVelocity.x -= CURVE_TURN_SPEED;  // 점진적으로 X 값을 0으로 감아차기
		}
		else if (ballPos.x < -0.1f) {
			ballVelocity.x += CURVE_TURN_SPEED;  // 반대로 X 값을 0으로 감아차기
		}
		// X 좌표가 거의 0에 도달하면, 미세하게 이동하지 않도록
		if (glm::abs(ballPos.x) < 0.05f) {
			ballVelocity.x = 0.0f;  // 감아차기 후 X 속도 0으로
		}
	}

	// 공의 위치 업데이트
	ballPos += ballVelocity;

	// 공이 바닥에 닿으면 반사
	if (ballPos.y < 0.0f) {
		ballPos.y = 0.0f;
		ballVelocity.y = -ballVelocity.y * BALL_BOUNCE_DAMPING;
	}

	// x와 z 경계에 닿으면 반사 처리
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





float playerRotation = 0.0f;  // 초기 값은 0도, z축을 향하도록 설정

void MovePlayer(glm::vec3 ballPos) {
	float moveSpeed = 1.0f;  // 플레이어 기본 이동 속도
	float acceleration = 0.05f;  // 플레이어의 가속도
	const float deceleration = 0.01f; // 감속 (가속도와 반대)
	float MAX_PLAYER_SPEED = 10.0f;
	glm::vec3 playerVelocity = glm::vec3(0.0f, 0.0f, 0.0f);  // 속도
	glm::vec3 moveDirection(0.0f);  // 이동 방향 초기화

	glm::vec3 distanceVec;
	float distance = glm::distance(glm::vec2(playerPos.x, playerPos.z), glm::vec2(ballPos.x, ballPos.z));
	const float maxDistance = 0.5f;  // 공과 플레이어 사이의 최대 거리

	playerPos.y = 0;
	if (sprint) {
		std::cout << "!!!";
		moveSpeed = 10.0f;
		MAX_PLAYER_SPEED = 50.0f;
		acceleration = 0.2f;
	}
		
	if (player_has_ball) {
		// 플레이어와 공 사이의 벡터 차이 계산
		distanceVec = playerPos - ballPos;

		// 거리가 maxDistance를 초과하는 경우
		if (glm::length(distanceVec) > maxDistance) {
			// 벡터를 정규화
			distanceVec = glm::normalize(distanceVec);

			// 플레이어가 공으로 점진적으로 다가가도록 이동
			playerPos.x -= distanceVec.x * 0.07f;  // 이동 속도만큼 플레이어 위치 변경
			playerPos.z -= distanceVec.z * 0.07f;  // 이동 속도만큼 플레이어 위치 변경
		}
	}
	else {
		// 방향키에 따른 플레이어 이동 방향 설정
		if (keyStates[GLUT_KEY_UP]) {
			moveDirection.z -= moveSpeed;  // 뒤쪽으로 이동
		}
		if (keyStates[GLUT_KEY_DOWN]) {
			moveDirection.z += moveSpeed;  // 앞쪽으로 이동
		}
		if (keyStates[GLUT_KEY_LEFT]) {
			moveDirection.x -= moveSpeed;  // 왼쪽으로 이동
		}
		if (keyStates[GLUT_KEY_RIGHT]) {
			moveDirection.x += moveSpeed;  // 오른쪽으로 이동
		}

		// 8방향으로 이동 가능하도록 조정
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
	// 회전 처리
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
	// 가속도를 적용하기 전에 이동 방향이 0이 아닌지 확인
	if (glm::length(moveDirection) > 0.0f) {
		// 이동 방향을 정규화하여 가속도를 적용
		moveDirection = glm::normalize(moveDirection);

		// 현재 속도에 가속도를 적용하여 속도 증가
		playerVelocity += moveDirection * acceleration;  // 가속도 적용

		// 최대 속도를 제한
		if (glm::length(playerVelocity) > MAX_PLAYER_SPEED) {
			playerVelocity = glm::normalize(playerVelocity) * MAX_PLAYER_SPEED;  // 최대 속도 제한
		}
	}
	else {
		// 이동하지 않으면 감속을 적용
		if (glm::length(playerVelocity) > 0.0f) {
			playerVelocity -= glm::normalize(playerVelocity) * deceleration;  // 감속
		}

		// 감속 후 속도가 너무 낮아지면 속도를 0으로 설정
		if (glm::length(playerVelocity) < 0.001f) {
			playerVelocity = glm::vec3(0.0f);
		}
	}

	// 속도를 기준으로 플레이어 위치 업데이트
	playerPos += playerVelocity;  // 현재 속도를 반영하여 플레이어 위치 이동

	// 모델 변환 행렬 업데이트
	glm::mat4 modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, playerPos);  // 위치 이동

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));

	//if (!player_has_ball) {
		// 카메라 설정: 플레이어를 따라가는 카메라
	cameraPos = playerPos + glm::vec3(0.0f, 1.0f, 5.0f);  // 플레이어 위치 기준으로 카메라 위치 설정 (위 2, 뒤 5)
	cameraDirection = playerPos;  // 카메라는 플레이어를 향하도록 설정

	// 카메라의 위치와 방향을 바탕으로 뷰 변환 행렬을 계산
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
	// xz 평면의 범위를 넓혀서 그리기
	GLfloat grassVertices[] = {
		// x, y, z
		-50.0f, -0.3f, -50.0f,  // 왼쪽 하단
		50.0f, -0.3f, -50.0f,   // 오른쪽 하단
		50.0f, -0.3f, 50.0f,    // 오른쪽 상단
		-50.0f, -0.3f, 50.0f    // 왼쪽 상단
	};

	GLfloat grassColor[] = {
		0.0f, 1.0f, 0.0f,  // 초록색
		0.0f, 1.0f, 0.0f,  // 초록색
		0.0f, 1.0f, 0.0f,  // 초록색
		0.0f, 1.0f, 0.0f   // 초록색
	};

	GLfloat grassNormal[] = {
		// x, y, z
		-50.0f, -0.3f, -50.0f,  // 왼쪽 하단
		50.0f, -0.3f, -50.0f,   // 오른쪽 하단
		50.0f, -0.3f, 50.0f,    // 오른쪽 상단
		-50.0f, -0.3f, 50.0f    // 왼쪽 상단
	};

	GLuint vao_grass, vbo_grass[3];

	glGenVertexArrays(1, &vao_grass); // VAO 생성
	glBindVertexArray(vao_grass); // VAO 바인드

	glGenBuffers(3, vbo_grass); // VBO 3개 생성

	// 1번째 VBO: Grass vertices (좌표)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_grass[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 2번째 VBO: Grass color (색상)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_grass[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassColor), grassColor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 3번째 VBO: Grass color (색상)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_grass[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassNormal), grassNormal, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// glDrawArrays를 이용하여 xz 평면을 그린다
	glDrawArrays(GL_QUADS, 0, 4); // 4개의 정점으로 사각형 그리기

	glBindVertexArray(0); // VAO 바인딩 해제
}

void drawPlayer(glm::vec3 ballPos) {
	MovePlayer(ballPos);

	glm::mat4 Trans = glm::mat4(1.0f);
	// 플레이어 이동을 위한 위치 업데이트
	Trans = glm::translate(Trans, playerPos);
	// 회전 적용
	Trans = glm::rotate(Trans, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));  // 회전 적용

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	// 플레이어 그리기
	glDrawArrays(GL_TRIANGLES, 0, firstObjectVertexCount);
}


void drawBall() {
	MoveBall(playerPos);
	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 S = glm::mat4(1.0f);
	glm::mat4 Trans = glm::mat4(1.0f);

	// 두 번째 객체(공) 이동을 위한 위치 업데이트
	T = glm::translate(T, ballPos);  // 두 번째 객체의 위치 적용
	S = glm::scale(S, glm::vec3(0.01f, 0.01f, 0.01f));  // 크기 조정
	Trans = T * S;

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	// 두 번째 객체(공) 그리기
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
