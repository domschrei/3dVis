
#include <unistd.h>
#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "CLI11.hpp"
#include "Graph.h"
#include "SceneParameters.h"
#include "arcball/arcball.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

// ----------------------------------------------------------------------

int width, height;
float zoom = 1.0;
float zNear = 0.1, zFar = 20.0;
int downX, downY;
bool leftButton = false, middleButton = false, rightButton = false;
bool ctrlKeyPressed = false;
bool shiftKeyPressed = false;

float xOffset = 0.0, yOffset = 0.0, zOffset = 0.0;

Graph3D *current_graph; // currently displayed graph
vector<Graph3D *> graph_stack;

float k = 1.0; // old: 5.0
float f_k = sqrt(4.0 / 7.0);
int curr_L;

bool next_graph = false, draw_edges = true, run_anim = false,
     adaptive_size = true;
bool draw_only_2clauses = false;
bool draw_helping_axes = false;

Vector3D min_p, max_p;

// ----------------------------------------------------------------------

void draw_axes() {
  // draw axes
	glBegin(GL_LINES);
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glVertex3f( 0.0,  0.0,  0.0);
		glVertex3f( 1.0,  0.0,  0.0);
		glColor4f(0.0, 1.0, 0.0, 1.0);
		glVertex3f( 0.0,  0.0,  0.0);
		glVertex3f( 0.0, 1.0,  0.0);
		glColor4f(0.0, 0.0, 1.0, 1.0);
		glVertex3f( 0.0,  0.0,  0.0);
		glVertex3f( 0.0,  0.0, 1.0);
	glEnd();
}

void display(void) {

  if (current_graph != NULL) {

    auto bg = current_graph->get_scene_params().color_background;
    glClearColor(bg[0], bg[1], bg[2], bg[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(
      0, 0, 0+10/zoom,
      0, 0, 0,
      0.0, 1.0,  0.0
    );
    arcball_setzoom(0.03, vec(0, 0, 10/zoom), vec(0, 1, 0));
    glTranslatef(xOffset, yOffset, 0);
    arcball_rotate();

    glPushMatrix();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (draw_helping_axes) draw_axes();
    current_graph->draw3D(k, draw_edges, draw_only_2clauses, adaptive_size);
    
    glPopMatrix();

    glutSwapBuffers();
  }
}

void setupNodes(Graph3D *g) {
  Node3D n(1), m(2), o(3);

  g->add_node(n);
  g->add_node(m);
  g->add_node(o);
  g->insert_edge(1, 2);
  g->insert_edge(1, 3);
}

// builds (global) stack of coarsened graphs
void init(const std::string &filename, const SceneParameters &scene_params,
          bool use_space_grid) {
  Graph3D *g = new Graph3D(scene_params, use_space_grid);
  graph_stack.push_back(g);

  // build initial graph
  if (filename[0] == '-')
    g->build_from_cnf(cin);
  else {
    ifstream is(filename);
    g->build_from_cnf(is);
    is.close();
  }
  cout << "Built initial graph G=(V,E) with |V| = " << g->nr_nodes()
       << " and |E| = " << g->nr_edges() << "." << endl;

  // check for multiple components
  vector<int> head_nodes;
  int nr_comp = g->independent_components(&head_nodes);
  cout << "Graph consists of " << nr_comp << " independent component(s)."
       << endl;
  if (nr_comp > 1) {
    cout << "Multiple components not yet supported!" << endl;
    exit(10);
  }

  // build graph stack
  Graph3D *a = g, *b;
  int level = 1;
  while (a->nr_nodes() > 2) {
    b = a->coarsen();
    graph_stack.push_back(b);
    cout << "Coarsened graph " << level << " consists of " << b->nr_nodes()
         << " vertices and " << b->nr_edges() << " edge(s)." << endl;
    a = b;
    level++;
  }

  // compute (random) layout of coarsest graph (with 2 nodes)
  curr_L = graph_stack.size() - 1;
  graph_stack[curr_L]->init_coarsest_graph_positions(k);
  pair<Vector3D, Vector3D> ep = graph_stack[curr_L]->compute_extremal_points();
  min_p = ep.first;
  max_p = ep.second;
  current_graph = graph_stack[curr_L];
  curr_L--;

  // set up OpenGL
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glLineWidth(1.5);

  // set up light sources
  for (int i = 0; i < (int)std::min(8UL, scene_params.light_sources.size());
       i++) {
    auto light_source = scene_params.light_sources[i];
    glLightfv(SceneParameters::LIGHT_SOURCE_IDS[i], GL_DIFFUSE,
              light_source.color);
    glLightfv(SceneParameters::LIGHT_SOURCE_IDS[i], GL_POSITION,
              light_source.position);
    glEnable(SceneParameters::LIGHT_SOURCE_IDS[i]);
  }
  glEnable(GL_LIGHTING);

  // hidden surface elimination
  glEnable(GL_DEPTH_TEST);
}

void motion(int x, int y) {
  if (leftButton) {
    if (ctrlKeyPressed) {
      zoom += (float)(y - downY) * 0.005;
    } else if (shiftKeyPressed) {
      xOffset += (float)(x - downX) / 200.0;
      yOffset += (float)(downY - y) / 200.0;
    } else {
      arcball_move(x, height-y-1);
    }
    glutPostRedisplay();
  }
  downX = x;
  downY = y;
}

void mouse(int button, int state, int x, int y) {

  if (state == GLUT_DOWN && button == 3) {
    // scroll up
    zoom *= 1.04;
    glutPostRedisplay();
    return;
  }
  if (state == GLUT_DOWN && button == 4) {
    // scroll down
    zoom /= 1.04;
    glutPostRedisplay();
    return;
  }

  downX = x;
  downY = y;
  leftButton = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
  middleButton = ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN));
  rightButton = ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN));
  ctrlKeyPressed = (state == GLUT_DOWN) && (glutGetModifiers() & GLUT_ACTIVE_CTRL);
  shiftKeyPressed = (state == GLUT_DOWN) && (glutGetModifiers() & GLUT_ACTIVE_SHIFT);
  if (button == GLUT_LEFT_BUTTON && !ctrlKeyPressed && !shiftKeyPressed) {
		if (state == GLUT_DOWN) {
			arcball_start(x, height-y-1);
		} else {
			
		}
		glutPostRedisplay();
	}
}

void reshape(int w, int h) {
  width = w;
  height = h;
	float aspect = (double)w / (double)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, aspect, 0.1, 150.0);
	glMatrixMode(GL_MODELVIEW);

  //glViewport(0, 0, w, h);
  //glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();
  //gluPerspective(40.0, (float)w / (float)h, zNear / 2.0,
  //               zFar * 2.0); // 1st arg.: view angle
  //glMatrixMode(GL_MODELVIEW);
}

void idle(void) {
  if (next_graph || run_anim) {
    if (curr_L >= 0) {
      // layout & display next finer graph
      cout << "L = " << curr_L << " " << flush;
      graph_stack[curr_L]->init_positions_from_graph(graph_stack[curr_L + 1],
                                                     k);
      //      graph_stack[curr_L]->init_positions_at_random();
      graph_stack[curr_L]->compute_layout(k);
      pair<Vector3D, Vector3D> ep =
          graph_stack[curr_L]->compute_extremal_points();
      min_p = ep.first;
      max_p = ep.second;
      float max_extent =
          max(max_p.x - min_p.x, max(max_p.y - min_p.y, max_p.z - min_p.z));
      // rescale to -10.0 .. +10.0 on all axes
      Vector3D shift(0, 0, 0); //= Vector3D(-1.0, -1.0, -1.0) - 2.0 / max_extent * min_p;
      graph_stack[curr_L]->rescale(2.0 / max_extent, shift);
      current_graph = graph_stack[curr_L];
      k *= f_k;
      curr_L--;
    }
    next_graph = false;
    glutPostRedisplay();
  }
}

void save_image(const char* filepath) {
  GLsizei nrChannels = 3;
  GLsizei stride = nrChannels * width;
  stride += (stride % 4) ? (4 - stride % 4) : 0;
  GLsizei bufferSize = stride * height;
  std::vector<char> buffer(bufferSize);
  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glReadBuffer(GL_FRONT);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
  stbi_flip_vertically_on_write(true);
  stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);
}

void save_scene_to_image() {
  int i = 0;
  std::string path;
  do {
    i++;
    path = "out-" + std::to_string(i) + ".png";
  } while (access(path.c_str(), F_OK) != -1);
  save_image(path.c_str());
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 'n':
    next_graph = true;
    break;
  case 'e':
    draw_edges = !draw_edges;
    break;
  case '2':
    draw_only_2clauses = !draw_only_2clauses;
    break;
  case 'g':
    run_anim = !run_anim;
    break;
  case 'r': // reset
    xOffset = yOffset = 0.0;
    zoom = 1.0;
    arcball_reset();
    break;
  case 'x': // draw axes
    draw_helping_axes = !draw_helping_axes;
    break;
  case 'a':
    adaptive_size = !adaptive_size;
    break;
  case 'p':
    save_scene_to_image();
    break;
  }
  glutPostRedisplay();
}

void getColorFromHexStr(const std::string &str, GLfloat *out) {
  std::string normalized;
  if (str.size() == 3 || str.size() == 4) {
    normalized = std::string(1, str[0]) + std::string(1, str[0]) +
                 std::string(1, str[1]) + std::string(1, str[1]) +
                 std::string(1, str[2]) + std::string(1, str[2]);
    if (str.size() == 4)
      normalized += std::string(1, str[3]) + std::string(1, str[3]);
    else
      normalized += "ff";
  }
  if (str.size() == 6)
    normalized = str + "ff";
  if (str.size() == 8)
    normalized = str;

  unsigned long x;
  std::stringstream ss;
  ss << std::hex << normalized;
  ss >> x;

  out[0] = (x & 0xff000000) >> 24;
  out[1] = (x & 0x00ff0000) >> 16;
  out[2] = (x & 0x0000ff00) >> 8;
  out[3] = (x & 0x000000ff);

  for (int i = 0; i < 4; i++)
    if (out[i] < 0 || out[i] > 255) {
      cout << "ERROR: Invalid color value \"" << out[i]
           << "\" (must be between 0 and 255)" << endl;
      exit(1);
    };

  for (int i = 0; i < 4; i++)
    out[i] /= 255;
}

SceneParameters initSceneParams(const std::string &str_color_background,
                                const std::string &str_color_vertex,
                                const std::string &str_color_edge_binary,
                                const std::string &str_color_edge_hyper) {
  // Set up colors
  GLfloat color_background[4];
  getColorFromHexStr(str_color_background, color_background);
  GLfloat color_vertex[4];
  getColorFromHexStr(str_color_vertex, color_vertex);
  GLfloat color_edge_binary[4];
  getColorFromHexStr(str_color_edge_binary, color_edge_binary);
  GLfloat color_edge_hyper[4];
  getColorFromHexStr(str_color_edge_hyper, color_edge_hyper);

  // Set up light sources
  SceneParameters::LightSource light1{{1.0, 1.0, 1.0, 0.0},
                                      {1.0, 1.0, 1.0, 1.0}};
  std::vector<SceneParameters::LightSource> light_sources{light1};

  return SceneParameters(light_sources, color_background, color_vertex,
                         color_edge_binary, color_edge_hyper);
}

int main(int argc, char *argv[]) {
  std::string description{
      "3D CNF Visualization (C) 2006 C. Sinz, JKU Linz | fork by D. Schreiber"};
  CLI::App app{description};

  std::string filename = "-";
  std::string str_color_background = "ffffffff";  // opaque white
  std::string str_color_vertex = "4444ffff";      // opaque blue
  std::string str_color_edge_binary = "8888ffff"; // opaque light blue
  std::string str_color_edge_hyper = "bbbbffff";  // opaque lighter blue
  bool use_space_grid = true;
  unsigned int seed = 0;

  app.add_option(
      "-f,--file", filename,
      "CNF input, or \"-\" to read from stdin (default: read from stdin)");
  app.add_option(
      "-s,--color-scene", str_color_background,
      "Color of scene / background (default: " + str_color_background + ")");
  app.add_option("-v,--color-vertex", str_color_vertex,
                 "Color of vertices (default: " + str_color_vertex + ")");
  app.add_option(
      "-b,--color-edge-binary", str_color_edge_binary,
      "Color of binary clause edges (default: " + str_color_edge_binary + ")");
  app.add_option("-y,--color-edge-hyper", str_color_edge_hyper,
                 "Color of hyper edges (clauses of length > 2) (default: " +
                     str_color_edge_hyper + ")");
  app.add_option("-g,--space-grid", use_space_grid,
                 "Use space grid for force directed layout (default: " +
                     std::to_string(use_space_grid) + ")");
  app.add_option("-r,--random-seed", seed,
                 "Random seed for graph initialization (default: " +
                     std::to_string(seed) + ")");

  CLI11_PARSE(app, argc, argv);
  srandom(seed);

  auto scene_params =
      initSceneParams(str_color_background, str_color_vertex,
                      str_color_edge_binary, str_color_edge_hyper);

  glutInitWindowSize(800, 600);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow(description.c_str());

  init(filename, scene_params, use_space_grid);

  glutDisplayFunc(display);
  glutMotionFunc(motion);
  glutMouseFunc(mouse);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
