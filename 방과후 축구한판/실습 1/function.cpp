#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iomanip>
#define M_PI 3.14159265358979323846

void MakeSphere(GLfloat arr[][3], GLfloat normal[][3], GLfloat cx, GLfloat cy, GLfloat cz, GLfloat radius, int first_index);
struct Rectangle {
    float left;
    float right;
    float top;
    float bottom;
};

void read_newline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}

void windowToOpenGL(int window_x, int window_y, int window_width, int window_height, float& gl_x, float& gl_y) {
    // OpenGL 좌표계의 x값 변환: 윈도우 좌표를 -1.0 ~ 1.0 사이로 정규화
    gl_x = (2.0f * window_x) / window_width - 1.0f;

    // OpenGL 좌표계의 y값 변환: 윈도우 좌표는 위아래가 반대이므로 y축 변환
    gl_y = 1.0f - (2.0f * window_y) / window_height;
}

char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb"); // Open file for reading
    if (!fptr) // Return NULL on failure
        return NULL;
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file
    length = ftell(fptr); // Find out how many bytes into the file we are
    buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
    fclose(fptr); // Close the file
    buf[length] = 0; // Null terminator
    return buf; // Return the buffer 
}
bool isCollision(const struct Rectangle& rect1, const struct Rectangle& rect2) {
    // 충돌이 없는 경우를 먼저 체크
    if (rect1.right < rect2.left || rect1.left > rect2.right ||
        rect1.bottom > rect2.top || rect1.top < rect2.bottom) {
        return false; // 충돌 없음
    }
    return true; // 충돌 발생
}

void MakeShape(GLfloat arr[][3], GLfloat normal[][3], GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, int first_index, std::string shape) {
    if (shape == "cube") {
        // 8개의 꼭짓점 정의
        GLfloat vertices[8][3] = {
            {x1, y1, z1},  // 0번 꼭짓점 (최소 좌표)
            {x2, y1, z1},  // 1번 꼭짓점
            {x2, y2, z1},  // 2번 꼭짓점
            {x1, y2, z1},  // 3번 꼭짓점
            {x1, y1, z2},  // 4번 꼭짓점
            {x2, y1, z2},  // 5번 꼭짓점
            {x2, y2, z2},  // 6번 꼭짓점
            {x1, y2, z2}   // 7번 꼭짓점 (최대 좌표)
        };

        int faces[6][6] = {
     {0, 3, 1, 1, 2, 3},  // 앞면
     {4, 7, 5, 5, 6, 7},  // 뒷면
     {0, 4, 3, 3, 7, 4},  // 왼쪽면
     {1, 2, 5, 5, 6, 2},  // 오른쪽면
     {3, 7, 2, 2, 6, 7},  // 위쪽면
     {0, 1, 4, 4, 5, 1}   // 아래쪽면
        };

        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                int index = first_index + i * 6 + j;
                arr[index][0] = vertices[faces[i][j]][0];
                arr[index][1] = vertices[faces[i][j]][1];
                arr[index][2] = vertices[faces[i][j]][2];
                normal[index][0] = vertices[faces[i][j]][0];
                normal[index][1] = vertices[faces[i][j]][1];
                normal[index][2] = vertices[faces[i][j]][2];
            }
        }
    }
    else if (shape == "pyramid") {
        // 피라미드의 꼭짓점 정의
        GLfloat vertices[5][3] = {
            {x1, y1, z1},  // 0번 꼭짓점 (바닥)
            {x2, y1, z1},  // 1번 꼭짓점
            {x2, y1, z2},  // 2번 꼭짓점
            {x1, y1, z2},  // 3번 꼭짓점
            {(x1 + x2) / 2, y2, (z1 + z2) / 2}  // 4번 꼭짓점 (피라미드 꼭짓점)
        };

        // 피라미드의 면 정의
        int faces[4][3] = {
            {0, 1, 4},  // 삼각형 1
            {1, 2, 4},  // 삼각형 2
            {2, 3, 4},  // 삼각형 3
            {3, 0, 4}   // 삼각형 4
        };

        // 삼각형 정점 저장 및 법선 계산
        for (int i = 0; i < 4; ++i) {  // 네 개의 면에 대해 반복
            // 면의 꼭짓점 가져오기
            GLfloat* p0 = vertices[faces[i][0]];
            GLfloat* p1 = vertices[faces[i][1]];
            GLfloat* p2 = vertices[faces[i][2]];

            // 두 벡터 계산
            GLfloat v1[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
            GLfloat v2[3] = { p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2] };

            // 외적 계산 (법선 벡터)
            GLfloat n[3] = {
                v1[1] * v2[2] - v1[2] * v2[1],  // 법선 벡터 방향 수정
                v1[2] * v2[0] - v1[0] * v2[2],
                v1[0] * v2[1] - v1[1] * v2[0]
            };

            // 법선 벡터 저장 (정규화는 셰이더에서 처리됨)
            for (int j = 0; j < 3; ++j) {
                normal[first_index / 3 + i * 3 + j][0] = n[0];
                normal[first_index / 3 + i * 3 + j][1] = n[1];
                normal[first_index / 3 + i * 3 + j][2] = n[2];
            }

            // 정점 데이터 삽입
            for (int j = 0; j < 3; ++j) {
                arr[first_index + i * 3 + j][0] = vertices[faces[i][j]][0];
                arr[first_index + i * 3 + j][1] = vertices[faces[i][j]][1];
                arr[first_index + i * 3 + j][2] = vertices[faces[i][j]][2];
            }
        }

        // 바닥면 추가 (법선은 아래를 향함)
        GLfloat base_normal[3] = { 0.0f, -1.0f, 0.0f };
        int base_faces[2][3] = {
            {0, 1, 2},  // 첫 번째 삼각형
            {0, 2, 3}   // 두 번째 삼각형
        };

        for (int i = 0; i < 2; ++i) {  // 바닥의 두 삼각형
            for (int j = 0; j < 3; ++j) {
                int idx = first_index + 12 + i * 3 + j;
                arr[idx][0] = vertices[base_faces[i][j]][0];
                arr[idx][1] = vertices[base_faces[i][j]][1];
                arr[idx][2] = vertices[base_faces[i][j]][2];

                // 바닥면의 법선 저장
                normal[first_index / 3 + 4 + i * 3 + j][0] = base_normal[0];
                normal[first_index / 3 + 4 + i * 3 + j][1] = base_normal[1];
                normal[first_index / 3 + 4 + i * 3 + j][2] = base_normal[2];
            }
        }
    }
    else if (shape == "sphere") {
        MakeSphere(arr, normal, (x1 + x2) / 2, y2, (z1 + z2) / 2, (x2 - x1) / 2, first_index); // 구의 중심과 반지름 계산
    }
    else if (shape == "cone") {
        // x1, y1, z1 : 원뿔 밑면의 중심
        // x2, y2, z2 : 원뿔 꼭짓점의 위치
        // radius : 원뿔 밑면의 반지름
        GLfloat radius = (x2 - x1) / 2;
        GLfloat angleStep = 2 * M_PI / 18;  // 각도 증가값

        // 원뿔의 꼭짓점 (꼭짓점은 y축으로 높이 y2만큼 올라간 점)
        GLfloat apex[3] = { (x1 + x2) / 2, y2, (z1 + z2) / 2 };

        // 밑면 중심점
        GLfloat baseCenter[3] = { (x1 + x2) / 2, y1, (z1 + z2) / 2 };

        // 밑면을 구성할 정점들 (36개의 점)
        GLfloat vertices[36][3];
        for (int i = 0; i < 36; ++i) {
            GLfloat angle = i * angleStep;
            vertices[i][0] = baseCenter[0] + radius * cos(angle);  // x 좌표
            vertices[i][1] = y1;                                   // y 좌표 (밑면은 동일한 y 좌표)
            vertices[i][2] = baseCenter[2] + radius * sin(angle);  // z 좌표
        }

        int index = first_index;

        // 옆면을 구성하는 삼각형들 (밑면의 각 점과 꼭짓점을 연결)
        for (int i = 0; i < 36; ++i) {
            int next = (i + 1) % 36;

            // 첫 번째 삼각형: 옆면
            arr[index][0] = vertices[i][0];   // 밑면 점 1
            arr[index][1] = vertices[i][1];
            arr[index][2] = vertices[i][2];

            arr[index + 1][0] = apex[0];      // 꼭짓점
            arr[index + 1][1] = apex[1];
            arr[index + 1][2] = apex[2];

            arr[index + 2][0] = vertices[next][0]; // 밑면 점 2
            arr[index + 2][1] = vertices[next][1];
            arr[index + 2][2] = vertices[next][2];

            index += 3;
        }

        // 밑면을 구성하는 삼각형들 (중심점과 각 밑면 점을 삼각형으로 연결)
        for (int i = 0; i < 36; ++i) {
            int next = (i + 1) % 36;

            // 두 번째 삼각형: 밑면
            arr[index][0] = baseCenter[0];    // 밑면 중심점
            arr[index][1] = baseCenter[1];
            arr[index][2] = baseCenter[2];

            arr[index + 1][0] = vertices[next][0]; // 밑면 점 2
            arr[index + 1][1] = vertices[next][1];
            arr[index + 1][2] = vertices[next][2];

            arr[index + 2][0] = vertices[i][0];   // 밑면 점 1
            arr[index + 2][1] = vertices[i][1];
            arr[index + 2][2] = vertices[i][2];

            index += 3;
           
        }
    }
    else if (shape == "rectangle") {
        // 직사각형의 4개의 꼭짓점 정의
        GLfloat vertices[4][3] = {
            {x1, y1, z1},  // 0번 꼭짓점
            {x2, y1, z1},  // 1번 꼭짓점
            {x2, y2, z2},  // 2번 꼭짓점
            {x1, y2, z2}   // 3번 꼭짓점
        };

        // 직사각형을 두 개의 삼각형으로 분할
        int faces[2][3] = {
            {0, 1, 2},  // 첫 번째 삼각형 (0, 1, 2)
            {0, 2, 3}   // 두 번째 삼각형 (0, 2, 3)
        };

        // 삼각형의 정점들을 arr 배열에 삽입
        for (int i = 0; i < 2; ++i) {  // 2개의 삼각형 면
            for (int j = 0; j < 3; ++j) {
                int index = first_index + i * 3 + j;
                arr[index][0] = vertices[faces[i][j]][0];  // x좌표
                arr[index][1] = vertices[faces[i][j]][1];  // y좌표
                arr[index][2] = vertices[faces[i][j]][2];  // z좌표
            }
        }
}
    else if (shape == "triangle") {
        // x1, y1, z1 : 삼각형의 첫 번째 꼭짓점
        // x2, y2, z2 : 삼각형의 두 번째 꼭짓점
        // x3, y3, z3 : 삼각형의 세 번째 꼭짓점

        // 삼각형의 꼭짓점 정의
    GLfloat vertices[3][3] = {
        {x1, y2, z1},  // 0번 꼭짓점
        {(x1+x2)/2, y1, z1},  // 1번 꼭짓점
        {x2, y2, z1}   // 2번 꼭짓점
    };

    // 삼각형의 정점들을 arr 배열에 삽입
    for (int i = 0; i < 3; ++i) {
        arr[first_index + i][0] = vertices[i][0];  // x좌표
        arr[first_index + i][1] = vertices[i][1];  // y좌표
        arr[first_index + i][2] = vertices[i][2];  // z좌표
    }
    }
}
void MakeColor(GLfloat arr[][3], int first_index, int index_count, GLfloat color[3]) {
    for (int j = first_index; j < first_index + index_count; j++){
            arr[j][0] = color[0];
            arr[j][1] = color[1];
            arr[j][2] = color[2];
    }
}
void MakeSphere(GLfloat arr[][3], GLfloat normal[][3], GLfloat cx, GLfloat cy, GLfloat cz, GLfloat radius, int first_index) {
    int longitudeSegments = 18;  // 경도 분할 수
    int latitudeSegments = 9;    // 위도 분할 수
    int vertexCount = (latitudeSegments + 1) * (longitudeSegments + 1);

    // 동적 메모리 할당
    GLfloat** vertices = new GLfloat * [vertexCount];
    for (int i = 0; i < vertexCount; ++i) {
        vertices[i] = new GLfloat[3];
    }

    // 구의 정점 정의
    for (int lat = 0; lat <= latitudeSegments; ++lat) {
        float theta = lat * M_PI / latitudeSegments; // 위도 각도
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longitudeSegments; ++lon) {
            float phi = lon * 2 * M_PI / longitudeSegments; // 경도 각도
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // 정점 좌표 계산
            vertices[lat * (longitudeSegments + 1) + lon][0] = cx + radius * cosPhi * sinTheta; // x
            vertices[lat * (longitudeSegments + 1) + lon][1] = cy + radius * cosTheta;         // y
            vertices[lat * (longitudeSegments + 1) + lon][2] = cz + radius * sinPhi * sinTheta; // z
        }
    }

    // 구의 면 정의
    int index = first_index; // 면 인덱스 시작 위치
    for (int lat = 0; lat < latitudeSegments; ++lat) {
        for (int lon = 0; lon < longitudeSegments; ++lon) {
            int first = lat * (longitudeSegments + 1) + lon;
            int second = first + longitudeSegments + 1;

            // 두 개의 삼각형으로 면 구성
            // 첫 번째 삼각형
            arr[index][0] = vertices[first][0];
            arr[index][1] = vertices[first][1];
            arr[index][2] = vertices[first][2];
            // 법선 벡터 계산
            normal[index][0] = (vertices[first][0] - cx) / radius;
            normal[index][1] = (vertices[first][1] - cy) / radius;
            normal[index][2] = (vertices[first][2] - cz) / radius;

            arr[index + 1][0] = vertices[second][0];
            arr[index + 1][1] = vertices[second][1];
            arr[index + 1][2] = vertices[second][2];
            // 법선 벡터 계산
            normal[index + 1][0] = (vertices[second][0] - cx) / radius;
            normal[index + 1][1] = (vertices[second][1] - cy) / radius;
            normal[index + 1][2] = (vertices[second][2] - cz) / radius;

            arr[index + 2][0] = vertices[first + 1][0];
            arr[index + 2][1] = vertices[first + 1][1];
            arr[index + 2][2] = vertices[first + 1][2];
            // 법선 벡터 계산
            normal[index + 2][0] = (vertices[first + 1][0] - cx) / radius;
            normal[index + 2][1] = (vertices[first + 1][1] - cy) / radius;
            normal[index + 2][2] = (vertices[first + 1][2] - cz) / radius;

            // 두 번째 삼각형
            arr[index + 3][0] = vertices[second][0];
            arr[index + 3][1] = vertices[second][1];
            arr[index + 3][2] = vertices[second][2];
            // 법선 벡터 계산
            normal[index + 3][0] = (vertices[second][0] - cx) / radius;
            normal[index + 3][1] = (vertices[second][1] - cy) / radius;
            normal[index + 3][2] = (vertices[second][2] - cz) / radius;

            arr[index + 4][0] = vertices[second + 1][0];
            arr[index + 4][1] = vertices[second + 1][1];
            arr[index + 4][2] = vertices[second + 1][2];
            // 법선 벡터 계산
            normal[index + 4][0] = (vertices[second + 1][0] - cx) / radius;
            normal[index + 4][1] = (vertices[second + 1][1] - cy) / radius;
            normal[index + 4][2] = (vertices[second + 1][2] - cz) / radius;

            arr[index + 5][0] = vertices[first + 1][0];
            arr[index + 5][1] = vertices[first + 1][1];
            arr[index + 5][2] = vertices[first + 1][2];
            // 법선 벡터 계산
            normal[index + 5][0] = (vertices[first + 1][0] - cx) / radius;
            normal[index + 5][1] = (vertices[first + 1][1] - cy) / radius;
            normal[index + 5][2] = (vertices[first + 1][2] - cz) / radius;

            index += 6; // 다음 삼각형으로 이동
        }
    }

    // 동적 메모리 해제
    for (int i = 0; i < vertexCount; ++i) {
        delete[] vertices[i];
    }
    delete[] vertices;
}
