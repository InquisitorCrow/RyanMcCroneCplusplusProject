#include "Apple.h"

// Apple contructor
Apple::Apple(cD3DManager* d3dMger, cD3DXSpriteMgr* d3dSrMger, int seed)
{
	d3dMgr = d3dMger;
	d3dSrMgr = d3dSrMger;
	appletexture = new cD3DXTexture(d3dMgr->getTheD3DDevice(),"Images\\apple.png");	// Fully load the texture
	srand((unsigned int)time(NULL)*seed); // Set the random number generator
	position = D3DXVECTOR3(1+(rand()% 800),-50,0); // Spawning location for the apples
}

void Apple::fall()
{ 
	position.y += 1; // Falling speed of the apples
	SetRect(&bounds,position.x,position.y,position.x+appletexture->getTWidth(),position.y+appletexture->getTHeight()); // Boundaries for the apples
}

void Apple::render()
{
	d3dSrMgr->drawSprite(appletexture->getTexture(),NULL,NULL,&position,0xFFFFFFFF); // Render the apples on screen
}

RECT Apple::boundingb()
{
	return bounds; // Return the boundaries of the apple
}