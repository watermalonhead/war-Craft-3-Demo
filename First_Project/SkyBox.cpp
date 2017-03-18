#include "skybox.h"


//Class SkyBoxVertx

SkyBoxVertex::SkyBoxVertex() : m_pos(0.0f, 0.0f, 0.0f), m_tex0(0.0f, 0.0f)
{
}

SkyBoxVertex::SkyBoxVertex(float x, float y, float z, float u, float v): m_pos(x, y, z), m_tex0(u, v)
{
}

SkyBoxVertex::SkyBoxVertex(const D3DXVECTOR3& p, const D3DXVECTOR2& uv) : m_pos(p), m_tex0(uv)
{
}


/////////////////////////////////////////////////////////////////////////////


CSkyBox::CSkyBox(LPDIRECT3DDEVICE9  g_pD3DDevice)
{
	m_pD3DDevice = g_pD3DDevice;
	m_pVB        = NULL;
	m_pIB        = NULL;
	memset(m_pTextrue,NULL,sizeof(m_pTextrue));
}

CSkyBox::~CSkyBox(void)
{
}

HRESULT CSkyBox::InitSkyBox(int scale)
{	
	if( FAILED( m_pD3DDevice->CreateVertexBuffer( 
		24*sizeof(SkyBoxVertex),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&m_pVB,
		0)))
	{
		return E_FAIL;
	}

	SkyBoxVertex* v = 0;
	m_pVB->Lock(0, 0, (void**)&v, 0);

	// positive x
	v[0] = SkyBoxVertex( 1.0f*scale, -1.0f*scale,  1.0f*scale, 0.0f, 1.0f);
	v[1] = SkyBoxVertex( 1.0f*scale,  1.0f*scale,  1.0f*scale, 0.0f, 0.0f);
	v[2] = SkyBoxVertex( 1.0f*scale,  1.0f*scale, -1.0f*scale, 1.0f, 0.0f);
	v[3] = SkyBoxVertex( 1.0f*scale, -1.0f*scale, -1.0f*scale, 1.0f, 1.0f);

	// negative x
	v[4] = SkyBoxVertex(-1.0f*scale, -1.0f*scale, -1.0f*scale, 0.0f, 1.0f);
	v[5] = SkyBoxVertex(-1.0f*scale,  1.0f*scale, -1.0f*scale, 0.0f, 0.0f);
	v[6] = SkyBoxVertex(-1.0f*scale,  1.0f*scale,  1.0f*scale, 1.0f, 0.0f);
	v[7] = SkyBoxVertex(-1.0f*scale, -1.0f*scale,  1.0f*scale, 1.0f, 1.0f);

	// positive y
	v[8]  = SkyBoxVertex(-1.0f*scale,  1.0f*scale,  1.0f*scale, 0.0f, 1.0f);
	v[9]  = SkyBoxVertex(-1.0f*scale,  1.0f*scale, -1.0f*scale, 0.0f, 0.0f); 
	v[10] = SkyBoxVertex( 1.0f*scale,  1.0f*scale, -1.0f*scale, 1.0f, 0.0f);
	v[11] = SkyBoxVertex( 1.0f*scale,  1.0f*scale,  1.0f*scale, 1.0f, 1.0f);

	// negative y
	v[12] = SkyBoxVertex(-1.0f*scale, -1.0f*scale, -1.0f*scale, 0.0f, 1.0f);
	v[13] = SkyBoxVertex(-1.0f*scale, -1.0f*scale,  1.0f*scale, 0.0f, 0.0f);
	v[14] = SkyBoxVertex( 1.0f*scale, -1.0f*scale,  1.0f*scale, 1.0f, 0.0f);
	v[15] = SkyBoxVertex( 1.0f*scale, -1.0f*scale, -1.0f*scale, 1.0f, 1.0f);

	// positive z
	v[16] = SkyBoxVertex(-1.0f*scale, -1.0f*scale,  1.0f*scale, 0.0f, 1.0f);
	v[17] = SkyBoxVertex(-1.0f*scale,  1.0f*scale,  1.0f*scale, 0.0f, 0.0f);
	v[18] = SkyBoxVertex( 1.0f*scale,  1.0f*scale,  1.0f*scale, 1.0f, 0.0f);
	v[19] = SkyBoxVertex( 1.0f*scale, -1.0f*scale,  1.0f*scale, 1.0f, 1.0f);

	// negative z
	v[20] = SkyBoxVertex( 1.0f*scale, -1.0f*scale, -1.0f*scale, 0.0f, 1.0f);
	v[21] = SkyBoxVertex( 1.0f*scale,  1.0f*scale, -1.0f*scale, 0.0f, 0.0f);
	v[22] = SkyBoxVertex(-1.0f*scale,  1.0f*scale, -1.0f*scale, 1.0f, 0.0f);
	v[23] = SkyBoxVertex(-1.0f*scale, -1.0f*scale, -1.0f*scale, 1.0f, 1.0f);

	m_pVB->Unlock();


	if( FAILED(m_pD3DDevice->CreateIndexBuffer( 36*sizeof(WORD),
												D3DUSAGE_WRITEONLY,
												D3DFMT_INDEX16,
												D3DPOOL_MANAGED,
												&m_pIB, 
												0)))
	{
		return E_FAIL;
	}

	WORD* g_Indices = 0;
	m_pIB->Lock(0, 0, (void**)&g_Indices, 0);

	// positive x
	g_Indices[0] = 0;	g_Indices[1] = 1;	g_Indices[2] = 2;
	g_Indices[3] = 0;	g_Indices[4] = 2;	g_Indices[5] = 3;

	// negative x
	g_Indices[6]  = 4;  g_Indices[7]  = 5;  g_Indices[8]  = 6; 
	g_Indices[9]  = 4;  g_Indices[10] = 6;  g_Indices[11] = 7;

	// positive y
	g_Indices[12] = 8;  g_Indices[13] = 9;  g_Indices[14] = 10;
	g_Indices[15] = 8;  g_Indices[16] = 10; g_Indices[17] = 11;

	// negative y
	g_Indices[18] = 12; g_Indices[19] = 13; g_Indices[20] = 14;
	g_Indices[21] = 12; g_Indices[22] = 14; g_Indices[23] = 15;

	// positive z
	g_Indices[24] = 16; g_Indices[25] = 17; g_Indices[26] = 18;
	g_Indices[27] = 16; g_Indices[28] = 18; g_Indices[29] = 19;

	// negative z
	g_Indices[30] = 20; g_Indices[31] = 21; g_Indices[32] = 22;
	g_Indices[33] = 20; g_Indices[34] = 22; g_Indices[35] = 23;

	m_pIB->Unlock();

	return S_OK;
}


bool  CSkyBox::SetTexture(const char *TextureFile, int flag)
{
	if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice,TextureFile,&m_pTextrue[flag])))
	{
		return false;
	}
	else
	{
		return true;
	}
}


void  CSkyBox::Render(D3DXVECTOR3 CamPos)
{
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING,false);//关闭灯光

	//防止z-buffer写入，主要是使天空盒看起来有无限远的结果
	m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,false);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, false);

	//利用夹取纹理寻址的方式来实现天空盒的无缝连接
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	//天空盒和摄影机一块运动
	D3DXMATRIX world;
	D3DXMatrixTranslation(&world,CamPos.x, CamPos.y, CamPos.z);
	m_pD3DDevice->SetTransform(D3DTS_WORLD,&world);


	m_pD3DDevice->SetStreamSource(0, m_pVB, 0, sizeof(SkyBoxVertex));
	m_pD3DDevice->SetFVF(D3DFVF_SKYBOX);
	m_pD3DDevice->SetIndices(m_pIB);

	for(int i = 0; i < 6; ++i)
	{
		m_pD3DDevice->SetTexture(0, m_pTextrue[i]);
		m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
			                               0,   // 将要绘制的索引缓冲区的起始地址
			                               i*4, //　索引数组中最小的索引值
			                               4,   // 要绘制的索引数组中的顶点数量
			                               i*6, // 从索引数组中的第几个元素开始绘制图元
			                               2);  // 绘制的图元数量
	}

	//恢复成默认的属性
    m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, true);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}



