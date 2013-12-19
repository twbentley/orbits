#ifndef GLOBALS_H
#define GLOBALS_H
#include <GL/glew.h>
#include <string>

// Gravitational constant
static const GLfloat G = .1f;

// Planet Types
static const std::string SUN = "SUN";
static const std::string PLANET = "PLANET";
static const std::string ASTEROID = "ASTEROID";

// Constants for window size
static const int SCREEN_WIDTH = 512;
static const int SCREEN_HEIGHT = 512;

// Max number of bodies per octant of octree
static const int MAX_BODIES;

#endif