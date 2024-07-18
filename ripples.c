/*
    Simulates water droplets w ANSI graphics inside the terminal
    Uses Bruno Levy's GL_tty header
*/

#include "GL_tty.h"
#include "ripples_ht.h"
#include <time.h>
#include <unistd.h>

#define DROPLET_RADIUS_PIXELS 5
#define FRAMES_BETWEEN_RIPPLES 10
#define FADE_RATE 0.05;

//Adjust relative to terminal
#define H GL_height
#define W GL_width

const int rand_blues[6][3] = {{0,22,252},{0,32,252},{0,32,242},
    {0,52,242},{0,62,232},{0,92,232}};

void create(tableEntry *cache[], int n, uint8_t color)
{
    for (int i = 0; i < n; i++) 
    {
        Ripple *n = get(i, cache);
        if (n == NULL) 
        {
            Ripple ripple;
            ripple.x = rand() % W;
            ripple.y = rand() % H;
            ripple.radius = 0;
            ripple.intensity = 1.0;
            ripple.color = color;
            store(i, ripple, cache);
            break;
        }
    }
}

void update(tableEntry *cache[], int n)
{
    for(int i = 0; i < n; i++)
    {
        Ripple *ripple = get(i, cache);
        if(ripple != NULL)
        {
            ripple->radius += 0.5;
            ripple->intensity -= 0.05;
            if(ripple->intensity <= 0)
            {
                remove_entry(i, cache);
            }
        }
    }
}

void draw(int pos_x, int pos_y, float radius, float intensity, uint8_t color)
{   
    uint8_t R = rand_blues[color][0];
    uint8_t G = rand_blues[color][1];
    uint8_t B = rand_blues[color][2];
    G = G/2+(G/2*intensity);
    B = B/2+(B/2*intensity);

    float inner_radius = radius-1;

    for(int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++) 
        {
            //x^2 + y^2 = r^2
            if (x*x + y*y <= radius*radius && x*x + y*y >= inner_radius*inner_radius) 
            {
                int px = pos_x + x;
                int py = pos_y + y;
                if (px >= 0 && px < W && py >= 0 && py < H) {
                    GL_setpixelRGB(px, py, R, G, B);
                }
            }
        }
    }
}

void run(tableEntry *cache[], int n)
{
    GL_init();
    srand(time(NULL));

    int frames = 0;
    while(1)
    {
        GL_clear();
        for(int i = 0; i < n; i++)
        {
            Ripple *ripple = get(i, cache);
            if(ripple != NULL)
            {
                draw(ripple->x, ripple->y, ripple->radius, ripple->intensity, ripple->color);
            }
        }

        //Play around with this macro to adjust speed of generation
        //Also update MAX_INST accordingly if interval increases a lot
        if(frames % FRAMES_BETWEEN_RIPPLES == 0)
        {
            uint8_t color = rand() % 6;
            create(cache, n, color);
        }

        frames++;
        update(cache, n);
        GL_swapbuffers();
        usleep(1000000 / GL_FPS);
    }
}

int main()
{
    srand(time(NULL));
    tableEntry *nt[MAX_INST] = {NULL};
    run(nt, MAX_INST);
    for (int i = 0; i < MAX_INST; i++) 
    {
        if (nt[i] != NULL) 
        {
            free(nt[i]);
        }
    }
    GL_terminate();
    return 0;
}