#pragma once

#include "map_types.h"

void wad_unload();
int wad_load(const char *name);


int wad_map_lump_index(const char *name);

int wad_read_things(uint32_t map_lump_index, thing_t **pp_things, uint32_t *n_things);
int wad_read_linedefs(uint32_t map_lump_index, linedef_t **pp_linedefs, uint32_t *n_linedefs);
int wad_read_sidedefs(uint32_t map_lump_index, sidedef_t **pp_sidedefs, uint32_t *n_sidedefs);
int wad_read_vertexes(uint32_t map_lump_index, vertex_t **pp_vertexes, uint32_t *n_vertexes);
int wad_read_segs(uint32_t map_lump_index, seg_t **pp_segs, uint32_t *n_segs);
int wad_read_ssectors(uint32_t map_lump_index, ssector_t **pp_ssectors, uint32_t *n_ssectors);
int wad_read_nodes(uint32_t map_lump_index, node_t **pp_nodes, uint32_t *n_nodes);
int wad_read_sectors(uint32_t map_lump_index, sector_t **pp_sectors, uint32_t *n_sectors);


