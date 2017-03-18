#include "SkinMesh.h"


#define IDLE_TRANSITION_TIME 0.125f
#define MOVE_TRANSITION_TIME 0.25f 



CSkinMesh::CSkinMesh()
{
	m_is_play_anim	  = true;
	m_device		  = NULL;
	m_anim_controller = NULL;
	m_root_frame	  = NULL;

	m_alloc_hierarchy = new cAllocateHierarchy();
	
	m_fStartTime=(float)timeGetTime();
	m_bMoving=false;
	m_fRotateAngle=0;
	m_sAnimationSet = "";
	m_v3Size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	D3DXMatrixIdentity(&m_matMoveMatrices);  //角色的移步矩阵单位化
}

CSkinMesh::~CSkinMesh()
{
	D3DXFrameDestroy(m_root_frame, m_alloc_hierarchy);
	SAFE_RELEASE(m_anim_controller);

	delete m_alloc_hierarchy;
}


HRESULT CSkinMesh::create(IDirect3DDevice9* device, const char* filename)
{
	m_device = device;

	load_from_xfile(filename);

	return S_OK;
}


void CSkinMesh::destroy()
{
	delete this;
}


HRESULT CSkinMesh::setup_bone_matrix_pointers(D3DXFRAME* frame)
{	
	HRESULT hr;

	if(frame == NULL)
		return S_OK;

	if(frame->pMeshContainer != NULL)
	{
		hr = setup_bone_matrix_pointers_on_mesh(frame->pMeshContainer);

		if(FAILED(hr))
			return hr;
	}

	if(frame->pFrameSibling != NULL)
	{
		hr = setup_bone_matrix_pointers(frame->pFrameSibling);

		if(FAILED(hr))
			return hr;
	}

	if(frame->pFrameFirstChild != NULL)
	{
		hr = setup_bone_matrix_pointers(frame->pFrameFirstChild);

		if(FAILED(hr))
			return hr;
	}
	
	return S_OK;	
}


HRESULT CSkinMesh::setup_bone_matrix_pointers_on_mesh(D3DXMESHCONTAINER* base_mesh_container)
{
	if(base_mesh_container == NULL)
		return S_OK;

	D3DXMESHCONTAINER_DERIVED* mesh_container = (D3DXMESHCONTAINER_DERIVED*) base_mesh_container;

	if(mesh_container->pSkinInfo != NULL)
	{
		UINT num_bones = mesh_container->pSkinInfo->GetNumBones();
		mesh_container->ppBoneMatrices = new D3DXMATRIX*[num_bones];

		if(mesh_container->ppBoneMatrices == NULL)
			return E_OUTOFMEMORY;

		for(UINT i = 0; i < num_bones; i++)
		{
			LPCSTR bone_name = mesh_container->pSkinInfo->GetBoneName(i);
			D3DXFRAME_DERIVED* frame = (D3DXFRAME_DERIVED*) D3DXFrameFind(m_root_frame, bone_name);

			if(frame == NULL)
				return E_FAIL;

			mesh_container->ppBoneMatrices[i] = &frame->CombinedTransformMatrix;
		}
	}

	return S_OK;
}


HRESULT CSkinMesh::load_from_xfile(const char* filename)
{
	D3DXLoadMeshHierarchyFromX(filename, D3DXMESH_MANAGED, m_device, m_alloc_hierarchy, NULL, &m_root_frame, &m_anim_controller);

	setup_bone_matrix_pointers(m_root_frame);

	D3DXFrameCalculateBoundingSphere(m_root_frame, &m_object_center, &m_object_radius);

	return S_OK;
}


void CSkinMesh::update_frame_matrices(D3DXFRAME* base_frame, CONST D3DXMATRIX* parent_matrix)
{
	D3DXFRAME_DERIVED* frame = (D3DXFRAME_DERIVED*) base_frame;

	if(parent_matrix != NULL)
		D3DXMatrixMultiply(&frame->CombinedTransformMatrix, &frame->TransformationMatrix, parent_matrix);
	else
		frame->CombinedTransformMatrix = frame->TransformationMatrix;

	if(frame->pFrameSibling != NULL)
		update_frame_matrices(frame->pFrameSibling, parent_matrix);

	if(frame->pFrameFirstChild != NULL)
		update_frame_matrices(frame->pFrameFirstChild, &frame->CombinedTransformMatrix);
}


HRESULT CSkinMesh::draw_mesh_container(CONST D3DXMESHCONTAINER* base_mesh_container, CONST D3DXFRAME* base_frame)
{
	D3DXMESHCONTAINER_DERIVED* mesh_container = (D3DXMESHCONTAINER_DERIVED*) base_mesh_container;
	D3DXFRAME_DERIVED* frame = (D3DXFRAME_DERIVED*) base_frame;

	if(mesh_container->pSkinInfo != NULL)
	{
		if(mesh_container->UseSoftwareVP)
		{
			m_device->SetSoftwareVertexProcessing(TRUE);
		}
		
		if(mesh_container->MaxBonesInflPerVertex)
			m_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

		if(mesh_container->MaxBonesInflPerVertex == 1)
			m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
		else if(mesh_container->MaxBonesInflPerVertex == 2)
			m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_1WEIGHTS);
		else if(mesh_container->MaxBonesInflPerVertex == 3)
			m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_2WEIGHTS);
		else
			m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS);

		D3DXBONECOMBINATION* bone_comb = (D3DXBONECOMBINATION*) mesh_container->pBoneCombBuffer->GetBufferPointer();

		for(UINT attr_index = 0; attr_index < mesh_container->NumAttrGroups; attr_index++)
		{
			// setup vertex index blending world matrix for every mesh group
			for(UINT palette_index = 0; palette_index < mesh_container->NumMatrixPalettes; palette_index++)
			{
				UINT matrix_index = bone_comb[attr_index].BoneId[palette_index];

				if(matrix_index != UINT_MAX)
				{
					D3DXMATRIX mat_palette;

					D3DXMatrixMultiply(&mat_palette,
									   mesh_container->ppBoneOffsetMatrices[matrix_index],
									   mesh_container->ppBoneMatrices[matrix_index]);

					m_device->SetTransform(D3DTS_WORLDMATRIX(palette_index), &mat_palette);
				}
			}

			DWORD attr_id = bone_comb[attr_index].AttribId;

			m_device->SetMaterial(&mesh_container->pMaterials[attr_id].MatD3D);
			m_device->SetTexture(0, mesh_container->ppTextures[attr_id]);
			
			mesh_container->MeshData.pMesh->DrawSubset(attr_index);
		}

		// restore render state

		m_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);

		if(mesh_container->UseSoftwareVP)
		{
			m_device->SetSoftwareVertexProcessing(FALSE);
		}
	}
	else
	{
		m_device->SetTransform(D3DTS_WORLD, &frame->CombinedTransformMatrix);

		for(UINT i = 0; i < mesh_container->NumMaterials; i++)
		{
			m_device->SetMaterial(&mesh_container->pMaterials[i].MatD3D);
			m_device->SetTexture(0, mesh_container->ppTextures[i]);

			mesh_container->MeshData.pMesh->DrawSubset(i);
		}
	}

	return S_OK;
}


HRESULT CSkinMesh::draw_frame(CONST D3DXFRAME* frame)
{
	D3DXMESHCONTAINER* mesh_container = frame->pMeshContainer;

	while(mesh_container != NULL)
	{
		draw_mesh_container(mesh_container, frame);
		mesh_container = mesh_container->pNextMeshContainer;
	}

	if(frame->pFrameSibling != NULL)
		draw_frame(frame->pFrameSibling);

	if(frame->pFrameFirstChild != NULL)
		draw_frame(frame->pFrameFirstChild);

	return S_OK;
}


HRESULT CSkinMesh::render(double app_elapsed_time)
{
	if(0.0f == app_elapsed_time)
		return S_OK;
	
	//建立世界坐标矩阵
    D3DXMATRIXA16 matWorld,matWorld2,matZoomin;

	//先将角色的中心点平移到世界坐标系的原点
    D3DXMatrixTranslation(&matWorld, -m_object_center.x,-m_object_center.y,-m_object_center.z );	
	//角色缩放
	//D3DXMatrixScaling(&matZoomin,1.0f,1.0f,1.0f);
	D3DXMatrixScaling(&matZoomin,m_v3Size.x, m_v3Size.y, m_v3Size.z);
	//合成矩阵,与缩放矩阵相乘
	D3DXMatrixMultiply(&matWorld,&matWorld,&matZoomin); 
	
	//将角色的中心点向上提高，保证角色的脚站在地面上
    D3DXMatrixTranslation( &matWorld2, 0,m_object_radius +m_fHeight ,0);
	
	//合成变换矩阵
	D3DXMatrixMultiply(&matWorld,&matWorld,&matWorld2);
	
	//角色绕Y轴旋转m_fRotateAngle角
	D3DXMatrixRotationY(&matWorld2,m_fRotateAngle);  //旋转矩阵
	//合成变换矩阵
	D3DXMatrixMultiply(&matWorld,&matWorld,&matWorld2);  

	//与移步矩阵相乘得到最终的合成矩阵
	D3DXMatrixMultiply(&matWorld,&matWorld,&m_matMoveMatrices);

	//--------------------------------------------------------
	
	LPD3DXANIMATIONSET  ppAnimSet=NULL;
	DWORD dwNewTrack=0;

	m_anim_controller->GetAnimationSetByName(m_sAnimationSet.c_str(),&ppAnimSet);
	m_anim_controller->SetTrackAnimationSet( dwNewTrack, ppAnimSet );
	ppAnimSet->Release();
	m_anim_controller->UnkeyAllTrackEvents(dwNewTrack);
	m_anim_controller->SetTrackEnable( dwNewTrack, TRUE );
	m_anim_controller->KeyTrackSpeed( dwNewTrack, 1.0f, 0.01f, 0.5f, D3DXTRANSITION_LINEAR );
	m_anim_controller->KeyTrackWeight( dwNewTrack, 1.0f, 0.01f, 0.5f, D3DXTRANSITION_LINEAR );

	if(m_is_play_anim && m_anim_controller != NULL)
		m_anim_controller->AdvanceTime(app_elapsed_time, NULL);

	update_frame_matrices(m_root_frame, &matWorld);
	
	draw_frame(m_root_frame);

	return S_OK;
}

//设置旋转角
void CSkinMesh::SetRotateAngle(float fAngle)
{
	m_fRotateAngle=fAngle;
}

//设置动画标志
void CSkinMesh::SetAnim(bool bAnim)
{
	m_bMoving=bAnim;
}

//设置角色在地面的高度
void CSkinMesh::SetTransHeight(float fheigh)
{
	m_fHeight = fheigh;
}

void CSkinMesh::SetAnimationSet(string set ,double times)
{
	m_sAnimationSet = set;
	m_dAnimationTimes = times;
}

VOID CSkinMesh::SetSize(D3DXVECTOR3 size)
{
	m_v3Size = size;
}
