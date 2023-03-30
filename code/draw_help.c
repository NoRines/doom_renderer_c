#include "draw_help.h"



bool draw_help_check_bbox(player_t *pplayer, double left, double right, double top, double bottom) {
    vec2_t bbox_sides[2][2] = {0};
    uint32_t n_sides = 0;

    vec2_t a = {left, bottom};
    vec2_t b = {left, top};
    vec2_t c = {right, top};
    vec2_t d = {right, bottom};

    /* bc */
    /* ad */
    vec2_t pos = pplayer->pos;

    if (pos.x < left) {
        if (pos.y > top) {
            bbox_sides[0][0] = b;
            bbox_sides[0][1] = a;
            bbox_sides[1][0] = c;
            bbox_sides[1][1] = b;
            n_sides = 2;
        } else if (pos.y < bottom) {
            bbox_sides[0][0] = b;
            bbox_sides[0][1] = a;
            bbox_sides[1][0] = a;
            bbox_sides[1][1] = d;
            n_sides = 2;
        } else {
            bbox_sides[0][0] = b;
            bbox_sides[0][1] = a;
            n_sides = 1;
        }
    } else if (pos.x > right) {
        if (pos.y > top) {
            bbox_sides[0][0] = c;
            bbox_sides[0][1] = b;
            bbox_sides[1][0] = d;
            bbox_sides[1][1] = c;
            n_sides = 2;
        } else if (pos.y < bottom) {
            bbox_sides[0][0] = a;
            bbox_sides[0][1] = d;
            bbox_sides[1][0] = d;
            bbox_sides[1][1] = c;
            n_sides = 2;
        } else {
            bbox_sides[0][0] = d;
            bbox_sides[0][1] = c;
            n_sides = 1;
        }
    } else {
        if (pos.y > top) {
            bbox_sides[0][0] = c;
            bbox_sides[0][1] = b;
            n_sides = 1;
        } else if (pos.y < bottom) {
            bbox_sides[0][0] = a;
            bbox_sides[0][1] = d;
            n_sides = 1;           
        } else {
            return true; // Player is inside bounding box
        }
    }

    for (uint32_t i = 0; i < n_sides; i++) {
        angle_t ang1 = player_angle_to_point(pplayer, bbox_sides[i][0]);
        angle_t ang2 = player_angle_to_point(pplayer, bbox_sides[i][1]);
        if (player_clip_angle_to_fov(pplayer, &ang1, &ang2)) {
            return true;
        }
    }

    return false;
}