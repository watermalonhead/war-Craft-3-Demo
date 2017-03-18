#pragma once

#include <d3dx9.h>
#include <string>
using namespace std;

#pragma warning(disable : 4127 4995)


//测试按键是否按下
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

//安全删除
#define SAFE_RELEASE(object) {if(object){object->Release();object=NULL;}}
#define SAFE_DELETE(object) {if(object){delete object;object=NULL;}}
#define SAFE_DELARRAY(object) {if(object){delete[] object;object=NULL;}}

////////////////////////////////////////////////////////////////////////////

struct D3DXFRAME_DERIVED : public D3DXFRAME
{
	D3DXMATRIX CombinedTransformMatrix;
};

struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
{
	IDirect3DTexture9**	ppTextures;
	ID3DXMesh*			pOrgMesh;
	DWORD				MaxBonesInflPerVertex;
	DWORD				NumAttrGroups;
	ID3DXBuffer*		pBoneCombBuffer; 
	D3DXMATRIX**		ppBoneMatrices;  //骨骼变换矩阵数组
	D3DXMATRIX**		ppBoneOffsetMatrices; //骨骼偏转矩阵数组
	DWORD				NumMatrixPalettes;
	bool				UseSoftwareVP;
};


class cAllocateHierarchy : public ID3DXAllocateHierarchy
{
private:
	HRESULT AllocateName(LPCSTR name, LPSTR* ret_name);
	HRESULT GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED* mesh_container);

public:
	STDMETHOD(CreateFrame)(LPCSTR name, LPD3DXFRAME* ret_frame);

	STDMETHOD(CreateMeshContainer)(LPCSTR name, 
                                   CONST D3DXMESHDATA* mesh_data,
                                   CONST D3DXMATERIAL* xmaterials, 
                                   CONST D3DXEFFECTINSTANCE* effect_instances, 
                                   DWORD num_materials, 
                                   CONST DWORD* adjacency, 
                                   LPD3DXSKININFO skin_info, 
                                   LPD3DXMESHCONTAINER* ret_mesh_container);    

    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME frame_to_free);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER base_mesh_container);
};

