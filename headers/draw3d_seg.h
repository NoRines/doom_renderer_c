#pragma once

#include "player.h"
#include "wad_types.h"

void draw3d_seg_quit();
void draw3d_seg_init();
void draw3d_seg_clear_solidsegs();
void draw3d_seg_reset_vertical_clip();
void draw3d_seg_plot(seg_t *pseg, player_t *pplayer);