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




#include "ENUM.h"

//#include "stb_image.h"

#pragma comment(lib, "ws2_32")

#define MAX_ALIVE 14
#define MAX_ZOMBIE 140
