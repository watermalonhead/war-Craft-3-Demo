#pragma once

#include <d3dx9.h>
#include <list>
#include "d3dUtility.h"

struct  RAINVERTEX
{
	D3DXVECTOR3  vPos;		 //雨点位置
	float		fDspeed;	//雨点下降速度

	DWORD        dwColor;
};
#define D3DFVF_RAINVERTEX   (D3DFVF_XYZ|D3DFVF_DIFFUSE)

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }


class RainParticle
{
public:
	RainParticle(LPDIRECT3DDEVICE9 pDevice,BoundingBox* boundingBox,int iNum,D3DXVECTOR3 dir);
	~RainParticle(void);
	void Render();

	D3DXVECTOR3 GetDir(void){return _RainDir;};
	void ResetParticle(std::list<RAINVERTEX>::iterator i);
	void SetDir(D3DXVECTOR3 dir){ _RainDir = dir;};

private:
	LPDIRECT3DDEVICE9          _pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9    _pRainVB;              //雨点粒子顶点缓冲区
	

	D3DXVECTOR3   _RainDir;              //雨点的下降方向
	std::list<RAINVERTEX>  _RainList;
	BoundingBox _boundingBox;
};

