// -*- C++ -*-

#ifndef _VIS_3D_SCENE_PARAMETERS
#define _VIS_3D_SCENE_PARAMETERS

#include <GL/glut.h>
#include <vector>

struct SceneParameters {

    static const int LIGHT_SOURCE_IDS[];

    float link_width_factor = 0.05;

    struct LightSource {
        GLfloat position[4];
        GLfloat color[4];
    };
    std::vector<LightSource> light_sources;

    GLfloat color_background[4];
    GLfloat color_vertex[4];
    GLfloat color_edge_binary[4];
    GLfloat color_edge_hyper[4];

    SceneParameters(const std::vector<LightSource>& light_sources,
        GLfloat color_background[4], GLfloat color_vertex[4], GLfloat color_edge_binary[4], GLfloat color_edge_hyper[4]) :
        light_sources(light_sources) {

        for (size_t i = 0; i < 4; i++) {
            this->color_background[i] = color_background[i];
            this->color_vertex[i] = color_vertex[i];
            this->color_edge_binary[i] = color_edge_binary[i];
            this->color_edge_hyper[i] = color_edge_hyper[i];
        }
    }
};


#endif
