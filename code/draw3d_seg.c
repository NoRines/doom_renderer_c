#include "draw3d_seg.h"
#include "map.h"
#include "sdl_help.h"
#include <assert.h>

static SDL_Renderer *prenderer = NULL;

static int32_t render_size_x = 0;
static int32_t render_size_y = 0;
static double half_size_x = 0.0;
static double half_size_y = 0.0;
static double dist_to_screen = 0.0;

static angle_t *angle_lookup_from_column_index = NULL;

/* Clipping state */
/************************************************************/
typedef struct {
    int32_t first;
    int32_t last;
} cliprange_t;

static cliprange_t *newend = NULL;
static cliprange_t *solidsegs = NULL;
static uint32_t max_solidsegs = 0;

static int32_t *clip_upper = NULL;
static int32_t *clip_lower = NULL;
/************************************************************/

/* Seg context */
/************************************************************/
static vec2_t p1 = {0};
static vec2_t p2 = {0};
static angle_t ang1 = 0;
static angle_t ang2 = 0;

static player_t *pplayer = NULL;

static seg_t *pseg = NULL;
static linedef_t *plinedef = NULL;
static sidedef_t *pright_sidedef = NULL;
static sidedef_t *pleft_sidedef = NULL;
static sector_t *pright_sector = NULL;
static sector_t *pleft_sector = NULL;
/************************************************************/

/* Render context */
/************************************************************/
static double ceiling1 = 0;
static double ceiling2 = 0;
static double floor1 = 0;
static double floor2 = 0;
static double ceiling_step = 0;
static double floor_step = 0;

static bool should_draw_upper_section = false;
static bool should_draw_lower_section = false;
static bool should_update_upper_clip = false;
static bool should_update_lower_clip = false;

static double upper1 = 0;
static double upper2 = 0;
static double lower1 = 0;
static double lower2 = 0;
static double upper_step = 0;
static double lower_step = 0;
/************************************************************/

//float classicDoomScreenXtoView[] = {
//  45.043945f, 44.824219f, 44.648437f, 44.472656f, 44.296875f, 44.121094f, 43.945312f, 43.725586f, 43.549805f, 43.374023f, 43.154297f, 42.978516f, 42.802734f, 42.583008f, 42.407227f, 42.187500f, 42.011719f, 41.791992f, 41.616211f, 41.396484f,
//  41.220703f, 41.000977f, 40.781250f, 40.605469f, 40.385742f, 40.166016f, 39.946289f, 39.770508f, 39.550781f, 39.331055f, 39.111328f, 38.891602f, 38.671875f, 38.452148f, 38.232422f, 38.012695f, 37.792969f, 37.573242f, 37.353516f, 37.133789f,
//  36.870117f, 36.650391f, 36.430664f, 36.210937f, 35.947266f, 35.727539f, 35.507812f, 35.244141f, 35.024414f, 34.760742f, 34.541016f, 34.277344f, 34.057617f, 33.793945f, 33.530273f, 33.310547f, 33.046875f, 32.783203f, 32.519531f, 32.299805f,
//  32.036133f, 31.772461f, 31.508789f, 31.245117f, 30.981445f, 30.717773f, 30.454102f, 30.190430f, 29.926758f, 29.663086f, 29.355469f, 29.091797f, 28.828125f, 28.564453f, 28.256836f, 27.993164f, 27.729492f, 27.421875f, 27.158203f, 26.850586f,
//  26.586914f, 26.279297f, 26.015625f, 25.708008f, 25.444336f, 25.136719f, 24.829102f, 24.521484f, 24.257812f, 23.950195f, 23.642578f, 23.334961f, 23.027344f, 22.719727f, 22.412109f, 22.104492f, 21.796875f, 21.489258f, 21.181641f, 20.874023f,
//  20.566406f, 20.258789f, 19.951172f, 19.643555f, 19.291992f, 18.984375f, 18.676758f, 18.325195f, 18.017578f, 17.709961f, 17.358398f, 17.050781f, 16.699219f, 16.391602f, 16.040039f, 15.732422f, 15.380859f, 15.073242f, 14.721680f, 14.370117f,
//  14.062500f, 13.710937f, 13.359375f, 13.051758f, 12.700195f, 12.348633f, 11.997070f, 11.645508f, 11.337891f, 10.986328f, 10.634766f, 10.283203f, 9.931641f, 9.580078f, 9.228516f, 8.876953f, 8.525391f, 8.173828f, 7.822266f, 7.470703f, 7.119141f,
//  6.767578f, 6.416016f, 6.064453f, 5.712891f, 5.361328f, 5.009766f, 4.658203f, 4.306641f, 3.955078f, 3.559570f, 3.208008f, 2.856445f, 2.504883f, 2.153320f, 1.801758f, 1.450195f, 1.054687f, 0.703125f, 0.351562f, 0.000000f, 359.648437f, 359.296875f,
//  358.945312f, 358.549805f, 358.198242f, 357.846680f, 357.495117f, 357.143555f, 356.791992f, 356.440430f, 356.044922f, 355.693359f, 355.341797f, 354.990234f, 354.638672f, 354.287109f, 353.935547f, 353.583984f, 353.232422f, 352.880859f, 352.529297f,
//  352.177734f, 351.826172f, 351.474609f, 351.123047f, 350.771484f, 350.419922f, 350.068359f, 349.716797f, 349.365234f, 349.013672f, 348.662109f, 348.354492f, 348.002930f, 347.651367f, 347.299805f, 346.948242f, 346.640625f, 346.289062f, 345.937500f,
//  345.629883f, 345.278320f, 344.926758f, 344.619141f, 344.267578f, 343.959961f, 343.608398f, 343.300781f, 342.949219f, 342.641601f, 342.290039f, 341.982422f, 341.674805f, 341.323242f, 341.015625f, 340.708008f, 340.356445f, 340.048828f, 339.741211f,
//  339.433594f, 339.125977f, 338.818359f, 338.510742f, 338.203125f, 337.895508f, 337.587891f, 337.280273f, 336.972656f, 336.665039f, 336.357422f, 336.049805f, 335.742187f, 335.478516f, 335.170898f, 334.863281f, 334.555664f, 334.291992f, 333.984375f,
//  333.720703f, 333.413086f, 333.149414f, 332.841797f, 332.578125f, 332.270508f, 332.006836f, 331.743164f, 331.435547f, 331.171875f, 330.908203f, 330.644531f, 330.336914f, 330.073242f, 329.809570f, 329.545898f, 329.282227f, 329.018555f, 328.754883f,
//  328.491211f, 328.227539f, 327.963867f, 327.700195f, 327.480469f, 327.216797f, 326.953125f, 326.689453f, 326.469727f, 326.206055f, 325.942383f, 325.722656f, 325.458984f, 325.239258f, 324.975586f, 324.755859f, 324.492187f, 324.272461f, 324.052734f,
//  323.789062f, 323.569336f, 323.349609f, 323.129883f, 322.866211f, 322.646484f, 322.426758f, 322.207031f, 321.987305f, 321.767578f, 321.547852f, 321.328125f, 321.108398f, 320.888672f, 320.668945f, 320.449219f, 320.229492f, 320.053711f, 319.833984f,
//  319.614258f, 319.394531f, 319.218750f, 318.999023f, 318.779297f, 318.603516f, 318.383789f, 318.208008f, 317.988281f, 317.812500f, 317.592773f, 317.416992f, 317.197266f, 317.021484f, 316.845703f, 316.625977f, 316.450195f, 316.274414f, 316.054687f,
//  315.878906f, 315.703125f, 315.527344f, 315.351562f, 315.175781f, 314.956055f };

void draw3d_seg_quit() {
    free(angle_lookup_from_column_index);
    angle_lookup_from_column_index = NULL;

    free(solidsegs);
    solidsegs = NULL;

    free(clip_upper);
    clip_upper = NULL;
    free(clip_lower);
    clip_lower = NULL;
}

void draw3d_seg_init() {
    draw3d_seg_quit();

    prenderer = sdl_help_renderer();

    SDL_RenderGetLogicalSize(prenderer, &render_size_x, &render_size_y);

    half_size_x = (double)render_size_x / 2.0;
    half_size_y = (double)render_size_y / 2.0;
    dist_to_screen = half_size_x / tan(HALF_FOV);

    angle_lookup_from_column_index = calloc(render_size_x, sizeof *angle_lookup_from_column_index);
    for (int32_t i = 0; i < render_size_x; i++) {
        angle_t a = angle_normalize(atan((double)(half_size_x - i) / (double)(half_size_x)));
        //angle_lookup_from_column_index[i] = classicDoomScreenXtoView[i] * ANG1;
        angle_lookup_from_column_index[i] = a;
    }


    max_solidsegs = (render_size_x / 2) + 1;
    solidsegs = calloc(max_solidsegs, sizeof *solidsegs);

    clip_upper = calloc(render_size_x, sizeof *clip_upper);
    clip_lower = calloc(render_size_x, sizeof *clip_lower);
}

static int32_t angle_to_screen_x(angle_t angle) {
    double x;
    angle = angle_sub(angle, ANG90);
    double t = tan(angle);
    x = t * half_size_x;
    x = half_size_x - x - 0.01;
    return (int32_t)ceil(x);
}



static bool clip_column(int32_t x, int32_t *py_start, int32_t *py_end) {
    if (*py_start < clip_upper[x] + 1) {
        *py_start = clip_upper[x] + 1;
    }

    if (*py_end >= clip_lower[x]) {
        *py_end = clip_lower[x] - 1;
    }

    if (*py_end < *py_start) {
        return false;
    }
    return true;
}

static void middle_section(int32_t x, int32_t y_start, int32_t y_end) {
    SDL_RenderDrawLine(prenderer, x, y_start, x, y_end);
    clip_upper[x] = render_size_y;
    clip_lower[x] = -1;
}

static void upper_section(int32_t x, int32_t y_start, double *upper) {
    if (should_draw_upper_section) {
        int32_t upper_height = *upper;
        *upper += upper_step;

        if (upper_height >= clip_lower[x]) {
            upper_height = clip_lower[x] - 1;
        }

        if (upper_height >= y_start) {
            SDL_RenderDrawLine(prenderer, x, y_start, x, upper_height);
            clip_upper[x] = upper_height;
        } else {
            clip_upper[x] = y_start - 1;
        }

    } else if (should_update_upper_clip) {
        clip_upper[x] = y_start - 1;
    }
}

static void lower_section(int32_t x, int32_t y_end, double *lower) {
    if (should_draw_lower_section) {
        int32_t lower_height = *lower;
        *lower += lower_step;

        if (lower_height <= clip_upper[x]) {
            lower_height = clip_upper[x] + 1;
        }

        if (lower_height <= y_end) {
            SDL_RenderDrawLine(prenderer, x, lower_height, x, y_end);
            clip_lower[x] = lower_height;
        } else {
            clip_lower[x] = y_end + 1;
        }

    } else if (should_update_lower_clip) {
        clip_lower[x] = y_end + 1;
    }
}

static void render_loop_seg(int32_t x1, int32_t x2) {
    int32_t x = x1;
    double ceiling = ceiling1;
    double floor = floor1;

    double upper = upper1;
    double lower = lower1;

    int32_t y_start;
    int32_t y_end;

    while (x <= x2) {
        y_start = ceiling;
        y_end = floor;

        if (clip_column(x, &y_start, &y_end)) {
            if (pleft_sector == NULL) { // Has no backside
                middle_section(x, y_start, y_end);
            } else { // Has backside
                upper_section(x, y_start, &upper);
                lower_section(x, y_end, &lower);
            }
        }

        x++;
        ceiling += ceiling_step;
        floor += floor_step;
    }
}

static bool seg_is_solid() {
    if (pright_sector != NULL && pleft_sector == NULL) {
        return true;
    }
    if (pright_sector->ceiling_height <= pleft_sector->floor_height) {
        return true;
    }
    if (pright_sector->floor_height >= pleft_sector->ceiling_height) {
        return true;
    }
    return false;
}

#define MAX_SCALEFACTOR 64.0
#define MIN_SCALEFACTOR 0.00390625

static double calculate_scale_factor(int32_t x, double dist_to_normal, angle_t seg_normal_angle) {
    assert(x >= 0 && x <= render_size_x);
    angle_t s_angle = angle_lookup_from_column_index[x];
    angle_t x_angle = angle_add(pplayer->angle, s_angle);
    angle_t skew_angle = angle_shortdist(x_angle, seg_normal_angle);

    if (skew_angle > ANG90) {
        skew_angle = ANG90;
    }

    double s_angle_cos = cos(s_angle);
    double skew_angle_cos = cos(skew_angle);

    double scale_factor = (dist_to_screen * skew_angle_cos) / (s_angle_cos * dist_to_normal);

    if (scale_factor > MAX_SCALEFACTOR) {
        return MAX_SCALEFACTOR;
    } else if (scale_factor < MIN_SCALEFACTOR) {
        return MIN_SCALEFACTOR;
    }

    return scale_factor;
}

static void store_current_seg(int32_t x1, int32_t x2) {
    angle_t seg_normal_angle = angle_add(pseg->angle, ANG90);

    angle_t theta = angle_shortdist(ang1, seg_normal_angle);
    if (theta > ANG90) {
        theta = ANG90;
    }

    // vec2_t to_p1 = pplayer->pos;
    // vec2_sub(&to_p1, p1);
    // double p1_dist = vec2_magnitude(to_p1);
    double p1_dist = player_dist_to_point(pplayer, p1.x, p1.y);

    double dist_to_normal = cos(theta) * p1_dist;

    double scale1;
    double scale2;
    scale1 = calculate_scale_factor(x1, dist_to_normal, seg_normal_angle);
    if (x2 > x1) {
        scale2 = calculate_scale_factor(x2, dist_to_normal, seg_normal_angle);
    } else {
        scale2 = scale1;
    }

    sector_t *sector = pright_sector;
    double rel_ceiling1 = sector->ceiling_height - pplayer->eye_height;
    double rel_floor1 = sector->floor_height - pplayer->eye_height;

    ceiling1 = half_size_y - (scale1 * rel_ceiling1);
    ceiling2 = half_size_y - (scale2 * rel_ceiling1);
    floor1 = half_size_y - (scale1 * rel_floor1);
    floor2 = half_size_y - (scale2 * rel_floor1);

    ceiling_step = (ceiling1 - ceiling2) / (double)(x1 - x2);
    floor_step = (floor1 - floor2) / (double)(x1 - x2);

    if (pleft_sector == NULL) {
        render_loop_seg(x1, x2);
    } else {
        should_update_upper_clip = pleft_sector->ceiling_height != pright_sector->ceiling_height; // TODO: double double comparison fix
        should_update_lower_clip = pleft_sector->floor_height != pright_sector->floor_height;

        if (seg_is_solid()) {
            should_update_upper_clip = true;
            should_update_lower_clip = true;
        }

        if (pright_sector->ceiling_height <= pplayer->eye_height) {
            should_update_upper_clip = false;
        }
        if (pright_sector->floor_height >= pplayer->eye_height) {
            should_draw_lower_section = false;
        }

        double rel_ceiling2 = pleft_sector->ceiling_height - pplayer->eye_height;
        double rel_floor2 = pleft_sector->floor_height - pplayer->eye_height;

        should_draw_lower_section = false;
        should_draw_upper_section = false;

        if (rel_ceiling2 < rel_ceiling1) { // We can see the upper section
            if (rel_ceiling2 < rel_floor1) { // (UPPER_OVERFLOW) Fixes if upper overflows down past the floor.
                rel_ceiling2 = rel_floor1;
            }
            should_draw_upper_section = true;
            upper1 = half_size_y - (scale1 * rel_ceiling2);
            upper2 = half_size_y - (scale2 * rel_ceiling2);
            upper_step = (upper1 - upper2) / (double)(x1 - x2);
        }
        if (rel_floor2 > rel_floor1) { // We can see the lower section
            //if (rel_floor1 > rel_ceiling2) { // Not sure if this fixes the opposite to (UPPER_OVERFLOW).
            //    rel_floor1 = rel_ceiling2;
            //}
            should_draw_lower_section = true;
            lower1 = half_size_y - (scale1 * rel_floor2);
            lower2 = half_size_y - (scale2 * rel_floor2);
            lower_step = (lower1 - lower2) / (double)(x1 - x2);
        }
        render_loop_seg(x1, x2);
    }
}

void draw3d_seg_clear_solidsegs() {
    solidsegs[0].first = INT32_MIN;
    solidsegs[0].last = -1;
    solidsegs[1].first = render_size_x;
    solidsegs[1].last = INT32_MAX;
    newend = solidsegs + 2;
}

void draw3d_seg_reset_vertical_clip() {
    for (int32_t i = 0; i < render_size_x; i++) {
        clip_upper[i] = -1;
        clip_lower[i] = render_size_y;
    }
}

static void clip_solid_wall_segment(int32_t first, int32_t last) {
    cliprange_t *next;
    cliprange_t *start;

    start = solidsegs;
    while (start->last < first - 1) {
        start++;
    }

    if (first < start->first) {
        if (last < start->first - 1) {
            store_current_seg(first, last);
            next = newend;
            newend++;

            while (next != start) {
                *next = *(next - 1);
                next--;
            }
            next->first = first;
            next->last = last;
            return;
        }

        store_current_seg(first, start->first - 1);
        start->first = first;
    }

    if (last <= start->last) {
        return;
    }

    next = start;
    while (last >= (next + 1)->first - 1) {
        store_current_seg(next->last + 1, (next + 1)->first - 1);
        next++;

        if (last <= next->last) {
            start->last = next->last;

            if (next != start) {
                while (next++ != newend) {
                    *++start = *next;
                }
                newend = start;
                return;
            }
        }
    }

    store_current_seg(next->last + 1, last);
    start->last = last;

    if (next != start) {
        while (next++ != newend) {
            *++start = *next;
        }
        newend = start;
    }
}

static void clip_pass_wall_segment(int32_t first, int32_t last) {
    cliprange_t *next;
    cliprange_t *start;

    start = solidsegs;
    while (start->last < first - 1) {
        start++;
    }

    if (first < start->first) {
        if (last < start->first - 1) {
            store_current_seg(first, last);
            return;
        }

        store_current_seg(first, start->first - 1);
    }

    if (last <= start->last) {
        return;
    }

    next = start;
    while (last >= (next + 1)->first - 1) {
        store_current_seg(next->last + 1, (next + 1)->first - 1);
        next++;

        if (last <= next->last) {
            if (next != start) {
                return;
            }
        }
    }

    store_current_seg(next->last + 1, last);
}

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} color_t;

static color_t get_random_color(int seed) {
    srand(seed);
    color_t c;
    c.r = rand() % 255;
    c.g = rand() % 255;
    c.b = rand() % 255;
    c.a = 255;
    return c;
}

void draw3d_seg_plot(seg_t *_pseg, player_t *_pplayer) {
    pseg = _pseg;
    pplayer = _pplayer;
    vertex_t *vert1 = &map_vertexes[pseg->start_vertex_id];
    vertex_t *vert2 = &map_vertexes[pseg->end_vertex_id];

    p1 = (vec2_t){vert1->x_pos, vert1->y_pos};
    p2 = (vec2_t){vert2->x_pos, vert2->y_pos};

    ang1 = player_angle_to_point(pplayer, p1);
    ang2 = player_angle_to_point(pplayer, p2);

    angle_t clip_ang1 = ang1;
    angle_t clip_ang2 = ang2;

    if (!player_clip_angle_to_fov(pplayer, &clip_ang1, &clip_ang2)) {
        return;
    }

    int32_t x1 = angle_to_screen_x(clip_ang1);
    int32_t x2 = angle_to_screen_x(clip_ang2);

    if (x1 == x2) {
        return;
    }
    x2 -= 1;

    plinedef = &map_linedefs[pseg->linedef_id];

    pright_sidedef = (plinedef->right_sidedef_id != 0xFFFF) ? 
        &map_sidedefs[plinedef->right_sidedef_id] : NULL;
    pleft_sidedef = (plinedef->left_sidedef_id != 0xFFFF) ? 
        &map_sidedefs[plinedef->left_sidedef_id] : NULL;

    if (pseg->direction) {
        /* Swap left and right sidedefs */
        sidedef_t *tmp = pright_sidedef;
        pright_sidedef = pleft_sidedef;
        pleft_sidedef = tmp;
    }

    pright_sector = pright_sidedef != NULL ?
        &map_sectors[pright_sidedef->sector] : NULL;
    pleft_sector = pleft_sidedef != NULL ?
        &map_sectors[pleft_sidedef->sector] : NULL;

    color_t c = get_random_color(pseg->linedef_id);
    SDL_SetRenderDrawColor(prenderer, c.r, c.g, c.b, c.a);

    if (seg_is_solid()) {
        clip_solid_wall_segment(x1, x2);
    } else {
        clip_pass_wall_segment(x1, x2);
    }
}

