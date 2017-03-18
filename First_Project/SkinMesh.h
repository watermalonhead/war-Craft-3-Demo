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

	D3DXMATRIXA16 m_matMoveMatrices; //角色的移步矩阵
	float m_fHeight;           //地面高度,使角色位于地面
	float m_fRotateAngle;	   //角色旋转角度
	bool m_bMoving;			   //是否移动
	float m_fStartTime;
	string m_sAnimationSet;    //角色的当前的动画名称
	double m_dAnimationTimes;  //角色当前动画的动作时间
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
	VOID SetAnimationSet( std::string set, double times );//设置角色的当前动画
	VOID SetSize(D3DXVECTOR3 size);//设置角色的缩放
};