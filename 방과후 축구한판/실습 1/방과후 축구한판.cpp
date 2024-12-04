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
//--- 필요한 변수 선언
extern GLuint vao, vbo[3];
extern GLint width, height;
extern GLuint shaderProgramID; //--- 세이더 프로그램 이름
extern GLuint vertexShader; //--- 버텍스 세이더 객체
extern GLuint fragmentShader; //--- 프래그먼트 세이더 객체

//------------------------------------------------------------------------------------------------------

GLfloat colors[10000][3]{};
std::vector<GLfloat> vertexArray;
std::vector<GLfloat> normalArray;
ObjData data{};
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 light = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lightp = glm::vec3(2.0f, 2.0f, 0.0f);

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
GLfloat red = 1.0f;
GLfloat blue = 1.0f;
GLfloat green = 1.0f;
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

GLvoid drawScene()
{
	if (once == 0) {
		once = 1;
		
		std::cout << "obj 데이터 파싱 중" << std::endl;

		data = parseObj("player.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		firstObjectVertexCount = vertexArray.size() / 3;

		data = parseObj("ball.obj");
		convertToGLArrays(data, vertexArray, normalArray);
		secondObjectVertexCount = vertexArray.size() / 3 - firstObjectVertexCount;

		std::cout << "obj 데이터 파싱 완료" << std::endl;

		glutTimerFunc(25, TimerFunction, 1);
		InitBuffer();
	}
	//--- 변경된 배경색 설정
	glClearColor(red, blue, green, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glEnable(GL_DEPTH_TEST);   // 깊이 테스트 활성화
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(vao);

	glm::mat4 R = glm::mat4(1.0f);
	glm::mat4 R2 = glm::mat4(1.0f);
	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 S = glm::mat4(1.0f);
	glm::mat4 Trans = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));
	glDrawArrays(GL_TRIANGLES, 0, firstObjectVertexCount);

	T = glm::translate(T, glm::vec3(0.0f, 0.0f, 0.5f));
	S = glm::scale(S, glm::vec3(0.01f, 0.01f, 0.01f));
	Trans = T * S;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount, secondObjectVertexCount);


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
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'z':
		cameraPos.z += 0.1f;
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
	case 'r': // 카메라 원점으로 이동
		cameraPos = glm::vec3(0.0f, 0.0f, 0.0f); // 카메라 원점으로 리셋
		cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); // 카메라의 목표 방향도 원점으로
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
void SpecialKeys(int key, int x, int y) {
	switch (key) {
	}
	glutPostRedisplay();
}