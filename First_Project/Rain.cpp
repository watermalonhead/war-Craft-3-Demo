#include <cstdlib>
#include "Rain.h"

const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

Rain::Rain(BoundingBox* boundingBox, int numParticles)
{
	_boundingBox   = *boundingBox;//包围盒描述了雨点下落的范围，如果雨点落到了范围外，会被杀死并重新复活
	_size          = 6.5f;
	_vbSize        = 2048;
	_vbOffset      = 0; 
	_vbBatchSize   = 512; 
	
	for(int i = 0; i < numParticles; i++)
		addParticle();
}
Rain::~Rain()
{
	Release<IDirect3DVertexBuffer9*>(_vb);
	Release<IDirect3DTexture9*>(_tex);
}

bool Rain::init(IDirect3DDevice9* device, char* texFileName)
{
	// vertex buffer's size does not equal the number of particles in our system.  We
	// use the vertex buffer to draw a portion of our particles at a time.  The arbitrary
	// size we choose for the vertex buffer is specified by the _vbSize variable.

	_device = device; // save a ptr to the device

	HRESULT hr = 0;

	hr = device->CreateVertexBuffer(
		_vbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,//使用了动态顶点缓冲区，因为每一帧中我们都需要更新我们的粒子
		Particle::FVF,
		D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
		&_vb,
		0);
	
	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexBuffer() - FAILED", "PSystem", 0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(
		device,
		texFileName,
		&_tex);

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateTextureFromFile() - FAILED", "PSystem", 0);
		return false;
	}

	return true;
}

void Rain::reset()
{
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		resetParticle( &(*i) );
	}
}

void Rain::addParticle()
{
	Attribute attribute;
	resetParticle(&attribute);

	_particles.push_back(attribute);
}

void Rain::preRender()
{
	_device->SetRenderState(D3DRS_LIGHTING, false);
	//true:表示将当前纹理整个映射到点精灵上（不根据纹理坐标），false：表示通过纹理坐标来指定texel的方法来让纹理应用到点精灵上。
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	//true:表示点的大小将通过视图坐标系单位来指定。即相对摄像机粒子越远就越小。false：表示通过屏幕坐标系单位来指定点的大小。
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, true); 
	//指定点精灵的尺寸，受到D3DRS_POINTSCALEENABLE的影响
	_device->SetRenderState(D3DRS_POINTSIZE, FtoDw(_size));
	//指定点精灵的最小尺寸
	_device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(2.3f));

	// control the size of the particle relative to distance 控制点精灵的大小是怎样根据距离改变的（距离是点精灵到摄像机的距离）
	//FinalSize = ViewportHeight(视口高度)*Size（POINTSIZE）*sqrt(1/(A + B*D + C*D*D))//D表示视图空间中点精灵到摄像机的距离
 	_device->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
 	_device->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
 	_device->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));
		
	// use alpha from texture 启动Alpha通道能够设置纹理的透明度
	_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void Rain::postRender()
{
	//_device->SetRenderState(D3DRS_LIGHTING,          true);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE,  false);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,  false);
}

void Rain::render()
{
	if( !_particles.empty() )
	{
		preRender();
		
		_device->SetTexture(0, _tex);
		_device->SetFVF(Particle::FVF);
		_device->SetMaterial(&WHITE_MTRL);
		_device->SetStreamSource(0, _vb, 0, sizeof(Particle));

		if(_vbOffset >= _vbSize)
			_vbOffset = 0;

		Particle* v = 0;

		_vb->Lock(
			_vbOffset    * sizeof( Particle ),
			_vbBatchSize * sizeof( Particle ),
			(void**)&v,
			_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		std::list<Attribute>::iterator i;
		for(i = _particles.begin(); i != _particles.end(); i++)
		{
			if( i->_isAlive )
			{
				v->_position = i->_position;
				v->_color    = (D3DCOLOR)i->_color;
				v++; 

				numParticlesInBatch++; 


				if(numParticlesInBatch == _vbBatchSize) 
				{

					_vb->Unlock();

					_device->DrawPrimitive(D3DPT_POINTLIST,_vbOffset,_vbBatchSize);

					_vbOffset += _vbBatchSize; 

					if(_vbOffset >= _vbSize) 
						_vbOffset = 0;       

					_vb->Lock(
						_vbOffset    * sizeof( Particle ),
						_vbBatchSize * sizeof( Particle ),
						(void**)&v,
						_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch
				}	
			}
		}

		_vb->Unlock();
		
		if( numParticlesInBatch )
		{
			_device->DrawPrimitive(D3DPT_POINTLIST,_vbOffset,numParticlesInBatch);
		}


		_vbOffset += _vbBatchSize; 
		postRender();

	}
}

bool Rain::isEmpty()
{
	return _particles.empty();
}

bool Rain::isDead()
{
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		// is there at least one living particle?  If yes,
		// the system is not dead.
		if( i->_isAlive )
			return false;
	}
	// no living particles found, the system must be dead.
	return true;
}

void Rain::removeDeadParticles()
{
	std::list<Attribute>::iterator i;

	i = _particles.begin();

	while( i != _particles.end() )
	{
		if( i->_isAlive == false )
		{
			// erase returns the next iterator, so no need to
		    // incrememnt to the next one ourselves.
			i = _particles.erase(i); 
		}
		else
		{
			i++; // next in list
		}
	}
}

void Rain::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;

	// get random x, z coordinate for the position of the Rain flake.
	GetRandomVector(
		&attribute->_position,
		&_boundingBox._min,
		&_boundingBox._max);

	// no randomness for height (y-coordinate).  Rain flake
	// always starts at the top of bounding box.
	attribute->_position.y = _boundingBox._max.y; 

	// Rain flakes fall downwards and slightly to the left
	attribute->_velocity.x = GetRandomFloat(0.0f, 1.0f) * + 8.0f;
	attribute->_velocity.y = GetRandomFloat(0.0f, 1.0f) * - 50.0f;
	attribute->_velocity.z = 0.0f;

	// white Rain flake
	attribute->_color = WHITE;
}

void Rain::update(float timeDelta)
{
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		i->_position += i->_velocity * timeDelta;

		// is the point outside bounds?
		if( _boundingBox.isPointInside( i->_position ) == false ) 
		{
			// nope so kill it, but we want to recycle dead 
			// particles, so respawn it instead.
			resetParticle( &(*i) );
		}
	}
}


