#pragma once

#include <stdint.h>

/* En angle ska vara mellan 0-360 grader i normaliserat tillstånd */
/* Radianer kommer att användas för att representera angles för enkelhetens skull */

typedef double angle_t;

#define PI 3.14159265359

#define ANG1 PI / 180.0
#define ANG45 PI / 4.0
#define ANG90 PI / 2.0
#define ANG180 PI
#define ANG270 3.0 * PI / 2.0
#define MAX_ANGLE 2.0 * PI



angle_t angle_normalize(angle_t angle);
angle_t angle_add(angle_t lhs, angle_t rhs);
angle_t angle_sub(angle_t lhs, angle_t rhs);
angle_t angle_shortdist(angle_t lhs, angle_t rhs);
angle_t angle_from_bam(uint32_t bam);


