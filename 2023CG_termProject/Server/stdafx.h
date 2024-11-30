//==============================================================
// stdafx.h			기본적으로 사용하는 헤더 모음
//==============================================================
#pragma once

#define _CRT_SECURE_NO_WARNINGS // 구형 C 함수 사용 시 경고 끄기
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 구형 소켓 API 사용 시 경고 끄기

#include <iostream>
#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/glm.hpp>
#include<gl/glm/ext.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include<sstream>
#include<fstream>
#include<string>
#include <vector>
#include <random>
#include <memory>



using namespace std;
using namespace glm;
#include "ENUM.h"

//#include "stb_image.h"

#pragma comment(lib, "ws2_32")

#define MAX_ALIVE 14
#define MAX_ZOMBIE 14


#pragma pack(1)
struct PlayerInfo {         // Store player information
    glm::vec3 cameraEYE;
    glm::vec2 Angle;
};

struct ObjectInfo {         // Store object(enemy, box, etc.) information
    int HP;
    glm::vec3 Pos;
    glm::vec2 Rot;
};

struct RenderInfo {         // The packet which server sends to client
    int HP;
    int ammo;
    PlayerInfo opposite;
    ObjectInfo alive_enemy[14];
    ObjectInfo box;
    int remainTime;
};

struct UpdateInfo {         // The packet which client sends to server
    int flag;
    bool useItem[4];
    glm::vec3 cameraEYE;
    glm::vec2 cameraangle;
    int ammo;
    int weaponType;

};
#pragma pack()