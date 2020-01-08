//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include <imgui.h>
#include "Subdivision_Viewer.h"
#include <cfloat>
#include <iostream>
#include <sstream>

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR)/sizeof(*_ARR)))

//=============================================================================
Subdivision_Viewer::Subdivision_Viewer(const char* title, int width, int height, bool showgui)
        : pmp::TrackballViewer(title, width, height, showgui)
{
    // setup draw modes
    clearDrawModes();
    addDrawMode("Hidden Line");
    addDrawMode("Smooth Shading");
    setDrawMode("Hidden Line");
    m_creaseAngle = 90.0;
    meshIndex_ = 0;
}

//-----------------------------------------------------------------------------
Subdivision_Viewer::~Subdivision_Viewer() = default;

//-----------------------------------------------------------------------------

bool Subdivision_Viewer::loadMesh(const char* filename)
{
    // load mesh
    if (m_mesh.read(filename))
    {
        control_mesh_.clear();
        control_mesh_.read(filename); // this will stay fixed
        surface_mesh_ = control_mesh_;

        // update scene center and bounds
        pmp::BoundingBox bb = m_mesh.bounds();
        setScene(bb.center(), 0.6 * bb.size());

        // compute face & vertex normals, update face indices
        updateMesh();
        control_mesh_.updateOpenGLBuffers();

        std::cout << "Load " << filename << ": " << m_mesh.nVertices()
                  << " vertices, " << m_mesh.nFaces() << " faces\n";

        m_filename = filename;
        m_creaseAngle = m_mesh.creaseAngle();
        return true;
    }

    std::cerr << "Failed to read mesh from " << filename << " !" << std::endl;
    return false;
}

//-----------------------------------------------------------------------------

void Subdivision_Viewer::updateMesh()
{
    // re-compute face and vertex normals
    m_mesh.updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void Subdivision_Viewer::processImGUI()
{
    if (ImGui::CollapsingHeader("Load Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushItemWidth(100);
        const char* listbox_items[] = { "cube.off", "kissmouth.obj", "suzanne.obj", "teacup.off", "teapot.off"};
        int listbox_item_current = meshIndex_;
        ImGui::ListBox(" ", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 5);
        if(listbox_item_current != meshIndex_ || control_mesh_.nVertices() == 0)
        {
            std::cout << "load new mesh" << std::endl;
            std::stringstream ss;

            ss << DATA_PATH << listbox_items[listbox_item_current];


            loadMesh(ss.str().c_str());

            meshIndex_ = listbox_item_current;
        }
        ImGui::PopItemWidth();
        ImGui::Spacing();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Mesh Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // output mesh statistics
        ImGui::BulletText("%d vertices", (int)m_mesh.nVertices());
        ImGui::BulletText("%d edges", (int)m_mesh.nEdges());
        ImGui::BulletText("%d faces", (int)m_mesh.nFaces());

        ImGui::Spacing();
        ImGui::Spacing();
        // control crease angle
        ImGui::PushItemWidth(80);
        ImGui::SliderFloat("Crease Angle", &m_creaseAngle, 0.0f, 180.0f,
                           "%.0f");
        ImGui::PopItemWidth();
        if (m_creaseAngle != m_mesh.creaseAngle())
        {
            m_mesh.setCreaseAngle(m_creaseAngle);
        }
        ImGui::Spacing();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Drawmode", ImGuiTreeNodeFlags_DefaultOpen)) {


        int mode = m_drawMode;
        ImGui::PushItemWidth(100);
        for(int i=0; i<m_drawModeNames.size(); ++i)
        {
            ImGui::RadioButton(m_drawModeNames[i].c_str(), &mode, i);
        }
        ImGui::PopItemWidth();
        m_drawMode = mode;

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Checkbox("Draw Control Mesh", &draw_control_mesh);
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Subdivision", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        if (ImGui::Button("Subdivide"))
        {
            m_mesh.subdivide();
            updateMesh();
        }
    }
}

//-----------------------------------------------------------------------------

void Subdivision_Viewer::draw(const std::string& drawMode)
{
    // draw mesh
    m_mesh.draw(m_projectionMatrix, m_modelviewMatrix, drawMode);
    if (draw_control_mesh){
        control_mesh_.draw(m_projectionMatrix, m_modelviewMatrix, "wireframe");
    }

}

//-----------------------------------------------------------------------------

void Subdivision_Viewer::keyboard(int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
        case GLFW_KEY_BACKSPACE: // reload model
        {
            loadMesh(m_filename.c_str());
            break;
        }

        case GLFW_KEY_S: // subdivide model
        {
            m_mesh.subdivide();
            updateMesh();
            break;
        }

        case GLFW_KEY_C: // adjust crease angle
        {
            draw_control_mesh = !draw_control_mesh;
            updateMesh();
            break;
        }

        case GLFW_KEY_O: // write mesh
        {
            m_mesh.write("output.off");
            break;
        }

        default:
        {
            TrackballViewer::keyboard(key, scancode, action, mods);
            break;
        }
    }
}

//=============================================================================

