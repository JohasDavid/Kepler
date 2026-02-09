#include<SDL2/SDL.h>
#include<stdio.h>
#include<math.h>

#define WIDTH 900
#define HEIGHT 600

#define COLOR_WHITE 0xffffff
#define COLOR_BLACK 0x000000
#define RAYS_NUMBER 300
#define COLOR_GRAY 0xFFF1A8 //amarillo
#define G 10000


struct Ray{
    double x_start, y_start;
    double angle;
};

struct vector{
    double x;
    double y;
};

struct vector homotecia(struct vector v, double a){
    struct vector b = {a * (v.x), a * (v.y)};
    return b;
}

double cdot(struct vector v1, struct vector v2){
    return ( v1.x ) * ( v2.x ) + ( v1.y ) * ( v2.y );
}

struct Circle{
    double x;
    double y;
    double r;
    double vx;
    double vy;
    double ax;
    double ay;
};

void UpdateCircle(struct Circle *circle, struct Circle *source){
    struct vector X = {1, 0};
    struct vector Y = {0 ,1};

    struct vector nn = {-(circle->x - source->x), -(circle->y - source->y)};
    double rr = cdot(nn, nn);
    double r = pow(rr, 3.0/2.0);
    struct vector F = homotecia(nn, G * ((1.0) / r));

    circle->ax = cdot(F,X);
    circle->ay = cdot(F,Y);

    circle->vx += circle->ax;
    circle->vy += circle->ay;

    circle->x += circle->vx;
    circle->y += circle->vy;

}


void FillRays(SDL_Surface* surface, struct Ray rays[RAYS_NUMBER], Uint32 color, struct Circle shadow){

    double check_r = ( shadow.r ) * ( shadow.r );
    double check_x = shadow.x;
    double check_y = shadow.y;

    for(int i = 0; i < RAYS_NUMBER; i++){
        double c = cos(rays[i].angle); 
        double s = sin(rays[i].angle);
        double x = rays[i].x_start;
        double y = rays[i].y_start;
        
        double j = 0;
        double check = pow( (x - check_x), 2 ) + pow( (y - check_y), 2 );

        while((x <= WIDTH) && (x >= 0) && (y >= 0) && (y <= HEIGHT) && (check > check_r) ){
            x += (j * c) / 200;
            y += (j * s) / 200;
            check = pow( (x - check_x), 2 ) + pow( (y - check_y), 2 );

            SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
            SDL_FillRect(surface, &pixel, color);

            j += 0.5;
        }
    }
}

void generate_rays(struct Circle circle, struct Ray rays[RAYS_NUMBER]){
    for(int i = 0; i < RAYS_NUMBER; i++){
        double angle = (((2 * M_PI) * i) / (RAYS_NUMBER));
        struct Ray ray ={circle.x, circle.y, angle};
        rays[i] = ray;
    }
}

void FillCircle(SDL_Surface* surface, struct Circle circle, Uint32 color){
    for(double x = circle.x - circle.r; x <= circle.x + circle.r; x++){
        for(double y = circle.y - circle.r; y <= circle.y + circle.r; y++){

            double distance_squared = pow( (x - circle.x ), 2) + pow( ( y - circle.y ), 2);
            double r_squared = circle.r * circle.r;
            if(distance_squared < r_squared){
                SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }

}

int main(){
    
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Window *window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    struct Circle circle = {.x = 200, .y = 200, .r = 10};
    struct Circle shadow_circle = {500, 80, 5, 3, 3, 0, 0};

    SDL_Rect erase_rect = (SDL_Rect){0, 0, WIDTH, HEIGHT};

    struct Ray rays[RAYS_NUMBER];
    generate_rays(circle, rays);
    SDL_Event e;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT){
                running = 0;
            }
        }

        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
        FillCircle(surface, circle, COLOR_GRAY);
        FillCircle(surface, shadow_circle, COLOR_WHITE);
        FillRays(surface, rays, COLOR_GRAY, shadow_circle);

        SDL_UpdateWindowSurface(window);
        UpdateCircle(&shadow_circle, &circle);
        SDL_Delay(16);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}