#include "d3dUtility.h"
#include "BillBoard.h"

BillBoard::BillBoard()
{
	m_size.x=0;
	m_size.y=0;
	m_angle=0;
	m_position.x=0;
	m_position.y=0;
	m_textureNum =0;
	m_isRender=false;
}

BillBoard::~BillBoard()
{
}

HRESULT BillBoard::Init(IDirect3DDevice9 *pd3dDevice)
{
	HRESULT hr;
	hr = D3DXCreateSprite( pd3dDevice, &m_pPiBillBoard );
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT BillBoard::LoadTexture(IDirect3DDevice9 *pd3dDevice,UINT index,LPCSTR sthPath)
{
	HRESULT hr;
	hr = D3DXCreateTextureFromFile(pd3dDevice,sthPath,&m_pTex[index]);
	if (FAILED(hr))
		return hr;
	
	return S_OK;
}

void BillBoard::OnLostDevice()
{
	Release(m_pPiBillBoard);
}

void BillBoard::Destory()
{
	if(m_pTex == NULL) 
		return;

	for(int i=0; i!=m_textureNum;++i)
	{
		if(m_pTex[i]!=NULL)
			Release(m_pTex[i]);
	}
}

HRESULT BillBoard::Render(int Index)
{
	HRESULT hr;
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixTransformation2D(&mat, NULL, m_angle, &m_size,NULL, 0, &m_position);  
	m_pPiBillBoard->SetTransform(&mat);

	m_pPiBillBoard->Begin(D3DXSPRITE_ALPHABLEND );
	hr=m_pPiBillBoard->Draw(m_pTex[Index],NULL,NULL,NULL,0xffffffff);
	m_pPiBillBoard->End();

	return hr;
}

HRESULT BillBoard::Render(int Index,RECT range)
{
	HRESULT hr;
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixTransformation2D(&mat, NULL, NULL, &m_size,&D3DXVECTOR2(16.0f,16.0f), m_angle, &m_position);  
	m_pPiBillBoard->SetTransform(&mat);

	m_pPiBillBoard->Begin(D3DXSPRITE_ALPHABLEND );
	hr=m_pPiBillBoard->Draw(m_pTex[Index],&range,NULL,NULL,0xffffffff);
	m_pPiBillBoard->End();

	return hr;
}

D3DXVECTOR2 BillBoard::SetPosition(D3DXVECTOR2*pos)
{
	D3DXVECTOR2 temp;
	temp = m_position;
	m_position =* pos;
	return temp;
}

D3DXVECTOR2* BillBoard::GetPosition()
{
	return &m_position;
}

float BillBoard::SetAngle(float angle)
{
	float temp=m_angle;
	m_angle=angle;
	return temp;
}

float BillBoard::GetAngle()
{
	return m_angle;
}

D3DXVECTOR2 BillBoard::SetSize(D3DXVECTOR2*size)
{
	D3DXVECTOR2 temp;
	temp=m_size;
	m_size=*size;
	return temp;
}

D3DXVECTOR2 *BillBoard::GetSize()
{
	return &m_size;
}

void BillBoard::SetIsRender(bool isrender)
{
	m_isRender=isrender;
}

bool BillBoard::IsRender()
{
	return m_isRender;
}