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

void Mesh::Initialize(std::string filename)
{
	if (not ReadOBJ(filename)) {
		std::cerr << "obj가 제대로 적용되지 않았습니다" << "\n";
		return;
	}
	

	modelTrans = glm::mat4(1.0f);

	cur_loc = glm::vec3(0.0);
	init_pos = glm::vec3(0.0);
	cur_rot = glm::vec3(0.0);
	init_rot = glm::vec3(0.0);

	
	LB = glm::vec3(0.0f);
	RT = glm::vec3(0.0f);
	for (glm::vec3& pos : vertexs) {
		if (LB.x > pos.x)
			LB.x = pos.x;
		if (LB.y > pos.y)
			LB.y = pos.y;
		if (LB.z > pos.z)
			LB.z = pos.z;
		if (RT.x < pos.x)
			RT.x = pos.x;
		if (RT.y < pos.y)
			RT.y = pos.y;
		if (RT.z < pos.z)
			RT.z = pos.z;
	}
	std::vector<glm::vec3> B_pos;
	{
		B_pos.push_back(glm::vec3(LB.x, LB.y, LB.z));
		B_pos.push_back(glm::vec3(LB.x, LB.y, RT.z));
		B_pos.push_back(glm::vec3(RT.x, LB.y, RT.z));
		B_pos.push_back(glm::vec3(RT.x, LB.y, LB.z));
		B_pos.push_back(glm::vec3(LB.x, RT.y, LB.z));
		B_pos.push_back(glm::vec3(LB.x, RT.y, RT.z));
		B_pos.push_back(glm::vec3(RT.x, RT.y, RT.z));
		B_pos.push_back(glm::vec3(RT.x, RT.y, LB.z));
	}

	std::vector<unsigned int> B_index;
	{
		// 아래
		B_index.push_back(0);
		B_index.push_back(3);
		B_index.push_back(1);
		B_index.push_back(1);
		B_index.push_back(3);
		B_index.push_back(2);
		// 왼쪽
		B_index.push_back(0);
		B_index.push_back(1);
		B_index.push_back(4);
		B_index.push_back(1);
		B_index.push_back(5);
		B_index.push_back(4);
		// 정면
		B_index.push_back(1);
		B_index.push_back(2);
		B_index.push_back(5);
		B_index.push_back(5);
		B_index.push_back(2);
		B_index.push_back(6);
		// 우측
		B_index.push_back(2);
		B_index.push_back(3);
		B_index.push_back(6);
		B_index.push_back(6);
		B_index.push_back(3);
		B_index.push_back(7);
		// 후면
		B_index.push_back(0);
		B_index.push_back(4);
		B_index.push_back(7);
		B_index.push_back(0);
		B_index.push_back(7);
		B_index.push_back(3);
		// 윗면
		B_index.push_back(5);
		B_index.push_back(6);
		B_index.push_back(4);
		B_index.push_back(4);
		B_index.push_back(6);
		B_index.push_back(7);
	}


	OBB_center = (RT + LB) / 2.0f;
	axis[0] = glm::vec3(1, 0, 0);
	axis[1] = glm::vec3(0, 1, 0);
	axis[2] = glm::vec3(0, 0, 1);
	radius.x = abs(OBB_center.x - RT.x);
	radius.y = abs(OBB_center.y - RT.y);
	radius.z = abs(OBB_center.z - RT.z);
}

bool Mesh::ReadOBJ(std::string filename)
{
	std::vector<glm::vec3> vertex_set;		// 정점 저장공간
	std::vector<glm::vec3> normal_set;		// 노말 저장공간
	std::vector<glm::vec3> uv_set;			// 텍스쳐 저장공간

	char c;
	std::string str;
	std::ifstream fin(filename);
	if (not fin) {
		std::cerr << "obj파일을 찾지 못했습니다. " << "\n";
		return false;
	}
	std::stringstream stream;
	stream.str("");
	fin >> std::noskipws;
	while (not fin.eof()) {
		fin >> c;
		str += c;
		if (c == '\n') {
			if (str[0] == 'v' && str[1] == ' ') {
				stream.str(str);
				char novalue;
				glm::vec3 temp_vertex;
				stream >> novalue >> temp_vertex.x >> temp_vertex.y >> temp_vertex.z;
				vertex_set.push_back(temp_vertex);
				stream.str("");
			}
			else if (str[0] == 'v' && str[1] == 'n') {
				stream.str(str);
				char novalue;
				glm::vec3 temp_vertex;
				stream >> novalue >> novalue >> temp_vertex.x >> temp_vertex.y >> temp_vertex.z;
				normal_set.push_back(temp_vertex);
				stream.str("");
			}
			else if (str[0] == 'v' && str[1] == 't') {
				stream.str(str);
				char novalue;
				glm::vec3 temp_vertex;
				stream >> novalue >> novalue >> temp_vertex.x >> temp_vertex.y >> temp_vertex.z;
				uv_set.push_back(temp_vertex);
				stream.str("");
			}
			else if (str[0] == 'f' && str[1] == ' ') {
				stream.str(str);
				int v_index[3]{};
				int n_index[3]{};
				int uv_index[3]{};
				char type;
				stream >> type >> v_index[0] >> type >> uv_index[0] >> type >> n_index[0]
					>> v_index[1] >> type >> uv_index[1] >> type >> n_index[1]
					>> v_index[2] >> type >> uv_index[2] >> type >> n_index[2];
				vertexs.push_back(vertex_set[v_index[0] - 1]);
				vertexs.push_back(vertex_set[v_index[1] - 1]);
				vertexs.push_back(vertex_set[v_index[2] - 1]);
				texture_coor.push_back(uv_set[uv_index[0] - 1]);
				texture_coor.push_back(uv_set[uv_index[1] - 1]);
				texture_coor.push_back(uv_set[uv_index[2] - 1]);
				vertex_normal.push_back(normal_set[n_index[0] - 1]);
				vertex_normal.push_back(normal_set[n_index[1] - 1]);
				vertex_normal.push_back(normal_set[n_index[2] - 1]);
				stream.str("");
			}
			str = "";
		}
	}
	return true;
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