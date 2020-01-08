//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#version 150

uniform vec3 color;

out vec4 f_color;

void main()
{
    f_color = vec4(color, 1.0);
}
