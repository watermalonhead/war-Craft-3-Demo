#pragma once

#include "d3dUtility.h"
#include <list>
using namespace std;

class Particle//���ӵ���Ⱦ���ԣ���Ⱦ����ʱʹ�ã�
{
public:
		D3DXVECTOR3 _position;
		D3DCOLOR    _color;
		static const DWORD FVF;
};
	
class Attribute//���ӵ��������ԣ����������ټ���������ʱ��ʹ�ã�
{
public:
	Attribute()
	{
		_lifeTime = 0.0f;
		_age      = 0.0f;
		_isAlive  = true;
	}

	D3DXVECTOR3 _position;   //λ��  
	D3DXVECTOR3 _velocity;   //�ٶ�
	D3DXVECTOR3 _acceleration; //���ٶ�
	float       _lifeTime;     //����������֮ǰ��������  
	float       _age;          //���ӵĵ�ǰ����
	D3DXCOLOR   _color;        //���ӵ���ɫ
	D3DXCOLOR   _colorFade;    //��������ʱ����ɫ
	bool        _isAlive;      //�Ƿ���
};

/*
����ϵͳ�࣬������¡���ʾ��ɱ������������
*/
class Rain
{
public:
	Rain(BoundingBox* boundingBox, int numParticles);
	 ~Rain();

	//��ʼ���������������㻺��������������
	 bool init(IDirect3DDevice9* device, char* texFileName);
	//����ϵͳ��ÿ�����ӵ�����
	 void reset();
	
	// sometimes we don't want to free the memory of a dead particle,
	// but rather respawn it instead.
	 void resetParticle(Attribute* attribute);//����һ�����ӵ����ԣ��ú������Ϊ���麯������ʵ������������
	//���һ�����ӵ�ϵͳ�С�
	 void addParticle();

	 void update(float timeDelta);//����ϵͳ�е��������ӣ���ʵ�������ھ�������

	 void preRender();//���ó�ʼ��Ⱦ״̬������������Ⱦ֮ǰ������
	 void render();//��ʾϵͳ�е���������
	 void postRender();//�ָ�ĳЩ��Ⱦ״̬

	bool isEmpty();//�Ƿ�û������
	bool isDead();//�Ƿ��������Ӷ�������


	 void removeDeadParticles();//�Ƴ�����������

protected:
	IDirect3DDevice9*       _device;
	D3DXVECTOR3             _origin;
	BoundingBox        _boundingBox;
	float                   _emitRate;   // rate new particles are added to system
	float                   _size;       // size of particles
	IDirect3DTexture9*      _tex;
	LPDIRECT3DVERTEXBUFFER9 _vb;
	std::list<Attribute>    _particles;
	int                     _maxParticles; // max allowed particles system can have

	//
	// Following three data elements used for rendering the p-system efficiently
	//

	DWORD _vbSize;      // size of vb
	DWORD _vbOffset;    // offset in vb to lock   
	DWORD _vbBatchSize; // number of vertices to lock starting at _vbOffset
};
