#include "map.h"

#include "wad.h"
#include "vec2.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* Map owns resources related to map structure. */
/* MAP GLOBALS */
/****************************************************************/
uint32_t map_nvertexes = 0;
vertex_t *map_vertexes = NULL;

uint32_t map_nlinedefs = 0;
linedef_t *map_linedefs = NULL;

uint32_t map_nthings = 0;
thing_t *map_things = NULL;

uint32_t map_nsidedefs = 0;
sidedef_t *map_sidedefs = NULL;

uint32_t map_nsegs = 0;
seg_t *map_segs = NULL;

uint32_t map_nssectors = 0;
ssector_t *map_ssectors = NULL;

uint32_t map_nnodes = 0;
node_t *map_nodes = NULL;

uint32_t map_nsectors = 0;
sector_t *map_sectors = NULL;
/****************************************************************/

static void correct_wad_vertex_point(linedef_t *plinedef, uint32_t seg_point_id) {
    vertex_t vert_linedef_start = map_vertexes[plinedef->start_vertex_id];
    vertex_t vert_linedef_end = map_vertexes[plinedef->end_vertex_id];

    vec2_t linedef_start = {vert_linedef_start.x_pos, vert_linedef_start.y_pos};
    vec2_t delta = {vert_linedef_end.x_pos, vert_linedef_end.y_pos};

    vec2_sub(&delta, linedef_start);

    vertex_t *pvert_seg_point = &map_vertexes[seg_point_id];

    double dx2 = delta.x * delta.x;
    double dy2 = delta.y * delta.y;
    double dxy = delta.x * delta.y;
    double x1 = linedef_start.x;
    double y1 = linedef_start.y;
    double x0 = pvert_seg_point->x_pos;
    double y0 = pvert_seg_point->y_pos;

    double s = dx2 + dy2;

    double new_x = (dx2 * x0 + dy2 * x1 + dxy * (y0 - y1)) / s;
    double new_y = (dy2 * y0 + dx2 * y1 + dxy * (x0 - x1)) / s;

    pvert_seg_point->x_pos = new_x;
    pvert_seg_point->y_pos = new_y;
}

static void correct_wad_vertex_data() {
    seg_t *pseg;
    linedef_t *plinedef;

    bool *vert_handled = NULL;
    vert_handled = calloc(map_nvertexes, sizeof *vert_handled);

    for (uint32_t i = 0; i < map_nsegs; i++) {
        pseg = &map_segs[i];
        plinedef = &map_linedefs[pseg->linedef_id];

        if (vert_handled[pseg->start_vertex_id] == false) {
            vert_handled[pseg->start_vertex_id] = true;
            if (pseg->start_vertex_id != plinedef->start_vertex_id && pseg->start_vertex_id != plinedef->end_vertex_id) {
                correct_wad_vertex_point(plinedef, pseg->start_vertex_id);
            }
        }

        if (vert_handled[pseg->end_vertex_id] == false) {
            vert_handled[pseg->end_vertex_id] = true;
            if (pseg->end_vertex_id != plinedef->start_vertex_id && pseg->end_vertex_id != plinedef->end_vertex_id) {
                correct_wad_vertex_point(plinedef, pseg->end_vertex_id);
            }
        }
    }

    free(vert_handled);
}

void map_destroy() {
    /* Freeing any loaded vertexes */
    free(map_vertexes);
    map_vertexes = NULL;
    /* Freeing any loaded linedefs */
    free(map_linedefs);
    map_linedefs = NULL;
    /* Freeing any loaded things */
    free(map_things);
    map_things = NULL;
    /* Freeing any loaded sidedefs */
    free(map_sidedefs);
    map_sidedefs = NULL;
    /* Freeing any loaded segs */
    free(map_segs);
    map_segs = NULL;
    /* Freeing any loaded ssectors */
    free(map_ssectors);
    map_ssectors = NULL;
    /* Freeing any loaded nodes */
    free(map_nodes);
    map_nodes = NULL;
    /* Freeing any loaded sectors */
    free(map_sectors);
    map_sectors = NULL;
}

int map_create(const char *map_name) {
    map_destroy();

    int map_lump_index = wad_lump_index_by_name(map_name);
    if (map_lump_index < 0) {
        printf("ERROR: Invalid map name provided: %s\n", map_name);
        return -1;
    }

    if (wad_read_vertexes(map_lump_index, &map_vertexes, &map_nvertexes) != 0) return -1;
    if (wad_read_linedefs(map_lump_index, &map_linedefs, &map_nlinedefs) != 0) return -1;
    if (wad_read_things(map_lump_index, &map_things, &map_nthings) != 0) return -1;
    if (wad_read_sidedefs(map_lump_index, &map_sidedefs, &map_nsidedefs) != 0) return -1;
    if (wad_read_segs(map_lump_index, &map_segs, &map_nsegs) != 0) return -1;
    if (wad_read_ssectors(map_lump_index, &map_ssectors, &map_nssectors) != 0) return -1;
    if (wad_read_nodes(map_lump_index, &map_nodes, &map_nnodes) != 0) return -1;
    if (wad_read_sectors(map_lump_index, &map_sectors, &map_nsectors) != 0) return -1;

    printf("INFO: %d, vertexes loaded\n", map_nvertexes);
    printf("INFO: %d, linedefs loaded\n", map_nlinedefs);
    printf("INFO: %d, things loaded\n", map_nthings);
    printf("INFO: %d, sidedefs loaded\n", map_nsidedefs);
    printf("INFO: %d, segs loaded\n", map_nsegs);
    printf("INFO: %d, ssectors loaded\n", map_nssectors);
    printf("INFO: %d, nodes loaded\n", map_nnodes);
    printf("INFO: %d, sectors loaded\n", map_nsectors);

    correct_wad_vertex_data();
    return 0;
}