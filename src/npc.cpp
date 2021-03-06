#ifndef __NPC__
#define __NPC__

#define DEBUG_NPC 0

#include "npc.h"

Rect *CreateMinimalRectInfo(GameMemory *gm, NPC *npc)
{
    Rect *r =  CreateRectangle(gm, npc->movement->position, COLOR_WHITE, npc->dim);

    r->bitmap = npc->bitmap;
    r->renderLayer = npc->renderLayer;
    r->frameDirection = npc->direction;
    return r;
};

RectUVCoords *CopyUVCoords(GameMemory *gm, RectUVCoords *ruvc, memory_index size)
{
    RectUVCoords *uvCoordsCopy = (RectUVCoords *)AllocateMemory(gm, sizeof(RectUVCoords) * size);
    memcpy(uvCoordsCopy, ruvc, sizeof(RectUVCoords) * size);
    return uvCoordsCopy;
}

Animation2D *CreateCopyOfSpriteAnimationInfo(GameMemory *gm, Animation2D *spriteSource)
{
    Animation2D *spriteCopy = (Animation2D *)AllocateMemory(gm, sizeof(Animation2D));
    memset(spriteCopy, 0, sizeof(Animation2D));

    spriteCopy->frameCoords = CopyUVCoords(gm, spriteSource->frameCoords, spriteSource->totalFrames);
    spriteCopy->totalFrames = spriteSource->totalFrames;
    spriteCopy->timePerFrame = spriteSource->timePerFrame;
    spriteCopy->frameTTL = spriteSource->frameTTL;
    spriteCopy->currentFrame = &spriteCopy->frameCoords[0];

    return spriteCopy;
}

void UpdateNPCAnimation(NPC *npc, Rect *r)
{
    UpdateCurrentFrame(npc->spriteAnimation, 17.6f);
    UpdateUV(r, *npc->spriteAnimation->currentFrame);
}

void NPCMoveRight(NPC *npc)
{
    npc->direction = RIGHT;
    npc->movement->velocity.x += 1.0f;
#if 0
    /*Debugging purposes*/
    npc->movement->velocity.y = 0;
    npc->movement->acceleration.y = 0;
#endif
}

void NPCMoveLeft(NPC *npc)
{
    npc->direction = LEFT;
    npc->movement->velocity.x -= 1.0f;
#if 0
    npc->movement->velocity.y = 0;
    npc->movement->acceleration.y = 0;
#endif
}

b32 CanMoveTheSameDirection(NPC *npc, GameMetadata *gameMetadata, f32 dt)
{
    v2 center = {0.5f * npc->dim.width, 0.5f * npc->dim.height};
    /* We only need the min and max from the rect to test for AABB, so we can be a bit reckless here. */
    Rect nextUpdate = {};

    nextUpdate.min =
        V2(npc->movement->position) + V2(npc->movement->velocity) * dt + 0.5f * V2(npc->movement->acceleration) * dt * dt - center;
    nextUpdate.max =
        V2(npc->movement->position) + V2(npc->movement->velocity) * dt + 0.5f * V2(npc->movement->acceleration) * dt * dt + center;

    MinMax temp = GetMinMax(&nextUpdate);
    AABB range = MinMaxToSquareAABB(&temp);
    /* XXX: The padding/range needs to be wide enough such that we can get the
     * other "NonTraversable"s. The issue is that the quadtree puts the rect at northwest firsts,
     * so even though the rect is in range, the AABB in the quadtree for the
     * rect might be smaller.
     *
     * tl;dr: The quadtree is not accurate enough, because of how we insert the rects.
     * So, we need to make the query range bigger.
     * The bigger the rect, the harder it is to be found.
     */
    f32 arbitraryPadding = 0.0f;
    range.halfDim = range.halfDim + arbitraryPadding;
    //Rect **arr = GetRectsWithInRange(gameMetadata->sm, &range);
    Rect **arr = SpatialHashGet(gameMetadata->sh, &range);

#if DEBUG_NPC
    AddDebugRect(gameMetadata, &range, COLOR_GREEN);
#endif

    switch(npc->direction)
    {
        case LEFT:
            /* XXX: make it such that the NPC turns early*/
            nextUpdate.max.x = 0.01f + nextUpdate.min.x;
            break;
        case RIGHT:
            /* XXX: Hack to make it such that the nextUpdate rect will be a
             * small sliver of the actual rect width.
             * This will make the NPC turn earlier instad of hitting the edge
             * of the platform with the last pixel.
             */
            nextUpdate.min.x = nextUpdate.max.x - 0.01f;
            break;
        default:
            ASSERT(!"I SHOUDLN'T GET HERE");
    }

#if DEBUG_NPC
    AddDebugRect(gameMetadata, &nextUpdate);
#endif

    for(memory_index i = 0; i < ARRAY_LIST_SIZE(arr); i++)
    {
        Rect *rect = arr[i];
#if DEBUG_NPC
        AddDebugRect(gameMetadata, rect, COLOR_YELLOW);
#endif
        if(TestAABBAABB(rect, &nextUpdate))
        {
            return true;
        }
    }

    return false;
}

void SwitchDirection(NPC *npc)
{
    switch(npc->direction)
    {
        case LEFT:
            npc->direction = RIGHT;
            break;
        case RIGHT:
            npc->direction = LEFT;
            break;
        default:
            ASSERT(!"I shoudln't get here");
            break;
    }
}

void MoveUniDirectional(NPC *npc, GameMetadata *gm, f32 dt)
{
    switch(npc->direction)
    {
        case LEFT:
            if (CanMoveTheSameDirection(npc, gm, dt) == false)
            {
                npc->direction = RIGHT;
            }
            break;
        case RIGHT:
            if (CanMoveTheSameDirection(npc, gm, dt) == false)
            {
                npc->direction = LEFT;
            }
            break;
        default:
            ASSERT(!"I shoudln't get here");
            break;
    }

    switch(npc->direction)
    {
        case LEFT:
            NPCMoveLeft(npc);
            break;
        case RIGHT:
            NPCMoveRight(npc);
            break;
        default:
            ASSERT(!"I shoudln't get here");
            break;
    }
}

void NPCMoveX(NPC *npc, GameMetadata *gm, f32 dt)
{
    switch(npc->movementPattern)
    {
        case UNI_DIRECTIONAL:
        {
            MoveUniDirectional(npc, gm, dt);
            break;
        }
        case BI_DIRECTIONAL:
        {
            break;
        }
        default:
            ASSERT(!"I SHOULDN'T GET HERE");
    }
}

void NPCMove(NPC *npc, GameMetadata *gm, f32 dt)
{
    /* This is where the NPC will decide where to move.
     * For now, let's make the NPC move left and right.*/

    // Check desired location
    switch (npc->movementType)
    {
        case X_MOVEMENT:
            NPCMoveX(npc, gm, dt);
            //or
            //NPCMoveRight(npc);
            break;
        case Y_MOVEMENT:
            break;
        case XY_MOVEMENT:
            break;
        default:
            ASSERT(!"I SHOULDN'T GET HERE");
    }
    //NPCMoveRight(npc);
    /*
    NPCMoveUp();
    NPCMoveDown();
    */
}

void UpdateNPCMovement(NPC *npc, GameMetadata *gm, f32 dt)
{
    NPCMove(npc, gm, dt);
}

NPC *CreateNPC(GameMetadata *gameMetadata, GameMemory *gameMemory)
{
    NPC *result = (NPC *)AllocateMemory0(gameMemory, sizeof(NPC));

    result->dim.height = 1.0f;
    result->dim.width = 1.5f;
    result->bitmap = FindBitmap(&gameMetadata->bitmapSentinelNode, "pshroom");
    result->spriteAnimation =
        CreateCopyOfSpriteAnimationInfo(
                gameMemory,
                GetSpriteAnimationInfo(
                    GetFrameAnimation(&gameMetadata->frameAnimationSentinelNode, "pshroom"),
                    "IDLE")
                );
    result->direction = LEFT;
    result->renderLayer = BEHIND_PLAYER;
    result->movementType = X_MOVEMENT;
    result->movementPattern = UNI_DIRECTIONAL;
    result->movement = (Movement *)AllocateMemory0(gameMemory, sizeof(Movement));
    result->movement->position = glm::vec3(1,2,0);

    return result;
}

void CreateEnemy(GameMetadata *gameMetadata, GameMemory *gameMemory, glm::vec3 position)
{
        Entity *entity = CreateAndAddNewEntity(gameMetadata, gameMemory);
        NPC *newEnemyNPC = CreateNPC(gameMetadata, gameMemory);
        newEnemyNPC->movement->position = position;
        Stat *stat = CreateStat(gameMemory, 1);

        HashAdd(gameMetadata->hashEntityNPC, entity, newEnemyNPC);
        HashAdd(gameMetadata->hashEntityMovement, entity, newEnemyNPC->movement);
        HashAdd(gameMetadata->hashEntityStat, entity, stat);
}

#endif
