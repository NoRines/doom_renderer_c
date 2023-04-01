#include "wad.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>



typedef struct {
    char id[5];
    uint32_t numlumps;
    uint32_t infotableofs;
} wadinfo_t;

#define WAD_WADINFO_T_SIZE 12

typedef struct {
    uint32_t filepos;
    uint32_t size;
    char name[9];
} filelump_t;

#define WAD_FILELUMP_T_SIZE 16


static long wad_buffer_size = 0;
static char *wad_buffer = NULL;


static wadinfo_t wadinfo = {0};
static filelump_t *lumps = NULL;


static uint16_t two_bytes(char *buffer, uint32_t offset) {
    uint8_t lo = (uint8_t)buffer[offset];
    uint8_t hi = (uint8_t)buffer[offset + 1];
    return lo | (hi << 8);
}

static uint32_t four_bytes(char *buffer, uint32_t offset) {
    uint16_t lo = two_bytes(buffer, offset);
    uint16_t hi = two_bytes(buffer, offset + 2);
    return lo | (hi << 16);
}


static wadinfo_t get_header(char *buffer, uint32_t offset) {
    wadinfo_t info = {0};
    strncpy(info.id, buffer + offset, 4);
    info.id[4] = '\0';
    info.numlumps = four_bytes(buffer, offset + 4);
    info.infotableofs = four_bytes(buffer, offset + 8);
    return info;
}

static filelump_t get_lump(char *buffer, uint32_t offset) {
    filelump_t lump = {0};
    lump.filepos = four_bytes(buffer, offset);
    lump.size = four_bytes(buffer, offset + 4);
    strncpy(lump.name, buffer + offset + 8, 8);
    lump.name[8] = '\0';
    return lump;
}

static int get_lumps() {
    wadinfo = get_header(wad_buffer, 0);
    if (strcmp(wadinfo.id, "IWAD") != 0) {
        printf("File does not fit wad specifications.\n");
        return -1;
    }

    lumps = malloc(wadinfo.numlumps * sizeof *lumps);
    for (uint32_t i = 0; i < wadinfo.numlumps; i++) {
        lumps[i] = get_lump(wad_buffer, wadinfo.infotableofs + i * WAD_FILELUMP_T_SIZE);
    }

    return 0;
}

void wad_unload() {
    free(wad_buffer);
    wad_buffer = NULL;
    free(lumps);
    lumps = NULL;
}

int wad_load(const char *name) {
    wad_unload();
    FILE *file = NULL;

    file = fopen(name, "rb");
    if (!file) {
        perror("Failed to open file: ");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer;
    buffer = malloc(filesize * sizeof *buffer);

    fread((void*)buffer, sizeof *buffer, filesize, file);

    wad_buffer = buffer;
    wad_buffer_size = filesize;

    if (get_lumps()) {
        wad_unload();
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}


int wad_lump_index_by_name(const char *name) {
    for (uint32_t i = 0; i < wadinfo.numlumps; i++) {
        if (strcmp(lumps[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}


static thing_t read_thing(char *buffer, uint32_t offset) {
    thing_t thing = {0};
    thing.x_pos         = (double)((int16_t)two_bytes(buffer, offset + 0));
    thing.y_pos         = (double)((int16_t)two_bytes(buffer, offset + 2));
    thing.facing        = (angle_t)two_bytes(buffer, offset + 4) * ANG1;
    thing.type          = two_bytes(buffer, offset + 6);
    thing.flags         = two_bytes(buffer, offset + 8);
    return thing;
}

int wad_read_things(uint32_t map_lump_index, thing_t **pp_things, uint32_t *n_things) {
    if (*pp_things) {
        printf("Error invalid things double pointer. (Base pointer should be NULL).\n");
        return -1;
    }

    filelump_t *lump = &lumps[map_lump_index + eTHINGS];

    if (strcmp(lump->name, "THINGS")) {
        printf("Error invalid map_lump_index.");
        return -1;
    }

    *n_things = lump->size / WAD_THING_T_SIZE;
    *pp_things = malloc(*n_things * sizeof **pp_things);

    for (uint32_t i = 0; i < *n_things; i++) {
        (*pp_things)[i] = read_thing(wad_buffer, lump->filepos + i * WAD_THING_T_SIZE);
    }
    return 0;
}


static linedef_t read_linedef(char *buffer, uint32_t offset) {
    linedef_t linedef = {0};
    linedef.start_vertex_id         = two_bytes(buffer, offset + 0);
    linedef.end_vertex_id           = two_bytes(buffer, offset + 2);
    linedef.flags                   = two_bytes(buffer, offset + 4);
    linedef.special_type            = two_bytes(buffer, offset + 6);
    linedef.sector_tag              = two_bytes(buffer, offset + 8);
    linedef.right_sidedef_id        = two_bytes(buffer, offset + 10);
    linedef.left_sidedef_id         = two_bytes(buffer, offset + 12);
    return linedef;
}

int wad_read_linedefs(uint32_t map_lump_index, linedef_t **pp_linedefs, uint32_t *n_linedefs) {
    if (*pp_linedefs) {
        printf("Error invalid things double pointer. (Base pointer should be NULL).\n");
        return -1;
    }

    filelump_t *lump = &lumps[map_lump_index + eLINEDEFS];

    if (strcmp(lump->name, "LINEDEFS")) {
        printf("Error invalid map_lump_index.");
        return -1;
    }

    *n_linedefs = lump->size / WAD_LINEDEF_T_SIZE;
    *pp_linedefs = malloc(*n_linedefs * sizeof **pp_linedefs);

    for (uint32_t i = 0; i < *n_linedefs; i++) {
        (*pp_linedefs)[i] = read_linedef(wad_buffer, lump->filepos + i * WAD_LINEDEF_T_SIZE);
    }
    return 0;
}


static sidedef_t read_sidedef(char *buffer, uint32_t offset) {
    sidedef_t sidedef = {0};
    sidedef.x_offset        = two_bytes(buffer, offset + 0);
    sidedef.y_offset        = two_bytes(buffer, offset + 2);
    strncpy(sidedef.upper_name, buffer + offset + 4, 8);
    sidedef.upper_name[8] = '\0';
    strncpy(sidedef.lower_name, buffer + offset + 12, 8);
    sidedef.lower_name[8] = '\0';
    strncpy(sidedef.middle_name, buffer + offset + 20, 8);
    sidedef.middle_name[8] = '\0';
    sidedef.sector          = two_bytes(buffer, offset + 28);
    return sidedef;
}

int wad_read_sidedefs(uint32_t map_lump_index, sidedef_t **pp_sidedefs, uint32_t *n_sidedefs) {
    if (*pp_sidedefs) {
        printf("Error invalid things double pointer. (Base pointer should be NULL).\n");
        return -1;
    }

    filelump_t *lump = &lumps[map_lump_index + eSIDEDEFS];

    if (strcmp(lump->name, "SIDEDEFS")) {
        printf("Error invalid map_lump_index.");
        return -1;
    }

    *n_sidedefs = lump->size / WAD_SIDEDEF_T_SIZE;
    *pp_sidedefs = malloc(*n_sidedefs * sizeof **pp_sidedefs);

    for (uint32_t i = 0; i < *n_sidedefs; i++) {
        (*pp_sidedefs)[i] = read_sidedef(wad_buffer, lump->filepos + i * WAD_SIDEDEF_T_SIZE);
    }
    return 0;
}


static vertex_t read_vertex(char *buffer, uint32_t offset) {
    vertex_t vertex = {0};
    vertex.x_pos = (double)((int16_t)two_bytes(buffer, offset + 0));
    vertex.y_pos = (double)((int16_t)two_bytes(buffer, offset + 2));
    return vertex;
}

int wad_read_vertexes(uint32_t map_lump_index, vertex_t **pp_vertexes, uint32_t *n_vertexes) {
    if (*pp_vertexes) {
        printf("Error invalid things double pointer. (Base pointer should be NULL).\n");
        return -1;
    }

    filelump_t *lump = &lumps[map_lump_index + eVERTEXES];

    if (strcmp(lump->name, "VERTEXES")) {
        printf("Error invalid map_lump_index.");
        return -1;
    }

    *n_vertexes = lump->size / WAD_VERTEX_T_SIZE;
    *pp_vertexes = malloc(*n_vertexes * sizeof **pp_vertexes);

    for (uint32_t i = 0; i < *n_vertexes; i++) {
        (*pp_vertexes)[i] = read_vertex(wad_buffer, lump->filepos + i * WAD_VERTEX_T_SIZE);
    }
    return 0;
}


static seg_t read_seg(char *buffer, uint32_t offset) {
    seg_t seg = {0};
    seg.start_vertex_id     = two_bytes(buffer, offset + 0);
    seg.end_vertex_id       = two_bytes(buffer, offset + 2);
    seg.angle               = angle_from_bam((uint32_t)two_bytes(buffer, offset + 4) << 16);
    seg.linedef_id          = two_bytes(buffer, offset + 6);
    seg.direction           = two_bytes(buffer, offset + 8);
    seg.offset              = two_bytes(buffer, offset + 10);
    return seg;
}

int wad_read_segs(uint32_t map_lump_index, seg_t **pp_segs, uint32_t *n_segs) {
    if (*pp_segs) {
        printf("Error invalid things double pointer. (Base pointer should be NULL).\n");
        return -1;
    }

    filelump_t *lump = &lumps[map_lump_index + eSEGS];

    if (strcmp(lump->name, "SEGS")) {
        printf("Error invalid map_lump_index.");
        return -1;
    }

    *n_segs = lump->size / WAD_SEG_T_SIZE;
    *pp_segs = malloc(*n_segs * sizeof **pp_segs);

    for (uint32_t i = 0; i < *n_segs; i++) {
        (*pp_segs)[i] = read_seg(wad_buffer, lump->filepos + i * WAD_SEG_T_SIZE);
    }
    return 0;
}


static ssector_t read_ssector(char *buffer, uint32_t offset) {
    ssector_t ssector = {0};
    ssector.seg_count = two_bytes(buffer, offset);
    ssector.first_seg_id = two_bytes(buffer, offset + 2);
    return ssector;
}

int wad_read_ssectors(uint32_t map_lump_index, ssector_t **pp_ssectors, uint32_t *n_ssectors) {
    if (*pp_ssectors) {
        printf("Error invalid things double pointer. (Base pointer should be NULL).\n");
        return -1;
    }

    filelump_t *lump = &lumps[map_lump_index + eSSECTORS];

    if (strcmp(lump->name, "SSECTORS")) {
        printf("Error invalid map_lump_index.");
        return -1;
    }

    *n_ssectors = lump->size / WAD_SSECTOR_T_SIZE;
    *pp_ssectors = malloc(*n_ssectors * sizeof **pp_ssectors);

    for (uint32_t i = 0; i < *n_ssectors; i++) {
        (*pp_ssectors)[i] = read_ssector(wad_buffer, lump->filepos + i * WAD_SSECTOR_T_SIZE);
    }
    return 0;
}



static node_t read_node(char *buffer, uint32_t offset) {
    node_t node = {0};
    node.x_part             = (double)((int16_t)two_bytes(buffer, offset + 0));
    node.y_part             = (double)((int16_t)two_bytes(buffer, offset + 2));
    node.dx                 = (double)((int16_t)two_bytes(buffer, offset + 4));
    node.dy                 = (double)((int16_t)two_bytes(buffer, offset + 6));

    node.right_bbox_top     = (double)((int16_t)two_bytes(buffer, offset + 8));
    node.right_bbox_bottom  = (double)((int16_t)two_bytes(buffer, offset + 10));
    node.right_bbox_left    = (double)((int16_t)two_bytes(buffer, offset + 12));
    node.right_bbox_right   = (double)((int16_t)two_bytes(buffer, offset + 14));

    node.left_bbox_top      = (double)((int16_t)two_bytes(buffer, offset + 16));
    node.left_bbox_bottom   = (double)((int16_t)two_bytes(buffer, offset + 18));
    node.left_bbox_left     = (double)((int16_t)two_bytes(buffer, offset + 20));
    node.left_bbox_right    = (double)((int16_t)two_bytes(buffer, offset + 22));

    node.right_child_id     = two_bytes(buffer, offset + 24);
    node.left_child_id      = two_bytes(buffer, offset + 26);
    return node;
}

int wad_read_nodes(uint32_t map_lump_index, node_t **pp_nodes, uint32_t *n_nodes) {
    if (*pp_nodes) {
        printf("Error invalid things double pointer. (Base pointer should be NULL).\n");
        return -1;
    }

    filelump_t *lump = &lumps[map_lump_index + eNODES];

    if (strcmp(lump->name, "NODES")) {
        printf("Error invalid map_lump_index.");
        return -1;
    }

    *n_nodes = lump->size / WAD_NODE_T_SIZE;
    *pp_nodes = malloc(*n_nodes * sizeof **pp_nodes);

    for (uint32_t i = 0; i < *n_nodes; i++) {
        (*pp_nodes)[i] = read_node(wad_buffer, lump->filepos + i * WAD_NODE_T_SIZE);
    }
    return 0;
}


static sector_t read_sector(char *buffer, uint32_t offset) {
    sector_t sector = {0};
    sector.floor_height     = (double)((int16_t)two_bytes(buffer, offset + 0));
    sector.ceiling_height   = (double)((int16_t)two_bytes(buffer, offset + 2));
    strncpy(sector.floor_name, buffer + offset + 4, 8);
    sector.floor_name[8] = '\0';
    strncpy(sector.ceiling_name, buffer + offset + 12, 8);
    sector.ceiling_name[8] = '\0';
    sector.light_level      = two_bytes(buffer, offset + 20);
    sector.special_type     = two_bytes(buffer, offset + 22);
    sector.tag_number       = two_bytes(buffer, offset + 24);
    return sector;
}

int wad_read_sectors(uint32_t map_lump_index, sector_t **pp_sectors, uint32_t *n_sectors) {
    if (*pp_sectors) {
        printf("Error invalid things double pointer. (Base pointer should be NULL).\n");
        return -1;
    }

    filelump_t *lump = &lumps[map_lump_index + eSECTORS];

    if (strcmp(lump->name, "SECTORS")) {
        printf("Error invalid map_lump_index.");
        return -1;
    }

    *n_sectors = lump->size / WAD_SECTOR_T_SIZE;
    *pp_sectors = malloc(*n_sectors * sizeof **pp_sectors);

    for (uint32_t i = 0; i < *n_sectors; i++) {
        (*pp_sectors)[i] = read_sector(wad_buffer, lump->filepos + i * WAD_SECTOR_T_SIZE);
    }
    return 0;
}


static color_t read_color(char *buffer, uint32_t offset) {
    color_t color = {0};
    color.r = (uint8_t)buffer[offset++];
    color.g = (uint8_t)buffer[offset++];
    color.b = (uint8_t)buffer[offset++];
    color.a = 255;
    return color;
}

static palette_t read_palette(char *buffer, uint32_t offset) {
    palette_t palette = {0};
    for (uint32_t i = 0; i < N_PALETTE_COLORS; i++) {
        palette.colors[i] = read_color(buffer, offset + i * WAD_COLOR_T_SIZE);
    }
    return palette;
}

int wad_read_palettes(palette_t **pp_palettes, uint32_t *n_palettes) {
    if (*pp_palettes) {
        printf("Error invalid pallete double pointer. (Base pointer should be NULL).\n");
        return -1;
    }

    int palletes_lump_index = wad_lump_index_by_name("PLAYPAL");

    filelump_t *lump = &lumps[palletes_lump_index];

    if (strcmp(lump->name, "PLAYPAL")) {
        printf("Error: loading PLAYPAL");
        return -1;
    }

    *n_palettes = N_PALETTES;
    *pp_palettes = malloc(*n_palettes * sizeof **pp_palettes);

    for (uint32_t i = 0; i < *n_palettes; i++) {
        (*pp_palettes)[i] = read_palette(wad_buffer, lump->filepos + i * WAD_PALETTE_T_SIZE);
    }

    return 0;
}