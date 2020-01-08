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

#include <pmp/gl/gl.h>
#include <pmp/gl/Shader.h>
#include <pmp/MatVec.h>
#include <pmp/SurfaceMesh.h>

//=============================================================================

/// Class for rendering surface meshes using OpenGL
class Mesh : public pmp::SurfaceMesh
{
public:
    /// Constructor
    Mesh();

    /// default destructor
    ~Mesh();

    /// get front color
    const pmp::vec3& frontColor() const { return m_frontColor; }
    /// set front color
    void setFrontColor(const pmp::vec3& color) { m_frontColor = color; }

    /// get back color
    const pmp::vec3& backColor() const { return m_backColor; }
    /// set back color
    void setBackColor(const pmp::vec3& color) { m_backColor = color; }

    /// get ambient reflection coefficient
    float ambient() const { return m_ambient; }
    /// set ambient reflection coefficient
    void setAmbient(float a) { m_ambient = a; }

    /// get diffuse reflection coefficient
    float diffuse() const { return m_diffuse; }
    /// set diffuse reflection coefficient
    void setDiffuse(float d) { m_diffuse = d; }

    /// get specular reflection coefficient
    float specular() const { return m_specular; }
    /// set specular reflection coefficient
    void setSpecular(float s) { m_specular = s; }

    /// get specular shininess coefficient
    float shininess() const { return m_shininess; }
    /// set specular shininess coefficient
    void setShininess(float s) { m_shininess = s; }

    /// get alpha value for transparent rendering
    float alpha() const { return m_alpha; }
    /// set alpha value for transparent rendering
    void setAlpha(float a) { m_alpha = a; }

    /// get crease angle (in degrees) for visualization of sharp edges
    pmp::Scalar creaseAngle() const { return m_creaseAngle; }
    /// set crease angle (in degrees) for visualization of sharp edges
    void setCreaseAngle(pmp::Scalar ca);

    /// draw the mesh
    void draw(const pmp::mat4& projectionMatrix, const pmp::mat4& modelviewMatrix,
              const std::string drawMode);

    /// update all opengl buffers for efficient core profile rendering
    void updateOpenGLBuffers();

    /// use color map to visualize pmp::Scalar fields
    void useColdWarmTexture();

    /// setup checkerboard texture
    void useCheckerboardTexture();

    /// load texture from file
    /// \param filename the location and name of the texture
    /// \param format internal format (GL_RGB, GL_RGBA, GL_SRGB8, etc.)
    /// \param minFilter interpolation filter for minification
    /// \param magFilter interpolation filter for magnification
    /// \param wrap texture coordinates wrap preference
    bool loadTexture(const char* filename, GLint format = GL_RGB,
                     GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                     GLint magFilter = GL_LINEAR,
                     GLint wrap = GL_CLAMP_TO_EDGE);

    /// renders mesh geometry
    void draw_wireframe();

    /// renders mesh geometry
    void draw_surface();

    /// subdivides the mesh using the Catmul-Clark scheme
    void subdivide();

//-----------------------------------------------------------------------
    /// should we render the control mesh?
    bool  render_control_mesh_;

    /// should we render the edges of the tessellated mesh?
    bool  render_wireframe_;



//-----------------------------------------------------------------------
private:
    /// OpenGL buffers
    GLuint m_vertexArrayObject;
    GLuint m_vertexBuffer;
    GLuint m_normalBuffer;
    GLuint m_texCoordBuffer;
    GLuint m_edgeBuffer;
    GLuint m_featureBuffer;
    GLuint m_triangleBuffer;

    /// buffer sizes
    GLsizei m_nVertices;
    GLsizei m_nEdges;
    GLsizei m_nTriangles;
    GLsizei m_nFeatures;

    /// shaders
    pmp::Shader m_phongShader;
    /// material properties
    pmp::vec3 m_frontColor, m_backColor;
    float m_ambient, m_diffuse, m_specular, m_shininess, m_alpha;
    bool m_srgb;
    float m_creaseAngle;

    /// 1D texture for pmp::Scalar field rendering
    GLuint m_texture;
    enum
    {
        ColdWarmTexture,
        CheckerboardTexture,
        OtherTexture
    } m_textureMode;
};

//=============================================================================
