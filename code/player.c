#include "player.h"
#include "map.h"

#include <math.h>
#include <assert.h>


static void find_current_sector_floor_height(player_t *pplayer) {
    uint32_t node_id = map_nnodes - 1;
    node_t *pnode;

    while (!(node_id & SUBSECTOR_IDENTIFIER)) {
        pnode = &map_nodes[node_id];
        double part_dist = vec2_dist_to_line(
            (vec2_t){pnode->x_part, pnode->y_part},
            (vec2_t){pnode->dx, pnode->dy},
            pplayer->pos
        );

        if (part_dist <= 0.0) {
            node_id = pnode->left_child_id;
        } else {
            node_id = pnode->right_child_id;
        }
    }

    uint32_t ssector_id = node_id & (~SUBSECTOR_IDENTIFIER);
    seg_t *pseg = &map_segs[map_ssectors[ssector_id].first_seg_id];
    linedef_t *plinedef = &map_linedefs[pseg->linedef_id];
    sidedef_t *psidedef;
    if (pseg->direction) {
        psidedef = &map_sidedefs[plinedef->left_sidedef_id];
    } else {
        psidedef = &map_sidedefs[plinedef->right_sidedef_id];
    }
    sector_t *psector = &map_sectors[psidedef->sector];

    pplayer->eye_height = 41.0 + psector->floor_height;
}

void player_move_forward(player_t *pplayer, double amount) {
    vec2_t move_vec = vec2_from_polar(pplayer->angle, amount);
    vec2_add(&(pplayer->pos), move_vec);
    find_current_sector_floor_height(pplayer);
}

angle_t player_angle_to_point(player_t *pplayer, vec2_t vertex) {
    vec2_sub(&vertex, pplayer->pos);
    return angle_normalize(atan2(vertex.y, vertex.x));
}

bool player_clip_angle_to_fov(player_t *pplayer, angle_t *pang1, angle_t *pang2) {
    angle_t span = angle_sub(*pang1, *pang2);

    if (span >= ANG180) {
        return false;
    }

    assert(pplayer->angle >= 0.0 && pplayer->angle < MAX_ANGLE);

    *pang1 = angle_sub(*pang1, pplayer->angle);
    *pang2 = angle_sub(*pang2, pplayer->angle);

    angle_t ang1_moved = angle_add(*pang1, ANG45); // Adding half fov to ang1
    if (ang1_moved > ANG90) { // If ang1_moved > fov
        ang1_moved -= ANG90;
        if (ang1_moved > span) {
            return false;
        }
        *pang1 = ANG45; // Clip ang1 to half fov
    }

    angle_t ang2_moved = angle_sub(ANG45, *pang2);
    if (ang2_moved > ANG90) {
        *pang2 = ANG270 + ANG45; // same as: -half fov
    }

    *pang1 = angle_add(*pang1, ANG90);
    *pang2 = angle_add(*pang2, ANG90);

    return true;
}

void player_set_start(player_t *pplayer) {
    for (uint32_t i = 0; i < map_nthings; i++) {
        if (map_things[i].type == pplayer->id) {
            pplayer->pos.x = map_things[i].x_pos;
            pplayer->pos.y = map_things[i].y_pos;
            pplayer->angle = map_things[i].facing;
            pplayer->eye_height = 41.0;
            break;
        }
    }

    find_current_sector_floor_height(pplayer);
}

double player_dist_to_point(player_t *pplayer, double x, double y) {
    double dx = fabs(x - pplayer->pos.x);
    double dy = fabs(y - pplayer->pos.y);

    if (dy > dx) {
        double tmp = dy;
        dy = dx;
        dx = tmp;
    }

    double result = sqrt((dx * dx) + (dy * dy));
    return result;
}