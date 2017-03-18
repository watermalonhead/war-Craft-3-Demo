#pragma once

#include "AllocateHierarchy.h"

class CSkinMesh
{
private:
	cAllocateHierarchy*			m_alloc_hierarchy;
	IDirect3DDevice9*			m_device;
	D3DXFRAME*					m_root_frame;

public:
	D3DXVECTOR3					m_object_center;
	float						m_object_radius;
	bool						m_is_play_anim;
	ID3DXAnimationController*	m_anim_controller;

	D3DXMATRIXA16 m_matMoveMatrices; //��ɫ���Ʋ�����
	float m_fHeight;           //����߶�,ʹ��ɫλ�ڵ���
	float m_fRotateAngle;	   //��ɫ��ת�Ƕ�
	bool m_bMoving;			   //�Ƿ��ƶ�
	float m_fStartTime;
	string m_sAnimationSet;    //��ɫ�ĵ�ǰ�Ķ�������
	double m_dAnimationTimes;  //��ɫ��ǰ�����Ķ���ʱ��
	D3DXVECTOR3 m_v3Size;

private:
	HRESULT setup_bone_matrix_pointers(D3DXFRAME* frame);
	HRESULT setup_bone_matrix_pointers_on_mesh(D3DXMESHCONTAINER* base_mesh_container);	

	HRESULT	load_from_xfile(const char* filename);
	void update_frame_matrices(D3DXFRAME* base_frame, CONST D3DXMATRIX* parent_matrix);

	HRESULT draw_frame(CONST D3DXFRAME* frame);
	HRESULT draw_mesh_container(CONST D3DXMESHCONTAINER* base_mesh_container, CONST D3DXFRAME* base_frame);

public:
	HRESULT create(IDirect3DDevice9* device, const char* wfilename);
	HRESULT render(double app_elapsed_time);
	void destroy();

	CSkinMesh();
	virtual ~CSkinMesh();

	VOID SetAnim( bool bAnim );
	VOID SetRotateAngle( float fAngle );
	VOID SetTransHeight( float fheigh );
	VOID SetAnimationSet( std::string set, double times );//���ý�ɫ�ĵ�ǰ����
	VOID SetSize(D3DXVECTOR3 size);//���ý�ɫ������
};