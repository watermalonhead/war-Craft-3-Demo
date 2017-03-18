#include "Sprite.h"
Sprite::Sprite(IDirect3DDevice9* device, const char* wfilename, D3DXVECTOR3 size, 
			   string action, double actiontime, float fheight, int maxlive)
{
	m_SkinMesh = new CSkinMesh();
	m_SkinMesh->create(device, wfilename);
	m_SkinMesh->SetAnimationSet(action, actiontime);
	m_SkinMesh->SetTransHeight(fheight);
	m_SkinMesh->SetSize(size);
	m_V3Speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_V3EndPoint = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fMaxMoveStep = 25.0;
	m_Action = A_STOP;
	m_fView = 250.0f;
	m_iTime = 65;
    m_iAttackTime = 0;
	m_iMaxHp = m_iNowHp = maxlive;
	m_iMaxMp = 500;
	m_iNowMp = 500;
	//ComputeBoundingSphere();
	m_iMaxAttackLen = 40;
	m_iMaxAttackNum = 50;
}

void Sprite::Render(double app_elapsed_time)
{
	m_SkinMesh->render(app_elapsed_time);
}

void Sprite::UpDate()
{
	if (m_Action == A_MOVE)
	{	
		if (Move())
		{
			
		}
	}
	if (m_Action == A_DEATH && m_iTime == 3)
	{
		m_Action = A_DISSIPATE;
		m_iTime = 0;
	}
	if (m_Action == A_DISSIPATE && m_iTime == 8)
	{
		m_Action = A_HIDE;
	}
	if (m_Action == A_ATTACK)
	{
		(++ m_iAttackTime) %= 60;
	}
	else
	{
		m_iAttackTime = 0;
	}
	//移动
	D3DXMATRIX matStep;
	D3DXMatrixTranslation(&matStep, m_V3Speed.x*0.015, 0, m_V3Speed.z*0.015);
	m_SkinMesh->m_matMoveMatrices = m_SkinMesh->m_matMoveMatrices * matStep; //修改角色位置
	SetSpeed(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	if (abs( m_V3EndPoint.x- m_SkinMesh->m_matMoveMatrices._41)<20&&abs( m_V3EndPoint.z- m_SkinMesh->m_matMoveMatrices._43)<20) //变换不精确变换后的位置与endpoint近视等于就当作到达目的地
	{
		if (m_Action==A_MOVE||m_Action==A_MOVE2)
		{
			SetAction(0); //到达目的地后停止，切换停止动画
		}
	}
	(++m_iTime) %= 120;//控制时间更新
	//魔法自动回复
	if(m_iTime % 32 == 0 && m_iNowMp < m_iMaxMp)
	{
		m_iNowMp ++;
	}
}

bool Sprite::Move()
{
	bool EndMove = false;//移动标记
	//计算精灵到目标点的距离
	D3DXVECTOR3 Len = m_V3EndPoint - 
		D3DXVECTOR3(m_SkinMesh->m_matMoveMatrices._41, 0.0f, m_SkinMesh->m_matMoveMatrices._43);
	//计算当前速度
	float length = (Len.x * Len.x + Len.z * Len.z);
	length = sqrt(length);
	
		m_V3Speed.x = length * cosf(2 * D3DX_PI - m_SkinMesh->m_fRotateAngle);
		m_V3Speed.z = length * sinf(2 * D3DX_PI - m_SkinMesh->m_fRotateAngle);
		m_V3Speed.y = 0;


		EndMove = true;

	////移动
	//D3DXMATRIX matStep;
	//D3DXMatrixTranslation(&matStep, m_V3Speed.x, 0, m_V3Speed.z);
	//m_SkinMesh->m_matMoveMatrices = m_SkinMesh->m_matMoveMatrices * matStep;
	//m_SkinMesh->m_matMoveMatrices._41 += m_V3Speed.x;
	//m_SkinMesh->m_matMoveMatrices._43 += m_V3Speed.z;
	return EndMove;
}

bool Sprite::FieldOfView(POINT pos)
{
	double length = sqrt((pos.y - m_SkinMesh->m_matMoveMatrices._43) * (pos.y - m_SkinMesh->m_matMoveMatrices._43) + 
		(pos.x - m_SkinMesh->m_matMoveMatrices._41) * (pos.x - m_SkinMesh->m_matMoveMatrices._41));

	if (length < m_fView / 2)
	{
		return true; 
	}
	if (length > m_fView)
	{
		return false;
	}

	float fAngle = 2 * D3DX_PI - atan2( pos.y - m_SkinMesh->m_matMoveMatrices._43, 
		pos.x - m_SkinMesh->m_matMoveMatrices._41);
	if (fabs(fAngle - m_SkinMesh->m_fRotateAngle) < D3DX_PI / 3.0f)
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool Sprite::AttackTest(POINT pos)
{
	double length = sqrt((pos.y - m_SkinMesh->m_matMoveMatrices._43) * (pos.y - m_SkinMesh->m_matMoveMatrices._43) + 
		(pos.x - m_SkinMesh->m_matMoveMatrices._41) * (pos.x - m_SkinMesh->m_matMoveMatrices._41));
	if (length > m_iMaxAttackLen)
	{
		return false;
	}
	if (length < m_iMaxAttackLen/2)
	{
		return true;
	}
	//计算攻击的角度
	float fAngle = 2 * D3DX_PI - atan2( pos.y - m_SkinMesh->m_matMoveMatrices._43, 
		pos.x - m_SkinMesh->m_matMoveMatrices._41);
	if (fabs(fAngle - m_SkinMesh->m_fRotateAngle) < D3DX_PI / 3.0f)
	{
		return true;
	}
	else 
	{
		return false;
	}

}

Sprite::~Sprite()
{
	Delete(m_SkinMesh);
}

void Sprite::Attack(Sprite* Target)
{
		Target->m_iNowHp -= m_iMaxAttackNum;
		if (Target->m_iNowHp<=0)
		{
			Target->m_iNowHp = 0;
		}

}
