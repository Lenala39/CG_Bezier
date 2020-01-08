//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================
#ifndef BEZIER_SURFACE_H
#define BEZIER_SURFACE_H
//=============================================================================

#include "bezier_patch.h"
#include <pmp/MatVec.h>
#include <vector>

//=============================================================================

/// A surface represented by a collection of Bezier patches.
/** This class stores a surface that is represented by a collection of
    bi-cubic tensor-product Bezier patches.
    \sa Bezier_patch
*/
class Bezier_surface
{
public:
    /// constructor, get name of the file to be loaded
    Bezier_surface(const char *_filename = NULL);

    /// destructor
    ~Bezier_surface();

    /// is the object empty, i.e., has nothing been loaded yet?
    bool empty() const { return patches_.empty(); }

    /// load Bezier object from a *.bez file
    bool load_file(const char *_filename);

    /// compute bounding box, store min/max position in _bbmin and _bbmax.
    /// simply calls Bezier_patch::bounding_box() for all its patches.
    bool bounding_box(pmp::vec3 &_bbmin, pmp::vec3 &_bbmax) const;

    /// tessellate Bezier surface into triangles with a prescribed resolution.
    /// simply calls Bezier_patch::tessellate() for all its patches.
    void tessellate(unsigned int _resolution);

    /// draw the control polygon for all Bezier patches.
    /// simply calls Bezier_patch::draw_control_polygon() for all its patches.
    void draw_control_polygon();

    /// draw the tessellated surface of all Bezier patches.
    /// simply calls Bezier_patch::draw_surface() for all its patches.
    void draw_surface(std::string drawmode, bool upload = false);

    void toggle_de_Casteljau();

    int get_t_mode();

    float tesselateTime_;

private:
    /// array of all Bezier patches
    std::vector<Bezier_patch> patches_;
};
//=============================================================================
#endif
//=============================================================================
