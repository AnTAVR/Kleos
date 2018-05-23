#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include "bitmap.h"
#include "entity.h"
#include "game_memory.h"

#define RECT_SIZE 36
#define NUM_OF_RECT_CORNER 4
enum RectType
{
    REGULAR = 0,
    COLLISION = 1,
    HITBOX = 2,
    HURTBOX = 3
};

enum Direction
{
    LEFT = 1,
    RIGHT = 2,
};

struct RectUVCoords
{
#pragma warning(push)
#pragma warning(disable : 4201)
    union {
        struct
        {
            v2 topRight;
            v2 bottomRight;
            v2 bottomLeft;
            v2 topLeft;
        };
        v2 UV[4];
#pragma warning(pop)
    };
};

#include "animation.h"

struct Rect
{
    Entity *entity;

    Vertex vertices[4];

    v3 topLeft;
    v3 topRight;
    v3 bottomLeft;
    v3 bottomRight;
    v4 color;

    real32 width, height;

    memory_index bitmapID;
    Bitmap *bitmap;
    b32 isScreenCoordinateSpace;
    v3 basePosition;

#pragma warning(push)
#pragma warning(disable : 4201)
    union {
        struct
        {
            real32 maxX;
            real32 maxY;
        };
        real32 max[2];
    };

    union {
        struct
        {
            real32 minX;
            real32 minY;
        };
        real32 min[2];
    };
#pragma warning(pop)

    Animation2D *sprites;
    u32 totalSprites;
    Direction frameDirection;

    bool isTextureUpsideDown;
    int size;
    RectType type;
    uint32 ttl; /* time to live / duration */
};

Rect *CreateRectangle(GameMemory *gm, v3 basePosition, v4 color, real32 width, real32 height);
void CreateVertices(Rect *rect);
void AssociateEntity(Rect *rect, Entity *entity, bool isTraversable);
inline void UpdateColors(Rect *r, v4 color);
void PushRectInfo(GameMemory *gm, Rect *rect);
void PushRectVertex(GameMemory *gm, Rect *rect);

static GLuint g_rectIndices[] = {
    // Note that we start from 0!
    0, 1, 3, // First Triangle
    1, 2, 3  // Second Triangle
};

inline void DrawRectangle()
{
    const int totalIndiciesFromEbo = 6;
    glDrawElements(GL_TRIANGLES, totalIndiciesFromEbo, GL_UNSIGNED_INT, nullptr);
}

inline void DrawRawRectangle(memory_index count)
{
    const u32 numOfPoints = 6;
    u32 totalVertices = SafeCastToU32(count) * numOfPoints;
    glDrawArrays(GL_TRIANGLES, 0, totalVertices);
}

inline void DrawRawPointRectangle(memory_index count)
{
    const u32 numOfPoints = 6;
    u32 totalVertices = SafeCastToU32(count) * numOfPoints;
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays(GL_POINTS, 0, totalVertices);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

inline void DrawPointRectangle()
{
    glEnable(GL_PROGRAM_POINT_SIZE);
    const int totalIndiciesFromEbo = 6;
    glDrawElements(GL_POINTS, totalIndiciesFromEbo, GL_UNSIGNED_INT, nullptr);

    glDisable(GL_PROGRAM_POINT_SIZE);
}

inline void DrawDebugRectangle()
{
    const int totalIndiciesFromEbo = 6;
    glDrawElements(GL_POINTS, totalIndiciesFromEbo, GL_UNSIGNED_INT, nullptr);
}

#endif
