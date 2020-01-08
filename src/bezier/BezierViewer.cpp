//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include "BezierViewer.h"

#include <cfloat>
#include <imgui.h>
#include <iostream>
#include <sstream>

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR)/sizeof(*_ARR)))

//=============================================================================

using namespace pmp;

BezierViewer::BezierViewer(const char *title, int width,
                           int height, bool showgui)
    : TrackballViewer(title, width, height, showgui),
      tesselation_resolution_(20)
{

	std::stringstream sscv, sscf, sspv, sspf;
	sscv << SHADER_PATH << "constant.vert";
	sscf << SHADER_PATH << "constant.frag";
	sspv << SHADER_PATH << "phong.vert";
	sspf << SHADER_PATH << "phong.frag";
    // load wireframe & Phong shaders
    wireframe_shader_.load(sscv.str().c_str(),sscf.str().c_str());
    surface_shader_.load(sspv.str().c_str(),sspf.str().c_str());

    render_control_mesh_ = true;
    meshIndex_ = 0;

    clearDrawModes();
    addDrawMode("Hidden Line");
    addDrawMode("Smooth Shading");
    addDrawMode("Points");
    setDrawMode("Hidden Line");
}

void BezierViewer::loadMesh(const char *filename)
{
    bezier_.load_file(filename);

    // store model dimensions
    vec3 bbmin, bbmax;
    bezier_.bounding_box(bbmin, bbmax);
    vec3 center = 0.5 * (bbmin + bbmax);
    float radius = 0.5 * distance(bbmin, bbmax);
    setScene(center, radius);
    // tessellate Bezier surface with resolution
    bezier_.tessellate(tesselation_resolution_);
}

void BezierViewer::processImGUI()
{
    if (ImGui::CollapsingHeader("Load Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushItemWidth(100);
        const char* listbox_items[] = { "heart.bez", "teacup.bez", "teapot.bez", "car.bez"};
        int listbox_item_current = meshIndex_;
        ImGui::ListBox(" ", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
        if(listbox_item_current != meshIndex_ || bezier_.empty())
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
        ImGui::Checkbox("Draw Control Mesh", &render_control_mesh_);
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Bezier", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("Tesselation Resolution");
        ImGui::PushItemWidth(120);
        int tesres = tesselation_resolution_;
        ImGui::SliderInt("  ", &tesres, 2, 30);
        ImGui::PopItemWidth();
        if(tesres != tesselation_resolution_)
        {
            tesselation_resolution_ = tesres;
            bezier_.tessellate(tesselation_resolution_);
        }

        static int last_tesselation_res = tesselation_resolution_;
        if (ImGui::Button("Tesselate")) {
            bezier_.tessellate(tesselation_resolution_);
            last_tesselation_res = tesselation_resolution_;
        }
        ImGui::Spacing();
        ImGui::Spacing();

        int t_mode = bezier_.get_t_mode();
        ImGui::RadioButton("Bernstein Evaluation", &t_mode, 0);
        ImGui::RadioButton("Casteljau Evaluation", &t_mode, 1);
        if(t_mode != bezier_.get_t_mode())
        {
            bezier_.toggle_de_Casteljau();
            bezier_.tessellate(tesselation_resolution_);
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("Tesselation time:\n%.2fms", bezier_.tesselateTime_);
    }
}

//-----------------------------------------------------------------------------


void BezierViewer::draw(const std::string &_draw_mode)
{

    // clear framebuffer and depth buffer first
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setup matrices
    mat4 mv_matrix = m_modelviewMatrix;
    mat4 mvp_matrix = m_projectionMatrix * mv_matrix;
    mat3 n_matrix = inverse(transpose(linearPart(mv_matrix)));

    // render control mesh in grey
    if (render_control_mesh_) {
        wireframe_shader_.use();
        wireframe_shader_.setUniform("modelview_projection_matrix", mvp_matrix);
        wireframe_shader_.setUniform("color", vec3(0.5, 0.5, 0.5));
        bezier_.draw_control_polygon();
        wireframe_shader_.disable();
    }


    // render filled surface triangles with Phong lighting
    surface_shader_.use();
    surface_shader_.setUniform("modelview_projection_matrix", mvp_matrix);
    surface_shader_.setUniform("modelview_matrix", mv_matrix);
    surface_shader_.setUniform("normal_matrix", n_matrix);
    surface_shader_.setUniform("light1", normalize(vec3(1.0, 1.0, 1.0)));
    surface_shader_.setUniform("light2", normalize(vec3(-1.0, 1.0, 1.0)));
    surface_shader_.setUniform("color", vec3(0.6, 0.0, 0.0));
    glDepthRange(0.01, 1.0); // reduce depth range for solid rendering,
    // such that wireframe will be slightly in front
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    bezier_.draw_surface(_draw_mode);
    glDepthRange(0.0, 1.0); // restore default depth range

    //         render surface edges with lighting, but a bit darker
    if (_draw_mode == "Hidden Line") {
        surface_shader_.setUniform("color", vec3(0.2, 0.0, 0.0));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        bezier_.draw_surface(_draw_mode);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glCheckError();
}

//-----------------------------------------------------------------------------

void BezierViewer::keyboard(int key, int scancode, int action, int mods)
{
    TrackballViewer::keyboard(key, scancode, action, mods);
}

//=============================================================================
