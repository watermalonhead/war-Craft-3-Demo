/*********************************************************
*           class  SPRITE
*           author 詹伟龙
*               V 1.0
**********************************************************/
#pragma once
extern const int Width;
extern const int Height;
#ifndef SPRITE_H
#define SPRITE_H

#include "SkinMesh.h"
#include "d3dUtility.h"

typedef WORD        ACTION;//边界动作
const ACTION  A_STOP      = 0,//停止
			  A_MOVE      = 1,//移动		
			  A_MOVE2     = 2,//巡逻
			  A_ATTACK    = 3,//攻击	
			  A_SPELL     = 4,//施法
			  A_DEATH     = 5,//死亡
			  A_DISSIPATE = 6,//消散
			  A_HIDE      = 7;//隐藏



class Sprite
{
public:
	CSkinMesh*  m_SkinMesh;      //角色骨骼对象
	ACTION      m_Action;        //动作类型
	D3DXVECTOR3 m_V3Speed;       //速度
	D3DXVECTOR3 m_V3EndPoint;    //目标点
	float       m_fMaxMoveStep;  //移动最大步长
	float       m_fView;         //视线的范围
	int         m_iTime;         //时间用于控制AI 或 魔法回复
	int         m_iMaxHp;        //最大生命值
	int         m_iNowHp;        //当前生命值
	int         m_iMaxAttackLen; //最大攻击范围
	int         m_iMaxAttackNum; //最大伤害值
	int      	m_iAttackTime;   //攻击时间控制
	int         m_iMaxMp;        //魔法的最大值
	int         m_iNowMp;        //魔法的当前值
	int         m_iKind;         //敌人的类型
	//BoundingSphere m_sphere;     //边界球
	//D3DXMATRIX g_mFirePDMoveMatrix;
	//float g_fFirePDAngle = 0.0f;
public:
	Sprite(IDirect3DDevice9* device, const char* wfilename, D3DXVECTOR3 size, 
		string action, double actiontime, float fheight, int maxlive);
	~Sprite();


	void SetSpeed(D3DXVECTOR3 NewSpeed);
	void SetAction(WORD action);
	void SetAnim( bool bAnim );
	void SetRotateAngle( float fAngle );
	void SetRotateAngle( D3DXVECTOR3 point );
	void SetEndPoint(D3DXVECTOR3 endpoint);
	void SetTransHeight( float fheigh );
	void SetAnimationSet( std::string set, double times );//设置角色的当前动画
	void SetPosition(POINT pPosition);//设置角色位置
	void SetView(float view);
	float GetView(){return m_fView;}
	D3DXVECTOR3 GetSpeed(){return m_V3Speed;}
	WORD GetAction(){return m_Action;}
	float GetRotateAngle(){return m_SkinMesh->m_fRotateAngle;}
	void Render(double app_elapsed_time);
	POINT GetPosition();//设置角色位置
	void UpDate();
	bool Move();
	void Attack(Sprite* Target);
	//可视范围
	bool FieldOfView(POINT pos);
	//碰撞检测
	bool AttackTest(POINT pos);
};


inline void Sprite::SetSpeed(D3DXVECTOR3 NewSpeed)
{
	m_V3Speed = NewSpeed;
}
inline void Sprite::SetRotateAngle( float fAngle )
{
	m_SkinMesh->SetRotateAngle(fAngle);
}
inline void Sprite::SetRotateAngle(D3DXVECTOR3 point )
{
	//计算旋转的角度
	float fAngle = 2 * D3DX_PI - atan2( point.z - m_SkinMesh->m_matMoveMatrices._43, 
		point.x - m_SkinMesh->m_matMoveMatrices._41);
	m_SkinMesh->SetRotateAngle(fAngle);
}
inline void Sprite::SetTransHeight( float fheigh )
{
	m_SkinMesh->SetTransHeight(fheigh);
}
inline void Sprite::SetAnim( bool bAnim )
{
	m_SkinMesh->SetAnim(bAnim);
}
inline void Sprite::SetAnimationSet( std::string set, double times )
{
	//设置角色的当前动画
	m_SkinMesh->SetAnimationSet(set, times);
}
inline void Sprite::SetAction(WORD action)
{
	m_Action = action;
}
inline void Sprite::SetEndPoint(D3DXVECTOR3 endpoint)
{
	m_V3EndPoint = endpoint;
	//设置旋转的角度
	SetRotateAngle(m_V3EndPoint);
	SetAction(A_MOVE);
}
inline void Sprite::SetPosition(POINT pPosition)
{
	m_SkinMesh->m_matMoveMatrices._41 = pPosition.x;
	m_SkinMesh->m_matMoveMatrices._43 = pPosition.y;
}

inline POINT Sprite::GetPosition()
{
	POINT newpoint;
	newpoint.x = m_SkinMesh->m_matMoveMatrices._41;
	newpoint.y = m_SkinMesh->m_matMoveMatrices._43;
	return newpoint;
}
inline void Sprite::SetView(float view)
{
	m_fView = view;
}

#endif