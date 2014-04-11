#ifndef _APPLE_H
#define _APPLE_H
#include "GameConstants.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"

// This is the header file for the apple class
class Apple
{
private:
D3DXVECTOR3 position; // Allow the use of positions
RECT bounds; // Allow boundaries to be set
float fallingspeed; // Set the falling speed of the apples
cD3DManager* d3dMgr;
cD3DXSpriteMgr* d3dSrMgr;
cD3DXTexture* appletexture; // Set the texture of the apples

public:
Apple(cD3DManager* d3dMgr, cD3DXSpriteMgr* d3dSrMgr, int seed);
void fall();
void render();

RECT boundingb();


};


#endif
