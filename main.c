#include <stdio.h>
#include <SDL/SDL.h>

#define KEY_COUNT (323)

SDL_Surface * screen = NULL;
int keys[KEY_COUNT];

int init()
{
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(800, 600, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);

	memset(keys, 0, sizeof(int) * KEY_COUNT);

	SDL_WM_SetCaption("Nucleon Tour!", 0);

	return 1;
}

void render()
{
	static int x = 0;

	SDL_LockSurface(screen);

	*((Uint32 *)(screen->pixels) + x) = 0xFF336699;

	x ++;

	if(x > 800 * 600) x = 0;

	SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 800, 600);
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

		render();

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


