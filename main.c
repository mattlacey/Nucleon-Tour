/*
	Nucelon Tour - main.c

	Yup, this is all in one place right now, and yup, there are very few 
	(if any, there arne't at the time of writing) forward declarations.

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
#define CAM_HEIGHT (16384)

// CAM_HEIGHT / 300, then rounded up to the nearest power of 2, then taking log
#define Z_SCALE (6)

SDL_Surface * screen = NULL;
SDL_Surface * road = NULL;

int keys[KEY_COUNT];

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

	// memcpy(screen->pixels, road->pixels, X_RES * Y_RES * 4);

	SDL_WM_SetCaption("Nucleon Tour!", 0);

	return 1;
}

void render()
{
	Uint32 * src, * dst;
	int i, x, y, yy;
	int xOff, dxOff;
	static int dz = 0; 

	SDL_LockSurface(screen);

	dz++;

	xOff = 0;
	dxOff = 0;

	yy = Y_RES - 1;

	for(y = Y_RES - 1; y >  Y_RES >> 1; y--)
	{
		int dx = y - (Y_RES >> 1);
		int mod = 0;

		// camera is 32 units above the road
		int z = -CAM_HEIGHT / (y - ((Y_RES >> 1) - 1));

		yy -= 2;

		if(y >> 9)
		{
			xOff ++;
			dxOff += xOff;
		}

		// need to map z to our road texture
	// 	yy = (CAM_HEIGHT -z) / 55;

		z -= dz; 

		// stripes are 64 z-units wide, keep dz wrapping
		if(z & 0x40)
		{
			mod = 0x00002222;
		}
		
		dz &= 0x7F;

		src = (Uint32 *)road->pixels + yy * X_RES;
		dst = (Uint32 *)screen->pixels + y * X_RES;

		// copy the line we want 
		for(x = 0; x < X_RES; x++)
		{
			*(dst + x) = *(src  + x) - mod;
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

		// if(keys[SDLK_SPACE])
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


