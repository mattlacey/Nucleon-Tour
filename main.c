/*
	Nucelon Tour - main.c

	Yup, this is all in one place right now, and yup, there are very few 
	(if any, there aren't at the time of writing) forward declarations.

	I'm not intending this to be a full project, more a simple experiment
	in this style of engine and it's really just a learning exercise
	before I tackle a similar setup in ARM assembly.

	There's also a few magic numbers, assumptions and other bad habits ;)
	
	Matt Lacey, Feb 2013.
*/

#include <stdio.h>
#include <SDL/SDL.h>

#define KEY_COUNT (323)
#define X_RES (800)
#define Y_RES (600)
#define Y_RES_2 (Y_RES >> 1)
#define Z_MAX (0x1000)
#define CAM_HEIGHT (16384)

SDL_Surface * screen = NULL;
SDL_Surface * road = NULL;

int keys[KEY_COUNT];
int heightMap[Z_MAX];

int init()
{
	int i, x, y, dx, dy;

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(X_RES, Y_RES, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);

	if(!screen)
	{
		printf("Failed to create screen surface.");
		return 0;
	}

	memset(keys, 0, sizeof(int) * KEY_COUNT);

	road = SDL_CreateRGBSurface(SDL_SWSURFACE, X_RES, Y_RES, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	if(!road)
	{
		printf("Failed to create road surface. Too much snow?");
		return 0;
	}

	dx = 0;

	// Draw a basic road
	for(y = 0; y < Y_RES; y++)
	{
		if(y & 1)
		{
			dx++;
		}

		for(x = 0; x < (X_RES >> 1) - dx; x++)
		{

			*((Uint32 *)road->pixels + y * X_RES + x) = 0xFF00FF8F;
			*((Uint32 *)road->pixels + (y + 1) * X_RES - x) = 0xFF00FF8F;
		}

		for(x = (X_RES >> 1) - dx; x < (X_RES >> 1) + 1 + dx; x++)
		{
			*((Uint32 *)road->pixels + y * X_RES + x) = 0xFF999999;
		}
	}

	// Generate some hills
	y = 0;
	dy = 0;

	for(i = 0; i < Z_MAX >> 2; i++)
	{
		heightMap[(Z_MAX - 1) - i] = heightMap[i] = dy >> 8;
		y++;
		dy += y >> 4;
	}

	x = heightMap[(Z_MAX >> 2) - 1] * 2;

	for(i = 0; i < Z_MAX >> 2; i++)
	{
		// second half of the up hill
		heightMap[(Z_MAX >> 1) + i] = heightMap[(Z_MAX >> 1) - (i + 1)] = x - heightMap[i];
	}

	for(i = 0; i < Z_MAX; i+= 32) printf("%i\n", heightMap[i]);

	SDL_WM_SetCaption("Nucleon Tour!", 0);

	return 1;
}

void render()
{
	Uint32 * src, * dst;
	int i, x, y, yy, yDraw;
	int xOff, dxOff;
	int hBase = 0;
	static int z = 0; 

	SDL_LockSurface(screen);

	z += 4;

	if(z & Z_MAX) 
	{
		z = 0;
	}

	printf("z = %i\n", z);

	xOff = 0;
	dxOff = 0;

	yy = 0;
	yDraw = Y_RES - 1;

	for(y = Y_RES - 1; yDraw >  1; y--)
	{
		int dx = y - Y_RES_2;
		int mod = 0;
		int h = 0;

		int zScr = (y == Y_RES >> 1) ? 0 : CAM_HEIGHT / (y - (Y_RES >> 1));

		// road map should really only map to half the screen
		yy += 2;

		dst = (Uint32 *)screen->pixels + yDraw * X_RES;

		// stops us going all the way to the horizon
		if(zScr < (CAM_HEIGHT >> 2) && zScr > 0)
		{
			// printf("yy = %i, z = %i\n", yy, z);
			
			zScr += z;

			// track is straight, left, straight, right
		
			// this needs to wrap for the height map 
			zScr &= Z_MAX - 1;

			// do stripes
			if(zScr & 0x20)
			{
				mod = 0x00222222;
			}

			if(zScr & 0x400 && zScr & 0x200)
			{
				xOff--;
			}
			else if(zScr & 0x400)
			{
				xOff++;
			}
			else if(zScr & 0x200)
			{
				xOff--;
			}

			// scale down xOff before using it, we do the same with dxOff later	
			dxOff += (xOff >> 4);

			// we only want to draw if the next line as above the last drawn line
			h = heightMap[zScr];

			if(hBase == 0)
			{
				hBase = h;
			}

			h = yDraw - (y - (h - hBase));

			src = (Uint32 *)road->pixels + (Y_RES - yy) * X_RES;

			for(; h >= 0; h--)
			{
				// copy the line we want 
				// right now this is going to wrap around, but we'll clamp and fix things later

				//printf("yDraw = %i, h = %i, hBase = %i, zScr = %i\n", yDraw, h, hBase, zScr);
				for(x = 0; x < X_RES; x++)
				{
					*(dst - (X_RES * h) + x) = *(src  + x + (dxOff >> 4)) - mod;
				}

				yDraw--;
			}
		}
		else
		{
			// BRRRRUE BRRUE SKYYYYY
			// Not needed every frame, but hey - easy optimisation later!
			for(x = 0; x < X_RES; x++)
			{
				*(dst + x) = 0xFF8888FF;
			}
			
			yDraw--;
		}
	}

	SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, X_RES, Y_RES);
}

void loop()
{
	int loop = 1;
	SDL_Event ev;

	while(loop)
	{
		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT)
			{
				printf("Quit requested...");
				loop = 0;
			}

			if(ev.type == SDL_KEYDOWN)
			{
				keys[ev.key.keysym.sym] = 1;
			}
			else if(ev.type == SDL_KEYUP)
			{
				keys[ev.key.keysym.sym] = 0;
			}

			if(keys[SDLK_ESCAPE])
			{
				loop = 0;
			}
		}

		//if(keys[SDLK_SPACE])
		{
			render();
		}

		SDL_Flip(screen);
	}
}

int main(int argc, char * argv[])
{
	if(!init())
	{
		printf("Failed to initialise SDL.");
	}

	loop();

	SDL_Quit();

	return 0;
}


