#include <cstdlib>
#include "Rain.h"

const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

Rain::Rain(BoundingBox* boundingBox, int numParticles)
{
	_boundingBox   = *boundingBox;//��Χ���������������ķ�Χ���������䵽�˷�Χ�⣬�ᱻɱ�������¸���
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
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,//ʹ���˶�̬���㻺��������Ϊÿһ֡�����Ƕ���Ҫ�������ǵ�����
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
	//true:��ʾ����ǰ��������ӳ�䵽�㾫���ϣ��������������꣩��false����ʾͨ������������ָ��texel�ķ�����������Ӧ�õ��㾫���ϡ�
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	//true:��ʾ��Ĵ�С��ͨ����ͼ����ϵ��λ��ָ������������������ԽԶ��ԽС��false����ʾͨ����Ļ����ϵ��λ��ָ����Ĵ�С��
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, true); 
	//ָ���㾫��ĳߴ磬�ܵ�D3DRS_POINTSCALEENABLE��Ӱ��
	_device->SetRenderState(D3DRS_POINTSIZE, FtoDw(_size));
	//ָ���㾫�����С�ߴ�
	_device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(2.3f));

	// control the size of the particle relative to distance ���Ƶ㾫��Ĵ�С���������ݾ���ı�ģ������ǵ㾫�鵽������ľ��룩
	//FinalSize = ViewportHeight(�ӿڸ߶�)*Size��POINTSIZE��*sqrt(1/(A + B*D + C*D*D))//D��ʾ��ͼ�ռ��е㾫�鵽������ľ���
 	_device->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
 	_device->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
 	_device->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));
		
	// use alpha from texture ����Alphaͨ���ܹ����������͸����
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


