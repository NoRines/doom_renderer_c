#include "draw2d.h"
#include "draw_help.h"
#include "sdl_help.h"

#include "map.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static SDL_Renderer *prenderer = NULL;

static int32_t render_x_size = 0;
static int32_t render_y_size = 0;

static int32_t min_x = INT32_MAX;
static int32_t max_x = INT32_MIN;
static int32_t min_y = INT32_MAX;
static int32_t max_y = INT32_MIN;

static int32_t remap_x(int32_t x) {
    x = (x - min_x) / 7;
    return x;
}

static int32_t remap_y(int32_t y) {
    y = (render_y_size - 1) - (y - min_y) / 7;
    return y;
}

void draw2d_init() {
    prenderer = sdl_help_renderer();

    for (uint32_t i = 0; i < map_nvertexes; i++) {
        min_x = MIN(min_x, (int32_t)map_vertexes[i].x_pos);
        max_x = MAX(max_x, (int32_t)map_vertexes[i].x_pos);
        min_y = MIN(min_y, (int32_t)map_vertexes[i].y_pos);
        max_y = MAX(max_y, (int32_t)map_vertexes[i].y_pos);
    }

    SDL_RenderGetLogicalSize(prenderer, &render_x_size, &render_y_size);
}

void draw2d_automap() {
    SDL_SetRenderDrawColor(prenderer, 255, 255, 255, 255);
    for (uint32_t i = 0; i < map_nlinedefs; i++) {
        vertex_t v1 = map_vertexes[map_linedefs[i].start_vertex_id];
        vertex_t v2 = map_vertexes[map_linedefs[i].end_vertex_id];

        SDL_RenderDrawLine(prenderer, remap_x(v1.x_pos), remap_y(v1.y_pos), remap_x(v2.x_pos), remap_y(v2.y_pos));
    }
}

void draw2d_player(player_t *pplayer) {
    int32_t x1, y1, x2, y2;

    x1 = remap_x((int32_t)pplayer->pos.x);
    y1 = remap_y((int32_t)pplayer->pos.y);

    /* Draw player fov */
    SDL_SetRenderDrawColor(prenderer, 0, 255, 0, 255);

    x2 = x1 + (int32_t)(cos(-(pplayer->angle - ANG45)) * 60.0);
    y2 = y1 + (int32_t)(sin(-(pplayer->angle - ANG45)) * 60.0);
    SDL_RenderDrawLine(prenderer, x1, y1, x2, y2);

    x2 = x1 + (int32_t)(cos(-(pplayer->angle + ANG45)) * 60.0);
    y2 = y1 + (int32_t)(sin(-(pplayer->angle + ANG45)) * 60.0);
    SDL_RenderDrawLine(prenderer, x1, y1, x2, y2);

    /* Draw player body and direction*/
    x2 = x1 + (int32_t)(cos(-pplayer->angle) * 15.0);
    y2 = y1 + (int32_t)(sin(-pplayer->angle) * 15.0);

    SDL_Rect r;
    r.x = x1 - 1;
    r.y = y1 - 1;
    r.w = 3;
    r.h = 3;

    SDL_SetRenderDrawColor(prenderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(prenderer, &r);
    SDL_RenderDrawLine(prenderer, x1, y1, x2, y2);
}

void draw2d_node_bboxes(uint32_t node_id) {
    node_t *pnode = &map_nodes[node_id];

    SDL_SetRenderDrawColor(prenderer, 0, 255, 0, 255); // Right bbox

    SDL_Rect left_rect;
    left_rect.x = remap_x(pnode->left_bbox_left);
    left_rect.y = remap_y(pnode->left_bbox_top);
    left_rect.w = remap_x(pnode->left_bbox_right) - left_rect.x + 1;
    left_rect.h = remap_y(pnode->left_bbox_bottom) - left_rect.y + 1;

    SDL_RenderDrawRect(prenderer, &left_rect);

    SDL_SetRenderDrawColor(prenderer, 255, 0, 0, 255); // Left bbox

    SDL_Rect right_rect;
    right_rect.x = remap_x(pnode->right_bbox_left);
    right_rect.y = remap_y(pnode->right_bbox_top);
    right_rect.w = remap_x(pnode->right_bbox_right) - right_rect.x + 1;
    right_rect.h = remap_y(pnode->right_bbox_bottom) - right_rect.y + 1;

    SDL_RenderDrawRect(prenderer, &right_rect);

    SDL_SetRenderDrawColor(prenderer, 0, 0, 255, 255);

    SDL_RenderDrawLine(
        prenderer,
        remap_x(pnode->x_part),
        remap_y(pnode->y_part),
        remap_x(pnode->x_part + pnode->dx),
        remap_y(pnode->y_part + pnode->dy)
    );
}

static void subsector(uint32_t ssector_id, player_t *pplayer) {
    ssector_t *ssector = &map_ssectors[ssector_id];
    seg_t *seg;

    for (uint32_t i = 0; i < ssector->seg_count; i++) {
        seg = &map_segs[ssector->first_seg_id + i];
        vertex_t v1 = map_vertexes[seg->start_vertex_id];
        vertex_t v2 = map_vertexes[seg->end_vertex_id];

        vec2_t vec1 = {v1.x_pos, v1.y_pos};
        vec2_t vec2 = {v2.x_pos, v2.y_pos};
        angle_t ang1 = player_angle_to_point(pplayer, vec1);
        angle_t ang2 = player_angle_to_point(pplayer, vec2);
        if (player_clip_angle_to_fov(pplayer, &ang1, &ang2)) {
            SDL_RenderDrawLine(prenderer, remap_x(v1.x_pos), remap_y(v1.y_pos), remap_x(v2.x_pos), remap_y(v2.y_pos));
        }
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

    if (part_dist <= 0.0) { // Is left
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
    } else { // Is right
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

void draw2d_visible_automap(player_t *pplayer) {
    SDL_SetRenderDrawColor(prenderer, 0, 0, 255, 255);
    bsp_search(map_nnodes - 1, pplayer);
}


