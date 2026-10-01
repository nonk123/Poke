#pragma once

#include "grid.h"

extern struct Camera {
	GridPoint position;
	GridCoord zoom;
} g_camera;

AABB screen_viewport(), viewport();
