//--- 메인 함수
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
//--- 필요 함수
void windowToOpenGL(int window_x, int window_y, int window_width, int window_height, float& gl_x, float& gl_y);
void TimerFunction(int value);
bool isCollision(const struct Rectangle& rect1, const struct Rectangle& rect2);
void MakeShape(GLfloat Shape[][3], GLfloat normal[][3], GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, int first_index, std::string shape);
void MakeColor(GLfloat arr[][3], int first_index, int index_count, GLfloat color[3]);
//--- 필요한 변수 선언
extern GLuint vao, vbo[3];
extern GLint width, height;
extern GLuint shaderProgramID; //--- 세이더 프로그램 이름
extern GLuint vertexShader; //--- 버텍스 세이더 객체
extern GLuint fragmentShader; //--- 프래그먼트 세이더 객체

//------------------------------------------------------------------------------------------------------

GLfloat Shape[10000][3]{};
GLfloat colors[10000][3]{};
GLfloat normal[10000][3]{};
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.5f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 light = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightp = glm::vec3(2.0f, 2.0f, 0.0f);
GLfloat ryradians{};

void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao); //--- VAO를 바인드하기

	glGenBuffers(3, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	//--- triShape 배열의 사이즈: 9 * float
	glBufferData(GL_ARRAY_BUFFER, 30000 * sizeof(GLfloat), Shape, GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);
	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 30000 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 30000 * sizeof(GLfloat), normal, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 2번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(2);

	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, lightp.x, lightp.y, lightp.z);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, light.x, light.y, light.z);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);
}

int once = 0;
GLfloat red = 0.5f;
GLfloat blue = 0.5f;
GLfloat green = 0.5f;
bool left_button = 0;
GLfloat redcolor[3] = { 1.0f, 0.0f, 0.0f };
GLfloat bluecolor[3] = { 0.0f, 0.0f, 1.0f };
GLfloat greencolor[3] = { 0.0f, 1.0f, 0.0f };
GLfloat pinkcolor[3] = { 1.0f, 0.0f, 1.0f };
GLfloat skycolor[3] = { 0.0f, 1.0f, 1.0f };
GLfloat yellowcolor[3] = { 1.0f, 1.0f, 0.0f };
GLfloat Long{ 2.0f }, dx{}, yradians{}, yradians2{}, dx2{}, zradians{}, zradians2{};
bool bcount{}, Bcount{}, mcount{}, Mcount{}, fcount{}, Fcount{}, ecount{}, Ecount{}, tcount{}, Tcount{}, acount{}, Acount{}, dcount{}, Dcount{}, rcount{}, Rcount{};
GLfloat camerax{}, cameray{}, cameraz{ 1.0f }, cameraR{}, camerar{};
int ccount{};
//cube 36
//pyramid 16
//cone 1263
//sphere 1008

GLvoid drawScene()
{
	if (once == 0) {
		once = 1;
		Shape[0][0] = -Long;
		Shape[0][2] = -Long;
		Shape[1][0] = -Long;
		Shape[1][2] = Long;
		Shape[2][0] = Long;
		Shape[2][2] = -Long;
		Shape[3][0] = -Long;
		Shape[3][2] = Long;
		Shape[4][0] = Long;
		Shape[4][2] = -Long;
		Shape[5][0] = Long;
		Shape[5][2] = Long;
		for (int i = 0; i < 6; ++i) {
			normal[i][0] = 0;
			normal[i][1] = 1.0;
			normal[i][2] = 0;
		}
		colors[0][0] = 1.0f;
		colors[1][1] = 1.0f;
		colors[2][2] = 1.0f;
		colors[3][1] = 1.0f;
		colors[4][2] = 1.0f;
		colors[5][0] = 1.0f;
		colors[5][1] = 1.0f;
		MakeShape(Shape, normal, 0.1, 0.2, 0.1, -0.1, 0, -0.1, 6, "cube");
		MakeShape(Shape, normal, 0.1, 0.2, 0.1, -0.1, 0, -0.1, 6 + 36, "cube");
		MakeShape(Shape, normal, 0.1, 0.2, 0.1, -0.1, 0, -0.1, 6 + 36 * 2, "cube");
		MakeShape(Shape, normal, 0.1, 0.2, 0.1, -0.1, 0, -0.1, 6 + 36 * 3, "cube");
		MakeShape(Shape, normal, 0.1, 0.2, 0.1, -0.1, 0, -0.1, 6 + 36 * 4, "cube");
		MakeShape(Shape, normal, 0.1, 0.2, 0.1, -0.1, 0, -0.1, 6 + 36 * 5, "cube");
		for (int i = 0; i < 6 + 36 * 5; i += 36) {
			MakeColor(colors, 6 + i, 6, redcolor);
			MakeColor(colors, 12 + i, 6, bluecolor);
			MakeColor(colors, 18 + i, 6, greencolor);
			MakeColor(colors, 24 + i, 6, yellowcolor);
			MakeColor(colors, 30 + i, 6, skycolor);
			MakeColor(colors, 36 + i, 6, pinkcolor);
		}
		glutTimerFunc(25, TimerFunction, 1);
		InitBuffer();
	}
	//--- 변경된 배경색 설정
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao);

	glm::mat4 R = glm::mat4(1.0f);
	glm::mat4 R2 = glm::mat4(1.0f);
	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 S = glm::mat4(1.0f);
	glm::mat4 Trans = glm::mat4(1.0f);
	glm::mat4 TX = glm::mat4(1.0f);
	glm::mat4 RY2 = glm::mat4(1.0f);
	glm::mat4 TX2 = glm::mat4(1.0f);
	glm::mat4 RY = glm::mat4(1.0f);
	glm::mat4 RZ = glm::mat4(1.0f);
	glm::mat4 T2 = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");

	// 바닥
	Trans = R2 * R;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	// 아래몸체
	S = glm::mat4(1.0f);
	Trans = glm::mat4(1.0f);

	TX = glm::translate(TX, glm::vec3(dx, 0.0, 0.0));
	S = glm::scale(S, glm::vec3(4.0f, 1.0f, 4.0f));
	Trans = R2 * R * TX * S;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	glDrawArrays(GL_TRIANGLES, 6, 36);

	// 아래 포신 1
	T = glm::mat4(1.0f);
	S = glm::mat4(1.0f);
	Trans = glm::mat4(1.0f);
	RY2 = glm::mat4(1.0f);
	TX2 = glm::mat4(1.0f);

	S = glm::scale(S, glm::vec3(0.5f, 0.5f, 2.0f));
	T = glm::translate(T, glm::vec3(0.2f, 0.0f, 0.6f));
	RY2 = glm::rotate(RY2, glm::radians(yradians2), glm::vec3(0.0f, 0.1f, 0.0f));
	TX2 = glm::translate(TX2, glm::vec3(-dx2, 0.0f, 0.0f));
	Trans = R2 * R * TX2 * TX * T * RY2 * S;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	glDrawArrays(GL_TRIANGLES, 6 + 36 * 2, 36);

	// 아래 포신 2
	T = glm::mat4(1.0f);
	S = glm::mat4(1.0f);
	Trans = glm::mat4(1.0f);
	RY2 = glm::mat4(1.0f);
	TX2 = glm::mat4(1.0f);

	RY2 = glm::rotate(RY2, glm::radians(-yradians2), glm::vec3(0.0f, 0.1f, 0.0f));
	TX2 = glm::translate(TX2, glm::vec3(dx2, 0.0f, 0.0f));
	S = glm::scale(S, glm::vec3(0.5f, 0.5f, 2.0f));
	T = glm::translate(T, glm::vec3(-0.2f, 0.0f, 0.6f));
	Trans = R2 * R * TX2 * TX * T * RY2 * S;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	glDrawArrays(GL_TRIANGLES, 6 + 36 * 3, 36);

	// 윗몸체

	T = glm::mat4(1.0f);
	S = glm::mat4(1.0f);
	Trans = glm::mat4(1.0f);
	RY = glm::mat4(1.0f);

	S = glm::scale(S, glm::vec3(2.0f, 1.0f, 2.0f));
	T = glm::translate(T, glm::vec3(0.0f, 0.2f, 0.0f));
	RY = glm::rotate(RY, glm::radians(yradians), glm::vec3(0.0f, 1.0f, 0.0f));
	Trans = R2 * R * TX * T * S * RY;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	glDrawArrays(GL_TRIANGLES, 6 + 36, 36);

	// 위 포신 1
	RZ = glm::mat4(1.0f);
	T2 = glm::mat4(1.0f);
	S = glm::mat4(1.0f);
	Trans = glm::mat4(1.0f);

	RZ = glm::rotate(RZ, glm::radians(-zradians - zradians2), glm::vec3(0.0f, 0.0f, 0.1f));
	S = glm::scale(S, glm::vec3(0.5f, 2.0f, 0.5f));
	T2 = glm::translate(T2, glm::vec3(0.1f, 0.2f, 0.0f));
	Trans = R2 * R * TX * T * RY * T2 * RZ * S;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	glDrawArrays(GL_TRIANGLES, 6 + 36 * 4, 36);

	// 위 포신 2
	T2 = glm::mat4(1.0f);
	S = glm::mat4(1.0f);
	Trans = glm::mat4(1.0f);
	RZ = glm::mat4(1.0f);

	RZ = glm::rotate(RZ, glm::radians(zradians + zradians2), glm::vec3(0.0f, 0.0f, 0.1f));
	S = glm::scale(S, glm::vec3(0.5f, 2.0f, 0.5f));
	T2 = glm::translate(T2, glm::vec3(-0.1f, 0.2f, 0.0f));
	Trans = R2 * R * TX * T * RY * T2 * RZ * S;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	glDrawArrays(GL_TRIANGLES, 6 + 36 * 5, 36);

	// 뷰잉 변환
	view = glm::mat4(1.0f);
	glm::mat4 cameraT(1.0f);
	glm::mat4 cameraT2(1.0f);
	glm::mat4 cameraR2(1.0f);
	glm::vec3 targetDirection(1.0f);
	glm::mat4 rotateDirection(1.0f);

	cameraT = glm::translate(cameraT, glm::vec3(camerax, cameray, cameraz));
	cameraT2 = glm::translate(cameraT2, glm::vec3(-camerax, -cameray, -cameraz));
	cameraR2 = glm::rotate(cameraR2, glm::radians(camerar), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 cameraRotate = glm::vec3(1.0f);
	cameraPos = glm::vec3(camerax, cameray, cameraz);
	cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(cameraR), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);

	cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// 투영 변환
	projection = glm::mat4(1.0f);

	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -4.0));
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	T = glm::mat4(1.0f);
	T2 = glm::mat4(1.0f);
	R = glm::mat4(1.0f);
	Trans = glm::mat4(1.0f);

	T = glm::translate(T, glm::vec3(light.x, light.y, light.z));
	R = glm::rotate(R, glm::radians(ryradians), glm::vec3(0.0f, 1.0f, 0.0f));
	T2 = glm::translate(T2, glm::vec3(-light.x, -light.y, -light.z));
	Trans = R;

	lightp = Trans * glm::vec4(lightp, 1.0f);
	ryradians = fmod(ryradians, 2 * 3.141592);

	float ambientStrength = 0.3f; // 기본 값 (0.0으로 설정하면 끔)
	unsigned int ambientStrengthLocation = glGetUniformLocation(shaderProgramID, "ambientStrength");
	glUniform1f(ambientStrengthLocation, ambientStrength);

	if (mcount)
		glUniform1f(ambientStrengthLocation, 0.0f);
	else
		glUniform1f(ambientStrengthLocation, 0.3f);

	InitBuffer();

	glutSwapBuffers(); // 화면에 출력하기
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'r':
		rcount = not rcount;
		break;
	case 'R':
		Rcount = not Rcount;
		break;
	case 'c':
		ccount++;
		if (ccount == 4)
			ccount = 0;
		if (ccount == 1) {
			light.x = 1.0f;
			light.y = 0.0f;
			light.z = 0.0f;
		}
		else if (ccount == 2) {
			light.x = 0.0f;
			light.y = 1.0f;
			light.z = 0.0f;
		}
		else if (ccount == 3) {
			light.x = 0.0f;
			light.y = 0.0f;
			light.z = 1.0f;
		}
		else if (ccount == 0) {
			light.x = 1.0f;
			light.y = 1.0f;
			light.z = 1.0f;
		}
		InitBuffer();
		break;
	case 'd':
		dcount = not dcount;
		break;
	case 'D':
		Dcount = not Dcount;
		break;
	case 'z':
		cameraz += 0.1f;
		break;
	case 'Z':
		cameraz -= 0.1f;
		break;
	case 'y':
		cameray += 0.1f;
		break;
	case 'Y':
		cameray -= 0.1;
		break;
	case 'x':
		camerax += 0.1f;
		break;
	case 'X':
		camerax -= 0.1f;
		break;
	case 'A':
		Acount = not Acount;
		break;
	case 'm':
		mcount = not mcount;
		break;
	case 'M':
		Mcount = not Mcount;
		break;
	case 'f':
		fcount = not fcount;
		break;
	case 'F':
		Fcount = not Fcount;
		break;
	case 'e':
		ecount = 1;
		break;
	case 'E':
		ecount = 0;
		break;
	case 't':
		tcount = not tcount;
		break;
	case 'T':
		Tcount = not Tcount;
		break;
	case 'b':
		bcount = not bcount;
		break;
	case 'B':
		Bcount = not Bcount;
		break;
	case 's':
		bcount = 0;
		mcount = 0;
		fcount = 0;
		ecount = 0;
		tcount = 0;
		Bcount = 0;
		Mcount = 0;
		Fcount = 0;
		Ecount = 0;
		Tcount = 0;
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
	if (rcount)
		ryradians++;
	if (Rcount)
		ryradians--;
	if (bcount)
		dx += 0.005f;
	if (Bcount)
		dx -= 0.005f;
	if (Mcount)
		yradians--;
	if (fcount)
		yradians2++;
	if (Fcount)
		yradians2--;
	if (ecount) {
		if (yradians2 < 0) {
			yradians2++;
			if (fcount)
				yradians2++;
		}
		if (yradians2 > 0) {
			yradians2--;
			if (fcount)
				yradians2--;
		}
		if (dx2 <= 0.2f)
			dx2 += 0.005f;
	}
	else {
		if (dx2 >= 0)
			dx2 -= 0.005f;
	}
	if (tcount) {
		if (zradians < 90)
			zradians++;
	}
	else if (tcount == 0) {
		if (zradians > 0)
			zradians--;
	}
	if (Tcount) {
		if (zradians2 > -90)
			zradians2--;
	}
	else if (Tcount == 0) {
		if (zradians2 < 0)
			zradians2++;
	}
	if (Acount)
		cameraR--;
	if (acount)
		cameraR++;
	if (dcount)
		camerar++;
	if (Dcount)
		camerar--;
	if (yradians2 == 360 || yradians2 == -360)
		yradians2 = 0;
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
void SpecialKeys(int key, int x, int y) {
	switch (key) {
	}
	glutPostRedisplay();
}