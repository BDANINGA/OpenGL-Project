//--- ���� �Լ�
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
//--- �ʿ� �Լ�
void windowToOpenGL(int window_x, int window_y, int window_width, int window_height, float& gl_x, float& gl_y);
void TimerFunction(int value);
bool isCollision(const struct Rectangle& rect1, const struct Rectangle& rect2);
void MakeShape(GLfloat Shape[][3], GLfloat normal[][3], GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, int first_index, std::string shape);
void MakeColor(GLfloat arr[][3], int first_index, int index_count, GLfloat color[3]);
//--- �ʿ��� ���� ����
extern GLuint vao, vbo[3];
extern GLint width, height;
extern GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
extern GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
extern GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

//------------------------------------------------------------------------------------------------------

GLfloat Shape[10000][3]{};
GLfloat colors[10000][3]{};
GLfloat normal[10000][3]{};
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.5f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 light = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightp = glm::vec3(2.0f, 2.0f, 0.0f);

void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�

	glGenBuffers(3, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	//--- triShape �迭�� ������: 9 * float
	glBufferData(GL_ARRAY_BUFFER, 30000 * sizeof(GLfloat), Shape, GL_STATIC_DRAW);
	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 0���� ��밡���ϰ� ��
	glEnableVertexAttribArray(0);
	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- colors �迭�� ������: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 30000 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 1���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(1);

	//--- 3��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- normal �迭�� ������: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 30000 * sizeof(GLfloat), normal, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 2���� ����Ѵ�: ���ؽ� �� 3*float
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 2���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(2);
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
GLfloat Long{ 2.0f };
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
		glutTimerFunc(25, TimerFunction, 1);
		InitBuffer();
	}
	//--- ����� ���� ����
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
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");

	// �ٴ�
	Trans = R2 * R;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	// ���� ��ȯ
	glm::mat4 view = glm::mat4(1.0f);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// ���� ��ȯ
	glm::mat4 projection = glm::mat4(1.0f);

	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
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
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'y':
		cameraPos.y += 0.1f;
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