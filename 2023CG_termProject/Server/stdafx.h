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




#include "ENUM.h"

//#include "stb_image.h"

#pragma comment(lib, "ws2_32")

#define MAX_ALIVE 14
#define MAX_ZOMBIE 140
