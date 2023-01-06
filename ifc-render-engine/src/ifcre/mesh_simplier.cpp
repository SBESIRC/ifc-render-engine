#include "mesh_simplier.h"
//#define threaddbg
//#define LOG0704
#define FAKER
namespace mesh_simplier {

    vector<Vertex> vertices = {}; // 6个一组 记录顶点 位置 和 法向量
    vector<int> same_vertex_map = { 0 };

    T dot(glm::vec3 a, glm::vec3 b) {
        return glm::dot(a, b);
    }

    glm::vec3 cross(glm::vec3 a, glm::vec3 b) {
        return glm::cross(a, b);
    }

    void Vertex::show_off_vertex() {
        cout << "v:  " << this->pos.x << " " << this->pos.y << " " << this->pos.z << "\n";
        cout << "vn: " << this->normal.x << " " << this->normal.y << " " << this->normal.z << "\n";
    }
    bool isSameVec3(const glm::vec3& v1, const glm::vec3& v2, T epsilon  = global_pos_epsilon) {
        if (fabs(v1.x - v2.x) <= epsilon) {
            if (fabs(v1.y - v2.y) <= epsilon) {
                if (fabs(v1.z - v2.z) <= epsilon) {
                        return true;
                }
            }
        }
        return false;
    }

    bool isSameVertex(const Vertex& v1, const Vertex& v2) {
        if (isSameVec3(v1.pos, v2.pos, global_pos_epsilon) && isSameVec3(v1.normal, v2.normal, global_pos_epsilon)) {
            return true;
        }
        return false;
    }
    /*
    T fabs(T f){
        return f<0?-f:f;
    }
    bool isSameVertex(const Vertex& v1, const Vertex& v2, T epsilon) {
        if (fabs(v1.pos.x - v2.pos.x) <= epsilon) {
            if (fabs(v1.pos.y - v2.pos.y) <= epsilon) {
                if (fabs(v1.pos.z - v2.pos.z) <= epsilon) {
                    if (dot(v1.normal, v2.normal) > 1 - epsilon)
                        return true;
                }
            }
        }
        return false;
    }*/

    T fabs(T f) {
        return f < 0 ? -f : f;
    }

    void Face::show_off_face() {
        cout << "Face normal:" << this->normal.x << " " << this->normal.y << " " << this->normal.z << "\n";
        cout << "Face index:\n";
        for (pair<uint32_t, uint32_t> i : this->indexpair) {
            cout << i.first << "->" << i.second << " ";
        }
        cout << endl;
     }

    void show_off_vertices() {
        for (auto& v : vertices) {
            v.show_off_vertex();
        }
    }

    void build_ms_vertices(const vector<T>&g_vertices, const vector<T>&g_normals) { // 一、存储顶点的位置和法向量
        assert(g_vertices.size() == g_normals.size());
        int s = g_vertices.size();

        vector<Vertex>(s / 3).swap(vertices);// 顶点数量
        for (int i = 0; i < s / 3; i++) { // 顶点数量
            vertices[i] = Vertex(glm::vec3(g_vertices[3 * i], g_vertices[3 * i + 1], g_vertices[3 * i + 2]), // 存入点的位置
                glm::normalize(glm::vec3(g_normals[3 * i], g_normals[3 * i + 1], g_normals[3 * i + 2]))); // 存入点的法向量
        }
        vector<int>(s, -1).swap(same_vertex_map); // 必须 // 清空数据
    }

    void merge_save_vertex() { // 合并相同的点
        int s = vertices.size();
        vector<int>(s, -1).swap(same_vertex_map);
        for (int i = 0; i < s - 1; i++) {
            if (same_vertex_map[i] != -1)
                continue;
            for (int j = i + 1; j < s; j++) {
                if (isSameVertex(vertices[i], vertices[j])) {
                    same_vertex_map[j] = i;
                }
            }
        }
    }

    Mesh::Mesh(const string & objfilename) {
        vnum = fnum = vnnum = 0;
        ifstream file(objfilename.c_str(), ios::in);
        string buffer;
        while (getline(file, buffer)) {
            if (buffer[0] == 'v') {
                if (buffer[1] == 'n') {// vertex normal
                    //do something
                    vnnum++;
                }
                else {//vertex pos
                    vnum++;
                }
            }
            else if (buffer[0] == 'f')
                fnum++;
        }
        assert(vnum == vnnum);
        vector<Vertex>(vnum).swap(vertices);
        vector<Face>(fnum).swap(faces);
        file.close();
        string s0, s1, s2, s3, s4;
        int vnn, vn, fn;
        vnn = vn = fn = 0;
        file.open(objfilename.c_str(), ios::in);
        while (getline(file, buffer)) {
            if (buffer[0] == 'v') {
                //continue;
                if (buffer[1] == 'n') {
                    istringstream ins(buffer);
                    ins >> s0;// vn
                    ins >> s0;

                    vertices[vnn].normal.x = stof(s0);
                    ins >> s0;

                    vertices[vnn].normal.z = stof(s0);
                    ins >> s0;
                    vertices[vnn].normal.y = stof(s0);

                    vnn++;
                    //why ins>>x>>z>>y not ins>>x>>y>>z?
                    //because goddamn ifcOpenShell use z-up, and when we change to y-up
                    //the y & z of position of one point have been swapt
                    //but the y & z of normal of this point havent
                }
                else {
                    istringstream ins(buffer);
                    // ins>>s0>>vertices[vn].pos.x>>vertices[vn].normal.y>>vertices[vn].pos.z;
                    ins >> s0;// v
                    ins >> s0;

                    vertices[vn].pos.x = stof(s0);
                    ins >> s0;
                    vertices[vn].pos.y = stof(s0);
                    ins >> s0;
                    vertices[vn].pos.z = stof(s0);

                    ++vn;
                }
            }
            else if (buffer[0] == 'f') {
                istringstream ins(buffer);
                ins >> s0;

                uint32_t lastind = -1;
                uint32_t firstind = -1;

                while (ins.rdbuf()->in_avail()) {
                    ins >> s0;
                    uint32_t a = 0;
                    int k = 0;
                    for (int k = 0; s0[k] != '/'; ++k) {
                        a *= 10;
                        a += s0[k] - 48;
                    }
                    if (a) {
                        if (lastind == -1) {
                            firstind = lastind = a;
                        }
                        else {
                            faces[fn].indexpair.push_back({ lastind,a });
                            lastind = a;
                        }
                        faces[fn].index.emplace_back(a - 1);
                        faces[fn].normal = vertices[a - 1].normal;
                    }
                }
                faces[fn].indexpair.push_back({ lastind,firstind });
                ++fn;
            }
        }
    }

    void Mesh::map_save_vertex()
    {
        for (int i = 0; i < faces.size(); ++i) {
            int s = faces[i].index.size();
            for (int j = 0; j < s - 1; ++j) {
                if (same_vertex_map[faces[i].index[j]] != -1)
                    continue;
                for (int k = j + 1; k < s; ++k) {
                    if (isSameVertex(vertices[faces[i].index[j]], vertices[faces[i].index[k]])) {
                        same_vertex_map[faces[i].index[k]] = faces[i].index[j];
                        faces[i].index[k] = faces[i].index[j];
                    }
                }
            }
        }
    }

    bool is_these_edge_opposite(const pair< uint32_t, uint32_t>&a, const pair< uint32_t, uint32_t>&b) {
        return a.first == b.second && a.second == b.first;
    }
    pair<unordered_map<uint32_t, uint32_t>, unordered_map<uint32_t, uint32_t>> get_opposite_edge(const Face & a, const Face & b) {
        uint32_t asize = a.indexpair.size();
        uint32_t bsize = b.indexpair.size();
        unordered_map<uint32_t, uint32_t> ret1;
        unordered_map<uint32_t, uint32_t> ret2;
        bool breakflag;
        for (int i = 0; i < asize; i++) {
            breakflag = false;
            for (int j = bsize - 1; j >= 0; j--) {
                if (is_these_edge_opposite(a.indexpair[i], b.indexpair[j])) {
                    breakflag = true;
                    ret1[i] = j;
                    ret2[j] = i;
                    break;
                }
            }
            if (breakflag)
                continue;
        }
        return { ret1,ret2 };
    }


    void get_new_indepair(Face & a, const Face & b) {
        uint32_t asize = a.indexpair.size();
        uint32_t bsize = b.indexpair.size();
        auto [same_pair_map_a, same_pair_map_b] = get_opposite_edge(a, b);
        if (same_pair_map_a.size() == 0) {
            vector<pair< uint32_t, uint32_t>> new_face_indexp(a.indexpair.begin(), a.indexpair.end());
            new_face_indexp.insert(new_face_indexp.end(), b.indexpair.begin(), b.indexpair.end());
            a.indexpair = new_face_indexp;
            return;
        }
        vector<pair< uint32_t, uint32_t>> ret;
        uint32_t ait = 0;
        uint32_t bit = 0;
        for (; ait < asize; ait++) { //面a每一条边
            if (same_pair_map_a.find(ait) == same_pair_map_a.end())
                ret.emplace_back(a.indexpair[ait]);
        }
        for (; bit < bsize; bit++) { //面b每一条边
            if (same_pair_map_b.find(bit) == same_pair_map_b.end())
                ret.emplace_back(b.indexpair[bit]);
        }
        a.indexpair = ret;
    }

    void update_new_index_list(Face & a, Face & b) { // 填充same_vertex_map：将面b可能相同的点用面a代替
        int asize = a.indexpair.size();
        int bsize = b.indexpair.size();
        //unordered_map<uint32_t, uint32_t> copymap;
        for (int i = 0; i < asize; i++) { // 遍历面a每一条边i
            for (int j = 0; j < bsize; j++) { // 遍历面b每一条边j
                if (a.indexpair[i].first == b.indexpair[j].first)
                    continue;
                if (isSameVertex(vertices[a.indexpair[i].first], vertices[b.indexpair[j].first])) {
                    if (same_vertex_map[a.indexpair[i].first] != -1) {
                        same_vertex_map[b.indexpair[j].first] = same_vertex_map[a.indexpair[i].first];
                    }
                    else {
                        same_vertex_map[b.indexpair[j].first] = a.indexpair[i].first;
                    }
                }
            }
        }
        for (int j = 0; j < bsize; j++) {
            if (same_vertex_map[b.indexpair[j].first] != -1) {
                b.indexpair[j].first = same_vertex_map[b.indexpair[j].first];
            }
            if (same_vertex_map[b.indexpair[j].second] != -1) {
                b.indexpair[j].second = same_vertex_map[b.indexpair[j].second];
            }
        }
    }

    void Mesh::face_comb(int a, int b) {
        update_new_index_list(faces[a], faces[b]);
        get_new_indepair(faces[a], faces[b]);
    }

    std::mutex mtx;
    void Mesh::merge_faces() {

        std::lock_guard<std::mutex> lk(mtx);
        vector<int> vis(faces.size(), -1);
        //vis[k]=-1 means faces[k] is not merged into other face

        for (int i = 0; i < faces.size() - 1; ++i) { // 对于构件中的某个三角面片i
            if (/*vis[i] != -1 ||*/ faces[i].index.size() < 1) //if this mesh has only one face, pass
                continue;
            for (int j = i + 1; j < faces.size(); ++j) { // 对于构件中的另一个三角面片j
                if (vis[j] != -1 || faces[j].index.size() < 1)
                    //face_j has been merged, so just pass
                    continue;
                //double xx = dot(faces[i].normal, faces[j].normal); // may be should use this but epsilon 0.1
                //if (!isSameVec3(faces[i].normal, faces[j].normal, global_nor_epsilon)) {
                //    continue; // 只对法向量相同的进行处理
                //}
                //if (dot(faces[i].normal, faces[j].normal) < 1.f - global_nor_epsilon) {
                /*if (dot(faces[i].normal, faces[j].normal) < 0.1f) {
                    continue;
                }*/
                if (vis[i] != -1) { // i 面已经处理过了
                    //face_i has been merged before
                    vis[j] = vis[i];
                    face_comb(vis[i], j);
                }
                else {
                    vis[j] = i;
                    face_comb(i, j);
                }
            }
        }
        // find all !vis face, and get the edges
        for (int i = 0; i < faces.size(); ++i) { // 对每一个面
            if (vis[i] == -1)
            {
                for (int j = 0; j < faces[i].index.size() - 1; ++j) {
                    edge_index.emplace_back(faces[i].index[j]);
                    edge_index.emplace_back(faces[i].index[j + 1]);
                }
                edge_index.emplace_back(faces[i].index[faces[i].index.size() - 1]);
                edge_index.emplace_back(faces[i].index[0]);
                for (int j = 0; j < faces[i].indexpair.size(); ++j) {
                    edge_indexp.emplace_back(faces[i].indexpair[j].first);
                    edge_indexp.emplace_back(faces[i].indexpair[j].second);
                }
            }
        }
    }
    vector<Face> generateFace(const vector<uint32_t>&indices) { // 某个构件中所有的顶点
        int s_end = indices.size() / 3; // 顶点数 除以 3 == 三角面数量
        vector<Face> ret;
        for (int i = 0; i < s_end; i++) {
            Face face;
            face.normal = vertices[indices[i * 3]].normal; // 记录三角面的法向量
            face.index = { indices[i * 3], indices[i * 3 + 1], indices[i * 3 + 2] }; // 三角面上的三个点的索引
            face.indexpair = { // 目前还是三角面，面上都有3根起始点构成的边
                {indices[i * 3], indices[i * 3 + 1]},
                {indices[i * 3 + 1], indices[i * 3 + 2]},
                {indices[i * 3 + 2], indices[i * 3]}
            };
            ret.emplace_back(face);
        }
        return ret;
    }

    void thread_task(int n, int end, int threadnum_t, vector<Mesh>&ret) {
        if (n >= end) {
            return;
        }
        ret[n].merge_faces();

        thread_task(n + threadnum_t, end, threadnum_t, ret);
    }

    vector<Mesh> generateMeshes(const vector<vector<uint32_t>>&c_indices) { // 二、
        int s_end = c_indices.size(); // 获取构件数量
        vector<Mesh> ret(s_end);
        cout << "ret size:" << s_end << endl;
        //vector<thread> threads;
        //int threadnum_t = thread::hardware_concurrency(); // 获取并发线程数
        //threadnum_t = threadnum_t >= 2 ? threadnum_t - 1 : threadnum_t;
        //cout << "Spawning " << threadnum_t << " threads.\n";
        clock_t start, end;
        start = clock();
        for (int i = 0; i < s_end; i++) { // 对每一个构件分别进行操作
            ret[i] = Mesh(generateFace(c_indices[i]));
        }
        end = clock();
        cout << (double)(end - start) / CLOCKS_PER_SEC << "s to generate face\n";

        //thread initialize
        //start = clock();
        for (int i = 0; i < s_end; ++i) {
            ret[i].merge_faces();
        }
        //for (int i = 0; i < threadnum_t; ++i) {
        //    threads.emplace_back(thread(thread_task, i, s_end, threadnum_t, ref(ret)));
        //}
        /*for (int i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }*/
        //end = clock();
        //cout << (double)(end - start) / CLOCKS_PER_SEC << "s to generate edges by " << threadnum_t << " threads\n";
        return ret;
    }
}
