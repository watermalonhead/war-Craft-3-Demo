#pragma once

#include <d3dx9.h>

/************************************************************************/
/* 公告板                                                               */
/************************************************************************/

class BillBoard
{
public:
	BillBoard();
	~BillBoard();

	HRESULT Init(IDirect3DDevice9* pd3dDevice);
	HRESULT LoadTexture(IDirect3DDevice9 *pd3dDevice,UINT index,LPCSTR sthPath);

	HRESULT Render(int Index);
	HRESULT Render(int Index,RECT range); //只渲染图像的指定区域

	D3DXVECTOR2 SetPosition(D3DXVECTOR2*pos);	
	D3DXVECTOR2* GetPosition();

	float SetAngle(float angle);
	float GetAngle();

	D3DXVECTOR2 SetSize(D3DXVECTOR2*size);
	D3DXVECTOR2* GetSize();

	void SetIsRender(bool isrender);
	bool IsRender();

	void OnLostDevice();
	void Destory();

private:
	UINT m_TexIndex;
	ID3DXSprite* m_pPiBillBoard;
	LPDIRECT3DTEXTURE9 m_pTex[50]; //纹理数组的大小由UI图片的数量决定，不够就改大
	float m_angle;
	bool m_isRender;
	D3DXVECTOR2 m_size;	
	D3DXVECTOR2 m_position;
	int m_textureNum;
};