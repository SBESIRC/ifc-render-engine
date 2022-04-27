#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <time.h>

#include <random>
#include <thread>

#define real_t float

using namespace std;

class Material_new {
public:
    float ka_r, ka_g, ka_b, a;
    float ks_r, ks_g, ks_b;
    int ns;
    Material_new() {}
    Material_new(float a_r, float a_g, float a_b, float s_r, float s_g, float s_b, float a, int ns) :ka_r(a_r), ka_g(a_g), ka_b(a_b), ks_r(s_r), ks_g(s_g), ks_b(s_b), a(a), ns(ns) {}
    int save_to_file(ofstream& os) {
        os.write((const char*)&ka_r, sizeof(float));
        os.write((const char*)&ka_g, sizeof(float));
        os.write((const char*)&ka_b, sizeof(float));
        os.write((const char*)&ks_r, sizeof(float));
        os.write((const char*)&ks_g, sizeof(float));
        os.write((const char*)&ks_b, sizeof(float));
        os.write((const char*)&a, sizeof(float));
        os.write((const char*)&ns, sizeof(int));
        return 0;
    }
};

struct Datas2OpenGL {
    vector<unsigned int> vert_indices;
    vector<vector<unsigned int>> search_m;
    vector<real_t> verts;
    vector<real_t> vert_normals2;
    vector<Material_new> face_mat;
};