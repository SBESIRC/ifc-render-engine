#pragma once
#include <vector>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <glm\glm.hpp>

#define real_t float

using namespace std;

//Math
//---------------------------------------------------------------------------------------
template <typename T>
vector<T> normalized(vector<T>& m) {
	int s = m.size();
	T cur = 0;
	for (int i = 0; i < s; i++) {
		cur += m[i] * m[i];
	}
	cur = sqrt(cur);
	for (int i = 0; i < s; i++) {
		m[i] /= cur;
	}
	return m;
}
struct mater {
	vector<glm::vec3> kds;
	vector<glm::vec3> ks;
	vector<real_t> as;
	vector<int> nss;
};

template <typename T>
class Mesh {
public:
	vector<T> vertices;
	vector<T> normals;//face-based
	vector<unsigned int> faces;

	Mesh(){}
	Mesh(const char* url, const T __scale = 1.0) {//load from obj
		FILE* fp = fopen(url, "r");
		char buffer[105];

		vector<unsigned int> ft(6,0);
		T x, y, z;
		while (!feof(fp)) {
			fgets(buffer, 100, fp);
			if (buffer[0] == '\0' || buffer[1] == '\0')
				continue;
			if (buffer[0] == 'f' && buffer[1] == ' ') {
				if (6 == sscanf_s(buffer, "f %d//%d %d//%d %d//%d", &ft[0], &ft[3], &ft[1], &ft[4], &ft[2], &ft[5])){
				/*if (3 == sscanf_s(buffer, "f %d %d %d", &ft[0], &ft[1], &ft[2])) { */
					for (int i = 0; i < 3; i++) {
						faces.emplace_back(ft[i]-1);
					}
				}
				else
					cout << "Wrong in f input\n";
			}
			else if (buffer[0] == 'v') {
				if (buffer[1] == ' ') {
					if (3 == sscanf_s(buffer, "v %f %f %f", &x, &y, &z)) {
						vertices.emplace_back(x * __scale);
						vertices.emplace_back(z * __scale);
						vertices.emplace_back(y * __scale);
					}
					else
						cout << "Wrong in v input\n";
				}
				else if (buffer[1] == 'n') {
					if (3 == sscanf_s(buffer, "vn %f %f %f", &x, &y, &z)) {
						normals.emplace_back(x * __scale);
						normals.emplace_back(y * __scale);
						normals.emplace_back(z * __scale);
					}
					else
						cout << "Wrong in vn input\n";
				}
			}
		}
		fclose(fp);
	}
	void getNormal() {
		int s = faces.size();
		T vec[3][3];
		for (int i = 0; i < s; i+=3) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					vec[j][k] = vertices[faces[i + j] * 3 + k];
				}
			}
			vector<T> dif1 = { vec[1][0] - vec[0][0],vec[1][1] - vec[0][1],vec[1][2] - vec[0][2] };
			vector<T> dif2 = { vec[2][0] - vec[1][0],vec[2][1] - vec[1][1],vec[2][2] - vec[1][2] };
			vector<T> dif3 = { dif1[1] * dif2[2] - dif1[2] * dif2[1] ,dif1[2] * dif2[0] - dif1[0] * dif2[2] ,dif1[0] * dif2[1] - dif1[1] * dif2[0] };
			normalized(dif3);
			for (int j = 0; j < 3; j++) {
				normals.emplace_back(dif3[j]);
			}
		}
	}

	vector<T> boundingbox() {
		T x_min, x_max, y_min, y_max, z_min, z_max;
		x_min = x_max = vertices[0];
		y_min = y_max = vertices[1];
		z_min = z_max = vertices[2];
		int s = vertices.size();
		for (int i = 3; i < s; i += 3) {
			x_min = min(x_min, vertices[i]);
			x_max = max(x_max, vertices[i]);
			y_min = min(y_min, vertices[i + 1]);
			y_max = max(y_max, vertices[i + 1]);
			z_min = min(z_min, vertices[i + 2]);
			z_max = max(z_max, vertices[i + 2]);
		}
		return { x_min,x_max, y_min,y_max, z_min,z_max };
	}
};
vector<real_t> read_from_midfile(const string filename, vector<real_t>& v, vector<real_t>& vn, vector<int>& f, vector<vector<int>>& fbc, mater& mt) {
	//load from ifc-midfile
	ifstream is(filename.c_str(), ios::binary);
	size_t s;
	is.read((char*)&s, sizeof(size_t));
	v.resize(s);
	for (int i = 0; i < s; i++) {
		is.read((char*)&v[i], sizeof(real_t));
	}
	real_t x_min, x_max, y_min, y_max, z_min, z_max;
	x_min = x_max = v[0];
	y_min = y_max = v[1];
	z_min = z_max = v[2];
	for (int i = 0; i < s; i += 3) {
		x_min = min(x_min, v[i]);
		x_max = max(x_max, v[i]);
		y_min = min(y_min, v[i + 1]);
		y_max = max(y_max, v[i + 1]);
		z_min = min(z_min, v[i + 2]);
		z_max = max(z_max, v[i + 2]);
	}

	is.read((char*)&s, sizeof(size_t));
	vn.resize(s);
	for (int i = 0; i < s; i++) {
		is.read((char*)&vn[i], sizeof(real_t));
	}

	is.read((char*)&s, sizeof(size_t));
	f.resize(s);
	for (int i = 0; i < s; i++) {
		is.read((char*)&f[i], sizeof(unsigned int));
		f[i]--;
	}
	is.read((char*)&s, sizeof(size_t));
	fbc.resize(s);
	size_t tmps;
	for (int i = 0; i < s; i++) {
		is.read((char*)&tmps, sizeof(size_t));
		vector<int> tmpvc(tmps);
		for (int j = 0; j < tmps; j++) {
			is.read((char*)&tmpvc[j], sizeof(unsigned int));
			tmpvc[j]--;
		}
		fbc[i] = tmpvc;
	}
	is.read((char*)&s, sizeof(size_t));
	mt.kds.resize(s), mt.ks.resize(s), mt.as.resize(s), mt.nss.resize(s);
	float r, g, b, a;
	int nst;
	for (int i = 0; i < s; i++) {
		//a
		is.read((char*)&r, sizeof(float));
		is.read((char*)&g, sizeof(float));
		is.read((char*)&b, sizeof(float));
		mt.kds[i] = glm::vec3(r, g, b);
		//s
		is.read((char*)&r, sizeof(float));
		is.read((char*)&g, sizeof(float));
		is.read((char*)&b, sizeof(float));
		mt.ks[i] = glm::vec3(r, g, b);
		//transparency
		is.read((char*)&a, sizeof(float));
		mt.as[i] = a;
		//ns
		is.read((char*)&nst, sizeof(int));
		mt.nss[i] = nst;
	}
	return { x_min,x_max, y_min,y_max, z_min,z_max };//return bounding box
}