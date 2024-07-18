#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define steps 30
#define hold 30
#define blast_duration 5

#include "GL_tty.h"

void render(int x, int y) {
    GL_setpixelRGB(x, y, 255, 0, 0);
    GL_setpixelRGB(x-2, y+1, 255, 255, 255);
    GL_setpixelRGB(x-2, y-1, 255, 255, 255);
    GL_setpixelRGB(x-2, y+2, 255, 255, 255);
    GL_setpixelRGB(x-2, y-2, 255, 255, 255);
    GL_setpixelRGB(x+2, y+1, 255, 255, 255);
    GL_setpixelRGB(x+2, y-1, 255, 255, 255);
    GL_setpixelRGB(x+2, y+2, 255, 255, 255);
    GL_setpixelRGB(x+2, y-2, 255, 255, 255);
    GL_setpixelRGB(x+1, y-2, 255, 255, 255);
    GL_setpixelRGB(x-1, y+2, 255, 255, 255);
    GL_setpixelRGB(x-1, y-2, 255, 255, 255);
    GL_setpixelRGB(x+1, y+2, 255, 255, 255);
}

void blast(int intensity, int x, int y)
{
    for (int i = -intensity; i <= intensity; i++) {
        for(int j = -intensity; j <= intensity; j++) {
            if (i*i + j*j <= (intensity-1)*(intensity-1)) 
            {
                int px = x + i;
                int py = y + j;
                GL_setpixelRGB(px, py, 200, 200, 30);
            }
            else if (i*i + j*j <= intensity*intensity) 
            {
                int px = x + i;
                int py = y + j;
                GL_setpixelRGB(px, py, 255, 40, 50);
            }
        }
    }
}

int main() {
    srand(time(NULL));
    GL_init();

    int x = GL_width / 2;
    int y = GL_height / 2;
    int target_x = x;
    int target_y = y;

    int n = 0;
    int shift = 0;

    while(1){
        if (n == 0) {
            if (shift) {
                shift = 0;
                n = steps;
            } else {
                shift = 1;
                n = hold;
                target_x = 1 + rand() % (2 - (GL_width - 3));
                target_y = 1 + rand() % (2 - (GL_height - 3));
            }
        }

        if (shift) {
            if (x < target_x) x++;
            else if (x > target_x) x--;
            if (y < target_y) y++;
            else if (y > target_y) y--;
        }
        

        GL_clear();
        if (n <= blast_duration+20 && !shift && n > 20){
            blast(blast_duration - abs(n-(blast_duration+20)), x, y);
        }
        render(x, y);
        GL_swapbuffers();

        #ifdef BIGCPU
        usleep(1000000 / GL_FPS);
        #endif

        n--;
    }
    GL_terminate();
}