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
layout (location = 1) in vec3 v_normal;

out vec3 v2f_normal;
out vec3 v2f_view;

uniform mat4 modelview_projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;

void main()
{
    v2f_normal  = normal_matrix * v_normal;
	v2f_view    = -(modelview_matrix*v_position).xyz;
    gl_Position = modelview_projection_matrix * v_position;
} 
