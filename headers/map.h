#pragma once

#include "map_types.h"

void map_destroy();
int map_create(const char *map_name);





/* Map globals */
extern uint32_t map_nvertexes;
extern vertex_t *map_vertexes;

extern uint32_t map_nlinedefs;
extern linedef_t *map_linedefs;

extern uint32_t map_nthings;
extern thing_t *map_things;

extern uint32_t map_nsidedefs;
extern sidedef_t *map_sidedefs;

extern uint32_t map_nsegs;
extern seg_t *map_segs;

extern uint32_t map_nssectors;
extern ssector_t *map_ssectors;

extern uint32_t map_nnodes;
extern node_t *map_nodes;

extern uint32_t map_nsectors;
extern sector_t *map_sectors;





