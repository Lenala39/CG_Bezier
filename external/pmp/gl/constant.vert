//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#version 150
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec4 v_position;

uniform mat4 modelview_projection_matrix;

void main()
{
    gl_Position = modelview_projection_matrix * v_position;
} 
