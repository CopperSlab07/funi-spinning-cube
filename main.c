#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

#define WIDTH	800
#define HEIGHT	800

#define defer(val) do {return_val = val;  printf("Errored! SDL_Error: %s\n", SDL_GetError()); goto exit;} while(0)
//BGColor = 0xFF50FF50, FGColor = 0xFF101010; : RGBA
//angle: in rad for now
typedef struct vec2f {
	float x, y;
} vec2f;

typedef struct vec3f {
	float x, y, z;
} vec3f;

/*typedef struct Indice_From{
	uint8_t *items;
	size_t count;
	size_t capacity;
} Indice_From;

typedef struct Indice_To{
	uint8_t *items;
	size_t count;
	size_t capacity;
} Indice_To;*/

#define da_append(da, item)\
	do\
	{\
		if(da.count >= da.capacity){\
			if(da.capacity == 0) da.capacity = 256;\
			else{\
				da.capacity *= 2;\
				da.items = realloc(da.items, da.capacity*sizeof(*da.items));\
			}\
		da.items[da.count++] = item;\
	}\
	while(0);

#define da_append_pointer(da, item)\
	do\
	{\
		if(da->count >= da->capacity){\
			if(da->capacity == 0) da->capacity = 256;\
			else{\
				da->capacity *= 2;\
				da->items = realloc(da->items, da->capacity*sizeof(*da->items));\
			}\
		}\
		da->items[da->count++] = item;\
	}\
	while(0);

#define MIN(in1, in2) ((in1 < in2) ? in1 : in2)

uint8_t BGColor[4] = {
	0x10,
	0x10,
	0x10,
	0xFF
}, FGColor[4] = {
	0x60,
	0xFF,
	0x60,
	0xFF
};

static int32_t win_width = WIDTH, win_height = HEIGHT;
const float pi = 3.14159f;
const float FPS = 60;
const float delta_time = 1/FPS;
const uint8_t p_size = 20;
float dz = 1;
float angleXZ = 0;
int16_t state = 0;
uint16_t frame = 0;

//const vec3f test = {0.0f, 0.25f, 1};

const vec3f vertices[8] = {
	{0.25, 0.25, 0.25},
	{-0.25, 0.25, 0.25},
	{-0.25, -0.25, 0.25},
	{0.25, -0.25, 0.25},
	
	{0.25, 0.25, -0.25},
	{-0.25, 0.25, -0.25},
	{-0.25, -0.25, -0.25},
	{0.25, -0.25, -0.25}
};

const uint8_t indices[2][12] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3},
	{1, 2, 3, 0, 5, 6, 7, 4, 4, 5, 6, 7}
};

void clear(SDL_Renderer *canva);
void point(SDL_Renderer *canva, vec2f p);
vec2f convert(vec2f vec);
vec2f convert_OGCenter(vec2f vec, int og_width, int og_height);
vec2f project(vec3f vec);
vec3f transform_z(vec3f vec, float dz);
vec3f rotate_rad(vec3f vec, float angle);
vec3f rotate_deg(vec3f vec, float angle);
void line(SDL_Renderer *canva, vec2f p1, vec2f p2);
//void indice_init(Indice_From *iF, Indice_To *iT);
void key_handle(SDL_Event);

int main(void){
	//Indice_From iFrom = {0};
	//Indice_To iTo = {0};

	//indice_init(&iFrom, &iTo);

	int8_t return_val = 0;
	if(SDL_Init(SDL_INIT_VIDEO) != 0) defer(-1);
	
	SDL_Window *win = SDL_CreateWindow("test", 100, 100, win_width, win_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(win == NULL)	defer(-1);

	//SDL_Renderer *canva = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
	SDL_Renderer *canva = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if(canva == NULL) defer(-1);

	SDL_GetRendererOutputSize(canva, &win_width, &win_height);
	printf("Current drawable size (in pixels): %d x %d\n", win_width, win_height);
	
	SDL_Event event;
	while(state >= 0){
		if(event.type == SDL_QUIT) state = -1;
		if(SDL_PollEvent(&event) == 0) goto skip;
		if(event.window.type == SDL_WINDOWEVENT_RESIZED){
			printf("Window resized!\n");
			SDL_GetRendererOutputSize(canva, &win_width, &win_height);
			printf("Current drawable size (in pixels): %d x %d\n", win_width, win_height);
		}
		//frame = 0;
		if(event.type == SDL_KEYDOWN){
			switch(event.key.keysym.sym){
				case SDLK_r:
					printf("Window resized!\n");
					SDL_GetRendererOutputSize(canva, &win_width, &win_height);
					printf("Current drawable size (in pixels): %d x %d\n", win_width, win_height);
					break;
				default:
					break;
			}
		}
skip:
		angleXZ += pi * delta_time;
		clear(canva);
		//point(canva, convert(project(rotate_rad(test, angle))));
		
		/*for(int8_t i = 0; i < 8; i++){
			point(canva, convert(project(transform_z(rotate_rad(vertices[i], angle), dz))));
		}*/

		for(uint8_t i = 0; i < 12; i++){
			vec3f a = vertices[indices[0][i]];
			vec3f b = vertices[indices[1][i]];
			line(canva,
				convert_OGCenter(project(transform_z(rotate_rad(a, angleXZ), dz)), WIDTH, HEIGHT),
				convert_OGCenter(project(transform_z(rotate_rad(b, angleXZ), dz)), WIDTH, HEIGHT)
			);
		}
		
		SDL_RenderPresent(canva);
		//printf("Skiped %d frames rn...\n", frame);
		//frame++;
		SDL_Delay(20);
	}
	
exit:
	if(canva) SDL_DestroyRenderer(canva);
	if(win) SDL_DestroyWindow(win);
	SDL_Quit();
	return return_val;
}

void clear(SDL_Renderer *canva){
	SDL_SetRenderDrawColor(canva, BGColor[0],  BGColor[1], BGColor[2], BGColor[3]);
	SDL_RenderClear(canva);
}

void point(SDL_Renderer *canva, vec2f p){
	SDL_SetRenderDrawColor(canva, FGColor[0],  FGColor[1], FGColor[2], FGColor[3]);
	SDL_FRect tmp_rect = {
		.x = p.x - p_size/2,
		.y = p.y - p_size/2,
		.w = p_size,
		.h = p_size
	};
	if(SDL_RenderDrawRectF(canva, &tmp_rect) == -1/* || SDL_RenderFillRectF(canva, &tmp_rect) == -1*/){
		printf("Cannot draw point at (%f, %f)!\n", p.x, p.y);
	}
	if(SDL_RenderFillRectF(canva, &tmp_rect) == -1){
		printf("Cannot draw point at (%f, %f)!\n", p.x, p.y);
	}
}

vec2f convert(vec2f vec){
	return (vec2f){
		.x = ((vec.x + 1)/2) * win_width,
		.y = (1 - (vec.y + 1)/2) * win_height
	};
}

vec2f convert_OGCenter(vec2f vec, int og_width, int og_height){
	return (vec2f){
		.x = (((vec.x + 1)/2) * og_width) + (win_width/2 - og_width/2),
		.y = (((1 - (vec.y + 1)/2)) * og_height) + (win_height/2 - og_height/2)
	};
}

vec2f project(vec3f vec){
	if(vec.z == 0.0f) return (vec2f){0, 0};
	return (vec2f){
		.x = vec.x/vec.z,
		.y = vec.y/vec.z
	};
}

vec3f transform_z(vec3f vec, float dz){
	return (vec3f){
		.x = vec.x,
		.y = vec.y,
		.z = vec.z + dz
	};
}

vec3f rotate_rad(vec3f vec, float angle){
	const float cosed = cos(angle);
	const float sined = sin(angle);
	
	return (vec3f){
		.x = vec.x*cosed - vec.z*sined,
		.y = vec.y,
		.z = vec.x*sined + vec.z*cosed
	};
}

vec3f rotate_deg(vec3f vec, float angle){
	float tmp_angle = (angle/180) * pi;
	return rotate_rad(vec, tmp_angle);
}

void line(SDL_Renderer *canva, vec2f p1, vec2f p2){
	SDL_SetRenderDrawColor(canva, FGColor[0],  FGColor[1], FGColor[2], FGColor[3]);
	if(SDL_RenderDrawLineF(canva, p1.x, p1.y, p2.x, p2.y) == -1)\
		printf("Cannot drawline from (%f, %f) to (%f, %f)!\n", p1.x, p1.y, p2.x, p2.y);
}

/*void indice_init(Indice_From *iF, Indice_To *iT){
	da_append_pointer(iF, 0);
	da_append_pointer(iF, 1);
	da_append_pointer(iF, 2);
	da_append_pointer(iF, 3);
	da_append_pointer(iF, 4);
	da_append_pointer(iF, 5);
	da_append_pointer(iF, 6);
	da_append_pointer(iF, 7);
	
	da_append_pointer(iT, 1);	
	da_append_pointer(iT, 2);	
	da_append_pointer(iT, 3);	
	da_append_pointer(iT, 0);	
	da_append_pointer(iT, 5);	
	da_append_pointer(iT, 6);	
	da_append_pointer(iT, 7);	
	da_append_pointer(iT, 4);	
}*/
