#pragma once

#include "angle.h"
#include <stdint.h>

typedef struct _thing_t
{
    double x_pos;
    double y_pos;
    angle_t facing;
    uint16_t type;
    uint16_t flags;
} thing_t;

#define WAD_THING_T_SIZE 10


typedef struct _linedef_t
{
    uint16_t start_vertex_id;
    uint16_t end_vertex_id;
    uint16_t flags;
    uint16_t special_type;
    uint16_t sector_tag;
    uint16_t right_sidedef_id;
    uint16_t left_sidedef_id;
} linedef_t;

#define WAD_LINEDEF_T_SIZE 14


typedef struct _sidedef_t
{
    int16_t x_offset;
    int16_t y_offset;
    char upper_name[9];
    char lower_name[9];
    char middle_name[9];
    uint16_t sector;
} sidedef_t;

#define WAD_SIDEDEF_T_SIZE 30


typedef struct _vertex_t
{
    double x_pos;
    double y_pos;
} vertex_t;

#define WAD_VERTEX_T_SIZE 4


typedef struct _seg_t
{
    uint16_t start_vertex_id;
    uint16_t end_vertex_id;
    angle_t angle;
    uint16_t linedef_id;
    uint16_t direction;
    int16_t offset;
} seg_t;

#define WAD_SEG_T_SIZE 12

typedef struct _ssector_t
{
    uint16_t seg_count;
    uint16_t first_seg_id;
} ssector_t;

#define WAD_SSECTOR_T_SIZE 4


typedef struct _node_t
{
    double x_part;
    double y_part;
    double dx;
    double dy;

    double right_bbox_top;
    double right_bbox_bottom;
    double right_bbox_left;
    double right_bbox_right;

    double left_bbox_top;
    double left_bbox_bottom;
    double left_bbox_left;
    double left_bbox_right;

    uint16_t right_child_id;
    uint16_t left_child_id;
} node_t;

#define WAD_NODE_T_SIZE 28
#define SUBSECTOR_IDENTIFIER 0x8000


typedef struct _sector_t
{
    double floor_height;
    double ceiling_height;
    char floor_name[9];
    char ceiling_name[9];
    int16_t light_level;
    uint16_t special_type;
    uint16_t tag_number;
} sector_t;

#define WAD_SECTOR_T_SIZE 26

typedef enum {
    eTHINGS = 1,
    eLINEDEFS,
    eSIDEDEFS,
    eVERTEXES,
    eSEGS,
    eSSECTORS,
    eNODES,
    eSECTORS
} wadtypeoffset_t;
