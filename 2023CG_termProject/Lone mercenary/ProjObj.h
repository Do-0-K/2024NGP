//===========================================================
// ProjObj.h
// ������ȯ�� �����ϴ� ���
//===========================================================
#pragma once
#include "stdafx.h"

class ShaderProgram;
class ProjObj {
public:
	ProjObj();

	// �ʿ��ϸ� ���� �Լ� �߰�
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