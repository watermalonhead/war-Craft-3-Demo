#pragma once

#include <d3dx9.h>

struct SkyBoxVertex
{
	SkyBoxVertex();
	SkyBoxVertex(float x, float y, float z, float u, float v);
	SkyBoxVertex(const D3DXVECTOR3& p, const D3DXVECTOR2& uv);

	D3DXVECTOR3 m_pos;
	D3DXVECTOR2 m_tex0;
};

#define  D3DFVF_SKYBOX (D3DFVF_XYZ | D3DFVF_TEX1 )


class CSkyBox
{
private:
	LPDIRECT3DDEVICE9            m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9      m_pVB;
	LPDIRECT3DINDEXBUFFER9       m_pIB;
	LPDIRECT3DTEXTURE9           m_pTextrue[6];

public:
	CSkyBox(LPDIRECT3DDEVICE9  g_pD3DDevice);
	~CSkyBox(void);
	HRESULT  InitSkyBox(int scale);
	void     Render(D3DXVECTOR3 CamPos);
	bool     SetTexture( const char *TextureFile, int flag );
};

