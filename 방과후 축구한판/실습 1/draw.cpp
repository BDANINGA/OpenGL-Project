#define _CRT_SECURE_NO_WARNINGS
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

void MovePlayer(glm::vec3 ballPos);
void MoveBall(glm::vec3& playerPos, glm::vec3 keeperPos);
void MoveKeeper(glm::vec3 ballPos, glm::vec3& keeperPos);
void deleteTexture(GLuint textureID);
GLuint loadBMP(const char* filepath);

extern glm::vec3 playerPos;
extern glm::vec3 ballPos;
extern glm::vec3 keeperPos;
extern float playerRotation;
extern GLuint shaderProgramID; //--- 세이더 프로그램 이름
extern int firstObjectVertexCount, secondObjectVertexCount, thirdObjectVertexCount[4];
extern float rotationAngle;  // 회전 각도 (라디안)
extern glm::vec3 rotationDirection;

void deleteTexture(GLuint textureID) {
	glDeleteTextures(1, &textureID);
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

	GLuint playerTextures = loadBMP("플레이어 색.bmp");
	glActiveTexture(GL_TEXTURE0);      // 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, playerTextures); // 텍스처 ID 사용

	// 셰이더에 텍스처 유닛 0을 연결
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // 유닛 0을 grassTexture에 연결

	// 플레이어 그리기
	glDrawArrays(GL_TRIANGLES, 0, firstObjectVertexCount);

	deleteTexture(playerTextures);
}

void drawBall(glm::vec3 keeperPos) {
	MoveBall(playerPos, keeperPos);

	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 S = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);  // 회전 행렬
	glm::mat4 Trans = glm::mat4(1.0f);

	// 회전 적용 (회전 각도 누적 적용)
	R = glm::rotate(R, rotationAngle, rotationDirection);  // 방향에 따라 회전

	// 두 번째 객체(공) 이동을 위한 위치 업데이트
	T = glm::translate(T, ballPos);  // 두 번째 객체의 위치 적용
	S = glm::scale(S, glm::vec3(0.01f, 0.01f, 0.01f));  // 크기 조정
	Trans = T * R * S; // 위치, 회전, 크기 순서로 적용

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	GLuint ballTextures = loadBMP("축구공.bmp");
	glActiveTexture(GL_TEXTURE0);      // 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, ballTextures); // 텍스처 ID 사용

	// 셰이더에 텍스처 유닛 0을 연결
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // 유닛 0을 grassTexture에 연결

	// 두 번째 객체(공) 그리기
	glDrawArrays(GL_TRIANGLES, firstObjectVertexCount, secondObjectVertexCount);

	deleteTexture(ballTextures);
}
void drawKeeper(glm::vec3 ballPos, glm::vec3& keeperPos) {
	MoveKeeper(ballPos, keeperPos);
	glm::mat4 Trans = glm::mat4(1.0f);
	// 플레이어 이동을 위한 위치 업데이트
	Trans = glm::translate(Trans, keeperPos);
	// 회전 적용
	//Trans = glm::rotate(Trans, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));  // 회전 적용

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Trans));

	GLuint keeperTextures = loadBMP("플레이어 색.bmp");
	glActiveTexture(GL_TEXTURE0);      // 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, keeperTextures); // 텍스처 ID 사용

	// 셰이더에 텍스처 유닛 0을 연결
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // 유닛 0을 grassTexture에 연결

	// 플레이어 그리기
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

	GLuint goalTextures = loadBMP("골대 색.bmp");
	glActiveTexture(GL_TEXTURE0);      // 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, goalTextures); // 텍스처 ID 사용

	// 셰이더에 텍스처 유닛 0을 연결
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // 유닛 0을 grassTexture에 연결

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
	// xz 평면의 범위를 넓혀서 그리기
	GLfloat grassVertices[] = {
		// x, y, z
		-20.0f, -0.3f, -40.0f,  // 왼쪽 하단
		20.0f, -0.3f, -40.0f,   // 오른쪽 하단
		20.0f, -0.3f, 40.0f,    // 오른쪽 상단
		-20.0f, -0.3f, 40.0f    // 왼쪽 상단
	};

	GLfloat grassColor[] = {
		1.0f, 1.0f, 1.0f,  // 초록색
		1.0f, 1.0f, 1.0f,  // 초록색
		1.0f, 1.0f, 1.0f,  // 초록색
		1.0f, 1.0f, 1.0f   // 초록색
	};

	GLfloat grassNormal[] = {
		// x, y, z
		0.0f, 0.0f, 0.0f,  // 왼쪽 하단
		0.0f, 0.0f, 0.0f,  // 오른쪽 하단
		0.0f, 0.0f, 0.0f,  // 오른쪽 상단
		0.0f, 0.0f, 0.0f   // 왼쪽 상단
	};

	GLfloat grassTexture[] = {
		// x, y, z
		1.0f, 0.0f,  // 왼쪽 하단
		1.0f, 1.0f,   // 오른쪽 하단
		0.0f, 1.0f,    // 오른쪽 상단
		0.0f, 0.0f    // 왼쪽 상단
	};

	GLuint vao_grass, vbo_grass[4];

	glGenVertexArrays(1, &vao_grass); // VAO 생성
	glBindVertexArray(vao_grass); // VAO 바인드

	glGenBuffers(4, vbo_grass); // VBO 4개 생성

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

	// 4번째 VBO: Grass color (색상)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_grass[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassTexture), grassTexture, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);



	GLuint grassTextures = loadBMP("축구장.bmp");
	glActiveTexture(GL_TEXTURE0);      // 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, grassTextures); // 텍스처 ID 사용

	// 셰이더에 텍스처 유닛 0을 연결
	GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
	glUniform1i(texLocation, 0);  // 유닛 0을 grassTexture에 연결

	// glDrawArrays를 이용하여 xz 평면을 그린다
	glDrawArrays(GL_QUADS, 0, 4); // 4개의 정점으로 사각형 그리기

	// VAO, VBO 정리
	glBindVertexArray(0);
	glDeleteBuffers(4, vbo_grass);
	glDeleteVertexArrays(1, &vao_grass);

	deleteTexture(grassTextures);
}

void drawBackground(int i) {
	// xz 평면의 범위를 넓혀서 그리기
	if (i == 0) {
		GLfloat backgroundVertices[] = {
			// x, y, z
			-20.0f, -0.3f, -40.0f,  // 왼쪽 하단
			20.0f, -0.3f, -40.0f,   // 오른쪽 하단
			20.0f, 30.0f, -40.0f,    // 오른쪽 상단
			-20.0f, 30.0f, -40.0f    // 왼쪽 상단
		};

		GLfloat backgroundColor[] = {
			1.0f, 1.0f, 1.0f,  // 초록색
			1.0f, 1.0f, 1.0f,  // 초록색
			1.0f, 1.0f, 1.0f,  // 초록색
			1.0f, 1.0f, 1.0f   // 초록색
		};

		GLfloat backgroundNormal[] = {
			// x, y, z
			0.0f, 1.0f, 0.0f,  // 왼쪽 하단
			0.0f, 1.0f, 0.0f,  // 오른쪽 하단
			0.0f, 1.0f, 0.0f,  // 오른쪽 상단
			0.0f, 1.0f, 0.0f   // 왼쪽 상단
		};

		GLfloat backgroundTexture[] = {
			// x, y, z
			0.0f, 0.0f,  // 왼쪽 하단
			1.0f, 0.0f,   // 오른쪽 하단
			1.0f, 1.0f,    // 오른쪽 상단
			0.0f, 1.0f    // 왼쪽 상단
		};

		GLuint vao_background, vbo_background[4];

		glGenVertexArrays(1, &vao_background); // VAO 생성
		glBindVertexArray(vao_background); // VAO 바인드

		glGenBuffers(4, vbo_background); // VBO 4개 생성

		// 1번째 VBO: Grass vertices (좌표)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// 2번째 VBO: Grass color (색상)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundColor), backgroundColor, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// 3번째 VBO: Grass color (색상)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundNormal), backgroundNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		// 4번째 VBO: Grass color (색상)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundTexture), backgroundTexture, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);



		GLuint backgroundTextures = loadBMP("배경1.bmp");
		glActiveTexture(GL_TEXTURE0);      // 텍스처 생성
		glBindTexture(GL_TEXTURE_2D, backgroundTextures); // 텍스처 ID 사용

		// 셰이더에 텍스처 유닛 0을 연결
		GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
		glUniform1i(texLocation, 0);  // 유닛 0을 backgroundTexture에 연결

		// glDrawArrays를 이용하여 xz 평면을 그린다
		glDrawArrays(GL_QUADS, 0, 4); // 4개의 정점으로 사각형 그리기

		// VAO, VBO 정리
		glBindVertexArray(0);
		glDeleteBuffers(4, vbo_background);
		glDeleteVertexArrays(1, &vao_background);

		deleteTexture(backgroundTextures);
	}
	else if (i == 1) {
		GLfloat backgroundVertices[] = {
			// x, y, z
			-20.0f, -0.3f, 40.0f,  // 왼쪽 하단
			-20.0f, -0.3f, -40.0f,   // 오른쪽 하단
			-20.0f, 30.0f, -40.0f,    // 오른쪽 상단
			-20.0f, 30.0f, 40.0f    // 왼쪽 상단
		};

		GLfloat backgroundColor[] = {
			1.0f, 1.0f, 1.0f,  // 초록색
			1.0f, 1.0f, 1.0f,  // 초록색
			1.0f, 1.0f, 1.0f,  // 초록색
			1.0f, 1.0f, 1.0f   // 초록색
		};

		GLfloat backgroundNormal[] = {
			// x, y, z
			0.0f, 1.0f, 0.0f,  // 왼쪽 하단
			0.0f, 1.0f, 0.0f,  // 오른쪽 하단
			0.0f, 1.0f, 0.0f,  // 오른쪽 상단
			0.0f, 1.0f, 0.0f   // 왼쪽 상단
		};

		GLfloat backgroundTexture[] = {
			// x, y, z
			0.0f, 0.0f,  // 왼쪽 하단
			1.0f, 0.0f,   // 오른쪽 하단
			1.0f, 1.0f,    // 오른쪽 상단
			0.0f, 1.0f    // 왼쪽 상단
		};

		GLuint vao_background, vbo_background[4];

		glGenVertexArrays(1, &vao_background); // VAO 생성
		glBindVertexArray(vao_background); // VAO 바인드

		glGenBuffers(4, vbo_background); // VBO 4개 생성

		// 1번째 VBO: Grass vertices (좌표)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// 2번째 VBO: Grass color (색상)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundColor), backgroundColor, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// 3번째 VBO: Grass color (색상)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundNormal), backgroundNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		// 4번째 VBO: Grass color (색상)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundTexture), backgroundTexture, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);



		GLuint backgroundTextures = loadBMP("배경1.bmp");
		glActiveTexture(GL_TEXTURE0);      // 텍스처 생성
		glBindTexture(GL_TEXTURE_2D, backgroundTextures); // 텍스처 ID 사용

		// 셰이더에 텍스처 유닛 0을 연결
		GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
		glUniform1i(texLocation, 0);  // 유닛 0을 backgroundTexture에 연결

		// glDrawArrays를 이용하여 xz 평면을 그린다
		glDrawArrays(GL_QUADS, 0, 4); // 4개의 정점으로 사각형 그리기

		// VAO, VBO 정리
		glBindVertexArray(0);
		glDeleteBuffers(4, vbo_background);
		glDeleteVertexArrays(1, &vao_background);

		deleteTexture(backgroundTextures);
	}
	else if (i == 2) {
		GLfloat backgroundVertices[] = {
			// x, y, z
			20.0f, -0.3f, 40.0f,  // 왼쪽 하단
			20.0f, -0.3f, -40.0f,   // 오른쪽 하단
			20.0f, 30.0f, -40.0f,    // 오른쪽 상단
			20.0f, 30.0f, 40.0f    // 왼쪽 상단
		};

		GLfloat backgroundColor[] = {
			1.0f, 1.0f, 1.0f,  // 초록색
			1.0f, 1.0f, 1.0f,  // 초록색
			1.0f, 1.0f, 1.0f,  // 초록색
			1.0f, 1.0f, 1.0f   // 초록색
		};

		GLfloat backgroundNormal[] = {
			// x, y, z
			0.0f, 1.0f, 0.0f,  // 왼쪽 하단
			0.0f, 1.0f, 0.0f,  // 오른쪽 하단
			0.0f, 1.0f, 0.0f,  // 오른쪽 상단
			0.0f, 1.0f, 0.0f   // 왼쪽 상단
		};

		GLfloat backgroundTexture[] = {
			// x, y, z
			0.0f, 0.0f,  // 왼쪽 하단
			1.0f, 0.0f,   // 오른쪽 하단
			1.0f, 1.0f,    // 오른쪽 상단
			0.0f, 1.0f    // 왼쪽 상단
		};

		GLuint vao_background, vbo_background[4];

		glGenVertexArrays(1, &vao_background); // VAO 생성
		glBindVertexArray(vao_background); // VAO 바인드

		glGenBuffers(4, vbo_background); // VBO 4개 생성

		// 1번째 VBO: Grass vertices (좌표)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// 2번째 VBO: Grass color (색상)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundColor), backgroundColor, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// 3번째 VBO: Grass color (색상)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundNormal), backgroundNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		// 4번째 VBO: Grass color (색상)
		glBindBuffer(GL_ARRAY_BUFFER, vbo_background[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundTexture), backgroundTexture, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);



		GLuint backgroundTextures = loadBMP("배경1.bmp");
		glActiveTexture(GL_TEXTURE0);      // 텍스처 생성
		glBindTexture(GL_TEXTURE_2D, backgroundTextures); // 텍스처 ID 사용

		// 셰이더에 텍스처 유닛 0을 연결
		GLuint texLocation = glGetUniformLocation(shaderProgramID, "Texture");
		glUniform1i(texLocation, 0);  // 유닛 0을 backgroundTexture에 연결

		// glDrawArrays를 이용하여 xz 평면을 그린다
		glDrawArrays(GL_QUADS, 0, 4); // 4개의 정점으로 사각형 그리기

		// VAO, VBO 정리
		glBindVertexArray(0);
		glDeleteBuffers(4, vbo_background);
		glDeleteVertexArrays(1, &vao_background);

		deleteTexture(backgroundTextures);
	}

}
