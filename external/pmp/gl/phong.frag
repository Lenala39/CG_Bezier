//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#version 150

in vec3 v2f_normal;
in vec3 v2f_view;

uniform vec3 light1;
uniform vec3 light2;
uniform vec3 color;

out vec4 f_color;

float p=100.0;

void main()
{
    vec3 L1 = normalize(light1);
	vec3 L2 = normalize(light2);
    vec3 N  = normalize(v2f_normal);
	vec3 V  = normalize(v2f_view);
	vec3 Rv = reflect(-V,N);
    
    float ambient  = 0.1;
    float diffuse  = abs(dot(N,L1)) + abs(dot(N,L2));
	float specular = max(0, pow(dot(Rv,L1),p)) + max(0, pow(dot(Rv,L2),p));

    vec3  rgb = color * (ambient + diffuse) + vec3(1,1,1) * specular;

    f_color = vec4(rgb, 1.0);
}
