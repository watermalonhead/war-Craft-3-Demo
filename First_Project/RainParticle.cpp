#include "RainParticle.h"
#include <Windows.h>
extern HWND g_hWnd;
#define SafeDelete(p) { if(p) { delete (p); (p)=NULL; } }
RainParticle::RainParticle(LPDIRECT3DDEVICE9 pDevice,BoundingBox* boundingBox,int iNum,D3DXVECTOR3 dir)
{
	_pD3DDevice = pDevice;
	_boundingBox  = *boundingBox;
	_pD3DDevice->CreateVertexBuffer( 2*sizeof(RAINVERTEX),
		0, D3DFVF_RAINVERTEX,
		D3DPOOL_MANAGED, &_pRainVB, NULL );

	//填充雨点顶点缓冲区
	RAINVERTEX*  pRainVertices;
	_pRainVB->Lock(0, 0, (void**)&pRainVertices, 0);
	pRainVertices[0].vPos = D3DXVECTOR3(1, 6, 0);
	pRainVertices[0].dwColor = 0x0fffffff;
	pRainVertices[1].vPos = D3DXVECTOR3(3, 1, 0);
	pRainVertices[1].dwColor = 0x0fffffff;
	_pRainVB->Unlock();

	//初始雨点下降方向
	_RainDir = dir;
	
	int temp,temp1,temp2;
	temp = 2*_boundingBox._max.x;
	temp1 = 2*_boundingBox._max.z;
	/*temp2 = _boundingBox._max.y - _boundingBox._min.y;*/
	for (int i = 0; i!= iNum; ++i)
	{	
		
		RAINVERTEX rv;
		//生成随机位置
		rv.vPos.x=float(rand()%temp - _boundingBox._max.x);
		rv.vPos.z=float(rand()%temp1 - _boundingBox._max.z);
		/*rv.vPos.y=0.0f+float(rand()%temp2);	*/	
		rv.vPos.y = _boundingBox._max.y;

		//随机下落速度
		rv.fDspeed = 17.0f + (rand()%2);


		_RainList.push_back( rv) ;

	}

}

RainParticle::~RainParticle(void)
{
	Release<LPDIRECT3DVERTEXBUFFER9>(_pRainVB);
}

void RainParticle::Render()
{
	
	if (!_RainList.empty())
	{
		std::list<RAINVERTEX>::iterator ir;
		for (ir = _RainList.begin();ir!=_RainList.end();++ir)
		{
			ir->vPos.x += ir->fDspeed * _RainDir.x;
			ir->vPos.y += ir->fDspeed * _RainDir.y;
			ir->vPos.z += ir->fDspeed * _RainDir.z;
			if( _boundingBox.isPointInside( ir->vPos ) == false )
				ResetParticle( ir );
			
			_pD3DDevice->SetTexture( 0, NULL );
			D3DXMATRIX matRain;
			D3DXMatrixTranslation(&matRain, ir->vPos.x, ir->vPos.y, ir->vPos.z);
			_pD3DDevice->SetTransform( D3DTS_WORLD,  &matRain );
			_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
			_pD3DDevice->SetStreamSource(0, _pRainVB, 0, sizeof(RAINVERTEX));
			_pD3DDevice->SetFVF(D3DFVF_RAINVERTEX);
			_pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, 1);

			_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
		}
	}

}

void RainParticle::ResetParticle(std::list<RAINVERTEX>::iterator i)
{
	int temp,temp1,temp2;
	temp = 2*_boundingBox._max.x;
	temp1 = 2*_boundingBox._max.z;
	//生成随机位置
	i->vPos.x=float(rand()%temp - _boundingBox._max.x);
	i->vPos.z=float(rand()%temp1 - _boundingBox._max.z);
	i->vPos.y=_boundingBox._max.y;
	//随机下落速度
	i->fDspeed = 17.0f + (rand()%2);
}