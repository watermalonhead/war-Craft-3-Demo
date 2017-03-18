#include "AllocateHierarchy.h"

#pragma warning(disable : 4127 4995)

extern IDirect3DDevice9*		g_pDevice ;

//------------------------------------------------------------------------------------

HRESULT cAllocateHierarchy::AllocateName(LPCSTR name, LPSTR* ret_name)
{
	if(name != NULL)
	{
		UINT length = (UINT)strlen(name) + 1;

		*ret_name = new CHAR[length];

		if(*ret_name == NULL)
			return E_OUTOFMEMORY;

		memcpy(*ret_name, name, length * sizeof(CHAR));
	}
	else
	{
		*ret_name = NULL;
	}

	return S_OK;
}

//------------------------------------------------------------------------------------

HRESULT cAllocateHierarchy::CreateFrame(LPCSTR name, LPD3DXFRAME* ret_frame)
{
	*ret_frame = NULL;

	D3DXFRAME_DERIVED* new_frame = new D3DXFRAME_DERIVED;

	if(new_frame == NULL)
		return E_OUTOFMEMORY;

	HRESULT hr = AllocateName(name, &new_frame->Name);

	if(FAILED(hr))
	{
		delete new_frame;
		return hr;
	}

	D3DXMatrixIdentity(&new_frame->TransformationMatrix);
	D3DXMatrixIdentity(&new_frame->CombinedTransformMatrix);

	new_frame->pMeshContainer   = NULL;
	new_frame->pFrameSibling    = NULL;
	new_frame->pFrameFirstChild = NULL;

	*ret_frame = new_frame;

	return S_OK;
}

//------------------------------------------------------------------------------------

HRESULT cAllocateHierarchy::GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED* mesh_container)
{
	ID3DXSkinInfo* skin_info = mesh_container->pSkinInfo;

	if(skin_info == NULL)//如果是非蒙皮网格，则直接返回
		return S_OK;

	SAFE_RELEASE(mesh_container->MeshData.pMesh);
	SAFE_RELEASE(mesh_container->pBoneCombBuffer);

	HRESULT hr;
	IDirect3DIndexBuffer9* index_buffer;
	
	if(FAILED(hr = mesh_container->pOrgMesh->GetIndexBuffer(&index_buffer)))
		return hr;

	DWORD max_faces_infl_per_triangle;
	hr = skin_info->GetMaxFaceInfluences(index_buffer, mesh_container->pOrgMesh->GetNumFaces(), 
										 &max_faces_infl_per_triangle);

	index_buffer->Release();

	if(FAILED(hr))
		return hr;

	max_faces_infl_per_triangle = min(max_faces_infl_per_triangle, 12);

	D3DCAPS9 caps;
	g_pDevice->GetDeviceCaps(&caps);

	//实际需要的混合矩阵调色板数目大于硬件的支持，则只能改由软件实现
	if((caps.MaxVertexBlendMatrixIndex+1)/2 < max_faces_infl_per_triangle)
	{
		// use software vertex processing
		mesh_container->NumMatrixPalettes = min(256, skin_info->GetNumBones());
		mesh_container->UseSoftwareVP = true;
	}
	else
	{
		// use hardware verterx processing
		mesh_container->NumMatrixPalettes = min((caps.MaxVertexBlendMatrixIndex+1)/2, skin_info->GetNumBones());
		mesh_container->UseSoftwareVP = false;
	}

	hr = skin_info->ConvertToIndexedBlendedMesh(mesh_container->pOrgMesh, 0, mesh_container->NumMatrixPalettes,
			mesh_container->pAdjacency, NULL, NULL, NULL, &mesh_container->MaxBonesInflPerVertex,
			&mesh_container->NumAttrGroups, &mesh_container->pBoneCombBuffer, &mesh_container->MeshData.pMesh);

	return hr;
}

//------------------------------------------------------------------------------------

HRESULT cAllocateHierarchy::CreateMeshContainer(LPCSTR name, 
                                                CONST D3DXMESHDATA* mesh_data,
                                                CONST D3DXMATERIAL* xmaterials, 
                                                CONST D3DXEFFECTINSTANCE* effect_instances, 
                                                DWORD num_materials, 
                                                CONST DWORD* adjacency, 
                                                LPD3DXSKININFO skin_info, 
                                                LPD3DXMESHCONTAINER* ret_mesh_container) 
{
	*ret_mesh_container = NULL;

	if(mesh_data->Type != D3DXMESHTYPE_MESH)
		return E_FAIL;

	ID3DXMesh* mesh_ptr = mesh_data->pMesh;
	DWORD fvf = mesh_ptr->GetFVF();

	if(fvf == 0)
		return E_FAIL;

	// create a mesh container and zero it

	D3DXMESHCONTAINER_DERIVED* new_mesh_container = new D3DXMESHCONTAINER_DERIVED;

	if(new_mesh_container == NULL)
		return E_OUTOFMEMORY;

	memset(new_mesh_container, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

	// copy mesh name

	HRESULT hr = AllocateName(name, &new_mesh_container->Name);

	if(FAILED(hr))
	{
		DestroyMeshContainer(new_mesh_container);
		return hr;
	}

	new_mesh_container->MeshData.Type = D3DXMESHTYPE_MESH;

	// be sure mesh contain normal
	if(!(fvf & D3DFVF_NORMAL))
	{
		hr = mesh_ptr->CloneMeshFVF(mesh_ptr->GetOptions(), fvf | D3DFVF_NORMAL, g_pDevice, &new_mesh_container->MeshData.pMesh);

		if(FAILED(hr))
		{
			DestroyMeshContainer(new_mesh_container);
			return hr;
		}

		mesh_ptr = new_mesh_container->MeshData.pMesh;
		D3DXComputeNormals(mesh_ptr, NULL);//计算顶点法线
	}
	else
	{
		new_mesh_container->MeshData.pMesh = mesh_ptr;
		mesh_ptr->AddRef();	// !! important, so DestroyMeshContainer() will not crash.
	}

	// load materials and textures

	UINT num_faces = mesh_ptr->GetNumFaces();

	new_mesh_container->NumMaterials = max(1, num_materials);
	new_mesh_container->pMaterials	 = new D3DXMATERIAL[new_mesh_container->NumMaterials];
	new_mesh_container->ppTextures	 = new LPDIRECT3DTEXTURE9[new_mesh_container->NumMaterials];
	new_mesh_container->pAdjacency	 = new DWORD[num_faces * 3];

	if((new_mesh_container->pAdjacency == NULL) || (new_mesh_container->pMaterials == NULL) ||
	   (new_mesh_container->ppTextures == NULL))
	{
		DestroyMeshContainer(new_mesh_container);
		return E_OUTOFMEMORY;
	}

	memcpy(new_mesh_container->pAdjacency, adjacency, sizeof(DWORD) * num_faces * 3);
	memset(new_mesh_container->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * new_mesh_container->NumMaterials);

	D3DXMATERIAL* xmaterials_ptr = new_mesh_container->pMaterials;

	if(num_materials > 0)
	{
		memcpy(xmaterials_ptr, xmaterials, sizeof(D3DXMATERIAL) * num_materials);
		xmaterials_ptr->MatD3D.Ambient = xmaterials_ptr->MatD3D.Diffuse;

		for(UINT i = 0; i < num_materials; i++)
		{
			if(xmaterials_ptr[i].pTextureFilename != NULL)
			{
				string stemp =  "Res\\Mesh\\";
				stemp += xmaterials_ptr[i].pTextureFilename;
				const TCHAR* strTexturePath  = stemp.c_str();

				if(D3D_OK != D3DXCreateTextureFromFile(g_pDevice, strTexturePath, &new_mesh_container->ppTextures[i]) )
					new_mesh_container->ppTextures[i] = NULL;
			}
		}
	}
	else
	{
		xmaterials_ptr[0].pTextureFilename = NULL;
		memset(&xmaterials_ptr[0].MatD3D, 0, sizeof(D3DMATERIAL9));

		xmaterials_ptr[0].MatD3D.Diffuse.r = 0.5f;
		xmaterials_ptr[0].MatD3D.Diffuse.g = 0.5f;
		xmaterials_ptr[0].MatD3D.Diffuse.b = 0.5f;
		xmaterials_ptr[0].MatD3D.Specular  = xmaterials_ptr[0].MatD3D.Diffuse;
		xmaterials_ptr[0].MatD3D.Ambient   = xmaterials_ptr[0].MatD3D.Diffuse;
	}

	// generate skin mesh
	if(skin_info != NULL)
	{
		new_mesh_container->pSkinInfo = skin_info;
		skin_info->AddRef();

		new_mesh_container->pOrgMesh = mesh_ptr;
		mesh_ptr->AddRef();

		UINT num_bones = skin_info->GetNumBones();
		new_mesh_container->ppBoneOffsetMatrices = new D3DXMATRIX*[num_bones];

		if(new_mesh_container->ppBoneOffsetMatrices == NULL)
		{
			DestroyMeshContainer(new_mesh_container);
			return E_OUTOFMEMORY;
		}

		for(UINT i = 0; i < num_bones; i++)
			new_mesh_container->ppBoneOffsetMatrices[i] = new_mesh_container->pSkinInfo->GetBoneOffsetMatrix(i);

		hr = GenerateSkinnedMesh(new_mesh_container);

		if(FAILED(hr))
		{
			DestroyMeshContainer(new_mesh_container);
			return hr;
		}
	}

	*ret_mesh_container = new_mesh_container;

	return S_OK;
}

//------------------------------------------------------------------------------------

HRESULT cAllocateHierarchy::DestroyFrame(LPD3DXFRAME frame_to_free) 
{
	if(frame_to_free != NULL)
	{
		SAFE_DELARRAY(frame_to_free->Name);
		SAFE_DELETE(frame_to_free);
	}

	return S_OK;
}

//------------------------------------------------------------------------------------

HRESULT cAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER base_mesh_container)
{
	if(base_mesh_container == NULL)
		return S_OK;

	D3DXMESHCONTAINER_DERIVED* mesh_container = (D3DXMESHCONTAINER_DERIVED*) base_mesh_container;

	SAFE_DELARRAY(mesh_container->Name);
	SAFE_DELARRAY(mesh_container->pAdjacency);
	SAFE_DELARRAY(mesh_container->pMaterials);
	SAFE_DELARRAY(mesh_container->ppBoneOffsetMatrices);
	SAFE_DELARRAY(mesh_container->ppBoneMatrices);

	if(mesh_container->ppTextures != NULL)
	{
		for(UINT i = 0; i < mesh_container->NumMaterials; i++)
			SAFE_RELEASE(mesh_container->ppTextures[i]);
	}

	SAFE_DELARRAY(mesh_container->ppTextures);

	SAFE_RELEASE(mesh_container->pBoneCombBuffer);
	SAFE_RELEASE(mesh_container->MeshData.pMesh);
	SAFE_RELEASE(mesh_container->pSkinInfo);
	SAFE_RELEASE(mesh_container->pOrgMesh);

	SAFE_DELETE(mesh_container);

	return S_OK;
}
