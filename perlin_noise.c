/*
    Computes and displays a randomized 60*60 perlin noise map
    https://adrianb.io/2014/08/09/perlinnoise.html
    https://mrl.cs.nyu.edu/~perlin/noise/
*/

#include <stdio.h>
#include <time.h>
#include <math.h>
#include "GL_tty.h"

#define H 64
#define W 64

#define NOISE_SCALE 0.15
#define OCTAVES 10
#define PERSISTENCE 0.5

static int grad2d[8][2] = {
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

static int perm_table[512];

void init_perm_table(int* perm_table)
{
    srand(time(NULL));
    for (int i = 0; i < 256; i++) {
        perm_table[i] = i;
    }
    int temp;
    for(int i = 0; i < 255; i++)
    {
        int j = rand() % (i + 1);
        temp = perm_table[i];
        perm_table[i] = perm_table[j];
        perm_table[j] = temp;
        perm_table[i+256] = perm_table[i];
    }
}

static inline float lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

static inline float dot(int* g, float x, float y)
{
    return g[0] * x + g[1] * y;
}

static inline float fade(float t) 
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float perlin_noise(float x, float y)
{
    int grid_x = (int)floor(x) & 255;
    int grid_y = (int)floor(y) & 255;

    x -= floor(x);
    y -= floor(y);
    
    long u = fade(x);
    long v = fade(y);

    //Gradient Vectors
    int* g00 = grad2d[perm_table[grid_x + perm_table[grid_y]] % 8];
    int* g01 = grad2d[perm_table[grid_x + perm_table[grid_y + 1]] % 8];
    int* g10 = grad2d[perm_table[grid_x + 1 + perm_table[grid_y]] % 8];
    int* g11 = grad2d[perm_table[grid_x + 1 + perm_table[grid_y + 1]] % 8];

    //Influence Values
    float d00 = dot(g00, x, y);
    float d01 = dot(g01, x, y - 1);
    float d10 = dot(g10, x - 1, y);
    float d11 = dot(g11, x - 1, y - 1);

    float nx0 = lerp(u, d00, d10);
    float nx1 = lerp(u, d01, d11);
    return lerp(v, nx0, nx1);
}

float perlin_octave(float x, float y, int octaves, float persistence)
{
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;
    for (int i = 0; i < octaves; i++)
    {
        total += perlin_noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }
    return total/maxValue;
}  

void draw_noise(int x, int y, float* r, float* g, float* b) 
{
    float noise = perlin_octave(x * NOISE_SCALE, y * NOISE_SCALE, OCTAVES, PERSISTENCE);
    noise = (noise + 1) / 2;
    *b = *g = (float)noise;
    *r = 0;
 }

void generate_perlin_noise(int width, int height) 
{
    GL_init();
    GL_scan_RGBf(width, height, draw_noise);
    GL_swapbuffers();
}

int main()
{
    init_perm_table(perm_table);
    generate_perlin_noise(H,W);
    GL_terminate();
    return 0;
}
