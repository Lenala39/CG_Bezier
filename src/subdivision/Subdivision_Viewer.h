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

#include "Mesh.h"
#include <pmp/gl/TrackballViewer.h>

//=============================================================================

/// Simple viewer for a SurfaceMesh

class Subdivision_Viewer : public pmp::TrackballViewer
{
public:
    /// constructor
    Subdivision_Viewer(const char* title, int width, int height, bool showgui = true);

    /// destructor
    virtual ~Subdivision_Viewer();

    /// load a mesh from file \c filename
    virtual bool loadMesh(const char* filename);

    /// update mesh normals and all buffers for OpenGL rendering.  call this
    /// function whenever you change either the vertex positions or the
    /// triangulation of the mesh
    virtual void updateMesh();

    /// draw the scene in different draw modes
    virtual void draw(const std::string& _draw_mode) override;

    /// handle ImGUI interface
    virtual void processImGUI() override;

    /// this function handles keyboard events
    virtual void keyboard(int key, int code, int action, int mod) override;


protected:
    Mesh m_mesh;   ///< the mesh
    std::string m_filename; ///< the current file
    float m_creaseAngle;


    bool draw_control_mesh = false;

    /// the initial control mesh (doesn't change)
    Mesh control_mesh_;

    /// the subdivided mesh
    Mesh surface_mesh_;

    /// index of currently loaded mesh
    int meshIndex_;
};

//=============================================================================
