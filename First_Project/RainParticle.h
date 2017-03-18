#pragma once

#include <d3dx9.h>
#include <list>
#include "d3dUtility.h"

struct  RAINVERTEX
{
	D3DXVECTOR3  vPos;		 //���λ��
	float		fDspeed;	//����½��ٶ�

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
	LPDIRECT3DVERTEXBUFFER9    _pRainVB;              //������Ӷ��㻺����
	

	D3DXVECTOR3   _RainDir;              //�����½�����
	std::list<RAINVERTEX>  _RainList;
	BoundingBox _boundingBox;
};

