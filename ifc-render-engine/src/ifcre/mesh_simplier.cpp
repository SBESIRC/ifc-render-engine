#include "mesh_simplier.h"
//#define threaddbg
//#define LOG0704
#define FAKER
namespace mesh_simplier {

    vector<Vertex> vertices; // 6个一组 记录顶点 位置 和 法向量
    vector<int> same_vertex_map;

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
        /*if (isSameVec3(v1.pos, v2.pos, global_pos_epsilon) && isSameVec3(v1.normal, v2.normal, global_nor_epsilon)) {
            return true;
        }*/
        return false;
    }


    T fabs(T f) {
        return f < 0 ? -f : f;
    }

    void Face::show_off_face() {
        cout << "Face normal:" << this->normal.x << " " << this->normal.y << " " << this->normal.z << "\n";
        cout << "Face index:\n";
#ifdef PAIRREP
        for (pair<uint32_t, uint32_t> i : this->indexpair) {
            cout << i.first << "->" << i.second << " ";
#else
        for (uint32_t i : this->index) {
            cout << i << " ";
#endif
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
        vector<Vertex>().swap(vertices);
        int s = g_vertices.size(); 
        vertices.resize(g_vertices.size() / 3);// 顶点数量
        for (int i = 0; i < s / 3; i++) { // 顶点数量
            vertices[i] = Vertex(glm::vec3(g_vertices[3 * i], g_vertices[3 * i + 1], g_vertices[3 * i + 2]), // 存入点的位置
                glm::normalize(glm::vec3(g_normals[3 * i], g_normals[3 * i + 1], g_normals[3 * i + 2]))); // 存入点的法向量
        }
        vector<int>().swap(same_vertex_map); // 必须 // 清空数据
        same_vertex_map.resize(vertices.size(), -1);
#ifdef coutlog
        cout << "vertices built, its size:" << vertices.size() << "\n";
#endif
    }

    void merge_save_vertex() { // 合并相同的点
        int s = vertices.size();
        //same_vertex_map.clear();
        same_vertex_map.resize(s, -1);
        for (int i = 0; i < s - 1; i++) {
            if (same_vertex_map[i] != -1)
                continue;
            for (int j = i + 1; j < s; j++) {
                if (isSameVertex(vertices[i], vertices[j])) {
                    /*if (same_vertex_map[i] != -1)
                        same_vertex_map[j] = same_vertex_map[i];
                    else
                        */same_vertex_map[j] = i;
                }
            }
        }/*
        for (int i = 0; i < s; i++) {
            int temp = same_vertex_map[i];
            while (temp != -1) {
                same_vertex_map[i] = temp;
                temp = same_vertex_map[temp];
            }
        }*/
    }

    Mesh::Mesh(const string & objfilename) {
#ifdef coutlog
        //show_off_vertices();
        cout << "\n";
#endif
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
        //vertices.reserve(vnum);
        //faces.reserve(fnum);
        vertices.resize(vnum);
        faces.resize(fnum);
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
                    // ins>>s0>>vertices[vnn].normal.x>>vertices[vnn].normal.y>>vertices[vnn].normal.z;
                    ins >> s0;// vn
                    ins >> s0;
#ifdef usflt
                    vertices[vnn].normal.x = stof(s0);
                    ins >> s0;
#ifdef y_up
                    vertices[vnn].normal.z = stof(s0);
                    ins >> s0;
                    vertices[vnn].normal.y = stof(s0);
#else
                    vertices[vnn].normal.y = stof(s0);
                    ins >> s0;
                    vertices[vnn].normal.z = stof(s0);
#endif
#else
                    vertices[vnn].normal.x = stod(s0);
                    ins >> s0;
                    vertices[vnn].normal.z = stod(s0);
                    ins >> s0;
                    vertices[vnn].normal.y = stod(s0);
#endif
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
#ifdef usflt
                    vertices[vn].pos.x = stof(s0);
                    ins >> s0;
                    vertices[vn].pos.y = stof(s0);
                    ins >> s0;
                    vertices[vn].pos.z = stof(s0);
#else
                    vertices[vn].pos.x = stod(s0);
                    ins >> s0;
                    vertices[vn].pos.y = stod(s0);
                    ins >> s0;
                    vertices[vn].pos.z = stod(s0);
#endif
                    vn++;
                }
            }
            else if (buffer[0] == 'f') {
                istringstream ins(buffer);
                ins >> s0;
#ifdef PAIRREP
                uint32_t lastind = -1;
                uint32_t firstind = -1;
#endif
                while (ins.rdbuf()->in_avail()) {
                    ins >> s0;
                    uint32_t a = 0;
                    int k = 0;
                    for (int k = 0; s0[k] != '/'; k++) {
                        a *= 10;
                        a += s0[k] - 48;
                    }
                    if (a) {
#ifdef PAIRREP
                        if (lastind == -1) {
                            firstind = lastind = a;
                        }
                        else {
                            faces[fn].indexpair.push_back({ lastind,a });
                            lastind = a;
                        }
#endif
                        faces[fn].index.emplace_back(a - 1);
                        faces[fn].normal = vertices[a - 1].normal;
                    }
                }
#ifdef PAIRREP
                faces[fn].indexpair.push_back({ lastind,firstind });
#endif // PAIRREP
                fn++;
            }

        }
#ifdef coutlog
        //show_off_vertices();
        for (auto& f : faces) {
            f.show_off_face();
        }
        cout << "\n";
#endif
    }

    void Mesh::map_save_vertex()
    {
        for (int i = 0; i < faces.size(); i++) {
            int s = faces[i].index.size();
            for (int j = 0; j < s - 1; j++) {
#ifdef PAIRREP
                //if (same_vertex_map[faces[i].indexpair[j].first] != -1)
                //    continue;
                //for (int k = j + 1; k < s; k++) {
                //    if (isSameVertex(vertices[faces[i].indexpair[j].first], vertices[faces[i].indexpair[k].first], global_epsilon)) {
                //        same_vertex_map[faces[i].indexpair[k].first] = faces[i].indexpair[j].first;
                //        // faces[i].index[k] = faces[i].index[j];
                //    }
                //}
#else

#endif // PAIRREP
                if (same_vertex_map[faces[i].index[j]] != -1)
                    continue;
                for (int k = j + 1; k < s; k++) {
                    if (isSameVertex(vertices[faces[i].index[j]], vertices[faces[i].index[k]])) {
                        same_vertex_map[faces[i].index[k]] = faces[i].index[j];
                        faces[i].index[k] = faces[i].index[j];
                    }
                }
            }
        }

        /*for(int i=0;i<faces.size();i++){
            for(int j=0;j<faces[i].index.size();j++){
                if(same_vertex_map[faces[i].index[j]]!=-1){
                    faces[i].index[j]=same_vertex_map[faces[i].index[j]];
                }
            }
        }*/

        // for(int i=0;i<faces.size();i++){
        //     cout<<"f ";
        //     for(int j=0;j<faces[i].index.size();j++){
        //         cout<<faces[i].index[j]+1<<"//"<<faces[i].index[j]+1<<" ";
        //     }
        //     cout<<endl;
        // }
    }
#ifdef PAIRREP
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
#ifdef FAKER
            vector<pair< uint32_t, uint32_t>> new_face_indexp(a.indexpair.begin(), a.indexpair.end());
            new_face_indexp.insert(new_face_indexp.end(), b.indexpair.begin(), b.indexpair.end());
            a.indexpair = new_face_indexp;
#else
            bool xlxmsc = false;
            for (int i = 0; i < asize; i++) {
                for (int j = 0; j < bsize; j++) {
                    if (a.indexpair[i].second == b.indexpair[j].first) {
                        xlxmsc = true;
                        vector<pair< uint32_t, uint32_t>> new_face_indexp(a.indexpair.begin(), a.indexpair.begin() + i + 1);
                        new_face_indexp.insert(new_face_indexp.end(), b.indexpair.begin() + j, b.indexpair.end());
                        new_face_indexp.insert(new_face_indexp.end(), b.indexpair.begin(), b.indexpair.begin() + j);
                        new_face_indexp.insert(new_face_indexp.end(), a.indexpair.begin() + i + 1, a.indexpair.end());
                        a.indexpair = new_face_indexp;
                        break;
                    }
                }
                if (xlxmsc)
                    break;
            }
#endif
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
#endif

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
                        //b.indexpair[j].first = same_vertex_map[a.indexpair[i].first];
                    }
                    else {
                        same_vertex_map[b.indexpair[j].first] = a.indexpair[i].first;
                        //b.indexpair[j].first = a.indexpair[i].first;
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

#ifndef FAKER
        int start1, start2;
        start1 = start2 = -1;

        //same vertex mapping
        int asize = faces[a].index.size();
        int bsize = faces[b].index.size();

        //same_vertex_map.clear();
        bool xflag = false;
        for (int i = 0; i < asize; i++) {
            for (int j = 0; j < bsize; j++) {
                if (faces[a].index[i] == faces[b].index[j]) {
                    xflag = true;
                }
                else if (isSameVertex(vertices[faces[a].index[i]], vertices[faces[b].index[j]], global_epsilon)) {
                    xflag = true;
                    if (same_vertex_map[faces[a].index[i]] != -1) {
                        same_vertex_map[faces[b].index[j]] = same_vertex_map[faces[a].index[i]];
                        faces[b].index[j] = same_vertex_map[faces[a].index[i]];
                    }
                    else {
                        same_vertex_map[faces[b].index[j]] = faces[a].index[i];
                        faces[b].index[j] = faces[a].index[i];
                    }
                }
            }
        }
        if (!xflag) {
            faces[a].index.insert(faces[a].index.end(), faces[b].index.begin(), faces[b].index.end());
            faces[a].indexpair.insert(faces[a].indexpair.end(), faces[b].indexpair.begin(), faces[b].indexpair.end());
            return;
        }

#ifdef PAIRREP
        // cout << a << " " << b << endl;
        int asizep = faces[a].indexpair.size();
        int bsizep = faces[b].indexpair.size();
        for (int i = 0; i < asizep; i++) {
            if (same_vertex_map[faces[a].indexpair[i].first] != -1)
                faces[a].indexpair[i].first = same_vertex_map[faces[a].indexpair[i].first];
            if (same_vertex_map[faces[a].indexpair[i].second] != -1)
                faces[a].indexpair[i].second = same_vertex_map[faces[a].indexpair[i].second];
        }
        for (int i = 0; i < bsizep; i++) {
            if (same_vertex_map[faces[b].indexpair[i].first] != -1)
                faces[b].indexpair[i].first = same_vertex_map[faces[b].indexpair[i].first];
            if (same_vertex_map[faces[b].indexpair[i].second] != -1)
                faces[b].indexpair[i].second = same_vertex_map[faces[b].indexpair[i].second];
        }
#endif

        unordered_map<int, int> m;
        vector<pair<int, int>> p;
        bool flag = false;
#else
        update_new_index_list(faces[a], faces[b]);
#endif

#ifdef PAIRREP
        /*
        for (int i = 0; i < asizep; i++) {
            for (int j = 0; j < bsizep; j++) {
                if(is_these_edge_opposite(faces[a].indexpair[i], faces[b].indexpair[j])){
                    flag = true;
                    start1 = i;
                    start2 = j;
                    break;
                }
            }
            if (flag)
                break;
        }
        if (!flag) {
            //the two faces have only one vertex same
            bool xlxmsc = false;
            for (int i = 0; i < asizep; i++) {
                for (int j = 0; j < bsizep; j++) {
                    if (faces[a].indexpair[i].second == faces[b].indexpair[j].first) {
                        xlxmsc = true;
                        vector<pair< uint32_t, uint32_t>> new_face_indexp(faces[a].indexpair.begin(), faces[a].indexpair.begin() + i + 1);
                        new_face_indexp.insert(new_face_indexp.end(), faces[b].indexpair.begin() + j, faces[b].indexpair.end());
                        new_face_indexp.insert(new_face_indexp.end(), faces[b].indexpair.begin(), faces[b].indexpair.begin() + j);
                        new_face_indexp.insert(new_face_indexp.end(), faces[a].indexpair.begin() + i + 1, faces[a].indexpair.end());
                        faces[a].indexpair = new_face_indexp;
                        break;
                    }
                }
                if (xlxmsc)
                    break;
            }
        }
        else {
            int i = start1;
            int j = start2;
            do {
                start1 = (start1 <= 0) ? asizep - 1 : start1 - 1;
                start2 = (start2 + 1) % bsizep;
                if (i == start1 || j == start2) break;
            } while(is_these_edge_opposite(faces[a].indexpair[start1], faces[b].indexpair[start2]));
            i = (start1 + 1) % asizep;
            j = (start2 <= 0) ? bsizep - 1 : start2 - 1;
            while (is_these_edge_opposite(faces[a].indexpair[i], faces[b].indexpair[j])) {
                if (m.find(i) != m.end())//recyle to the origin point which linked already
                    break;
                m[i] = j;
                p.emplace_back(pair<int, int>{i, j});
                i = (i + 1) % asizep;
                j = (j == 0) ? bsizep - 1 : j - 1;
            }
            if (p.empty())
                return;
            auto [end1, end2] = p.back();
            vector<pair< uint32_t, uint32_t>> new_face_indexp;

            new_face_indexp.emplace_back(faces[a].indexpair[start1]);
            j = start2;
            while (j != end2) {
                new_face_indexp.emplace_back(faces[b].indexpair[j]);
                j = (j + 1) % bsizep;
            }

            i = (end1 + 1) % asizep;
            while (i != start1) {
                new_face_indexp.emplace_back(faces[a].indexpair[i]);
                i = (i + 1) % asizep;
            }
            faces[a].indexpair = new_face_indexp;
        }

        m.clear();
        p.clear();
        flag = false;*/
        get_new_indepair(faces[a], faces[b]);
#endif
#ifndef FAKER
        for (int i = 0; i < asize; i++) {
            for (int j = bsize - 1; j >= 0; j--) {
                if (faces[a].index[i] == faces[b].index[j]) {
                    flag = true;
                    start1 = i;
                    start2 = j;
#ifdef coutlog
                    cout << "im here!\n";
                    cout << i << " " << j << "\n";
#endif
                    break;
                }
            }
            if (flag)
                break;
        }
        if (start1 == -1)
        {
#ifdef coutlog
            cout << "No same vertex.\n";
#endif
            return;
        }
        int i1 = start1;
        int j1 = start2;
        do {
            start1 = (start1 == 0) ? asize - 1 : start1 - 1;
            start2 = (start2 + 1) % bsize;
            if (i1 == start1 || j1 == start2) break;
        } while (faces[a].index[start1] == faces[b].index[start2]);
        start1 = (start1 + 1) % asize;
        start2 = (start2 == 0) ? bsize - 1 : start2 - 1;
        i1 = start1, j1 = start2;
#ifdef coutlog
        cout << "current start:\n" << i << " " << j << endl;
#endif
        while (faces[a].index[i1] == faces[b].index[j1]) {
            if (m.find(i1) != m.end())//recyle to the origin point which linked already
                break;
            m[i1] = j1;
            p.emplace_back(pair<int, int>{i1, j1});
            i1 = (i1 + 1) % asize;
            j1 = (j1 == 0) ? bsize - 1 : j1 - 1;
        }

        auto [end11, end21] = p.back();
#ifdef coutlog
        cout << "back:\n";
        cout << end1 << " " << end2 << endl;
#endif
        vector<uint32_t> new_face_index;

#ifdef LOG0704
        cout << "START\n";
#endif
        new_face_index.emplace_back(faces[a].index[start1]);
        j1 = (start2 + 1) % bsize;
        while (j1 != end21) {
            new_face_index.emplace_back(faces[b].index[j1]);
            j1 = (j1 + 1) % bsize;
        }
        new_face_index.emplace_back(faces[b].index[j1]);
        i1 = (end11 + 1) % asize;
        while (i1 != start1) {
            new_face_index.emplace_back(faces[a].index[i1]);
            i1 = (i1 + 1) % asize;
        }
#ifdef LOG0704
        cout << "FINISHED\n";
#endif
        faces[a].index = new_face_index;/*
        for(auto x:faces[a].index){
            cout<<x+1<<"//"<<x+1<<" ";
        }
        cout<<endl;*/
#endif
    }
    void Mesh::merge_faces() {

        vector<int> vis(faces.size(), -1);
        //vis[k]=-1 means faces[k] is not merged into other face

        for (int i = 0; i < faces.size() - 1; i++) { // 对于构件中的某个三角面片i
            if (/*vis[i] != -1 ||*/ faces[i].index.size() < 1) //if this mesh has only one face, pass
                continue;
            for (int j = i + 1; j < faces.size(); j++) { // 对于构件中的另一个三角面片j
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
#ifdef coutlog
                    cout << "face merge:\n";
                    cout << vis[i] << " " << j << "\n";
#endif
                    face_comb(vis[i], j);
                }
                else {
                    vis[j] = i;
#ifdef coutlog
                    cout << "face merge:\n";
                    cout << i << " " << j << "\n";
#endif
                    face_comb(i, j);
                }
            }
        }
        // find all !vis face, and get the edges
        for (int i = 0; i < faces.size(); i++) { // 对每一个面
            if (vis[i] == -1)
            {
                for (int j = 0; j < faces[i].index.size() - 1; j++) {
                    edge_index.push_back(faces[i].index[j]);
                    edge_index.push_back(faces[i].index[j + 1]);
                }
                edge_index.push_back(faces[i].index[faces[i].index.size() - 1]);
                edge_index.push_back(faces[i].index[0]);
#ifdef PAIRREP
                for (int j = 0; j < faces[i].indexpair.size(); j++) {
                    edge_indexp.push_back(faces[i].indexpair[j].first);
                    edge_indexp.push_back(faces[i].indexpair[j].second);
                }
#endif
            }
        }
#ifdef coutlog
        cout << "edge list:\n";
        for (auto v : edge_index) {
            cout << v << " ";
        }
        cout << endl;
#endif
        // for(auto v:vis){
        //     cout<<v<<" ";
        // }
    }
    vector<Face> generateFace(const vector<uint32_t>&indices) { // 某个构件中所有的顶点
        vector<Face> ret;
        int s_end = indices.size() / 3; // 顶点数 除以 3 == 三角面数量
#ifdef coutlog
        cout << "s_end2:" << s_end << "\n";
#endif
        ret.resize(s_end);
        for (int i = 0; i < s_end; i++) {
            ret[i].normal = vertices[indices[i * 3]].normal; // 记录三角面的法向量
            ret[i].index = { indices[i * 3], indices[i * 3 + 1], indices[i * 3 + 2] }; // 三角面上的三个点的索引
#ifdef PAIRREP
            ret[i].indexpair = { // 目前还是三角面，面上都有3根起始点构成的边
                {indices[i * 3], indices[i * 3 + 1]},
                {indices[i * 3 + 1], indices[i * 3 + 2]},
                {indices[i * 3 + 2], indices[i * 3]}
            };
#endif
        }
#ifdef coutlog
        for (auto& f : ret) {
            f.show_off_face();
        }
#endif
        return ret;
    }

    void thread_task(int n, int end, int threadnum_t, vector<Mesh>&ret) {

        if (n >= end) {
            // cout << "Thread no." << n % threadnum_t << " finished.\n";
            return;
        }
        //ret[n].map_save_vertex();
        ret[n].merge_faces();
#ifdef threaddbg
        if (end - n <= threadnum)
            cout << "ret[" << n << "] done, s_end = " << end << ";\n";
#endif
        thread_task(n + threadnum_t, end, threadnum_t, ret);
    }

    vector<Mesh> generateMeshes(const vector<vector<uint32_t>>&c_indices) { // 二、
        vector<Mesh> ret;
        int s_end = c_indices.size(); // 获取构件数量
        ret.resize(s_end);
        cout << "ret size:" << s_end << endl;
        vector<thread> threads;
#ifdef ustrd
        int threadnum_t = thread::hardware_concurrency(); // 获取并发线程数
        cout << "Spawning " << threadnum_t << " threads.\n";
        clock_t start, end;
        start = clock();
        for (int i = 0; i < s_end; i++) { // 对每一个构件分别进行操作
            ret[i] = Mesh(generateFace(c_indices[i]));
            //ret[i].map_save_vertex();
        }
        end = clock();
        cout << (double)(end - start) / CLOCKS_PER_SEC << "s to generate face\n";

        //thread initialize
        start = clock();
        for (int i = 0; i < threadnum_t; i++) {
            threads.emplace_back(thread(thread_task, i, s_end, threadnum_t, ref(ret)));
        }
        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
        end = clock();
        cout << (double)(end - start) / CLOCKS_PER_SEC << "s to generate edges by " << threadnum_t << " threads\n";
#else
        for (int i = 0; i < s_end; i++) {
            ret[i] = Mesh(generateFace(c_indices[i]));
            ret[i].map_save_vertex();
            ret[i].merge_faces();
#ifdef coutlog
            cout << "edge list:\n";
            for (auto v : ret[i].edge_index) {
                cout << v << " ";
            }
            cout << endl;
#endif
        }
#endif
        return ret;
    }
    }/*
    int main(int argc,char* argv[]){
        auto mesh = ms::Mesh(string(argv[1]));
        mesh.map_save_vertex();
        mesh.merge_faces();
        return 0;
    }*/
