#include <SDL_ttf.h>

#include "simple_logger.h"
#include "graphics.h"


static Graphics graphics = { 0 };

void GraphicsInit(Uint32 winWidth, Uint32 winHeight, Bool fullscreen, Uint32 fDelay, Vector4D bgCol)
{
	Uint32 flags = 0;
	SDL_Rect textRect = { 0 };

	graphics.window = NULL;
	graphics.renderer = NULL;
	graphics.tex = NULL;
	graphics.surface = NULL;
	graphics.tempSurface = NULL;
	graphics.frameDelay = 0;
	graphics.prevTime = 0;
	graphics.curTime = 0;

	if (fDelay < 0)
	{
		graphics.frameDelay = 0;
	}
	else
	{
		graphics.frameDelay = fDelay;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		slog("SDL failed to init! SDL_Error: %s", SDL_GetError());

		GraphicsClose();

		return;
	}

	slog("SDL initialization success!");

	atexit(SDL_Quit);

	if (fullscreen)
	{
		if (winWidth == 0)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		else
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}
	}

	graphics.window = SDL_CreateWindow("60 Seconds To Failure",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		winWidth,
		winHeight,
		flags);

	if (!graphics.window)
	{
		slog("SDL window failed to create! %s", SDL_GetError());

		GraphicsClose();
	}

	graphics.renderer = SDL_CreateRenderer(graphics.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	if (!graphics.renderer)
	{
		slog("Renderer was not created: %s", SDL_GetError());
		
		GraphicsClose();
	}

	ClearScreen();

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(graphics.renderer, winWidth, winHeight);

	graphics.tex = SDL_CreateTexture(
		graphics.renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		winWidth, winHeight);
	if (!graphics.tex)
	{
		slog("failed to create screen texture: %s", SDL_GetError());
		GraphicsClose();
		return;
	}

	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888,
		&graphics.bitdepth,
		&graphics.rmask,
		&graphics.gmask,
		&graphics.bmask,
		&graphics.amask);


	graphics.surface = SDL_CreateRGBSurface(0, winWidth, winHeight, graphics.bitdepth,
		graphics.rmask,
		graphics.gmask,
		graphics.bmask,
		graphics.amask);

	if (!graphics.surface)
	{
		slog("failed to create screen surface: %s", SDL_GetError());
		GraphicsClose();
		return;
	}

	graphics.w = winWidth;
	graphics.h = winHeight;
	graphics.bgColorVal = SDL_MapRGB(graphics.surface->format, bgCol.x, bgCol.y, bgCol.z);
	vector4d_set(graphics.bgColor, bgCol.x, bgCol.y, bgCol.z, bgCol.w);
	SDL_SetRenderDrawBlendMode(GetRenderer(), SDL_BLENDMODE_BLEND);

	srand(SDL_GetTicks());

	atexit(GraphicsClose);
	slog("Graphics initialized successfully");
}

void GraphicsClose()
{
	if (graphics.renderer)
	{
		SDL_DestroyRenderer(graphics.renderer);
	}
	if (graphics.window)
	{
		SDL_DestroyWindow(graphics.window);
	}
	SDL_FreeSurface(graphics.surface);
	SDL_FreeSurface(graphics.tempSurface);
	SDL_DestroyTexture(graphics.tex);
	
	graphics.window = NULL;
	graphics.renderer = NULL;

	memset(&graphics, 0, sizeof(Graphics));

	SDL_Quit();

	slog("SDL graphics shut down successfully");
}

void ClearScreen()
{
	SDL_SetRenderDrawColor(graphics.renderer, 16, 64, 192, 255);

	SDL_RenderClear(graphics.renderer);
}

SDL_Renderer* GetRenderer()
{
	return graphics.renderer;
}

void NextFrame()
{
	SDL_RenderPresent(graphics.renderer);

	FrameDelay();
}

float GetFrameRate()
{
	return graphics.framerate;
}

void FrameDelay()
{
	Uint32 diff;
	graphics.prevTime = graphics.curTime;
	slog_sync();// make sure logs get written when we have time to write it
	graphics.curTime = SDL_GetTicks();
	diff = (graphics.curTime - graphics.prevTime);
	if (diff < graphics.frameDelay)
	{
		SDL_Delay(graphics.frameDelay - diff);
	}
	graphics.framerate = 1000.0 / MAX(SDL_GetTicks() - graphics.prevTime, 0.001);
	//slog("FPS: %f", graphics.framerate);
}

Uint32 GetFrameDelay()
{
	return graphics.frameDelay;
}

SDL_Surface *gf2d_graphics_screen_convert(SDL_Surface **surface)
{
	SDL_Surface *convert;
	if (!(*surface))
	{
		slog("surface provided was NULL");
		return NULL;
	}
	if (!graphics.surface)
	{
		slog("graphics not yet initialized");
		return NULL;
	}
	convert = SDL_ConvertSurface(*surface,
		graphics.surface->format,
		0);
	if (!convert)
	{
		slog("failed to convert surface: %s", SDL_GetError());
		return NULL;
	}
	SDL_FreeSurface(*surface);
	*surface = NULL;
	return convert;
}

Vector2D GetRenderDimensions(){
	Vector2D dim;
	dim.x = graphics.w;
	dim.y = graphics.h;

	return dim;
}