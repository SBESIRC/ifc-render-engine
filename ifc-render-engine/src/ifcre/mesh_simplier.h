#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <limits>
#include <unordered_map>
#include <map>

#include <fstream>
#include <sstream>

#include <cassert>
#include <thread>
#include <time.h>

#include "common/ifc_util.h"

#define usflt
#define y_up
// #define coutlog
#define ustrd
#define PAIRREP

#define threadnum 8

namespace mesh_simplier {
    using namespace std;
    using T = ifcre::Real;/*
    struct Vec3
    {
        T x,y,z;
        Vec3(){
            x=y=z=0;
        }
        Vec3(T _a,T _b,T _c):x(_a),y(_b),z(_c){}
        Vec3(glm::vec3 v):x(v.x),y(v.y),z(v.z){}
        Vec3 normalized(){
            T sum=x*x+y*y+z*z;
            return Vec3(x/sum,y/sum,z/sum);
        }
    };*/
    static T global_pos_epsilon = 0.01f, global_nor_epsilon = 0.01f;
    T dot(glm::vec3 a, glm::vec3 b);

    glm::vec3 cross(glm::vec3 a, glm::vec3 b);

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        Vertex() = default;
        Vertex(glm::vec3 p, glm::vec3 n) :pos(p), normal(n) {}
        void show_off_vertex();
    };
    bool isSameVertex(const Vertex& v1, const Vertex& v2);
    T fabs(T f);

    struct Face {
        glm::vec3 normal;
        vector<uint32_t> index;
#ifdef PAIRREP
        vector<pair< uint32_t, uint32_t>> indexpair;
#endif // PAIRREP
        Face() = default;
        void show_off_face();
    };

    void show_off_vertices();

    void build_ms_vertices(const vector<T>& g_vertices, const vector<T>& g_normals);

    void merge_save_vertex();

    struct Mesh {
        vector<Face> faces;
        int vnnum, vnum, fnum;
        // T same_plane_epsilon=numeric_limits<T>::epsilon();
        // T same_plane_epsilon = global_nor_epsilon;
        Mesh() = default;
        Mesh(const vector<Face>& _faces) {
            faces = _faces;
        }
        Mesh(const string& objfilename);
        vector<uint32_t> edge_index;
#ifdef PAIRREP
        vector<uint32_t> edge_indexp;
#endif // PAIRREP
        void map_save_vertex();
        void face_comb(int a, int b);
        void merge_faces();
    };
    vector<Face> generateFace(const vector<uint32_t>& indices);

    vector<Mesh> generateMeshes(const vector<vector<uint32_t>>& c_indices);

}/*
int main(int argc,char* argv[]){
    auto mesh = ms::Mesh(string(argv[1]));
    mesh.map_save_vertex();
    mesh.merge_faces();
    return 0;
}*/
