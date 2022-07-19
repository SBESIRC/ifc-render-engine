#pragma once
//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

/* Header for
 *
 * Triangle-Triangle Overlap Test Routines ,
 * by P. Guigue - O. Devillers, 1997.
 * See article "Fast and Robust Triangle-Triangle Overlap Test
 *  Using Orientation Predicates",
 * Journal of Graphics Tools, 8(1), 2003
 * Updated December 2003
 *
 * Header created by Justin Gravett, ESAero (20 May, 2020)
 *
 * Replaced Triangle/triangle intersection test routine,
 * by Tomas Moller, 1997.
 * See article "A Fast Triangle-Triangle Intersection Test",
 * Journal of Graphics Tools, 2(2), 1997
 * updated: 2001-06-20 (added line of intersection)
 *
 *
 */

#ifndef TRI_TRI_INTERSECT
#define TRI_TRI_INTERSECT


using tti_real = float;
 //////////////////////////////////////////////////////
 //============ tri_tri_intersect.cpp ===============//
 //////////////////////////////////////////////////////

 // Three-dimensional Triangle-Triangle Overlap Test
int tri_tri_overlap_test_3d(tti_real p1[3], tti_real q1[3], tti_real r1[3],
    tti_real p2[3], tti_real q2[3], tti_real r2[3]);


// Three-dimensional Triangle-Triangle Overlap Test
// additionaly computes the segment of intersection of the two triangles if it exists. 
// coplanar returns whether the triangles are coplanar, 
// source and target are the endpoints of the line segment of intersection 
int tri_tri_intersection_test_3d(tti_real p1[3], tti_real q1[3], tti_real r1[3],
    tti_real p2[3], tti_real q2[3], tti_real r2[3],
    int* coplanar,
    tti_real source[3], tti_real target[3]);


int coplanar_tri_tri3d(tti_real  p1[3], tti_real  q1[3], tti_real  r1[3],
    tti_real  p2[3], tti_real  q2[3], tti_real  r2[3],
    tti_real  N1[3], tti_real  N2[3]);


// Two dimensional Triangle-Triangle Overlap Test
int tri_tri_overlap_test_2d(tti_real p1[2], tti_real q1[2], tti_real r1[2],
    tti_real p2[2], tti_real q2[2], tti_real r2[2]);

int ccw_tri_tri_intersection_2d(tti_real p1[2], tti_real q1[2], tti_real r1[2],
    tti_real p2[2], tti_real q2[2], tti_real r2[2]);

//////////////////////////////////////////////////////
//============ intersect_triangle.cpp ==============//
//////////////////////////////////////////////////////

int intersect_triangle(tti_real orig[3], tti_real dir[3],
    tti_real vert0[3], tti_real vert1[3], tti_real vert2[3],
    tti_real* t, tti_real* u, tti_real* v);

//////////////////////////////////////////////////////
//============== intersect_AABB.cpp ================//
//////////////////////////////////////////////////////

bool intersectRayAABB(const tti_real MinB[3], const tti_real MaxB[3],
    const tti_real origin[3], const tti_real dir[3],
    tti_real coord[3]);

#endif // TRI_TRI_INTERSECT