// Low-poly space - vsechny modely
// Kazde pole: 8 floatu na vrchol (pozice, normala, uv), GL_TRIANGLES
//
// Pouziti (OpenGL):
//   glBufferData(GL_ARRAY_BUFFER, sizeof(planet_earth), planet_earth, GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);                   // pozice
//   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // normala
//   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // uv
//   glDrawArrays(GL_TRIANGLES, 0, planet_earth_vertex_count);
//
// Textura space_palette.png: nastavte GL_NEAREST filtrovani (ostre barvy).

#pragma once

// --- star ---
#include "sun.h"   // 4776 vrcholu
// --- planets ---
#include "planet_mercury.h"   // 960 vrcholu
#include "planet_venus.h"   // 960 vrcholu
#include "planet_earth.h"   // 4380 vrcholu
#include "planet_mars.h"   // 960 vrcholu
#include "planet_jupiter.h"   // 3840 vrcholu
#include "planet_saturn.h"   // 9024 vrcholu
#include "planet_uranus.h"   // 1920 vrcholu
#include "planet_neptune.h"   // 960 vrcholu
#include "planet_pluto.h"   // 960 vrcholu
#include "planet_alien.h"   // 5922 vrcholu
// --- moons ---
#include "moon_luna.h"   // 960 vrcholu
#include "moon_phobos.h"   // 240 vrcholu
#include "moon_deimos.h"   // 240 vrcholu
#include "moon_io.h"   // 960 vrcholu
#include "moon_europa.h"   // 960 vrcholu
#include "moon_ganymede.h"   // 960 vrcholu
#include "moon_titan.h"   // 960 vrcholu
// --- asteroids ---
#include "asteroid_1.h"   // 240 vrcholu
#include "asteroid_2.h"   // 240 vrcholu
#include "asteroid_3.h"   // 60 vrcholu
#include "asteroid_4.h"   // 480 vrcholu
#include "asteroid_cluster.h"   // 1140 vrcholu
#include "asteroid_crystal.h"   // 570 vrcholu
#include "comet.h"   // 438 vrcholu
#include "meteor.h"   // 360 vrcholu
// --- fun ---
#include "ufo.h"   // 2406 vrcholu
#include "alien.h"   // 3624 vrcholu
#include "rocket.h"   // 1158 vrcholu
#include "satellite.h"   // 828 vrcholu
#include "space_station.h"   // 2940 vrcholu
#include "astronaut.h"   // 1746 vrcholu
#include "moon_rover.h"   // 1380 vrcholu
#include "black_hole.h"   // 7104 vrcholu
#include "monolith.h"   // 36 vrcholu
#include "star_field.h"   // 10800 vrcholu
#include "orbit_ring.h"   // 2304 vrcholu
