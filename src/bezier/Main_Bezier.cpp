//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) 2014/2015 by Computer Graphics Group, Bielefeld University
//
//=============================================================================


#include "BezierViewer.h"

//=============================================================================

int main(int argc, char **argv)
{
    BezierViewer viewer("BezierViewer", 800, 600);
    if(argc > 1)
    {
        viewer.loadMesh(argv[1]);
    }
    return viewer.run();
}

//=============================================================================
