#include "entity.h"
#include "simple_logger.h"



typedef struct
{
	Uint32		maxEntities;
	Uint64		numEntities;

	Entity		*entityList;
}EntityManager;

//local global 
static EntityManager entityManager = { 0 };

void EntityManagerInit(Uint32 maxEnt)
{
	if (!maxEnt)
	{
		slog("Cant allocate 0 entities");
		return;
	}

	entityManager.entityList = malloc(sizeof(Entity) * maxEnt);
	memset(entityManager.entityList, 0, sizeof(Entity) * maxEnt);

	if (!entityManager.entityList)
	{
		slog("failed to allocate entity list");
		EntityManagerClose();
		return;
	}
	entityManager.maxEntities = maxEnt;
	atexit(EntityManagerClose);
	slog("entity system initialized");

	atexit(EntityManagerClose);
}

void EntityManagerClose()
{
	int i = 0;

	if (entityManager.entityList != NULL)
	{
		EntityFreeAll();

		free(entityManager.entityList);
	}

	memset(&entityManager, 0, sizeof(EntityManager));
	slog("Entities freed and manager closed");
}

Entity* NewEntity(Actor* act)
{
	int i;
	Entity* ent = NULL;

	for (i = 0; i < entityManager.maxEntities; i++)
	{
		if (entityManager.entityList[i]._inUse == 0)
		{
			ent = &entityManager.entityList[i];

			ent->id = entityManager.numEntities++;
			ent->_inUse = 1;

			vector2d_set(ent->scale, 1, 1);
			
			ent->actor = act;
			ent->artState = ART_INPROGRESS;
			
			ent->id = i;

			ent->gravityEnabled = 0;
			ent->speed = 0;
			ent->maxSpeed = 0;

			ent->Draw = EntityDraw;

			ent->Think = EntityThink;

			ent->Update = EntityUpdate;

			ent->Free = EntityFree;
			
			++entityManager.numEntities;

			ent->actor->color = vector4d(1, 1, 1, 1);// no color shift, opaque
			return ent;
		}
	}
	return NULL;
}

void EntityDraw(Entity *self)
{
	if (!self)
	{
		return;
	}

	if (!self->_inUse)
	{
		return;
	}

	DrawSprite(self->actor->currentSprite,
		self->position,
		&self->scale,
		&self->scaleCenter,
		NULL,
		NULL,
		NULL,
		self->actor->currentAnimation->currentFrame,
		self->actor->currentAnimation->yOffset,
		self->actor->currentAnimation->cellWidth,
		self->actor->currentAnimation->cellHeight);
}

void EntityDrawAll()
{
	int i;

	for (i = 0; i < entityManager.maxEntities; ++i)
	{	
		if (entityManager.entityList[i]._inUse == 0)
		{
			continue;
		}

		if (entityManager.entityList[i].Draw != NULL)
		{
			//slog("Entity name: %s", entityManager.entityList[i].actor->name);
			entityManager.entityList[i].Draw(&entityManager.entityList[i]);
		}
	}
}

void EntityUpdate(Entity *self)
{	
	if (!self)
	{
		return;
	}

	if (!self->_inUse)
	{
		return;
	}

	if (self->dead)
	{
		EntityFree(self);
	}

	//update position logic

	if (self->Update != NULL && self->Update != EntityUpdate)
	{
		self->Update(self);
	}
}

void EntityUpdateAll()
{
	int i;

	for (i = 0; i < entityManager.maxEntities; ++i)
	{
		if (entityManager.entityList[i]._inUse == 0)
		{
			continue;
		}

		if (entityManager.entityList[i].Update != NULL)
		{
			entityManager.entityList[i].Update(&entityManager.entityList[i]);
		}
	}
}

void EntityThinkAll()
{
	int i;
	for (i = 0; i < entityManager.maxEntities; i++)
	{
		if (entityManager.entityList[i]._inUse == 0)continue;
		if (entityManager.entityList[i].Think != NULL)
		{
			entityManager.entityList[i].Think(&entityManager.entityList[i]);
		}
	}
}

void EntityThink(Entity *self)
{
	if (!self)
	{
		return;
	}

	if (self->Think != NULL && self->Think != EntityThink)
	{
		self->Think(self);
	}
}

void EntityFreeAll()
{
	int i;
	for (i = 0; i < entityManager.maxEntities; i++)
	{
		EntityFree(&entityManager.entityList[i]);
	}
}

Entity* GetEntityByID(Uint32 id)
{
	int i;
	for (i = 0; i < entityManager.maxEntities; i++)
	{
		if (entityManager.entityList[i]._inUse == 0)continue;
		if (entityManager.entityList[i].id == id)
		{
			return &entityManager.entityList[i];
		}
	}
	return NULL;
}

void EntityFree(Entity *self)
{
	int i;
	
	if (!self)return;

	FreeActor(self->actor);
	
	free(self);
}

Entity* EntityIterate(Entity *start)
{
	Entity *p = NULL;

	if (!start)
	{
		p = entityManager.entityList;
	}
	else
	{
		p = start;
		p++;
	}
	
	for (; p != &entityManager.entityList[entityManager.maxEntities]; p++)
	{
		if (p->_inUse)return p;
	}
	return NULL;
}

int IsEntityPtrValid(void *p)
{
	Entity *ent;
	if (!p)return 0;
	ent = (Entity *)p;
	if (ent < entityManager.entityList)return 0;
	if (ent >= &entityManager.entityList[entityManager.maxEntities])return 0;
	return 1;
}