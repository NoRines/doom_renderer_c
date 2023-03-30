#pragma once

#include "angle.h"

typedef struct {
    double x;
    double y;
} vec2_t;


vec2_t vec2_from_polar(angle_t angle, double magnitude);
void vec2_add(vec2_t *a, vec2_t b);
void vec2_sub(vec2_t *a, vec2_t b);
double vec2_magnitude(vec2_t vec);
double vec2_dist_to_line(vec2_t line_point, vec2_t line_delta, vec2_t point);