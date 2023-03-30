#pragma once

#include "vec2.h"
#include "angle.h"

#define FOV ANG90
#define HALF_FOV ANG45

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t id;
    vec2_t pos;
    angle_t angle;
    double eye_height;
} player_t;

void player_move_forward(player_t *pplayer, double amount);
angle_t player_angle_to_point(player_t *pplayer, vec2_t vertex);
bool player_clip_angle_to_fov(player_t *pplayer, angle_t *pang1, angle_t *pang2);
void player_set_start(player_t *pplayer);
double player_dist_to_point(player_t *pplayer, double x, double y);