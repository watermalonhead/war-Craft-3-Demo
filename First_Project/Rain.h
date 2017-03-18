#pragma once

#include "d3dUtility.h"
#include <list>
using namespace std;

class Particle//粒子的渲染属性（渲染粒子时使用）
{
public:
		D3DXVECTOR3 _position;
		D3DCOLOR    _color;
		static const DWORD FVF;
};
	
class Attribute//粒子的其他属性（创建、销毁及更新粒子时，使用）
{
public:
	Attribute()
	{
		_lifeTime = 0.0f;
		_age      = 0.0f;
		_isAlive  = true;
	}

	D3DXVECTOR3 _position;   //位置  
	D3DXVECTOR3 _velocity;   //速度
	D3DXVECTOR3 _acceleration; //加速度
	float       _lifeTime;     //粒子在死亡之前的生命期  
	float       _age;          //粒子的当前年龄
	D3DXCOLOR   _color;        //粒子的颜色
	D3DXCOLOR   _colorFade;    //粒子消亡时的颜色
	bool        _isAlive;      //是否存活
};

/*
粒子系统类，负责更新、显示、杀死及创建粒子
*/
class Rain
{
public:
	Rain(BoundingBox* boundingBox, int numParticles);
	 ~Rain();

	//初始化函数，创建顶点缓冲区，创建纹理
	 bool init(IDirect3DDevice9* device, char* texFileName);
	//重置系统中每个粒子的属性
	 void reset();
	
	// sometimes we don't want to free the memory of a dead particle,
	// but rather respawn it instead.
	 void resetParticle(Attribute* attribute);//重置一个粒子的属性，该函数设计为纯虚函数，其实现依赖于子类
	//添加一个粒子到系统中。
	 void addParticle();

	 void update(float timeDelta);//更新系统中的所有粒子，其实现依赖于具体子类

	 void preRender();//设置初始渲染状态，它必须在渲染之前被调用
	 void render();//显示系统中的所有粒子
	 void postRender();//恢复某些渲染状态

	bool isEmpty();//是否没有粒子
	bool isDead();//是否所有粒子都死亡了


	 void removeDeadParticles();//移除死亡的粒子

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
