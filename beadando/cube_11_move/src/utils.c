#include "utils.h"
#include <math.h> // Ensure this is here for sqrtf and M_PI

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double degree_to_radian(double degree)
{
	return degree * M_PI / 180.0;
}

vec3 vec3_add(vec3 a, vec3 b) {
    vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

vec3 vec3_subtract(vec3 a, vec3 b) {
    vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

vec3 vec3_scale(vec3 v, float s) {
    vec3 result = {v.x * s, v.y * s, v.z * s};
    return result;
}

float vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vec3_length(vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 vec3_normalize(vec3 v) {
    float l = vec3_length(v);
    vec3 result = {0.0f, 0.0f, 0.0f};
    // Check for zero length to avoid division by zero
    if (l > 1e-6f) { // Use a small epsilon
        result.x = v.x / l;
        result.y = v.y / l;
        result.z = v.z / l;
    }
    return result;
}

float vec3_distance(vec3 a, vec3 b) {
    vec3 diff = vec3_subtract(a, b);
    return vec3_length(diff);
}