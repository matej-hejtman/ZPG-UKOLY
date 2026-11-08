// Slunecni soustava - vsechny modely + data
// Includuj jen v JEDNOM .cpp souboru.

#pragma once

#include "sun.h"
#include "mercury.h"
#include "venus.h"
#include "earth.h"
#include "moon.h"
#include "mars.h"
#include "phobos.h"
#include "deimos.h"
#include "jupiter.h"
#include "io.h"
#include "europa.h"
#include "ganymede.h"
#include "callisto.h"
#include "saturn.h"
#include "saturn_rings.h"
#include "enceladus.h"
#include "rhea.h"
#include "titan.h"
#include "iapetus.h"
#include "uranus.h"
#include "uranus_rings.h"
#include "titania.h"
#include "oberon.h"
#include "neptune.h"
#include "triton.h"
#include "pluto.h"
#include "charon.h"

enum class BodyKind { Star, Planet, DwarfPlanet, Moon, Rings };

struct CelestialBody {
    const char*  name;
    const float* vertices;           // 9 floatu na vrchol
    int          vertexCount;
    BodyKind     kind;
    int          parent;             // index v BODIES, -1 = zadny
    float        radiusKm;           // skutecny polomer
    float        orbitRadius;        // planety v AU, mesice v km
    float        orbitPeriodDays;    // zaporne = retrogradni obeh
    float        rotationPeriodHours;// zaporne = retrogradni rotace
    float        axialTiltDeg;
    float        sceneRadius;        // doporucene meritko modelu
    float        sceneDistance;      // doporucena vzdalenost od rodice
};

static const CelestialBody BODIES[] = {
    { "Slunce", sun_vertices, SUN_VERTEX_COUNT, BodyKind::Star, -1, 696340.0f, 0.000f, 0.000f, 609.120f, 7.25f, 2.5000f, 0.0000f },
    { "Merkur", mercury_vertices, MERCURY_VERTEX_COUNT, BodyKind::Planet, 0, 2439.7f, 0.387f, 87.970f, 1407.600f, 0.03f, 0.1948f, 10.2209f },
    { "Venuse", venus_vertices, VENUS_VERTEX_COUNT, BodyKind::Planet, 0, 6051.8f, 0.723f, 224.700f, -5832.500f, 177.40f, 0.2931f, 12.5029f },
    { "Zeme", earth_vertices, EARTH_VERTEX_COUNT, BodyKind::Planet, 0, 6371.0f, 1.000f, 365.256f, 23.934f, 23.44f, 0.3000f, 14.0000f },
    { "Mesic", moon_vertices, MOON_VERTEX_COUNT, BodyKind::Moon, 3, 1737.4f, 384400.000f, 27.322f, 655.700f, 6.68f, 0.1672f, 0.8900f },
    { "Mars", mars_vertices, MARS_VERTEX_COUNT, BodyKind::Planet, 0, 3389.5f, 1.524f, 686.980f, 24.623f, 25.19f, 0.2258f, 16.3450f },
    { "Phobos", phobos_vertices, PHOBOS_VERTEX_COUNT, BodyKind::Moon, 5, 11.3f, 9376.000f, 0.319f, 7.660f, 0.00f, 0.0400f, 0.7936f },
    { "Deimos", deimos_vertices, DEIMOS_VERTEX_COUNT, BodyKind::Moon, 5, 6.2f, 23463.000f, 1.263f, 30.300f, 0.00f, 0.0400f, 1.2936f },
    { "Jupiter", jupiter_vertices, JUPITER_VERTEX_COUNT, BodyKind::Planet, 0, 69911.0f, 5.203f, 4332.590f, 9.925f, 3.13f, 0.8816f, 26.8101f },
    { "Io", io_vertices, IO_VERTEX_COUNT, BodyKind::Moon, 8, 1821.6f, 421700.000f, 1.769f, 42.460f, 0.00f, 0.1708f, 1.6461f },
    { "Europa", europa_vertices, EUROPA_VERTEX_COUNT, BodyKind::Moon, 8, 1560.8f, 671034.000f, 3.551f, 85.230f, 0.10f, 0.1593f, 2.1461f },
    { "Ganymedes", ganymede_vertices, GANYMEDE_VERTEX_COUNT, BodyKind::Moon, 8, 2634.1f, 1070412.000f, 7.155f, 171.700f, 0.33f, 0.2016f, 2.6461f },
    { "Callisto", callisto_vertices, CALLISTO_VERTEX_COUNT, BodyKind::Moon, 8, 2410.3f, 1882709.000f, 16.689f, 400.500f, 0.00f, 0.1937f, 3.1461f },
    { "Saturn", saturn_vertices, SATURN_VERTEX_COUNT, BodyKind::Planet, 0, 58232.0f, 9.537f, 10759.220f, 10.656f, 26.73f, 0.8120f, 34.8820f },
    { "Saturnovy prstence", saturn_rings_vertices, SATURN_RINGS_VERTEX_COUNT, BodyKind::Rings, 13, 0.0f, 0.000f, 0.000f, 0.000f, 26.73f, 0.8120f, 0.0000f },
    { "Enceladus", enceladus_vertices, ENCELADUS_VERTEX_COUNT, BodyKind::Moon, 13, 252.1f, 237948.000f, 1.370f, 32.880f, 0.00f, 0.0701f, 2.8962f },
    { "Rhea", rhea_vertices, RHEA_VERTEX_COUNT, BodyKind::Moon, 13, 763.8f, 527108.000f, 4.518f, 108.400f, 0.00f, 0.1155f, 3.3962f },
    { "Titan", titan_vertices, TITAN_VERTEX_COUNT, BodyKind::Moon, 13, 2574.7f, 1221870.000f, 15.945f, 382.700f, 0.30f, 0.1996f, 3.8962f },
    { "Iapetus", iapetus_vertices, IAPETUS_VERTEX_COUNT, BodyKind::Moon, 13, 734.5f, 3560820.000f, 79.320f, 1903.700f, 0.00f, 0.1135f, 4.3962f },
    { "Uran", uranus_vertices, URANUS_VERTEX_COUNT, BodyKind::Planet, 0, 25362.0f, 19.191f, 30688.500f, -17.240f, 97.77f, 0.5586f, 47.8075f },
    { "Uranovy prstence", uranus_rings_vertices, URANUS_RINGS_VERTEX_COUNT, BodyKind::Rings, 19, 0.0f, 0.000f, 0.000f, 0.000f, 97.77f, 0.5586f, 0.0000f },
    { "Titania", titania_vertices, TITANIA_VERTEX_COUNT, BodyKind::Moon, 19, 788.4f, 435910.000f, 8.706f, 208.900f, 0.00f, 0.1172f, 1.9742f },
    { "Oberon", oberon_vertices, OBERON_VERTEX_COUNT, BodyKind::Moon, 19, 761.4f, 583520.000f, 13.463f, 323.100f, 0.00f, 0.1153f, 2.4742f },
    { "Neptun", neptune_vertices, NEPTUNE_VERTEX_COUNT, BodyKind::Planet, 0, 24622.0f, 30.070f, 60195.000f, 16.110f, 28.32f, 0.5512f, 58.8361f },
    { "Triton", triton_vertices, TRITON_VERTEX_COUNT, BodyKind::Moon, 23, 1353.4f, 354759.000f, -5.877f, 141.000f, 0.00f, 0.1494f, 1.2166f },
    { "Pluto", pluto_vertices, PLUTO_VERTEX_COUNT, BodyKind::DwarfPlanet, 0, 1188.3f, 39.480f, 90560.000f, -153.290f, 122.53f, 0.1409f, 66.8331f },
    { "Charon", charon_vertices, CHARON_VERTEX_COUNT, BodyKind::Moon, 25, 606.0f, 19591.000f, 6.387f, 153.300f, 0.00f, 0.1041f, 0.6832f },
};

static const int BODY_COUNT = 27;
