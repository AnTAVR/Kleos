#ifndef __RECT_MANAGER__
#define __RECT_MANAGER__

struct RectDynamicArray {
    int size;
    int allocatedSize; /*not in bytes but amount of entities */
    Rect **rects;
};

#pragma warning(push)
#pragma warning (disable: 4201)
struct RectManager {

    union {
        struct {
            RectDynamicArray Traversable;
            RectDynamicArray NonTraversable;
        };

        RectDynamicArray rda[2];
    };

    Rect *player;
};
#pragma warning(pop)

RectDynamicArray *CreateRectDynamicArray(uint32 size = 15000)
{
    RectDynamicArray *rda = (RectDynamicArray*)malloc(sizeof(RectDynamicArray));
    memset(rda, 0, sizeof(RectDynamicArray));
    rda->allocatedSize = size;
    rda->rects = (Rect**)malloc(sizeof(Rect)*rda->allocatedSize);

    return rda;
}

RectManager *CreateRectManager()
{
    RectManager *rm = NULL;
    rm = (RectManager*)malloc(sizeof(RectManager));
    rm->rda[0] = *CreateRectDynamicArray();
    rm->rda[1] = *CreateRectDynamicArray();
    return rm;
}


void DeleteRectDynamicArray(RectDynamicArray *rda)
{
    if (!rda)
        return;

    free(rda->rects);

    printf("freeing size: %d\n", rda->size);
    free(rda);
}

void PushBack(RectDynamicArray *rda, Rect *rect)
{
    if (rda->allocatedSize <= rda->size) {
        PAUSE_HERE("I should not get here\n");
    }

    rda->rects[rda->size] = rect;
    rda->size++;
}
#endif
