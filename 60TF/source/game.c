#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "sprites.h                                                                                                                                                                          "
#include "simple_logger.h"
#include "input.h"
#include "entity.h"
#include "tilemap.h"


int main(int agrc, char *arg[])
{
	Bool done = false;
	SDL_Rect dimensions = { 0, 0, 1280, 720 };
	const Uint8 *keys = NULL;
	Vector4D  bgcolor = { 25, 128, 50, 255 };
	TileMap *map = NULL;

	init_logger("60TF.log");
	slog("---==== BEGIN ====---");

	GraphicsInit(1280, 720, 0, 16, bgcolor);
	SpriteManagerInit(1024);
	gf2d_text_init("config/font.cfg");
	GUISetupHUD();

	EntityManagerInit(1024);
		
	TileMapManagerInit(1);

	map = LoadTileMapFromFile("config/overworld.cfg");

	PlayerInit();


	//WorldInit("config/overworld.cfg", "config/sideview.cfg");
	//PlayerInit();

	//AddEntityToTileMap(GetPlayerEntity());

	//SetCameraDimensions(dimensions);


	//SetCameraBounds(GetCurrentTileMap()->boundingBox);


	//SetCameraPosition(GetPlayerEntity()->position);

	SDL_ShowCursor(SDL_DISABLE);

	while (!done)
	{
		done = HandleInput(keys) == Quit ? true : false;
				
		EntityThinkAll();

		EntityUpdateAll();

		DrawMap(GetCurrentTileMap());

		EntityDrawAll();
		
		GUIDrawHUD();

		NextFrame();
	}

	//CleanUpWorld(GetGameWorld());

	slog("---==== END ====---");

	return 0;
}