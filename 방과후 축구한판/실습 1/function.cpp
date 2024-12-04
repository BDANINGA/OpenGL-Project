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
    // OpenGL ��ǥ���� x�� ��ȯ: ������ ��ǥ�� -1.0 ~ 1.0 ���̷� ����ȭ
    gl_x = (2.0f * window_x) / window_width - 1.0f;

    // OpenGL ��ǥ���� y�� ��ȯ: ������ ��ǥ�� ���Ʒ��� �ݴ��̹Ƿ� y�� ��ȯ
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
    // �浹�� ���� ��츦 ���� üũ
    if (rect1.right < rect2.left || rect1.left > rect2.right ||
        rect1.bottom > rect2.top || rect1.top < rect2.bottom) {
        return false; // �浹 ����
    }
    return true; // �浹 �߻�
}

void MakeShape(GLfloat arr[][3], GLfloat normal[][3], GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, int first_index, std::string shape) {
    if (shape == "cube") {
        // 8���� ������ ����
        GLfloat vertices[8][3] = {
            {x1, y1, z1},  // 0�� ������ (�ּ� ��ǥ)
            {x2, y1, z1},  // 1�� ������
            {x2, y2, z1},  // 2�� ������
            {x1, y2, z1},  // 3�� ������
            {x1, y1, z2},  // 4�� ������
            {x2, y1, z2},  // 5�� ������
            {x2, y2, z2},  // 6�� ������
            {x1, y2, z2}   // 7�� ������ (�ִ� ��ǥ)
        };

        int faces[6][6] = {
     {0, 3, 1, 1, 2, 3},  // �ո�
     {4, 7, 5, 5, 6, 7},  // �޸�
     {0, 4, 3, 3, 7, 4},  // ���ʸ�
     {1, 2, 5, 5, 6, 2},  // �����ʸ�
     {3, 7, 2, 2, 6, 7},  // ���ʸ�
     {0, 1, 4, 4, 5, 1}   // �Ʒ��ʸ�
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
        // �Ƕ�̵��� ������ ����
        GLfloat vertices[5][3] = {
            {x1, y1, z1},  // 0�� ������ (�ٴ�)
            {x2, y1, z1},  // 1�� ������
            {x2, y1, z2},  // 2�� ������
            {x1, y1, z2},  // 3�� ������
            {(x1 + x2) / 2, y2, (z1 + z2) / 2}  // 4�� ������ (�Ƕ�̵� ������)
        };

        // �Ƕ�̵��� �� ����
        int faces[4][3] = {
            {0, 1, 4},  // �ﰢ�� 1
            {1, 2, 4},  // �ﰢ�� 2
            {2, 3, 4},  // �ﰢ�� 3
            {3, 0, 4}   // �ﰢ�� 4
        };

        // �ﰢ�� ���� ���� �� ���� ���
        for (int i = 0; i < 4; ++i) {  // �� ���� �鿡 ���� �ݺ�
            // ���� ������ ��������
            GLfloat* p0 = vertices[faces[i][0]];
            GLfloat* p1 = vertices[faces[i][1]];
            GLfloat* p2 = vertices[faces[i][2]];

            // �� ���� ���
            GLfloat v1[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
            GLfloat v2[3] = { p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2] };

            // ���� ��� (���� ����)
            GLfloat n[3] = {
                v1[1] * v2[2] - v1[2] * v2[1],  // ���� ���� ���� ����
                v1[2] * v2[0] - v1[0] * v2[2],
                v1[0] * v2[1] - v1[1] * v2[0]
            };

            // ���� ���� ���� (����ȭ�� ���̴����� ó����)
            for (int j = 0; j < 3; ++j) {
                normal[first_index / 3 + i * 3 + j][0] = n[0];
                normal[first_index / 3 + i * 3 + j][1] = n[1];
                normal[first_index / 3 + i * 3 + j][2] = n[2];
            }

            // ���� ������ ����
            for (int j = 0; j < 3; ++j) {
                arr[first_index + i * 3 + j][0] = vertices[faces[i][j]][0];
                arr[first_index + i * 3 + j][1] = vertices[faces[i][j]][1];
                arr[first_index + i * 3 + j][2] = vertices[faces[i][j]][2];
            }
        }

        // �ٴڸ� �߰� (������ �Ʒ��� ����)
        GLfloat base_normal[3] = { 0.0f, -1.0f, 0.0f };
        int base_faces[2][3] = {
            {0, 1, 2},  // ù ��° �ﰢ��
            {0, 2, 3}   // �� ��° �ﰢ��
        };

        for (int i = 0; i < 2; ++i) {  // �ٴ��� �� �ﰢ��
            for (int j = 0; j < 3; ++j) {
                int idx = first_index + 12 + i * 3 + j;
                arr[idx][0] = vertices[base_faces[i][j]][0];
                arr[idx][1] = vertices[base_faces[i][j]][1];
                arr[idx][2] = vertices[base_faces[i][j]][2];

                // �ٴڸ��� ���� ����
                normal[first_index / 3 + 4 + i * 3 + j][0] = base_normal[0];
                normal[first_index / 3 + 4 + i * 3 + j][1] = base_normal[1];
                normal[first_index / 3 + 4 + i * 3 + j][2] = base_normal[2];
            }
        }
    }
    else if (shape == "sphere") {
        MakeSphere(arr, normal, (x1 + x2) / 2, y2, (z1 + z2) / 2, (x2 - x1) / 2, first_index); // ���� �߽ɰ� ������ ���
    }
    else if (shape == "cone") {
        // x1, y1, z1 : ���� �ظ��� �߽�
        // x2, y2, z2 : ���� �������� ��ġ
        // radius : ���� �ظ��� ������
        GLfloat radius = (x2 - x1) / 2;
        GLfloat angleStep = 2 * M_PI / 18;  // ���� ������

        // ������ ������ (�������� y������ ���� y2��ŭ �ö� ��)
        GLfloat apex[3] = { (x1 + x2) / 2, y2, (z1 + z2) / 2 };

        // �ظ� �߽���
        GLfloat baseCenter[3] = { (x1 + x2) / 2, y1, (z1 + z2) / 2 };

        // �ظ��� ������ ������ (36���� ��)
        GLfloat vertices[36][3];
        for (int i = 0; i < 36; ++i) {
            GLfloat angle = i * angleStep;
            vertices[i][0] = baseCenter[0] + radius * cos(angle);  // x ��ǥ
            vertices[i][1] = y1;                                   // y ��ǥ (�ظ��� ������ y ��ǥ)
            vertices[i][2] = baseCenter[2] + radius * sin(angle);  // z ��ǥ
        }

        int index = first_index;

        // ������ �����ϴ� �ﰢ���� (�ظ��� �� ���� �������� ����)
        for (int i = 0; i < 36; ++i) {
            int next = (i + 1) % 36;

            // ù ��° �ﰢ��: ����
            arr[index][0] = vertices[i][0];   // �ظ� �� 1
            arr[index][1] = vertices[i][1];
            arr[index][2] = vertices[i][2];

            arr[index + 1][0] = apex[0];      // ������
            arr[index + 1][1] = apex[1];
            arr[index + 1][2] = apex[2];

            arr[index + 2][0] = vertices[next][0]; // �ظ� �� 2
            arr[index + 2][1] = vertices[next][1];
            arr[index + 2][2] = vertices[next][2];

            index += 3;
        }

        // �ظ��� �����ϴ� �ﰢ���� (�߽����� �� �ظ� ���� �ﰢ������ ����)
        for (int i = 0; i < 36; ++i) {
            int next = (i + 1) % 36;

            // �� ��° �ﰢ��: �ظ�
            arr[index][0] = baseCenter[0];    // �ظ� �߽���
            arr[index][1] = baseCenter[1];
            arr[index][2] = baseCenter[2];

            arr[index + 1][0] = vertices[next][0]; // �ظ� �� 2
            arr[index + 1][1] = vertices[next][1];
            arr[index + 1][2] = vertices[next][2];

            arr[index + 2][0] = vertices[i][0];   // �ظ� �� 1
            arr[index + 2][1] = vertices[i][1];
            arr[index + 2][2] = vertices[i][2];

            index += 3;
           
        }
    }
    else if (shape == "rectangle") {
        // ���簢���� 4���� ������ ����
        GLfloat vertices[4][3] = {
            {x1, y1, z1},  // 0�� ������
            {x2, y1, z1},  // 1�� ������
            {x2, y2, z2},  // 2�� ������
            {x1, y2, z2}   // 3�� ������
        };

        // ���簢���� �� ���� �ﰢ������ ����
        int faces[2][3] = {
            {0, 1, 2},  // ù ��° �ﰢ�� (0, 1, 2)
            {0, 2, 3}   // �� ��° �ﰢ�� (0, 2, 3)
        };

        // �ﰢ���� �������� arr �迭�� ����
        for (int i = 0; i < 2; ++i) {  // 2���� �ﰢ�� ��
            for (int j = 0; j < 3; ++j) {
                int index = first_index + i * 3 + j;
                arr[index][0] = vertices[faces[i][j]][0];  // x��ǥ
                arr[index][1] = vertices[faces[i][j]][1];  // y��ǥ
                arr[index][2] = vertices[faces[i][j]][2];  // z��ǥ
            }
        }
}
    else if (shape == "triangle") {
        // x1, y1, z1 : �ﰢ���� ù ��° ������
        // x2, y2, z2 : �ﰢ���� �� ��° ������
        // x3, y3, z3 : �ﰢ���� �� ��° ������

        // �ﰢ���� ������ ����
    GLfloat vertices[3][3] = {
        {x1, y2, z1},  // 0�� ������
        {(x1+x2)/2, y1, z1},  // 1�� ������
        {x2, y2, z1}   // 2�� ������
    };

    // �ﰢ���� �������� arr �迭�� ����
    for (int i = 0; i < 3; ++i) {
        arr[first_index + i][0] = vertices[i][0];  // x��ǥ
        arr[first_index + i][1] = vertices[i][1];  // y��ǥ
        arr[first_index + i][2] = vertices[i][2];  // z��ǥ
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
    int longitudeSegments = 18;  // �浵 ���� ��
    int latitudeSegments = 9;    // ���� ���� ��
    int vertexCount = (latitudeSegments + 1) * (longitudeSegments + 1);

    // ���� �޸� �Ҵ�
    GLfloat** vertices = new GLfloat * [vertexCount];
    for (int i = 0; i < vertexCount; ++i) {
        vertices[i] = new GLfloat[3];
    }

    // ���� ���� ����
    for (int lat = 0; lat <= latitudeSegments; ++lat) {
        float theta = lat * M_PI / latitudeSegments; // ���� ����
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longitudeSegments; ++lon) {
            float phi = lon * 2 * M_PI / longitudeSegments; // �浵 ����
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // ���� ��ǥ ���
            vertices[lat * (longitudeSegments + 1) + lon][0] = cx + radius * cosPhi * sinTheta; // x
            vertices[lat * (longitudeSegments + 1) + lon][1] = cy + radius * cosTheta;         // y
            vertices[lat * (longitudeSegments + 1) + lon][2] = cz + radius * sinPhi * sinTheta; // z
        }
    }

    // ���� �� ����
    int index = first_index; // �� �ε��� ���� ��ġ
    for (int lat = 0; lat < latitudeSegments; ++lat) {
        for (int lon = 0; lon < longitudeSegments; ++lon) {
            int first = lat * (longitudeSegments + 1) + lon;
            int second = first + longitudeSegments + 1;

            // �� ���� �ﰢ������ �� ����
            // ù ��° �ﰢ��
            arr[index][0] = vertices[first][0];
            arr[index][1] = vertices[first][1];
            arr[index][2] = vertices[first][2];
            // ���� ���� ���
            normal[index][0] = (vertices[first][0] - cx) / radius;
            normal[index][1] = (vertices[first][1] - cy) / radius;
            normal[index][2] = (vertices[first][2] - cz) / radius;

            arr[index + 1][0] = vertices[second][0];
            arr[index + 1][1] = vertices[second][1];
            arr[index + 1][2] = vertices[second][2];
            // ���� ���� ���
            normal[index + 1][0] = (vertices[second][0] - cx) / radius;
            normal[index + 1][1] = (vertices[second][1] - cy) / radius;
            normal[index + 1][2] = (vertices[second][2] - cz) / radius;

            arr[index + 2][0] = vertices[first + 1][0];
            arr[index + 2][1] = vertices[first + 1][1];
            arr[index + 2][2] = vertices[first + 1][2];
            // ���� ���� ���
            normal[index + 2][0] = (vertices[first + 1][0] - cx) / radius;
            normal[index + 2][1] = (vertices[first + 1][1] - cy) / radius;
            normal[index + 2][2] = (vertices[first + 1][2] - cz) / radius;

            // �� ��° �ﰢ��
            arr[index + 3][0] = vertices[second][0];
            arr[index + 3][1] = vertices[second][1];
            arr[index + 3][2] = vertices[second][2];
            // ���� ���� ���
            normal[index + 3][0] = (vertices[second][0] - cx) / radius;
            normal[index + 3][1] = (vertices[second][1] - cy) / radius;
            normal[index + 3][2] = (vertices[second][2] - cz) / radius;

            arr[index + 4][0] = vertices[second + 1][0];
            arr[index + 4][1] = vertices[second + 1][1];
            arr[index + 4][2] = vertices[second + 1][2];
            // ���� ���� ���
            normal[index + 4][0] = (vertices[second + 1][0] - cx) / radius;
            normal[index + 4][1] = (vertices[second + 1][1] - cy) / radius;
            normal[index + 4][2] = (vertices[second + 1][2] - cz) / radius;

            arr[index + 5][0] = vertices[first + 1][0];
            arr[index + 5][1] = vertices[first + 1][1];
            arr[index + 5][2] = vertices[first + 1][2];
            // ���� ���� ���
            normal[index + 5][0] = (vertices[first + 1][0] - cx) / radius;
            normal[index + 5][1] = (vertices[first + 1][1] - cy) / radius;
            normal[index + 5][2] = (vertices[first + 1][2] - cz) / radius;

            index += 6; // ���� �ﰢ������ �̵�
        }
    }

    // ���� �޸� ����
    for (int i = 0; i < vertexCount; ++i) {
        delete[] vertices[i];
    }
    delete[] vertices;
}
