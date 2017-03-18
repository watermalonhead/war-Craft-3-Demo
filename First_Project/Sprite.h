/*********************************************************
*           class  SPRITE
*           author ղΰ��
*               V 1.0
**********************************************************/
#pragma once
extern const int Width;
extern const int Height;
#ifndef SPRITE_H
#define SPRITE_H

#include "SkinMesh.h"
#include "d3dUtility.h"

typedef WORD        ACTION;//�߽綯��
const ACTION  A_STOP      = 0,//ֹͣ
			  A_MOVE      = 1,//�ƶ�		
			  A_MOVE2     = 2,//Ѳ��
			  A_ATTACK    = 3,//����	
			  A_SPELL     = 4,//ʩ��
			  A_DEATH     = 5,//����
			  A_DISSIPATE = 6,//��ɢ
			  A_HIDE      = 7;//����



class Sprite
{
public:
	CSkinMesh*  m_SkinMesh;      //��ɫ��������
	ACTION      m_Action;        //��������
	D3DXVECTOR3 m_V3Speed;       //�ٶ�
	D3DXVECTOR3 m_V3EndPoint;    //Ŀ���
	float       m_fMaxMoveStep;  //�ƶ���󲽳�
	float       m_fView;         //���ߵķ�Χ
	int         m_iTime;         //ʱ�����ڿ���AI �� ħ���ظ�
	int         m_iMaxHp;        //�������ֵ
	int         m_iNowHp;        //��ǰ����ֵ
	int         m_iMaxAttackLen; //��󹥻���Χ
	int         m_iMaxAttackNum; //����˺�ֵ
	int      	m_iAttackTime;   //����ʱ�����
	int         m_iMaxMp;        //ħ�������ֵ
	int         m_iNowMp;        //ħ���ĵ�ǰֵ
	int         m_iKind;         //���˵�����
	//BoundingSphere m_sphere;     //�߽���
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
	void SetAnimationSet( std::string set, double times );//���ý�ɫ�ĵ�ǰ����
	void SetPosition(POINT pPosition);//���ý�ɫλ��
	void SetView(float view);
	float GetView(){return m_fView;}
	D3DXVECTOR3 GetSpeed(){return m_V3Speed;}
	WORD GetAction(){return m_Action;}
	float GetRotateAngle(){return m_SkinMesh->m_fRotateAngle;}
	void Render(double app_elapsed_time);
	POINT GetPosition();//���ý�ɫλ��
	void UpDate();
	bool Move();
	void Attack(Sprite* Target);
	//���ӷ�Χ
	bool FieldOfView(POINT pos);
	//��ײ���
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
	//������ת�ĽǶ�
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
	//���ý�ɫ�ĵ�ǰ����
	m_SkinMesh->SetAnimationSet(set, times);
}
inline void Sprite::SetAction(WORD action)
{
	m_Action = action;
}
inline void Sprite::SetEndPoint(D3DXVECTOR3 endpoint)
{
	m_V3EndPoint = endpoint;
	//������ת�ĽǶ�
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