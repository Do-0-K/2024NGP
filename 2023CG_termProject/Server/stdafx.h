//==============================================================
// stdafx.h			�⺻������ ����ϴ� ��� ����
//==============================================================
#pragma once

#define _CRT_SECURE_NO_WARNINGS // ���� C �Լ� ��� �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // ���� ���� API ��� �� ��� ����

#include <iostream>
#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���
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