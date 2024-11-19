//===========================================================
// ProjObj.h
// 투영변환을 관리하는 헤더
//===========================================================
#pragma once
#include "stdafx.h"

class ShaderProgram;
class ProjObj {
public:
	ProjObj();

	// 필요하면 변경 함수 추가
	void setOrtho(bool bOrtho);
	void OutToShader();

protected:
private:
	ShaderProgram* shader;
	float fovy;
	float aspect;
	float n;
	float f;

	glm::mat4 ProjMatrix;

	bool m_bOrtho = false;

	void Initialize();
};