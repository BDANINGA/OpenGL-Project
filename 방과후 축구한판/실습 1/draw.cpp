#define _CRT_SECURE_NO_WARNINGS
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

void MovePlayer(glm::vec3 ballPos);
void MoveBall(glm::vec3& playerPos, glm::vec3 keeperPos);
void MoveKeeper(glm::vec3 ballPos, glm::vec3& keeperPos);
void deleteTexture(GLuint textureID);
GLuint loadBMP(const char* filepath);

extern glm::vec3 playerPos;
extern glm::vec3 ballPos;
extern glm::vec3 keeperPos;
extern float playerRotation;
extern GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
extern int firstObjectVertexCount, secondObjectVertexCount, thirdObjectVertexCount[4];
extern float rotationAngle;  // ȸ�� ���� (����)
extern glm::vec3 rotationDirection;

void deleteTexture(GLuint textureID) {
	glDeleteTextures(1, &textureID);
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

	GLuint playerTextures = loadBMP("�÷��̾� ��.bmp");
	glActiveTexture(GL_TEXTURE0);      // �ؽ�ó ����
	glBindTexture(GL_TEXTURE_2D, playerTextures); // �ؽ�ó ID ���

	// ���̴��� �ؽ�ó ���� 0�� ����
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // ���� 0�� grassTexture�� ����

	// �÷��̾� �׸���
	glDrawArrays(GL_TRIANGLES, 0, firstObjectVertexCount);

	deleteTexture(playerTextures);
}

void drawBall(glm::vec3 keeperPos) {
	MoveBall(playerPos, keeperPos);

	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 S = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);  // ȸ�� ���
	glm::mat4 Trans = glm::mat4(1.0f);

	// ȸ�� ���� (ȸ�� ���� ���� ����)
	R = glm::rotate(R, rotationAngle, rotationDirection);  // ���⿡ ���� ȸ��

	// �� ��° ��ü(��) �̵��� ���� ��ġ ������Ʈ
	T = glm::translate(T, ballPos);  // �� ��° ��ü�� ��ġ ����
	S = glm::scale(S, glm::vec3(0.01f, 0.01f, 0.01f));  // ũ�� ����
	Trans = T * R * S; // ��ġ, ȸ��, ũ�� ������ ����

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	GLuint ballTextures = loadBMP("�౸��.bmp");
	glActiveTexture(GL_TEXTURE0);      // �ؽ�ó ����
	glBindTexture(GL_TEXTURE_2D, ballTextures); // �ؽ�ó ID ���

	// ���̴��� �ؽ�ó ���� 0�� ����
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // ���� 0�� grassTexture�� ����

	// �� ��° ��ü(��) �׸���
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount, secondObjectVertexCount);

	deleteTexture(ballTextures);
}
void drawKeeper(glm::vec3 ballPos, glm::vec3& keeperPos) {
	MoveKeeper(ballPos, keeperPos);
	glm::mat4 Trans = glm::mat4(1.0f);
	// �÷��̾� �̵��� ���� ��ġ ������Ʈ
	Trans = glm::translate(Trans, keeperPos);
	// ȸ�� ����
	//Trans = glm::rotate(Trans, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));  // ȸ�� ����

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	GLuint keeperTextures = loadBMP("�÷��̾� ��.bmp");
	glActiveTexture(GL_TEXTURE0);      // �ؽ�ó ����
	glBindTexture(GL_TEXTURE_2D, keeperTextures); // �ؽ�ó ID ���

	// ���̴��� �ؽ�ó ���� 0�� ����
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // ���� 0�� grassTexture�� ����

	// �÷��̾� �׸���
	glDrawArrays(GL_TRIANGLES, 0, firstObjectVertexCount);

	deleteTexture(keeperTextures);
}

void drawGoal() {
	glm::mat4 Trans = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Transform = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");


	Scale = glm::scale(Scale, glm::vec3(2.0f, 0.05f, 1.0f));
	Trans = glm::translate(Trans, glm::vec3(0.0f, 2.0f, -35.0f));
	Transform = Trans * Scale;

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));

	GLuint goalTextures = loadBMP("��� ��.bmp");
	glActiveTexture(GL_TEXTURE0);      // �ؽ�ó ����
	glBindTexture(GL_TEXTURE_2D, goalTextures); // �ؽ�ó ID ���

	// ���̴��� �ؽ�ó ���� 0�� ����
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // ���� 0�� grassTexture�� ����

	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount + secondObjectVertexCount, thirdObjectVertexCount[0]);

	Trans = glm::mat4(1.0f);
	Scale = glm::mat4(1.0f);
	Transform = glm::mat4(1.0f);

	Scale = glm::scale(Scale, glm::vec3(0.05f, 1.0f, 1.0f));
	Trans = glm::translate(Trans, glm::vec3(-2.0f, 1.0f, -35.0f));
	Transform = Trans * Scale;

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0], thirdObjectVertexCount[1]);

	Trans = glm::mat4(1.0f);
	Scale = glm::mat4(1.0f);
	Transform = glm::mat4(1.0f);

	Scale = glm::scale(Scale, glm::vec3(0.05f, 1.0f, 1.0f));
	Trans = glm::translate(Trans, glm::vec3(2.0f, 1.0f, -35.0f));
	Transform = Trans * Scale;

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0] + thirdObjectVertexCount[1], thirdObjectVertexCount[2]);

	Trans = glm::mat4(1.0f);
	Scale = glm::mat4(1.0f);
	Transform = glm::mat4(1.0f);

	Scale = glm::scale(Scale, glm::vec3(2.0f, 1.0f, 0.05f));
	Trans = glm::translate(Trans, glm::vec3(0.0f, 1.0f, -36.0f));
	Transform = Trans * Scale;

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount + secondObjectVertexCount + thirdObjectVertexCount[0] + thirdObjectVertexCount[1] + thirdObjectVertexCount[2], thirdObjectVertexCount[3]);

	Transform = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Transform));

	deleteTexture(goalTextures);
}
void drawGrass() {
	// xz ����� ������ ������ �׸���
	GLfloat grassVertices[] = {
		// x, y, z
		-20.0f, -0.3f, -40.0f,  // ���� �ϴ�
		20.0f, -0.3f, -40.0f,   // ������ �ϴ�
		20.0f, -0.3f, 40.0f,    // ������ ���
		-20.0f, -0.3f, 40.0f    // ���� ���
	};

	GLfloat grassColor[] = {
		1.0f, 1.0f, 1.0f,  // �ʷϻ�
		1.0f, 1.0f, 1.0f,  // �ʷϻ�
		1.0f, 1.0f, 1.0f,  // �ʷϻ�
		1.0f, 1.0f, 1.0f   // �ʷϻ�
	};

	GLfloat grassNormal[] = {
		// x, y, z
		0.0f, 0.0f, 0.0f,  // ���� �ϴ�
		0.0f, 0.0f, 0.0f,  // ������ �ϴ�
		0.0f, 0.0f, 0.0f,  // ������ ���
		0.0f, 0.0f, 0.0f   // ���� ���
	};

	GLfloat grassTexture[] = {
		// x, y, z
		1.0f, 0.0f,  // ���� �ϴ�
		1.0f, 1.0f,   // ������ �ϴ�
		0.0f, 1.0f,    // ������ ���
		0.0f, 0.0f    // ���� ���
	};

	GLuint vao_grass, vbo_grass[4];

	glGenVertexArrays(1, &vao_grass); // VAO ����
	glBindVertexArray(vao_grass); // VAO ���ε�

	glGenBuffers(4, vbo_grass); // VBO 4�� ����

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

	// 4��° VBO: Grass color (����)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_grass[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassTexture), grassTexture, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);



	GLuint grassTextures = loadBMP("�౸��.bmp");
	glActiveTexture(GL_TEXTURE0);      // �ؽ�ó ����
	glBindTexture(GL_TEXTURE_2D, grassTextures); // �ؽ�ó ID ���

	// ���̴��� �ؽ�ó ���� 0�� ����
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // ���� 0�� grassTexture�� ����

	// glDrawArrays�� �̿��Ͽ� xz ����� �׸���
	glDrawArrays(GL_QUADS, 0, 4); // 4���� �������� �簢�� �׸���

	// VAO, VBO ����
	glBindVertexArray(0);
	glDeleteBuffers(4, vbo_grass);
	glDeleteVertexArrays(1, &vao_grass);

	deleteTexture(grassTextures);
}

void drawBackground(int i) {
	// xz ����� ������ ������ �׸���
	if (i == 0) {
		GLfloat backgroundVertices[] = {
			// x, y, z
			-20.0f, -0.3f, -40.0f,  // ���� �ϴ�
			20.0f, -0.3f, -40.0f,   // ������ �ϴ�
			20.0f, 30.0f, -40.0f,    // ������ ���
			-20.0f, 30.0f, -40.0f    // ���� ���
		};

		GLfloat backgroundColor[] = {
			1.0f, 1.0f, 1.0f,  // �ʷϻ�
			1.0f, 1.0f, 1.0f,  // �ʷϻ�
			1.0f, 1.0f, 1.0f,  // �ʷϻ�
			1.0f, 1.0f, 1.0f   // �ʷϻ�
		};

		GLfloat backgroundNormal[] = {
			// x, y, z
			0.0f, 1.0f, 0.0f,  // ���� �ϴ�
			0.0f, 1.0f, 0.0f,  // ������ �ϴ�
			0.0f, 1.0f, 0.0f,  // ������ ���
			0.0f, 1.0f, 0.0f   // ���� ���
		};

		GLfloat backgroundTexture[] = {
			// x, y, z
			0.0f, 0.0f,  // ���� �ϴ�
			1.0f, 0.0f,   // ������ �ϴ�
			1.0f, 1.0f,    // ������ ���
			0.0f, 1.0f    // ���� ���
		};

		GLuint vao_background, vbo_background[4];

		glGenVertexArrays(1, &vao_background); // VAO ����
		glBindVertexArray(vao_background); // VAO ���ε�

		glGenBuffers(4, vbo_background); // VBO 4�� ����

		// 1��° VBO: Grass vertices (��ǥ)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// 2��° VBO: Grass color (����)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundColor), backgroundColor, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// 3��° VBO: Grass color (����)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundNormal), backgroundNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		// 4��° VBO: Grass color (����)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundTexture), backgroundTexture, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);



		GLuint backgroundTextures = loadBMP("���1.bmp");
		glActiveTexture(GL_TEXTURE0);      // �ؽ�ó ����
		glBindTexture(GL_TEXTURE_2D, backgroundTextures); // �ؽ�ó ID ���

		// ���̴��� �ؽ�ó ���� 0�� ����
		GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
		glUniform1i(texLocation, 0);  // ���� 0�� backgroundTexture�� ����

		// glDrawArrays�� �̿��Ͽ� xz ����� �׸���
		glDrawArrays(GL_QUADS, 0, 4); // 4���� �������� �簢�� �׸���

		// VAO, VBO ����
		glBindVertexArray(0);
		glDeleteBuffers(4, vbo_background);
		glDeleteVertexArrays(1, &vao_background);

		deleteTexture(backgroundTextures);
	}
	else if (i == 1) {
		GLfloat backgroundVertices[] = {
			// x, y, z
			-20.0f, -0.3f, 40.0f,  // ���� �ϴ�
			-20.0f, -0.3f, -40.0f,   // ������ �ϴ�
			-20.0f, 30.0f, -40.0f,    // ������ ���
			-20.0f, 30.0f, 40.0f    // ���� ���
		};

		GLfloat backgroundColor[] = {
			1.0f, 1.0f, 1.0f,  // �ʷϻ�
			1.0f, 1.0f, 1.0f,  // �ʷϻ�
			1.0f, 1.0f, 1.0f,  // �ʷϻ�
			1.0f, 1.0f, 1.0f   // �ʷϻ�
		};

		GLfloat backgroundNormal[] = {
			// x, y, z
			0.0f, 1.0f, 0.0f,  // ���� �ϴ�
			0.0f, 1.0f, 0.0f,  // ������ �ϴ�
			0.0f, 1.0f, 0.0f,  // ������ ���
			0.0f, 1.0f, 0.0f   // ���� ���
		};

		GLfloat backgroundTexture[] = {
			// x, y, z
			0.0f, 0.0f,  // ���� �ϴ�
			1.0f, 0.0f,   // ������ �ϴ�
			1.0f, 1.0f,    // ������ ���
			0.0f, 1.0f    // ���� ���
		};

		GLuint vao_background, vbo_background[4];

		glGenVertexArrays(1, &vao_background); // VAO ����
		glBindVertexArray(vao_background); // VAO ���ε�

		glGenBuffers(4, vbo_background); // VBO 4�� ����

		// 1��° VBO: Grass vertices (��ǥ)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// 2��° VBO: Grass color (����)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundColor), backgroundColor, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// 3��° VBO: Grass color (����)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundNormal), backgroundNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		// 4��° VBO: Grass color (����)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundTexture), backgroundTexture, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);



		GLuint backgroundTextures = loadBMP("���1.bmp");
		glActiveTexture(GL_TEXTURE0);      // �ؽ�ó ����
		glBindTexture(GL_TEXTURE_2D, backgroundTextures); // �ؽ�ó ID ���

		// ���̴��� �ؽ�ó ���� 0�� ����
		GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
		glUniform1i(texLocation, 0);  // ���� 0�� backgroundTexture�� ����

		// glDrawArrays�� �̿��Ͽ� xz ����� �׸���
		glDrawArrays(GL_QUADS, 0, 4); // 4���� �������� �簢�� �׸���

		// VAO, VBO ����
		glBindVertexArray(0);
		glDeleteBuffers(4, vbo_background);
		glDeleteVertexArrays(1, &vao_background);

		deleteTexture(backgroundTextures);
	}
	else if (i == 2) {
		GLfloat backgroundVertices[] = {
			// x, y, z
			20.0f, -0.3f, 40.0f,  // ���� �ϴ�
			20.0f, -0.3f, -40.0f,   // ������ �ϴ�
			20.0f, 30.0f, -40.0f,    // ������ ���
			20.0f, 30.0f, 40.0f    // ���� ���
		};

		GLfloat backgroundColor[] = {
			1.0f, 1.0f, 1.0f,  // �ʷϻ�
			1.0f, 1.0f, 1.0f,  // �ʷϻ�
			1.0f, 1.0f, 1.0f,  // �ʷϻ�
			1.0f, 1.0f, 1.0f   // �ʷϻ�
		};

		GLfloat backgroundNormal[] = {
			// x, y, z
			0.0f, 1.0f, 0.0f,  // ���� �ϴ�
			0.0f, 1.0f, 0.0f,  // ������ �ϴ�
			0.0f, 1.0f, 0.0f,  // ������ ���
			0.0f, 1.0f, 0.0f   // ���� ���
		};

		GLfloat backgroundTexture[] = {
			// x, y, z
			0.0f, 0.0f,  // ���� �ϴ�
			1.0f, 0.0f,   // ������ �ϴ�
			1.0f, 1.0f,    // ������ ���
			0.0f, 1.0f    // ���� ���
		};

		GLuint vao_background, vbo_background[4];

		glGenVertexArrays(1, &vao_background); // VAO ����
		glBindVertexArray(vao_background); // VAO ���ε�

		glGenBuffers(4, vbo_background); // VBO 4�� ����

		// 1��° VBO: Grass vertices (��ǥ)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// 2��° VBO: Grass color (����)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundColor), backgroundColor, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// 3��° VBO: Grass color (����)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundNormal), backgroundNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		// 4��° VBO: Grass color (����)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundTexture), backgroundTexture, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);



		GLuint backgroundTextures = loadBMP("���1.bmp");
		glActiveTexture(GL_TEXTURE0);      // �ؽ�ó ����
		glBindTexture(GL_TEXTURE_2D, backgroundTextures); // �ؽ�ó ID ���

		// ���̴��� �ؽ�ó ���� 0�� ����
		GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
		glUniform1i(texLocation, 0);  // ���� 0�� backgroundTexture�� ����

		// glDrawArrays�� �̿��Ͽ� xz ����� �׸���
		glDrawArrays(GL_QUADS, 0, 4); // 4���� �������� �簢�� �׸���

		// VAO, VBO ����
		glBindVertexArray(0);
		glDeleteBuffers(4, vbo_background);
		glDeleteVertexArrays(1, &vao_background);

		deleteTexture(backgroundTextures);
	}

}
