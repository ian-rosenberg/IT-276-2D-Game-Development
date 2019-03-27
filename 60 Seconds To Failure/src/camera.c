#include "camera.h"
#include "graphics.h"

typedef struct 
{
	Vector2D position;

	SDL_Rect dimensions,
			 bounds;
}Camera;

static Camera cam = { 0 };

void SetCameraDimensions(SDL_Rect dim)
{
	cam.dimensions = dim;
}

void SetCameraBounds(SDL_Rect bounds)
{
	cam.bounds = bounds;
}

void SetCameraPosition(Vector2D pos)
{
	Vector2D screenDim = GetRenderDimensions();

	cam.position = vector2d(pos.x - screenDim.x / 2.0f, pos.y - screenDim.y / 2.0f);

	if (cam.position.x < 0)
	{
		cam.position.x = 0;
	}
	if (cam.position.x > cam.bounds.w - cam.dimensions.w)
	{
		cam.position.x = cam.bounds.w - cam.dimensions.w;
	}
	if (cam.position.y < 0)
	{
		cam.position.y = 0;
	}
	if (cam.position.y > cam.bounds.h - cam.dimensions.h)
	{
		cam.position.y = cam.bounds.h - cam.dimensions.h;
	}
}

Vector2D GetCameraPosition()
{
	return cam.position;
}