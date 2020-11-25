#include"Obj.h"
#include"shader_sunkue.h"


void ObjData::DelObjData() { glDeleteVertexArrays(1, &this->VAO); vertices.clear(); verIndices.clear(); }
glm::mat4 Obj::world_M() {
	glm::mat4 WM(1.0f);
	std::for_each(this->M.begin(), this->M.end(), [&WM](glm::mat4& m) { WM *= m; });
	//M0*M1*M2 TRS
	return WM;

}
void Obj::DelObj() { if (use_texture)glDeleteTextures(1, &this->texture), use_texture = false; objData.DelObjData(); }
void Obj::Reverse_nor() {
	for (std::vector<Vertex>::iterator i{ this->objData.vertices.begin() }, e{ this->objData.vertices.end() }; i != e; i++) {
		i->nor = -i->nor;
	}
}
void Obj::Set_Alpha(const GLfloat alpha) {
	for (std::vector<Vertex>::iterator i{ this->objData.vertices.begin() }, e{ this->objData.vertices.end() }; i != e; i++) {
		i->col.a = alpha;
	}
	InitBuffer(*this);
}
void Obj::Set_Color(const glm::vec4& color) {
	for (std::vector<Vertex>::iterator i{ this->objData.vertices.begin() }, e{ this->objData.vertices.end() }; i != e; i++) {
		i->col = color;
	}
	InitBuffer(*this);
}


bool LoadObj(const GLchar objFile[], Obj& obj, const GLchar f_style[]) 
{
	std::string OBJ{ "obj/" };
	std::ifstream in(OBJ + objFile, std::ios::in);
	if (!in.is_open()) {
		std::cerr << "ERROR: Objfile read ����\n" << objFile << '\n';
		return false;
	}

	char line[512]{ '\0' };
	Vertex V;
	std::vector<glm::vec3> P(1);
	std::vector<glm::vec2> T(1);
	std::vector<glm::vec3> N(1);
	V.col = glm::vec4(1.0f);
	glm::vec3 tv3;
	glm::vec2 tv2;

	while (!in.eof()) {
		in.getline(line, 512);
		if (strncmp(line, "v ", 2) == 0) {
			sscanf_s(&line[2], "%f %f %f", &tv3.x, &tv3.y, &tv3.z);
			P.push_back(tv3);
		}
		else if (strncmp(line, "vt ", 3) == 0) {
			sscanf_s(&line[3], "%f %f", &tv2.x, &tv2.y);
			T.push_back(tv2);
		}
		else if (strncmp(line, "vn ", 3) == 0) {
			sscanf_s(&line[3], "%f %f %f", &tv3.x, &tv3.y, &tv3.z);
			N.push_back(tv3);
		}
		else if (strncmp(line, "f ", 2) == 0) {
			/*
			char* end = line + sizeof(line) / sizeof(line[0]);
			int num = std::count_if(line, end, [](char c) {return c == '/'; });
			switch (num)
			{
			default:
				break;
			}*/
			unsigned int verI[3], texI[3], norI[3];
			bool vc{ false }, tc{ false }, nc{ false };
			if (f_style == "8") {
				vc = true;
				sscanf_s(&line[2], "%d %d %d", &verI[0], &verI[1], &verI[2]);
			}
			else if (f_style == "8//") {
				vc = true;
				sscanf_s(&line[2], "%d// %d// %d//", &verI[0], &verI[1], &verI[2]);
			}
			else if (f_style == "8/8/") {
				vc = true, tc = true;
				sscanf_s(&line[2], "%d/%d/ %d/%d/ %d/%d/", &verI[0], &texI[0],
					&verI[1], &texI[1], &verI[2], &texI[2]);
			}
			else if (f_style == "8//8") {
				vc = true, nc = true;
				sscanf_s(&line[2], "%d//%d %d//%d %d//%d", &verI[0], &norI[0],
					&verI[1], &norI[1], &verI[2], &norI[2]);
			}
			else if (f_style == "8/8/8") {
				vc = true, tc = true, nc = true;
				sscanf_s(&line[2], "%d/%d/%d %d/%d/%d %d/%d/%d", &verI[0], &texI[0], &norI[0],
					&verI[1], &texI[1], &norI[1], &verI[2], &texI[2], &norI[2]);
			}
			else if (f_style == "8/") {
				vc = true;
				sscanf_s(&line[2], "%d/ %d/ %d/", &verI[0], &verI[1], &verI[2]);
			}
			else if (f_style == "8/8") {
				vc = true, tc = true;
				sscanf_s(&line[2], "%d/%d %d/%d %d/%d", &verI[0], &texI[0],
					&verI[1], &texI[1], &verI[2], &texI[2]);
			}
			else return false;

			for (int i = 0; i < 3; i++) {
				if (vc)V.pos = P[verI[i]];
				if (tc)V.tex = T[texI[i]];
				if (nc)V.nor = N[norI[i]];
				obj.objData.vertices.push_back(V);
			}
		}
	}
	obj.objData.verIndices.resize(obj.objData.vertices.size());
	std::generate(obj.objData.verIndices.begin(), obj.objData.verIndices.end(),
		[n = 0]() mutable { return n++; });
	in.close();
	InitBuffer(obj);
	return true;
}

GLvoid drawObj(Obj& o) {
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(o.world_M()));
	glUniform1i(shineLoc, o.objData.shine);
	glUniform1i(use_texLoc, o.use_texture);
	glBindVertexArray(o.objData.VAO);
	glPolygonMode(GL_FRONT, o.drawmode);
	if (o.use_texture) { glActiveTexture(GL_TEXTURE0), glBindTexture(GL_TEXTURE_2D, o.texture); }
	glDrawElements(o.shape, o.objData.verIndices.size(), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}