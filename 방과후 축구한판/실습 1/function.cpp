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
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iomanip>
#define M_PI 3.14159265358979323846

// 함수 선언
void MakeSphere(GLfloat arr[][3], GLfloat normal[][3], GLfloat cx, GLfloat cy, GLfloat cz, GLfloat radius, int first_index);

// 함수들
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
void MakeSphere(GLfloat arr[][3], GLfloat normal[][3], GLfloat cx, GLfloat cy, GLfloat cz, GLfloat radius, int first_index)
{
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


// obj 데이터 파싱
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
ObjData parseObj(const std::string& filePath) {
    ObjData data;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") { // Vertex
            Vertex vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            data.vertices.push_back(vertex);
        }
        else if (prefix == "vt") { // Texture Coordinate
            TextureCoord texCoord;
            iss >> texCoord.u >> texCoord.v;
            data.texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") { // Normal
            Normal normal;
            iss >> normal.nx >> normal.ny >> normal.nz;
            data.normals.push_back(normal);
        }
        else if (prefix == "f") { // Face
            Face face;
            std::string vertexData;
            while (iss >> vertexData) {
                int vIdx = 0, tIdx = 0, nIdx = 0;
                std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                std::istringstream vertexStream(vertexData);
                vertexStream >> vIdx;
                if (vertexStream.peek() == ' ') vertexStream >> tIdx;
                if (vertexStream.peek() == ' ') vertexStream >> nIdx;
                face.vertices.emplace_back(vIdx - 1, tIdx - 1, nIdx - 1); // Convert to 0-based index
            }
            data.faces.push_back(face);
        }
    }

    file.close();
    return data;
}

void convertToGLArrays(const ObjData& objData, std::vector<GLfloat>& vertexArray, std::vector<GLfloat>& normalArray, std::vector<GLfloat>& texCoordArray) {
    for (const auto& face : objData.faces) {
        // 각 Face의 정점 정보를 삼각형 단위로 분리
        for (size_t i = 1; i + 1 < face.vertices.size(); ++i) {
            // 첫 번째 정점
            int vIdx1 = std::get<0>(face.vertices[0]);
            int nIdx1 = std::get<2>(face.vertices[0]);
            int tIdx1 = std::get<1>(face.vertices[0]); // 텍스쳐 좌표 인덱스

            // 두 번째 정점
            int vIdx2 = std::get<0>(face.vertices[i]);
            int nIdx2 = std::get<2>(face.vertices[i]);
            int tIdx2 = std::get<1>(face.vertices[i]); // 텍스쳐 좌표 인덱스

            // 세 번째 정점
            int vIdx3 = std::get<0>(face.vertices[i + 1]);
            int nIdx3 = std::get<2>(face.vertices[i + 1]);
            int tIdx3 = std::get<1>(face.vertices[i + 1]); // 텍스쳐 좌표 인덱스

            // 정점 좌표 추가
            vertexArray.push_back(objData.vertices[vIdx1].x);
            vertexArray.push_back(objData.vertices[vIdx1].y);
            vertexArray.push_back(objData.vertices[vIdx1].z);

            vertexArray.push_back(objData.vertices[vIdx2].x);
            vertexArray.push_back(objData.vertices[vIdx2].y);
            vertexArray.push_back(objData.vertices[vIdx2].z);

            vertexArray.push_back(objData.vertices[vIdx3].x);
            vertexArray.push_back(objData.vertices[vIdx3].y);
            vertexArray.push_back(objData.vertices[vIdx3].z);

            // 법선 벡터 추가
            if (nIdx1 >= 0) {
                normalArray.push_back(objData.normals[nIdx1].nx);
                normalArray.push_back(objData.normals[nIdx1].ny);
                normalArray.push_back(objData.normals[nIdx1].nz);
            }
            else {
                normalArray.insert(normalArray.end(), { 0.0f, 0.0f, 0.0f });
            }

            if (nIdx2 >= 0) {
                normalArray.push_back(objData.normals[nIdx2].nx);
                normalArray.push_back(objData.normals[nIdx2].ny);
                normalArray.push_back(objData.normals[nIdx2].nz);
            }
            else {
                normalArray.insert(normalArray.end(), { 0.0f, 0.0f, 0.0f });
            }

            if (nIdx3 >= 0) {
                normalArray.push_back(objData.normals[nIdx3].nx);
                normalArray.push_back(objData.normals[nIdx3].ny);
                normalArray.push_back(objData.normals[nIdx3].nz);
            }
            else {
                normalArray.insert(normalArray.end(), { 0.0f, 0.0f, 0.0f });
            }

            // 텍스쳐 좌표 추가
            if (tIdx1 >= 0) {
                texCoordArray.push_back(objData.texCoords[tIdx1].u);
                texCoordArray.push_back(objData.texCoords[tIdx1].v);
            }
            else {
                texCoordArray.push_back(0.0f);
                texCoordArray.push_back(0.0f); // 기본값
            }

            if (tIdx2 >= 0) {
                texCoordArray.push_back(objData.texCoords[tIdx2].u);
                texCoordArray.push_back(objData.texCoords[tIdx2].v);
            }
            else {
                texCoordArray.push_back(0.0f);
                texCoordArray.push_back(0.0f); // 기본값
            }

            if (tIdx3 >= 0) {
                texCoordArray.push_back(objData.texCoords[tIdx3].u);
                texCoordArray.push_back(objData.texCoords[tIdx3].v);
            }
            else {
                texCoordArray.push_back(0.0f);
                texCoordArray.push_back(0.0f); // 기본값
            }
        }
    }
}

GLuint loadBMP(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        std::cout << "Failed to open BMP file: " << filepath << std::endl;
        return 0;
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, file); // BMP 헤더 읽기

    // BMP 파일 검증
    if (header[0] != 'B' || header[1] != 'M') {
        std::cout << "Not a valid BMP file!" << std::endl;
        fclose(file);
        return 0;
    }

    // 이미지 크기 정보 추출
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int imageSize = *(int*)&header[34];

    if (imageSize == 0) imageSize = width * height * 3; // 24비트 BMP의 경우
    unsigned char* data = new unsigned char[imageSize];

    // 이미지 데이터 읽기
    fread(data, sizeof(unsigned char), imageSize, file);
    fclose(file);

    // 텍스처 생성
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 텍스처 데이터 업로드
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 텍스처 필터링 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] data;
    return textureID;
}
