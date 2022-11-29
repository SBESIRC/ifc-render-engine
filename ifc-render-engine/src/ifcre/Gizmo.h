#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "common/ifc_util.h"
namespace ifcre {
	static std::vector<uint32_t> counter_clockwise_cube_element_object_buffer = {
		//back			 //left			   //front
		0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11,
		//right				   //up					   //down
		12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 22, 21, 20, 23, 22,
		//edges
		25, 24, 29, 25, 29, 26,		28, 27, 26, 28, 26, 29,		31, 30, 35, 31, 35, 34,		32, 31, 34, 32, 34, 33,
		40, 37, 36, 40, 36, 41,		39, 38, 37, 39, 37, 40,		43, 42, 47, 43, 47, 46,		44, 43, 46, 44, 46, 45,

		49, 48, 53, 49, 53, 52,		52, 51, 50, 52, 50, 49,		54, 55, 58, 54, 58, 59,		57, 58, 55, 57, 55, 56,
		65, 64, 61, 65, 61, 60,		63, 62, 61, 63, 61, 64,		70, 71, 66, 70, 66, 67,		70, 67, 68, 70, 68, 69,

		72, 73, 76, 72, 76, 77,		73, 74, 75, 73, 75, 76,		80, 79, 82, 80, 82, 81,		79, 78, 83, 79, 83, 82,
		86, 87, 88, 86, 88, 85,		85, 88, 89, 85, 89, 84,		91, 94, 93, 91, 93, 92,		91, 90, 95, 91, 95, 94,
		//points
		97, 96, 101, 97, 101, 100,		98, 97, 100, 98, 100, 99,		101, 102, 99, 101, 99, 100,
		109, 108, 107, 109, 107, 106,	103, 104, 107, 103, 107, 108,	104, 105, 106, 104, 106, 107,
		114, 115, 110, 114, 110, 111,	113, 114, 111, 113, 111, 112,	116, 115, 114, 116, 114, 113,
		122, 123, 120, 122, 120, 121,	121, 120, 119, 121, 119, 118,	122, 121, 118, 122, 118, 117,
		125, 126, 127, 125, 127, 128,	128, 127, 130, 128, 130, 129,	124, 125, 128, 124, 128, 129,
		133, 132, 135, 133, 135, 134,	134, 135, 136, 134, 136, 137,	132, 131, 136, 132, 136, 135,
		138, 143, 142, 138, 142, 139,	142, 141, 140, 142, 140, 139,	141, 142, 143, 141, 143, 144,
		148, 149, 146, 148, 146, 147,	149, 148, 151, 149, 151, 150,	149, 150, 145, 149, 145, 146
	};
	struct SceneGizmo {
		GLuint gizmoVAO, gizmoVBO, gizmoEBO;
		GLuint gizmo_textID;
		float k = .15f;
		float k2 = 2 * k;
		SceneGizmo(int id_num = 0) {
			float coord_plane[] = {
				// position     uv cordination	ids
				//back 0
                                 k, -k, -k2,        0.00f, 0.00f,        util::int_as_float(id_num + 0),
                -k, -k, -k2,        0.25f, 0.00f,        util::int_as_float(id_num + 0),
                -k,  k, -k2,        0.25f, 0.50f,        util::int_as_float(id_num + 0),
                 k,  k, -k2,        0.00f, 0.50f,        util::int_as_float(id_num + 0),
                 //left 4
                 -k2, -k,  k,        0.75f, 0.50f,        util::int_as_float(id_num + 1),
                 -k2,  k,  k,        0.75f, 1.00f,        util::int_as_float(id_num + 1),
                 -k2,  k, -k,        0.50f, 1.00f,        util::int_as_float(id_num + 1),
                 -k2, -k, -k,        0.50f, 0.50f,        util::int_as_float(id_num + 1),
                 //front 8
                 -k, -k,  k2,        0.00f, 0.50f,        util::int_as_float(id_num + 2),
                  k, -k,  k2,        0.25f, 0.50f,        util::int_as_float(id_num + 2),
                  k,  k,  k2,        0.25f, 1.00f,        util::int_as_float(id_num + 2),
                 -k,  k,  k2,        0.00f, 1.00f,        util::int_as_float(id_num + 2),
                 //right 12
                  k2, -k,  k,        0.50f, 0.00f,        util::int_as_float(id_num + 3),
                  k2, -k, -k,        0.75f, 0.00f,        util::int_as_float(id_num + 3),
                  k2,  k, -k,        0.75f, 0.50f,        util::int_as_float(id_num + 3),
                  k2,  k,  k,        0.50f, 0.50f,        util::int_as_float(id_num + 3),
                  //up 16
                  -k,  k2,  k,        0.25f, 0.50f,        util::int_as_float(id_num + 4),
                   k,  k2,  k,        0.50f, 0.50f,        util::int_as_float(id_num + 4),
                   k,  k2, -k,        0.50f, 1.00f,        util::int_as_float(id_num + 4),
                  -k,  k2, -k,        0.25f, 1.00f,        util::int_as_float(id_num + 4),
                  //down 20
                   k, -k2, -k,        0.50f, 0.00f,        util::int_as_float(id_num + 5),
                  -k, -k2, -k,        0.25f, 0.00f,        util::int_as_float(id_num + 5),
                  -k, -k2,  k,        0.25f, 0.50f,        util::int_as_float(id_num + 5),
                   k, -k2,  k,        0.50f, 0.50f,        util::int_as_float(id_num + 5),

                   //edge back up 24
                   -k,     k, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 6),
                   k,  k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 6),
                   k,  k2, -k2,      0.00f, 0.00f,  util::int_as_float(id_num + 6),
                   k,  k2, -k,       0.00f, 0.00f,     util::int_as_float(id_num + 6),
                   -k, k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 6),
                   -k, k2, -k2,       0.00f, 0.00f, util::int_as_float(id_num + 6),

                   //edge back left 30
                   -k2, -k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 7),
                   -k2, -k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 7),
                   -k,  -k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 7),
                   -k,   k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 7),
                   -k2,  k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 7),
                   -k2,  k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 7),

                   //edge back down 36
                   -k,  -k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 8),
                   -k, -k2, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 8),
                   -k, -k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 8),
                    k, -k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 8),
                    k, -k2, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 8),
                    k,  -k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 8),

                    //edge back right 42
                     k,  -k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 9),
                     k2, -k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 9),
                     k2, -k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 9),
                     k2,  k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 9),
                     k2,  k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 9),
                     k,   k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 9),

                     //edge left up 48
                     -k2,  k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 10),
                     -k2, k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 10),
                     -k,  k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 10),
                     -k,  k2,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 10),
                     -k2, k2,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 10),
                     -k2,  k,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 10),

                     //edge left down 54
                     -k2, -k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 11),
                     -k2,-k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 11),
                     -k, -k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 11),
                     -k, -k2,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 11),
                     -k2,-k2,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 11),
                     -k2, -k,  k,        0.00f, 0.00f,     util::int_as_float(id_num + 11),
                     //edge right up 60
                       k, k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 12),
                      k2, k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 12),
                      k2,  k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 12),
                      k2,  k,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 12),
                      k2, k2,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 12),
                       k, k2,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 12),

                       //edge right down 66
                        k, -k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 13),
                        k2,-k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 13),
                        k2, -k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 13),
                        k2, -k,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 13),
                        k2, -k2, k,        0.00f, 0.00f,    util::int_as_float(id_num + 13),
                        k,  -k2, k,        0.00f, 0.00f,    util::int_as_float(id_num + 13),

                        //edge left front 72
                        -k2, -k,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 14),
                        -k2, -k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 14),
                         -k, -k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 14),
                         -k,  k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 14),
                        -k2,  k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 14),
                        -k2,  k,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 14),

                        //edge right front 78
                         k2, -k,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 15),
                         k2, -k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 15),
                          k, -k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 15),
                          k,  k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 15),
                         k2,  k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 15),
                         k2,  k,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 15),

                         //edge up front 84
                         -k, k2,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 16),
                         -k, k2, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 16),
                         -k,  k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 16),
                          k,  k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 16),
                          k, k2, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 16),
                          k, k2,  k,        0.00f, 0.00f,    util::int_as_float(id_num + 16),

                          //edge down front 90
                          -k, -k2, k,        0.00f, 0.00f,    util::int_as_float(id_num + 17),
                          -k, -k2, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 17),
                          -k, -k,  k2,        0.00f, 0.00f,    util::int_as_float(id_num + 17),
                           k, -k,  k2,        0.00f, 0.00f,    util::int_as_float(id_num + 17),
                           k, -k2, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 17),
                           k, -k2, k,        0.00f, 0.00f,    util::int_as_float(id_num + 17),

                           //point back left up 96
                           -k2, k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 18),
                           -k2, k, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 18),
                           -k, k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 18),
                           -k, k2, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 18),
                           -k2, k2, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 18),
                           -k2, k2, -k,    0.00f, 0.00f,    util::int_as_float(id_num + 18),
                           -k, k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 18),

                           //point back right up 103
                           k2, k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 19),
                           k2, k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 19),
                           k, k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 19),
                           k, k2, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 19),
                           k2, k2, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 19),
                           k2, k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 19),
                           k, k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 19),

                           //point back left down 110
                           -k2, -k, -k,    0.00f, 0.00f,    util::int_as_float(id_num + 20),
                           -k2, -k, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 20),
                           -k, -k, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 20),
                           -k, -k2, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 20),
                           -k2, -k2, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 20),
                           -k2, -k2, -k,    0.00f, 0.00f,    util::int_as_float(id_num + 20),
                           -k, -k2, -k,    0.00f, 0.00f,     util::int_as_float(id_num + 20),

                           //point back left down 117
                           k2, -k, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 21),
                           k2, -k, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 21),
                           k, -k, -k2,        0.00f, 0.00f,    util::int_as_float(id_num + 21),
                           k, -k2, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 21),
                           k2, -k2, -k2,    0.00f, 0.00f,    util::int_as_float(id_num + 21),
                           k2, -k2, -k,    0.00f, 0.00f,    util::int_as_float(id_num + 21),
                           k, -k2, -k,        0.00f, 0.00f,    util::int_as_float(id_num + 21),

                           //point front left up 124
                           -k2, k, k,        0.00f, 0.00f,    util::int_as_float(id_num + 22),
                           -k2, k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 22),
                           -k, k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 22),
                           -k, k2, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 22),
                           -k2, k2, k2,    0.00f, 0.00f,    util::int_as_float(id_num + 22),
                           -k2, k2, k,        0.00f, 0.00f,    util::int_as_float(id_num + 22),
                           -k, k2, k,        0.00f, 0.00f,    util::int_as_float(id_num + 22),

                           //point front right up 131
                           k2, k, k,        0.00f, 0.00f,    util::int_as_float(id_num + 23),
                           k2, k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 23),
                           k, k, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 23),
                           k, k2, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 23),
                           k2, k2, k2,        0.00f, 0.00f,    util::int_as_float(id_num + 23),
                           k2, k2, k,        0.00f, 0.00f,    util::int_as_float(id_num + 23),
                           k, k2, k,        0.00f, 0.00f,    util::int_as_float(id_num + 23),

                           //point front left down 138
                           -k2, -k, k,     0.00f, 0.00f,      util::int_as_float(id_num + 24),
                           -k2,-k, k2,      0.00f, 0.00f,     util::int_as_float(id_num + 24),
                           -k, -k, k2,     0.00f, 0.00f,      util::int_as_float(id_num + 24),
                           -k,-k2, k2,      0.00f, 0.00f,     util::int_as_float(id_num + 24),
                           -k2,-k2, k2,      0.00f, 0.00f, util::int_as_float(id_num + 24),
                           -k2,-k2, k,      0.00f, 0.00f,     util::int_as_float(id_num + 24),
                           -k, -k2, k,     0.00f, 0.00f,      util::int_as_float(id_num + 24),

                           //point front right down 145
                           k2, -k, k,        1.00f, 1.00f,    util::int_as_float(id_num + 25),
                           k2,-k, k2,        0.75f, 1.00f,    util::int_as_float(id_num + 25),
                           k, -k, k2,        0.75f, 1.00f,    util::int_as_float(id_num + 25),
                           k,-k2, k2,        1.00f, 0.00f,    util::int_as_float(id_num + 25),
                           k2, -k2, k2,    0.75f, 0.00f,    util::int_as_float(id_num + 25),
                           k2,-k2, k,        1.00f, 0.00f,    util::int_as_float(id_num + 25),
                           k, -k2, k,        1.00f, 1.00f,    util::int_as_float(id_num + 25)
                           //// position     uv cordination    ids
                           ////back
                           // k, -k, -k,        .00f, .00f,        id_num + 0.f,
                           //-k, -k, -k,        .33f, .00f,        id_num + 0.f,
                           //-k,  k, -k,        .33f, .50f,        id_num + 0.f,
                           // k,  k, -k,        .00f, .50f,        id_num + 0.f,
                           ////left
                           //-k, -k,  k,        1.0f, .50f,        id_num + 1.f,
                           //-k,  k,  k,        1.0f, 1.0f,        id_num + 1.f,
                           //-k,  k, -k,        .67f, 1.0f,        id_num + 1.f,
                           //-k, -k, -k,        .67f, .50f,        id_num + 1.f,
                           ////front
                           //-k, -k,  k,        .00f, .50f,        id_num + 2.f,
                           // k, -k,  k,        .33f, .50f,        id_num + 2.f,
                           // k,  k,  k,        .33f, 1.0f,        id_num + 2.f,
                           //-k,  k,  k,        .00f, 1.0f,        id_num + 2.f,
                           ////right
                           // k, -k,  k,        .67f, .00f,        id_num + 3.f,
                           // k, -k, -k,        1.0f, .00f,        id_num + 3.f,
                           // k,  k, -k,        1.0f, .50f,        id_num + 3.f,
                           // k,  k,  k,        .67f, .50f,        id_num + 3.f,
                           ////up
                           //-k,  k,  k,        .33f, .50f,        id_num + 4.f,
                           // k,  k,  k,        .67f, .50f,        id_num + 4.f,
                           // k,  k, -k,        .67f, 1.0f,        id_num + 4.f,
                           //-k,  k, -k,        .33f, 1.0f,        id_num + 4.f,
                           ////down
                           // k, -k, -k,        .67f, .00f,        id_num + 5.f,
                           //-k, -k, -k,        .33f, .00f,        id_num + 5.f,
                           //-k, -k,  k,        .33f, .50f,        id_num + 5.f,
                           // k, -k,  k,        .67f, .50f,        id_num + 5.f
				//// position     uv cordination	ids
				////back
				// k, -k, -k,		.00f, .00f,		id_num + 0.f,
				//-k, -k, -k,		.33f, .00f,		id_num + 0.f,
				//-k,  k, -k,		.33f, .50f,		id_num + 0.f,
				// k,  k, -k,		.00f, .50f,		id_num + 0.f,
				////left
				//-k, -k,  k,		1.0f, .50f,		id_num + 1.f,
				//-k,  k,  k,		1.0f, 1.0f,		id_num + 1.f,
				//-k,  k, -k,		.67f, 1.0f,		id_num + 1.f,
				//-k, -k, -k,		.67f, .50f,		id_num + 1.f,
				////front
				//-k, -k,  k,		.00f, .50f,		id_num + 2.f,
				// k, -k,  k,		.33f, .50f,		id_num + 2.f,
				// k,  k,  k,		.33f, 1.0f,		id_num + 2.f,
				//-k,  k,  k,		.00f, 1.0f,		id_num + 2.f,
				////right
				// k, -k,  k,		.67f, .00f,		id_num + 3.f,
				// k, -k, -k,		1.0f, .00f,		id_num + 3.f,
				// k,  k, -k,		1.0f, .50f,		id_num + 3.f,
				// k,  k,  k,		.67f, .50f,		id_num + 3.f,
				////up
				//-k,  k,  k,		.33f, .50f,		id_num + 4.f,
				// k,  k,  k,		.67f, .50f,		id_num + 4.f,
				// k,  k, -k,		.67f, 1.0f,		id_num + 4.f,
				//-k,  k, -k,		.33f, 1.0f,		id_num + 4.f,
				////down
				// k, -k, -k,		.67f, .00f,		id_num + 5.f,
				//-k, -k, -k,		.33f, .00f,		id_num + 5.f,
				//-k, -k,  k,		.33f, .50f,		id_num + 5.f,
				// k, -k,  k,		.67f, .50f,		id_num + 5.f
			};

			glGenVertexArrays(1, &gizmoVAO);
			glBindVertexArray(gizmoVAO);
			glGenBuffers(1, &gizmoVBO);
			glGenBuffers(1, &gizmoEBO);
			glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(coord_plane), &coord_plane, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gizmoEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, counter_clockwise_cube_element_object_buffer.size() * sizeof(uint32_t), counter_clockwise_cube_element_object_buffer.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0); //  glVertexAttribPointer(shader位置，每段数据长，x，x，步长，起始位置)
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // uv cordination
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float))); // ids

			//texture
			glGenTextures(1, &gizmo_textID); // 要生成的纹理数量和id数组
			glBindTexture(GL_TEXTURE_2D, gizmo_textID); //绑定
			// 为当前绑定的纹理对象设置环绕、过滤方式
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // S(x)坐标多余部分镜像重复
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // T(y)坐标多余部分镜像重复
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 缩小线性过滤（推荐GL_NEAREST就近过滤）
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 放大线性过滤
			// 加载并生成纹理
			int width, height, nrChannels;
			//unsigned char* data = stbi_load("resources\\textures\\x1x.png", &width, &height, &nrChannels, 0);
			unsigned char* data = stbi_load("resources\\textures\\view_cube.png", &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // 生成目标纹理、多级渐远级别、gl纹理存储格式、、、
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void drawGizmo() {
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glActiveTexture(GL_TEXTURE0); // 只有一个纹理则默认激活
			glDisable(DEPTH_TEST);
			glDepthFunc(GL_ALWAYS);
			glDepthMask(GL_FALSE);

			glBindVertexArray(gizmoVAO);
			glBindTexture(GL_TEXTURE_2D, gizmo_textID); // 绑定id到当前GL_TEXTURE_2D
			glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);

			glDrawElements(GL_TRIANGLES, counter_clockwise_cube_element_object_buffer.size(), GL_UNSIGNED_INT, 0); // 把纹理赋值给片段着色器的采样器

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glDepthMask(GL_TRUE);
			glDisable(GL_CULL_FACE);
		}

		void drawGizmoInUiLayer() {
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);

			glBindVertexArray(gizmoVAO);
			glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
			glDrawElements(GL_TRIANGLES, counter_clockwise_cube_element_object_buffer.size(), GL_UNSIGNED_INT, 0); // 传入EBO

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glDisable(GL_CULL_FACE);
		}

		glm::mat4 private_transform(const glm::vec2 window_size) {
			//glm::vec3 newpos = glm::vec3(1.f - k, k - 1.f, 0);
			glm::vec3 window_scale = glm::vec3((window_size.y - 65) / window_size.x, 1.f, 1.f) * (220 / window_size.y); // .2可以设置gizmo大小
			glm::vec3 newpos = glm::vec3(0.f, 0.f, -.5f);

			glm::mat4 ret = glm::translate(glm::scale(glm::mat4(1.f), window_scale), newpos);
			return ret;
			/*ret = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), newpos, glm::vec3(0.f, 1.f, 0.f)) * ret;
			return ret;*/
		}
	};
}
