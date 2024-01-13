#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))

#define MAXRULES 30
#define MAXDOTS 1000

#define LOOP(i, max) for(int i = 0; i < max; i++)
    
float clamp(float d, float min, float max) {
  const float t = d < min ? min : d;
  return t > max ? max : t;
}
    
const int screenWidth = 1280;
const int screenHeight = 720;

const int maxX = 1000, maxY = 600;

float colors = 6.f;
float num_dots = (float)MAXDOTS;

struct Color color_a[MAXRULES];
struct Color color_b[MAXRULES];
float g_values[MAXRULES];
int counter = 0;
int dot_counter = 0;

struct Colourdot{
    Color col;
    float x, y;
    float velx, vely;
};

struct Colourdot dots[MAXDOTS];

void drawDot(struct Colourdot* dot){
    DrawCircle((int)(dot->x), (int)(dot->y), 4, (Color)(dot->col));
}

void Gravity(struct Colourdot* dot, struct Colourdot* otherdot, float attraction){
    float fx = 0., fy = 0.;
    float dx = dot->x - otherdot->x;
    float dy = dot->y - otherdot->y;
    float dist = (float)sqrt(dx*dx + dy*dy);
    
    if(dist>0&&dist<80){
        dist = max(1.f, dist);
        float F = -attraction/(dist*dist);
        fx += F*dx;
        fy += F*dy;
    }
    dot->velx = (dot->velx + fx);
    dot->vely = (dot->vely + fy);
    otherdot->velx = (otherdot->velx + fx * -1);
    otherdot->vely = (otherdot->vely + fy * -1);
    dot->x += dot->velx * GetFrameTime() * 0.0125f;
    dot->y += dot->vely * GetFrameTime() * 0.0125f;
    otherdot->x += otherdot->velx * GetFrameTime() * 0.0125f;
    otherdot->y += otherdot->vely * GetFrameTime() * 0.0125f;
    if(dot->x <= 0 || dot->x >= maxX){
        dot->velx *= -1;
    }
    if(dot->y <= 0 || dot->y >= maxY){
        dot->vely *= -1;
    }
    if(otherdot->x <= 0 || otherdot->x >= maxX){
        otherdot->velx *= -1;
    }
    if(otherdot->y <= 0 || otherdot->y >= maxY){
        otherdot->vely *= -1;
    }
    dot->velx = clamp(dot->velx, -10, 10);
    dot->vely = clamp(dot->vely, -10, 10);
    otherdot->velx = clamp(otherdot->velx, -10, 10);
    otherdot->vely = clamp(otherdot->vely, -10, 10);
    
}

int NewRule(struct Color col_a, struct Color col_b, float g_val){
    int retval = 0;
    if(counter < MAXRULES){
        color_a[counter] = col_a;
        color_b[counter] = col_b;
        g_values[counter] = g_val;
        counter += 1;
        retval = counter - 1;
    }
    else {
        printf("Too many rules!\n");
        retval = -1;
    };
    return retval;
}

bool CmpCols(struct Color *a, struct Color *b){
    //printf("%d %d %d %d %d %d\n", a->r, b->r, a->g, b->g, a->b, b->b);
    return(a->r == b->r && a->g == b->g && a->b == b->b && a->a == b->a);
}

void NewDot(float x, float y, struct Color col){
    if(dot_counter < MAXDOTS){
        struct Colourdot a = {(Color)col, x, y, ((float)GetRandomValue(-30, 30))/3.f, ((float)GetRandomValue(-30, 30))/3.f};
        dots[dot_counter] = a;
        dot_counter += 1;
    }
}

void EnforceRules(struct Colourdot *a){
    for(int i = 0; i < MAXDOTS; i++){
        for(int j = 0; j < MAXRULES; j++){
            if(CmpCols(&color_a[j], &a->col) && CmpCols(&color_b[j], &dots[i].col)){
                if(a->x != dots[i].x && a->y != dots[i].y){
                    Gravity(a, &dots[i], g_values[j]);
                }
                //printf("Gravity\n");
            }
        }
    }
}

void Play(){
    for(int i = 0; i < MAXDOTS; i++){
        EnforceRules(&dots[i]);
        //printf("Updated dot at %d\n", i);
    }
}

void AddRandomDots(int num, int num_colors){
    struct Color colours[] = {ORANGE, YELLOW, RED, BLUE, GREEN, WHITE};
    for(int i = 0; i < num; i++){
        float x = (float)GetRandomValue(0, maxX);
        float y = (float)GetRandomValue(0, maxY);
        struct Color col = colours[GetRandomValue(0, min(num_colors-1, 5))];
        NewDot(x, y, col);
        //printf("Dot added at %d %d\n", (int)x, (int)y);
    }
}

void Ruleset1(){
    NewRule(ORANGE, YELLOW, 90.f);
    NewRule(ORANGE, RED, -30.f);
    NewRule(ORANGE, BLUE, 30.f);
    NewRule(ORANGE, GREEN, -15.f);
    NewRule(ORANGE, WHITE, -4.f);
    NewRule(YELLOW, RED, 15.f);
    NewRule(YELLOW, BLUE, 30.f);
    NewRule(YELLOW, GREEN, 3.f);
    NewRule(YELLOW, WHITE, 5.f);
    NewRule(RED, BLUE, 11.f);
    NewRule(RED, GREEN, 4.f);
    NewRule(RED, WHITE, 7.f);
    NewRule(BLUE, GREEN, -12.f);
    NewRule(BLUE, WHITE, -2.f);
    NewRule(GREEN, WHITE, 3.f);
    
    NewRule(ORANGE, ORANGE, 10.f);
    NewRule(RED, RED, 10.f);
    NewRule(YELLOW, YELLOW, 10.f);
    NewRule(BLUE, BLUE, -61.f);
    NewRule(GREEN, GREEN, 10.f);
    NewRule(WHITE, WHITE, 10.f);
}

void DrawDots(){
    for(int i = 0; i < MAXDOTS; i++){
        drawDot(&dots[i]);
    }
}

void Reset(){
            LOOP(i, MAXDOTS){
                struct Colourdot none = {};
                dots[i] = none;
                dot_counter = 0;
            }
            AddRandomDots((int)num_dots, (int)colors);
}

int main(void)
{

    InitWindow(screenWidth, screenHeight, "Gravity Simulator");

    SetTargetFPS(60);
    //ToggleFullscreen();
    
    //Ruleset1();
            
            
    int OY = NewRule(ORANGE, YELLOW, 90.f);
    int OR = NewRule(ORANGE, RED, -30.f);
    int OB = NewRule(ORANGE, BLUE, 30.f);
    int OG = NewRule(ORANGE, GREEN, -15.f);
    int OW = NewRule(ORANGE, WHITE, -4.f);
    int YR = NewRule(YELLOW, RED, 15.f);
    int YB = NewRule(YELLOW, BLUE, 30.f);
    int YG = NewRule(YELLOW, GREEN, 3.f);
    int YW = NewRule(YELLOW, WHITE, 5.f);
    int RB = NewRule(RED, BLUE, 11.f);
    int RG = NewRule(RED, GREEN, 4.f);
    int RW = NewRule(RED, WHITE, 7.f);
    int BG = NewRule(BLUE, GREEN, -12.f);
    int BW = NewRule(BLUE, WHITE, -2.f);
    int GW = NewRule(GREEN, WHITE, 3.f);
    
    int OO = NewRule(ORANGE, ORANGE, 10.f);
    int RR = NewRule(RED, RED, 10.f);
    int YY = NewRule(YELLOW, YELLOW, 10.f);
    int BB = NewRule(BLUE, BLUE, -61.f);
    int GG = NewRule(GREEN, GREEN, 10.f);
    int WW = NewRule(WHITE, WHITE, 10.f);
    
    AddRandomDots((int)num_dots, (int)colors);
    
    int space = screenWidth - 120;
    
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_R)){
            Reset();
        }
        
        BeginDrawing();
            ClearBackground(BLACK);
            Play();
            int col_change = GuiSliderBar((Rectangle){ space, 0, 120, 20 }, "Number of Colors", NULL, &colors, 1.f, 6.f);
            
            int dot_change = GuiSliderBar((Rectangle){ space, 20, 120, 20 }, "Number of Dots", NULL, &num_dots, 0.f, (float)MAXDOTS);
            
            if(dot_change || col_change) Reset();
            
            GuiSliderBar((Rectangle){ space, 40, 120, 20 }, "Orange-Yellow", NULL, &g_values[OY], -45, 45);
            GuiSliderBar((Rectangle){ space, 60, 120, 20 }, "Orange-Red", NULL, &g_values[OR], -45, 45);
            GuiSliderBar((Rectangle){ space, 80, 120, 20 }, "Orange-Blue", NULL, &g_values[OB], -45, 45);
            GuiSliderBar((Rectangle){ space, 100, 120, 20 }, "Orange-Green", NULL, &g_values[OG], -45, 45);
            GuiSliderBar((Rectangle){ space, 120, 120, 20 }, "Orange-White", NULL, &g_values[OW], -45, 45);
            GuiSliderBar((Rectangle){ space, 140, 120, 20 }, "Yellow-Red", NULL, &g_values[YR], -45, 45);
            GuiSliderBar((Rectangle){ space, 160, 120, 20 }, "Yellow-Blue", NULL, &g_values[YB], -45, 45);
            GuiSliderBar((Rectangle){ space, 180, 120, 20 }, "Yellow-Green", NULL, &g_values[YG], -45, 45);
            GuiSliderBar((Rectangle){ space, 200, 120, 20 }, "Yellow-White", NULL, &g_values[YW], -45, 45);
            GuiSliderBar((Rectangle){ space, 220, 120, 20 }, "Red-Blue", NULL, &g_values[RB], -45, 45);
            GuiSliderBar((Rectangle){ space, 240, 120, 20 }, "Red-Green", NULL, &g_values[RG], -45, 45);
            GuiSliderBar((Rectangle){ space, 260, 120, 20 }, "Red-White", NULL, &g_values[RW], -45, 45);
            GuiSliderBar((Rectangle){ space, 280, 120, 20 }, "Blue-Green", NULL, &g_values[BG], -45, 45);
            GuiSliderBar((Rectangle){ space, 300, 120, 20 }, "Blue-White", NULL, &g_values[BW], -45, 45);
            GuiSliderBar((Rectangle){ space, 320, 120, 20 }, "Green-White", NULL, &g_values[GW], -45, 45);
            
            
            GuiSliderBar((Rectangle){ space, 360, 120, 20 }, "Orange-Orange", NULL, &g_values[OO], -45, 45);
            GuiSliderBar((Rectangle){ space, 380, 120, 20 }, "Red-Red", NULL, &g_values[RR], -45, 45);
            GuiSliderBar((Rectangle){ space, 400, 120, 20 }, "Yellow-Yellow", NULL, &g_values[YY], -45, 45);
            GuiSliderBar((Rectangle){ space, 420, 120, 20 }, "Blue-Blue", NULL, &g_values[BB], -45, 45);
            GuiSliderBar((Rectangle){ space, 440, 120, 20 }, "Green-Green", NULL, &g_values[GG], -45, 45);
            GuiSliderBar((Rectangle){ space, 460, 120, 20 }, "White-White", NULL, &g_values[WW], -45, 45);
            
            DrawDots();
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
