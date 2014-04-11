/*
=================
main.cpp
Main entry point for the Game.
=================
*/

#include "GameConstants.h"
#include "GameResources.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"
#include "cSprite.h"
#include "cExplosion.h"
#include "cXAudio.h"
#include "cD3DXFont.h"
#include "Apple.h"
#include "windows.h"

using namespace std;

__int64 previousTime2; // A timer used in the game
bool changescreen; // Changing the screen that is currently displayed
int currentscreen; // The current screen that is being displayed
LPDIRECT3DSURFACE9 backgrounds[3]; // Three backgrounds. Menu, Game and End.
int seed = 0; // Standard seed for the random number generator. 
vector<Apple*> apple; // The apples that will appear on screen
vector<Apple*>::iterator appleit; // Number of collected Apples
HINSTANCE hInst; // global handle to hold the application instance
HWND wndHandle; // global variable to hold the window handle

// Get a reference to the DirectX Manager
static cD3DManager* d3dMgr = cD3DManager::getInstance();

// Get a reference to the DirectX Sprite renderer Manager 
static cD3DXSpriteMgr* d3dxSRMgr = cD3DXSpriteMgr::getInstance();

// Boundaries for the game
RECT clientBounds;

// The character limit
TCHAR szTempOutput[30];


// The maximum number of possible gathered apples
char gApplesGatheredStr[50];
int gApplesGathered = 0;

// Audio Files used in game
cXAudio gAppleget;
cXAudio gBGM;

// Texture, location and boundaries for the basket
cD3DXTexture* basketTexture;
D3DXVECTOR3 basketposition;
RECT basketbounds;

// Mouse location
POINT mouseXY;

/*
==================================================================
* LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
* LPARAM lParam)
* The window procedure
==================================================================
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check any available messages from the queue
	switch (message)
	{
// Screen will change and spawn the basket and apples upon pressing any key
	case WM_KEYDOWN:
		{
			if(currentscreen == 0)
			{
				gApplesGathered = 0;
				basketposition = D3DXVECTOR3(400-basketTexture->getTWidth()/2,300-basketTexture->getTHeight()/2,0);
				previousTime2 = 0;
				apple.clear();
				QueryPerformanceCounter((LARGE_INTEGER*)&previousTime2);
				currentscreen++;
			}
// If the end screen is detected, it will return the player to screen 0 on keypress

			if(currentscreen == 2)
			{
				currentscreen = 0;
			}

			return 0;
		}

// Spawns the basket for the player to collect apples with (on mouse location)
		case WM_LBUTTONDOWN:
			{
				if(currentscreen == 1)
				{
				mouseXY.x = LOWORD(lParam);
				mouseXY.y = HIWORD(lParam);
				basketposition = D3DXVECTOR3(mouseXY.x-64, mouseXY.y-32,0);
				}
				return 0;
			}


		case WM_CLOSE:
			{
			// Exit the Game
				PostQuitMessage(0);
				 return 0;
			}

		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
	}
	// Always return the message to the default window
	// procedure for further processing
	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
==================================================================
* bool initWindow( HINSTANCE hInstance )
* initWindow registers the window class for the application, creates the window
==================================================================
*/
bool initWindow( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	// Fill in the WNDCLASSEX structure. This describes how the window
	// will look to the system
	wcex.cbSize = sizeof(WNDCLASSEX); // the size of the structure
	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcex.hInstance = hInstance; // handle to the application instance
	wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MyWindowIcon)); // icon to associate with the application
	wcex.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_GUNSIGHT));// the default cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcex.lpszMenuName = NULL; // the resource name for the menu
	wcex.lpszClassName = "Apples"; // the class name being created
	wcex.hIconSm = LoadIcon(hInstance,"Balloon.ico"); // the handle to the small icon

	RegisterClassEx(&wcex);
	// Create the window
	wndHandle = CreateWindow("Apples",			// the window class to use
							 "Apple Drop!",			// the title bar text
							WS_OVERLAPPEDWINDOW,	// the window style
							CW_USEDEFAULT, // the starting x coordinate
							CW_USEDEFAULT, // the starting y coordinate
							800, // the pixel width of the window
							600, // the pixel height of the window
							NULL, // the parent window; NULL for desktop
							NULL, // the menu for the application; NULL for none
							hInstance, // the handle to the application instance
							NULL); // no values passed to the window
	// Make sure that the window handle that is created is valid
	if (!wndHandle)
		return false;
	// Display the window on the screen
	ShowWindow(wndHandle, SW_SHOW);
	UpdateWindow(wndHandle);
	return true;
}

/*
==================================================================
// This is winmain, the main entry point for Windows applications
==================================================================
*/
// think of w in main as the game class from c#
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	// Initialize the window
	if ( !initWindow( hInstance ) )
		return false;
	// called after creating the window
	if ( !d3dMgr->initD3DManager(wndHandle) )
		return false;
	if ( !d3dxSRMgr->initD3DXSpriteMgr(d3dMgr->getTheD3DDevice()))
		return false;

	// Grab the frequency of the high def timer
	__int64 freq = 0;				// measured in counts per second;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	float sPC = 1.0f / (float)freq;			// number of seconds per count

	__int64 currentTime = 0;				// current time measured in counts per second;
	__int64 previousTime = 0;				// previous time measured in counts per second;
	__int64 currentTime2 = 0;
	previousTime2 = 0;


	float numFrames   = 0.0f;				// Used to hold the number of frames
	float timeElapsed = 0.0f;				// cumulative elapsed time

	GetClientRect(wndHandle,&clientBounds);


	float fpsRate = 1.0f/25.0f;

	

	//====================================================================
	// Load four textures for the Apples; Also loads the background textures.
	//====================================================================
	
	srand (time(NULL)  );

	LPDIRECT3DSURFACE9 aSurface;				// the Direct3D surface
	LPDIRECT3DSURFACE9 theBackbuffer = NULL;  // This will hold the back buffer
	
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	// Create the background surface

	// load The font
	cD3DXFont* appleFont = new cD3DXFont(d3dMgr->getTheD3DDevice(),hInstance, "JI Balloon Caps");

	// Text position on screen
	RECT textPos;
	SetRect(&textPos, 50, 10, 550, 100);

	QueryPerformanceCounter((LARGE_INTEGER*)&previousTime);
	QueryPerformanceCounter((LARGE_INTEGER*)&previousTime2);

	// Make sure to change it to png for transparency
	// This is the loading of the basket texture and the location
	basketTexture = new cD3DXTexture(d3dMgr->getTheD3DDevice(),"Images\\basket.png");
	basketposition = D3DXVECTOR3(5,5,0);

	// Sets the screen to the main menu, and sets the screen values accordingly
	currentscreen = 0;
	backgrounds[0] = d3dMgr->getD3DSurfaceFromFile("Images\\Menu.png"); 
	backgrounds[1] = d3dMgr->getD3DSurfaceFromFile("Images\\Background.png");
	backgrounds[2] = d3dMgr->getD3DSurfaceFromFile("Images\\Gameover.jpg");

	// Plays the awesome background music.
	gBGM.playSound(L"Sounds\\BGM.wav",true);

	// this is essentially the update method from XNA.
	while( msg.message!=WM_QUIT )
	{
		// Check the message queue
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// Set the details for the basket
			SetRect(&basketbounds,basketposition.x,basketposition.y,basketposition.x+basketTexture->getTWidth(),basketposition.y+basketTexture->getTHeight());


			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			if(currentscreen == 1)
			{
				QueryPerformanceCounter((LARGE_INTEGER*)&currentTime2);
			}
			float dt = (currentTime - previousTime)*sPC;

			// Accumulate how much time has passed.
			timeElapsed += dt;

			// Allows the creation of apples
			if(currentscreen == 1)
			{
			if((currentTime2-previousTime2)*sPC>1)
			{
				apple.push_back(new Apple(d3dMgr, d3dxSRMgr, currentTime2));
				previousTime2=currentTime2;
			}

			// Destroys apples when collected by the basket, and increments the score by 1.
			for(appleit=apple.begin();
				appleit != apple.end();
				appleit++)
			{
				RECT T;
				if(IntersectRect(&T,&(*appleit)->boundingb(),&basketbounds))
				{
					appleit=apple.erase(appleit);
					gAppleget.playSound(L"Sounds\\Appleget.wav",false);
					gApplesGathered++;
					sprintf_s( gApplesGatheredStr, 50, "Apples Gathered : %d", gApplesGathered);
					break;
				}

				// ends the game when the player collects 15 apples
			if(gApplesGathered >= 15)
				{
			    currentscreen = 2;
				}

			}
			}

		
			//==============================================================
			//| Update the postion of the Apples and check for collisions
			//==============================================================

			// begin the drawing/falling of the apples
				for(appleit = apple.begin();
					appleit != apple.end();
					appleit++)
				{
					(*appleit)->fall();
				}


				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				// Loads the background
				d3dMgr->updateTheSurface(backgrounds[currentscreen], theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				
				d3dxSRMgr->beginDraw();

				// draws the apples
				if (currentscreen == 1)
				{
				for(appleit = apple.begin();
					appleit != apple.end();
					appleit++)
				{
					(*appleit)->render();
				}
				}

				// Draws the basket onscreen if the game screen is showing
				if (currentscreen == 1) 
				{
				d3dxSRMgr->drawSprite(basketTexture->getTexture(),NULL,NULL,&basketposition,0xFFFFFFFF);
				}
				d3dxSRMgr->endDraw();
				appleFont->printText(gApplesGatheredStr,textPos);
				d3dMgr->endRender();
				timeElapsed = 0.0f;

			}

			previousTime = currentTime;
		}
	}