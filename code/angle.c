#include "angle.h"

#include <math.h>


angle_t angle_normalize(angle_t angle) {
    while (angle >= MAX_ANGLE) {
        angle -= MAX_ANGLE;
    }
    while (angle < 0.0) {
        angle += MAX_ANGLE;
    }
    return angle;
}

angle_t angle_add(angle_t lhs, angle_t rhs) {
    angle_t result = lhs + rhs;

    while (result >= MAX_ANGLE) {
        result -= MAX_ANGLE;
    }
    while (result < 0.0) {
        result += MAX_ANGLE;
    }

    return result;
}

angle_t angle_sub(angle_t lhs, angle_t rhs) {
    angle_t result = lhs - rhs;

    while (result >= MAX_ANGLE) {
        result -= MAX_ANGLE;
    }
    while (result < 0.0) {
        result += MAX_ANGLE;
    }

    return result;
}

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

angle_t angle_shortdist(angle_t lhs, angle_t rhs) {
    angle_t a = angle_sub(lhs, rhs);
    angle_t b = angle_sub(rhs, lhs);
    return MIN(a, b);
}

#define BAM_FACTOR 8.38190317e-8

angle_t angle_from_bam(uint32_t bam) {
    angle_t degrees = (angle_t)bam * BAM_FACTOR;
    return degrees * ANG1;
}
