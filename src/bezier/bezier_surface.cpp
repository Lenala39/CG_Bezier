//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include "bezier_surface.h"
#include <cfloat>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <pmp/Timer.h>
#include <string>

//=============================================================================

using namespace pmp;

Bezier_surface::Bezier_surface(const char *_filename)
{
    if (_filename) {
        if (!load_file(_filename)) {
            exit(1);
        }
    }
}

//-----------------------------------------------------------------------------

Bezier_surface::~Bezier_surface() {}

//-----------------------------------------------------------------------------

bool Bezier_surface::load_file(const char *_filename)
{
    // try to open file
    std::ifstream file(_filename);
    if (!file) {
        std::cerr << "Cannot open " << _filename << std::endl;
        return false;
    }

    // read header: "BEZ", number of control points and patches
    std::string token;
    file >> token;
    if (token != "BEZ") {
        std::cerr << "Not a BEZ file\n";
        return false;
    }
    unsigned int n_points, n_patches;
    file >> n_points >> n_patches;

    // read control points
    std::vector<vec3> control_points(n_points);
    for (unsigned int i = 0; i < n_points; ++i) {
        file >> control_points[i];
    }

    // read control patches (4x4 indices)
    unsigned int index;
    patches_.clear();
    patches_.resize(n_patches);
    for (Bezier_patch &patch : patches_) {
        for (unsigned int i = 0; i < 4; ++i) {
            for (unsigned int j = 0; j < 4; ++j) {
                file >> index;
                --index; // indices in file are 1-based, not 0-based
                patch.control_points_[i][j] = control_points[index];
            }
        }
    }

    // close file, print statistic
    file.close();
    std::cout << n_points << " control points, " << n_patches
              << " Bezier patches\n";

    return true;
}

//-----------------------------------------------------------------------------

bool Bezier_surface::bounding_box(vec3 &_bbmin, vec3 &_bbmax) const
{
    // initialize bbmin and bbmax
    _bbmin = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    _bbmax = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    // nothing to do?
    if (patches_.empty()) {
        return false;
    }

    // compute bounding box for all patches and update _bbmin and _bbmax
    for (const Bezier_patch &patch : patches_) {
        vec3 bbmin, bbmax;
        patch.bounding_box(bbmin, bbmax);
        _bbmin = min(_bbmin, bbmin);
        _bbmax = max(_bbmax, bbmax);
    }

    return true;
}

//-----------------------------------------------------------------------------

void Bezier_surface::tessellate(unsigned int _resolution)
{
    pmp::Timer timer;
    timer.start();

    // tessellate all Bezier patches
    for (Bezier_patch &patch : patches_) {
        patch.tessellate(_resolution);
    }

    timer.stop();
    tesselateTime_ = timer.elapsed();
}

//-----------------------------------------------------------------------------

void Bezier_surface::draw_control_polygon()
{
    // draw control polygons of all patches
    for (Bezier_patch &patch : patches_) {
        patch.draw_control_polygon();
    }
}

//-----------------------------------------------------------------------------

void Bezier_surface::draw_surface(std::string drawmode, bool upload)
{
    // draw tessellated surface of all patches
    for (Bezier_patch &patch : patches_) {
        patch.draw_surface(drawmode, upload);
    }
}

//-----------------------------------------------------------------------------
int Bezier_surface::get_t_mode()
{
    if (patches_[0].de_Casteljau_used())
        return 1;
    else
        return 0;
}

void Bezier_surface::toggle_de_Casteljau()
{
    for (Bezier_patch &patch : patches_) {
        patch.toggle_de_Casteljau();
    }
}
//=============================================================================
