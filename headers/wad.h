#pragma once

#include "wad_types.h"

void wad_unload();
int wad_load(const char *name);


int wad_lump_index_by_name(const char *name);

/* MAP LOADING */
int wad_read_things(uint32_t map_lump_index, thing_t **pp_things, uint32_t *n_things);
int wad_read_linedefs(uint32_t map_lump_index, linedef_t **pp_linedefs, uint32_t *n_linedefs);
int wad_read_sidedefs(uint32_t map_lump_index, sidedef_t **pp_sidedefs, uint32_t *n_sidedefs);
int wad_read_vertexes(uint32_t map_lump_index, vertex_t **pp_vertexes, uint32_t *n_vertexes);
int wad_read_segs(uint32_t map_lump_index, seg_t **pp_segs, uint32_t *n_segs);
int wad_read_ssectors(uint32_t map_lump_index, ssector_t **pp_ssectors, uint32_t *n_ssectors);
int wad_read_nodes(uint32_t map_lump_index, node_t **pp_nodes, uint32_t *n_nodes);
int wad_read_sectors(uint32_t map_lump_index, sector_t **pp_sectors, uint32_t *n_sectors);

/* RESOURCE LOADING */
int wad_read_palettes(palette_t **pp_palettes, uint32_t *n_palettes);

