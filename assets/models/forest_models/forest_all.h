// Low-poly forest - vsechny modely
// Kazde pole: 8 floatu na vrchol (pozice, normala, uv), GL_TRIANGLES
//
// Pouziti (OpenGL):
//   glBufferData(GL_ARRAY_BUFFER, sizeof(tree_pine), tree_pine, GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);                   // pozice
//   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // normala
//   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // uv
//   glDrawArrays(GL_TRIANGLES, 0, tree_pine_vertex_count);
//
// Textura forest_palette.png: nastavte GL_NEAREST filtrovani (ostre barvy).

#pragma once

// --- trees ---
#include "tree_pine.h"   // 204 vrcholu
#include "tree_oak.h"   // 2808 vrcholu
#include "tree_birch.h"   // 2340 vrcholu
// --- bushes ---
#include "bush_round.h"   // 1440 vrcholu
#include "bush_berry.h"   // 1584 vrcholu
#include "bush_fern.h"   // 3768 vrcholu
// --- flowers ---
#include "flower_daisy.h"   // 846 vrcholu
#include "flower_tulip.h"   // 414 vrcholu
#include "flower_bluebell.h"   // 894 vrcholu
#include "flower_poppy.h"   // 594 vrcholu
#include "flower_lavender.h"   // 2127 vrcholu
// --- props ---
#include "rock_small.h"   // 60 vrcholu
#include "rock_medium.h"   // 240 vrcholu
#include "rock_boulder.h"   // 240 vrcholu
#include "rock_mossy.h"   // 240 vrcholu
#include "rock_cluster.h"   // 540 vrcholu
#include "pebbles.h"   // 540 vrcholu
#include "stepping_stones.h"   // 300 vrcholu
#include "tree_stump.h"   // 348 vrcholu
#include "stump_with_axe.h"   // 420 vrcholu
#include "log_fallen.h"   // 324 vrcholu
#include "log_mossy.h"   // 324 vrcholu
#include "branch.h"   // 162 vrcholu
#include "firewood_pile.h"   // 1440 vrcholu
#include "mushroom_red.h"   // 912 vrcholu
#include "mushroom_brown.h"   // 240 vrcholu
#include "mushroom_cluster.h"   // 1200 vrcholu
#include "grass_tuft.h"   // 297 vrcholu
#include "tall_grass.h"   // 486 vrcholu
#include "cattails.h"   // 567 vrcholu
#include "pine_cone.h"   // 420 vrcholu
#include "leaf_pile.h"   // 783 vrcholu
#include "dead_tree.h"   // 546 vrcholu
#include "sapling_pine.h"   // 108 vrcholu
#include "campfire.h"   // 1038 vrcholu
#include "fence.h"   // 216 vrcholu
#include "signpost.h"   // 144 vrcholu
#include "bench.h"   // 318 vrcholu
#include "wooden_bridge.h"   // 828 vrcholu
#include "tent.h"   // 168 vrcholu
#include "barrel.h"   // 468 vrcholu
#include "birdhouse.h"   // 264 vrcholu
#include "ruin_pillar.h"   // 234 vrcholu
#include "lily_pads.h"   // 396 vrcholu
// --- terrain ---
#include "terrain.h"   // 11526 vrcholu
#include "water.h"   // 42 vrcholu
