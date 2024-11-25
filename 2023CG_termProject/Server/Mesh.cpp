#include "Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Mesh::box_check = false;

Mesh::Mesh(std::string filename, std::string f_path, int w_size, int h_size) {
	//Initialize(filename);
	//Init_texture(f_path, w_size, h_size);
	//ambient = glm::vec3(0.3);
}

Mesh::~Mesh()
{
	// 사용했으면 반환해라
	/*glBindVertexArray(VAO);
	glDeleteBuffers(3, VBO);
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);

	glBindVertexArray(B_VAO);
	glDeleteBuffers(1, &B_EBO);
	glDeleteBuffers(3, B_VBO);
	glDeleteVertexArrays(1, &B_VAO);
	std::cout << "Mesh 삭제" << '\n';*/
}





void Mesh::init_scale(float size)
{
	glm::mat4 temp = glm::mat4(1.0f);
	modelTrans = glm::scale(temp, glm::vec3(size, size, size)) * modelTrans;
}

void Mesh::init_position(float x, float y, float z)
{
	glm::mat4 temp = glm::mat4(1.0f);
	modelTrans = glm::translate(temp, glm::vec3(x, y, z)) * modelTrans;
	init_pos = glm::vec3(x, y, z);
}

void Mesh::init_rotate(float rad, float x, float y, float z)
{
	if (x > y && x > z)
		init_rot.x += rad;
	else if (y > x && y > z)
		init_rot.y += rad;
	else if (z > x && z > y)
		init_rot.z += rad;
	glm::mat4 temp = glm::mat4(1.0f);
	modelTrans = glm::rotate(temp, glm::radians(rad), glm::vec3(x, y, z)) * modelTrans;
}

void Mesh::setLoc(glm::vec3 new_loc)
{
	glm::mat4 temp(1.0f);
	modelTrans = glm::translate(temp, -cur_loc) * modelTrans; temp = glm::mat4(1.0f);
	cur_loc = new_loc;
	modelTrans = glm::translate(temp, cur_loc) * modelTrans;
}

void Mesh::setRot(glm::vec2 new_rot)
{
	glm::mat4 temp(1.0f);

	modelTrans = glm::translate(temp, -cur_loc) * modelTrans; temp = glm::mat4(1.0f);

	modelTrans = glm::rotate(temp, glm::radians(-(360 - cur_rot.x)), glm::vec3(0, 1, 0)) * modelTrans; temp = glm::mat4(1.0f);

	modelTrans = glm::rotate(temp, glm::radians(-cur_rot.y), glm::vec3(0, 0, 1)) * modelTrans; temp = glm::mat4(1.0f);

	cur_rot = new_rot;

	modelTrans = glm::rotate(temp, glm::radians(cur_rot.y), glm::vec3(0, 0, 1)) * modelTrans; temp = glm::mat4(1.0f);

	modelTrans = glm::rotate(temp, glm::radians((360 - cur_rot.x)), glm::vec3(0, 1, 0)) * modelTrans; temp = glm::mat4(1.0f);

	modelTrans = glm::translate(temp, cur_loc) * modelTrans; temp = glm::mat4(1.0f);
}

glm::vec3 Mesh::getLB()
{
	return LB;
}

glm::vec3 Mesh::getRT()
{
	return RT;
}

glm::mat4 Mesh::getModelTrans()
{
	return modelTrans;
}

bool Mesh::collision_check(const Mesh& other)
{
	glm::vec3 w_center = glm::vec3(modelTrans * glm::vec4(OBB_center, 1.0f));
	glm::vec3 a_axis[3];
	glm::vec3 o_w_center = glm::vec3(other.modelTrans * glm::vec4(other.OBB_center, 1.0f));
	
	glm::vec3 w_radius = glm::vec3(modelTrans * glm::vec4(radius, 0.0f));

	glm::vec3 o_w_radius = glm::vec3(other.modelTrans * glm::vec4(other.radius, 0.0f));
	for (int i = 0; i < 3; ++i) {
		a_axis[i] = glm::normalize(glm::vec3(modelTrans * glm::vec4(axis[i], 0.0f)));
		/*std::cout << "변환 후: " << a_axis[i].x << ", " << a_axis[i].y << ", " << a_axis[i].z << '\n';
		std::cout << "w_radius: " << w_radius.x << ", " << w_radius.y << ", " << w_radius.z << '\n';*/
		float proj_1 = glm::abs(glm::dot(a_axis[i], w_radius));
		float proj_2 = glm::abs(glm::dot(a_axis[i], o_w_radius));

		float center_dis = glm::abs(glm::dot(a_axis[i], w_center - o_w_center));

		if (center_dis > proj_1 + proj_2)
			return false;
	}
	return true;
}

void Mesh::setAmb(float n)
{
	ambient = glm::vec3(n);
}