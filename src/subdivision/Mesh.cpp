//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include "Mesh.h"
#include <pmp/gl/phong_shader.h>
#include <pmp/algorithms/SurfaceNormals.h>

#include <cfloat>

//=============================================================================

Mesh::Mesh()
{
    // initialize GL buffers to zero
    m_vertexArrayObject = 0;
    m_vertexBuffer = 0;
    m_normalBuffer = 0;
    m_texCoordBuffer = 0;
    m_edgeBuffer = 0;
    m_featureBuffer = 0;

    // initialize buffer sizes
    m_nVertices = 0;
    m_nEdges = 0;
    m_nTriangles = 0;
    m_nFeatures = 0;

    // material parameters
    m_frontColor = pmp::vec3(0.6, 0.6, 0.6);
    m_backColor = pmp::vec3(0.5, 0.0, 0.0);
    m_ambient = 0.1;
    m_diffuse = 0.8;
    m_specular = 0.6;
    m_shininess = 100.0;
    m_alpha = 1.0;
    m_srgb = false;
    m_creaseAngle = 70.0;

    // initialize texture
    m_texture = 0;
    m_textureMode = OtherTexture;
}

//-----------------------------------------------------------------------------

Mesh::~Mesh()
{
    // delete OpenGL buffers
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_normalBuffer);
    glDeleteBuffers(1, &m_texCoordBuffer);
    glDeleteBuffers(1, &m_edgeBuffer);
    glDeleteBuffers(1, &m_featureBuffer);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
    glDeleteTextures(1, &m_texture);
}

//-----------------------------------------------------------------------------

void Mesh::setCreaseAngle(pmp::Scalar ca)
{
    if (ca != m_creaseAngle)
    {
        m_creaseAngle = std::max(pmp::Scalar(0), std::min(pmp::Scalar(180), ca));
        updateOpenGLBuffers();
    }
}

//-----------------------------------------------------------------------------

void Mesh::updateOpenGLBuffers()
{
    using namespace pmp;
    // are buffers already initialized?
    if (!m_vertexArrayObject)
    {
        glGenVertexArrays(1, &m_vertexArrayObject);
        glBindVertexArray(m_vertexArrayObject);
        glGenBuffers(1, &m_vertexBuffer);
        glGenBuffers(1, &m_normalBuffer);
        glGenBuffers(1, &m_texCoordBuffer);
        glGenBuffers(1, &m_edgeBuffer);
        glGenBuffers(1, &m_featureBuffer);
    }

    // activate VAO
    glBindVertexArray(m_vertexArrayObject);

    // get vertex properties
    auto vpos = getVertexProperty<Point>("v:point");
    auto vtex = getVertexProperty<TexCoord>("v:tex");
    auto htex = getHalfedgeProperty<TexCoord>("h:tex");

    // produce arrays of points, normals, and texcoords
    // (duplicate vertices to allow for flat shading)
    std::vector<pmp::vec3> positionArray;
    positionArray.reserve(3 * nFaces());
    std::vector<pmp::vec3> normalArray;
    normalArray.reserve(3 * nFaces());
    std::vector<vec2> texArray;
    texArray.reserve(3 * nFaces());

    // data per face (for all corners)
    std::vector<Halfedge> cornerHalfedges;
    std::vector<Vertex> cornerVertices;
    std::vector<pmp::vec3> cornerNormals;

    // convert from degrees to radians
    const pmp::Scalar creaseAngle = m_creaseAngle / 180.0 * M_PI;

    auto vertex_indices = addVertexProperty<size_t>("v:index");
    size_t vidx(0);

    // loop over all faces
    for (auto f : faces())
    {
        // collect corner positions and normals
        cornerHalfedges.clear();
        cornerVertices.clear();
        cornerNormals.clear();
        for (auto h : halfedges(f))
        {
            cornerHalfedges.push_back(h);
            cornerVertices.push_back(toVertex(h));
            cornerNormals.push_back(
                SurfaceNormals::computeCornerNormal(*this, h, creaseAngle));
        }
        assert(cornerVertices.size() >= 3);

        // tessellate face into triangles
        int i0, i1, i2, nc = cornerVertices.size();
        for (i0 = 0, i1 = 1, i2 = 2; i2 < nc; ++i1, ++i2)
        {
            positionArray.push_back(vpos[cornerVertices[i0]]);
            positionArray.push_back(vpos[cornerVertices[i1]]);
            positionArray.push_back(vpos[cornerVertices[i2]]);

            normalArray.push_back(cornerNormals[i0]);
            normalArray.push_back(cornerNormals[i1]);
            normalArray.push_back(cornerNormals[i2]);

            if (htex)
            {
                texArray.push_back(htex[cornerHalfedges[i0]]);
                texArray.push_back(htex[cornerHalfedges[i1]]);
                texArray.push_back(htex[cornerHalfedges[i2]]);
            }
            else if (vtex)
            {
                texArray.push_back(vtex[cornerVertices[i0]]);
                texArray.push_back(vtex[cornerVertices[i1]]);
                texArray.push_back(vtex[cornerVertices[i2]]);
            }

            vertex_indices[cornerVertices[i0]] = vidx++;
            vertex_indices[cornerVertices[i1]] = vidx++;
            vertex_indices[cornerVertices[i2]] = vidx++;
        }
    }

    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, positionArray.size() * 3 * sizeof(float),
                 positionArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    m_nVertices = positionArray.size();

    // normals
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normalArray.size() * 3 * sizeof(float),
                 normalArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // texture coordinates
    if (!texArray.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, texArray.size() * 2 * sizeof(float),
                     texArray.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
    }

    // edge indices
    std::vector<unsigned int> edgeArray;
    edgeArray.reserve(nEdges());
    for (auto e : edges())
    {
        edgeArray.push_back(vertex_indices[vertex(e, 0)]);
        edgeArray.push_back(vertex_indices[vertex(e, 1)]);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_edgeBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 edgeArray.size() * sizeof(unsigned int), edgeArray.data(),
                 GL_STATIC_DRAW);
    m_nEdges = edgeArray.size();

    // feature edges
    auto efeature = getEdgeProperty<bool>("e:feature");
    if (efeature)
    {
        std::vector<unsigned int> features;

        for (auto e : edges())
        {
            if (efeature[e])
            {
                features.push_back(vertex_indices[vertex(e, 0)]);
                features.push_back(vertex_indices[vertex(e, 1)]);
            }
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_featureBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     features.size() * sizeof(unsigned int), features.data(),
                     GL_STATIC_DRAW);
        m_nFeatures = features.size();
    }
    else
    {
        m_nFeatures = 0;
    }

    // unbind vertex arry
    glBindVertexArray(0);

    // remove vertex index property again
    removeVertexProperty(vertex_indices);
}

//-----------------------------------------------------------------------------

void Mesh::draw(const pmp::mat4& projectionMatrix,
                         const pmp::mat4& modelviewMatrix,
                         const std::string drawMode)
{
    // did we generate buffers already?
    if (!m_vertexArrayObject)
    {
        updateOpenGLBuffers();
    }

    // load shader?
    if (!m_phongShader.isValid())
    {
        if (!m_phongShader.source(phong_vshader, phong_fshader))
            exit(1);
    }

    // empty mesh?
    if (isEmpty())
        return;

    // setup matrices
    pmp::mat4 mv_matrix = modelviewMatrix;
    pmp::mat4 mvp_matrix = projectionMatrix * modelviewMatrix;
    pmp::mat3 n_matrix = inverse(transpose(linearPart(mv_matrix)));

    // setup shader
    m_phongShader.use();
    m_phongShader.setUniform("modelview_projection_matrix", mvp_matrix);
    m_phongShader.setUniform("modelview_matrix", mv_matrix);
    m_phongShader.setUniform("normal_matrix", n_matrix);
    m_phongShader.setUniform("point_size", 5.0f);
    m_phongShader.setUniform("light1", pmp::vec3(1.0, 1.0, 1.0));
    m_phongShader.setUniform("light2", pmp::vec3(-1.0, 1.0, 1.0));
    m_phongShader.setUniform("front_color", m_frontColor);
    m_phongShader.setUniform("back_color", m_backColor);
    m_phongShader.setUniform("ambient", m_ambient);
    m_phongShader.setUniform("diffuse", m_diffuse);
    m_phongShader.setUniform("specular", m_specular);
    m_phongShader.setUniform("shininess", m_shininess);
    m_phongShader.setUniform("alpha", m_alpha);
    m_phongShader.setUniform("use_lighting", true);
    m_phongShader.setUniform("use_texture", false);
    m_phongShader.setUniform("use_srgb", false);
    m_phongShader.setUniform("show_texture_layout", false);

    glBindVertexArray(m_vertexArrayObject);

    // render control mesh in orange
    if (drawMode == "wireframe")
    {
		m_phongShader.setUniform("front_color", pmp::vec3(0.9, 0.5, 0.1));
		m_phongShader.setUniform("back_color", pmp::vec3(0.9, 0.5, 0.1));
        m_phongShader.setUniform("use_lighting", false);
        draw_wireframe();
    }
    else if (drawMode == "Hidden Line")
    {
        // draw faces
        glDepthRange(0.01, 1.0);
        glDrawArrays(GL_TRIANGLES, 0, m_nVertices);

        // overlay edges
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        m_phongShader.setUniform("front_color", pmp::vec3(0.1, 0.1, 0.1));
        m_phongShader.setUniform("back_color", pmp::vec3(0.1, 0.1, 0.1));
        m_phongShader.setUniform("use_lighting", false);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_edgeBuffer);
        glDrawElements(GL_LINES, m_nEdges, GL_UNSIGNED_INT, nullptr);
        glDepthFunc(GL_LESS);
    }
    else if (drawMode == "Smooth Shading")
    {
        glDrawArrays(GL_TRIANGLES, 0, m_nVertices);
    }


    // draw feature edges
    if (m_nFeatures)
    {
        m_phongShader.setUniform("front_color", pmp::vec3(0,1,0));
        m_phongShader.setUniform("back_color", pmp::vec3(0,1,0));
        m_phongShader.setUniform("use_lighting", false);
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_featureBuffer);
        glDrawElements(GL_LINES, m_nFeatures, GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDepthFunc(GL_LESS);
    }

    glBindVertexArray(0);
    glCheckError();
}


void Mesh::draw_wireframe()
{
    // did we generate buffers already?
    if (!m_vertexArrayObject)
    {
        updateOpenGLBuffers();
    }

    // draw edges of mesh
    glBindVertexArray(m_vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_edgeBuffer);
    glDrawElements(GL_LINES, 2*m_nEdges, GL_UNSIGNED_INT, NULL);
}


//-----------------------------------------------------------------------------


void Mesh::draw_surface()
{
    // did we generate buffers already?
    if (!m_vertexArrayObject)
    {
        updateOpenGLBuffers();
    }

    // draw triangles (might be tessellated quads)
    glBindVertexArray(m_vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_triangleBuffer);
    glDrawElements(GL_TRIANGLES, 3*m_nTriangles, GL_UNSIGNED_INT, NULL);
}


void Mesh::subdivide()
{

    using namespace pmp;

    // reserve memory
    int nv = nVertices();
    int ne = nEdges();
    int nf = nFaces();
    reserve(nv+ne+nf, 2*ne+4*nf, 4*nf);

    // get properties
    auto points = vertexProperty<Point>("v:point");
    auto vpoint = addVertexProperty<Point>("catmull:vpoint");
    auto epoint = addEdgeProperty<Point>("catmull:epoint");
    auto fpoint = addFaceProperty<Point>("catmull:fpoint");

    /** \todo Implement the generalized version of Catmull-Clark subdivision
      *   that can handle arbitrary polygonal meshes (not just quad meshes).          \n
      *   You have to compute
      *   - a new point to be inserted in each face `f`, to be stored in `fpoint[f]`,
      *   - a new point for each edge `e`, to be stored in `epoint[e]`,
      *   - a new position for every old vertex `v`, to be stored in `vpoint[v]`.
      *
      *   Note that special rules exist for boundary edges and boundary vertices.
      *   You can test whether an edge `e` or a vertex `v` is on the boundary by
      *   `isSurfaceBoundary(e)` and `isSurfaceBoundary(v)`.                                      \n
      *   The actual mesh refinement, i.e., the edge and face splitting, is
      *   given at the bottom.
      *
      *   Hints:
      *   0. We use the pmp mesh structure, for a short tutorial see: "http://www.pmp-library.org/tutorial.html"
      *   1. A `Vertex v` is just a kind of index referencing the 3D position accessed by `points[v]`
      *   2. You get the two vertices of an `Edge e` by `vertex(e,0)` and `vertex(e,1)`
      *   3. To compute how many vertices are direct neighbors of a `Vertex v`, use `valence(v)`
      *   4. To compute how many vertices are compose a `Face f`, use `valence(f)`
      *   5. You can use special range based loops:
      *         for(auto v : vertices())    -->  loop through all vertices
      *         for(auto f : faces())       -->  loop through all faces
      *         for(auto e : edges())       -->  loop through all edges
      *         for(auto h : halfedges())   -->  loop through all halfedges
      *         for(auto vv : vertices(v))  -->  loop through all vertices in one ring neighborhood of `Vertex v`
      *         for(auto vf : vertices(f))  -->  loop through all vertices of `Face f`
      *         for(auto vf : vertices(f))  -->  loop through all vertices of `Face f`
      *         for(auto fv : faces(v))     -->  loop through all faces of `Vertex v`
      *         for(auto hv : halfedges(v)) -->  loop through all outgoing halfedges of `Vertex v`
      */



    // assign new positions to old vertices
    for (auto v: vertices())
    {
        points[v] = vpoint[v];
    }


    // split edges
    for (auto e: edges())
    {
        insertVertex(e, epoint[e]);
    }


    // split faces
    for (auto f: faces())
    {
        Halfedge h0 = halfedge(f);
        insertEdge(h0, nextHalfedge(nextHalfedge(h0)));

        Halfedge h1 = nextHalfedge(h0);
        insertVertex(edge(h1), fpoint[f]);

        Halfedge h = nextHalfedge(nextHalfedge(nextHalfedge(h1)));
        while (h != h0)
        {
            insertEdge(h1, h);
            h = nextHalfedge(nextHalfedge(nextHalfedge(h1)));
        }
    }


    // clean-up properties
    removeVertexProperty(vpoint);
    removeEdgeProperty(epoint);
    removeFaceProperty(fpoint);


    // upload new mesh to GPU
    updateOpenGLBuffers();
}
//=============================================================================
