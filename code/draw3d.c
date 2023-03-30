#include "draw3d.h"
#include "draw3d_seg.h"
#include "draw_help.h"
#include "map.h"

#include <stdio.h>


void draw3d_quit() {
    draw3d_seg_quit();
}

void draw3d_init() {
    draw3d_seg_init();
}

static void frame_start() {
    draw3d_seg_clear_solidsegs();
    draw3d_seg_reset_vertical_clip();
}



static void subsector(uint32_t ssector_id, player_t *pplayer) {
    ssector_t *pssector = &map_ssectors[ssector_id];

    for (uint32_t i = 0; i < pssector->seg_count; i++) {
        seg_t *pseg = &map_segs[pssector->first_seg_id + i];
        draw3d_seg_plot(pseg, pplayer);
    }
}

static void bsp_search(uint32_t node_id, player_t *pplayer) {
    if (node_id & SUBSECTOR_IDENTIFIER) {
        subsector(node_id & (~SUBSECTOR_IDENTIFIER), pplayer);
        return;
    }

    node_t *pnode = &map_nodes[node_id];

    double part_dist = vec2_dist_to_line(
        (vec2_t){pnode->x_part, pnode->y_part},
        (vec2_t){pnode->dx, pnode->dy},
        pplayer->pos
    );

    if (part_dist <= 0.0) {
        bsp_search(pnode->left_child_id, pplayer);
        if (draw_help_check_bbox(
                pplayer,
                pnode->right_bbox_left,
                pnode->right_bbox_right,
                pnode->right_bbox_top,
                pnode->right_bbox_bottom)
            ) {
            bsp_search(pnode->right_child_id, pplayer);
        }
    } else {
        bsp_search(pnode->right_child_id, pplayer);
        if (draw_help_check_bbox(
                pplayer,
                pnode->left_bbox_left,
                pnode->left_bbox_right,
                pnode->left_bbox_top,
                pnode->left_bbox_bottom)
            ) {
            bsp_search(pnode->left_child_id, pplayer);
        }
    }
}

void draw3d_view(player_t *pplayer) {
    frame_start();
    bsp_search(map_nnodes - 1, pplayer);
}

