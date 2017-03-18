#pragma once
#include<d3dx9.h>
#include<Mmsystem.h>
#include"d3dUtility.h"
#include <string>

#include"terrain.h"
#include"camera.h"
#include"SkyBox.h"
#include"Sprite.h"  //mesh + animate
#include"sRay.h"
#include"input.h"
#include"RainParticle.h"
#include"BillBoard.h"
#include<process.h>
//外部链接库
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment( lib, "dxguid.lib" )
#pragma comment(lib, "Dsound.lib")


//DEFINE 
#define KEYDOWN(vk_code)((GetAsyncKeyState(vk_code)&0x8000)?1:0)
#define KEYUP(vk_code)((GetAsyncKeyState(vk_code)&0x8000)?0:1)

#define SAFE_RELEASE(object){if(object){object->Release();object=NULL;}}
#define SAFE_DELETE(object){if(object){delete object;object=NULL;}}
#define SAFE_DELARRAY(object){if(object){delete[] object;object=NULL;}}

//桌面分辨率的宽和高
const int Width = GetSystemMetrics(SM_CXSCREEN);
const int Height = GetSystemMetrics(SM_CYSCREEN);