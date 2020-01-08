//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#pragma once
//=============================================================================

#include <imgui.h>
#include <pmp/gl/TrackballViewer.h>

#include "bezier_surface.h"

//=============================================================================

//! Simple viewer for a SurfaceMesh
class BezierViewer : public pmp::TrackballViewer
{
    //! constructor
public:
    /** Constructor, which gets the filename of the Bezier object as its
          parameter. The contructor loads the Bezier object,
          triggers its tessellation, and sets up the model matrix to transform
          the object into the unit sphere. */
    BezierViewer(const char *_title, int _width,
                 int _height, bool showgui = true);

    void loadMesh(const char* filename);

private:
    void processImGUI();

    //! draw the scene in different draw modes
    virtual void draw(const std::string &_draw_mode) override;

    /// keyboard interaction
    virtual void keyboard(int key, int scancode, int action, int mods);

private:
    /// the Bezier object
    Bezier_surface bezier_;

    int tesselation_resolution_;

    /// simple shader for wireframe rendering
    pmp::Shader wireframe_shader_;

    /// Phong shader
    pmp::Shader surface_shader_;

    /// should we render the control mesh?
    bool render_control_mesh_;

    int meshIndex_;
};

//=============================================================================
