#include "vec2.h"

#include <math.h>

vec2_t vec2_from_polar(angle_t angle, double magnitude) {
    vec2_t vec;
    vec.x = cos(angle) * magnitude;
    vec.y = sin(angle) * magnitude;
    return vec;
}

void vec2_add(vec2_t *a, vec2_t b) {
    a->x += b.x;
    a->y += b.y;
}

void vec2_sub(vec2_t *a, vec2_t b) {
    a->x -= b.x;
    a->y -= b.y;
}

double vec2_magnitude(vec2_t vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

double vec2_dist_to_line(vec2_t line_point, vec2_t line_delta, vec2_t point) {
    vec2_sub(&point, line_point);
    return point.x * line_delta.y - point.y * line_delta.x;
}