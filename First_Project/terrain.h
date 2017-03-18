#pragma once

#include "d3dUtility.h"
#include <string>
#include <vector>
using namespace std;

class Terrain
{
public:
	Terrain(
		IDirect3DDevice9* device,
		std::string heightmapFileName, 
		int numVertsPerRow,  //ÿ�еĶ�����
		int numVertsPerCol,  //ÿ�еĶ�����
		int cellSpacing,    // space between cells
		float heightScale   //�߶����ű���
		);   

	~Terrain();

	int  getHeightmapEntry(int row, int col);
	void setHeightmapEntry(int row, int col, int value);
	int  getWidth(void);
	int  getLength(void);

	float getHeight(float x, float z);

	bool  loadTexture(std::string fileName);
	bool  genTexture(D3DXVECTOR3* directionToLight);
	bool  draw(D3DXMATRIX* world, bool drawTris);

private:
	IDirect3DDevice9*       _device;
	IDirect3DTexture9*      _tex;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9*  _ib;

	int _numVertsPerRow;
	int _numVertsPerCol;
	int _cellSpacing;

	int _numCellsPerRow;  //=_numVertsPerRow-1
	int _numCellsPerCol;   //=_numVertsPerCol-1

public:
	int _width;  //���ο��
	int _length; //���γ���
	int _numVertices;//�ܵĶ�����
	int _numTriangles;//�ܵ�����������

	float _heightScale;

	std::vector<int> _heightmap; //��Ÿ߶����ݵ�����

	// helper methods
	bool  readRawFile(std::string fileName); //�Ӹ߶�ͼ�ж�ȡ����߶���Ϣ
	bool  computeVertices();			//������
	bool  computeIndices();			//��������
	bool  lightTerrain(D3DXVECTOR3* directionToLight);			//������Ӱ����
	float computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight);		//���㷽�����Ӱֵ
	
	void GetPressPointInTerrain( D3DXVECTOR3 & pressp, D3DXVECTOR3 &Eyeat, D3DXVECTOR3 &pressonworld);//��ȡ���������ĵ��ε�λ��
	
	
	struct TerrainVertex
	{
		TerrainVertex(){}
		TerrainVertex(float x, float y, float z, float u, float v)
		{
			_x = x; _y = y; _z = z; _u = u; _v = v;
		}
		float _x, _y, _z;
		float _u, _v;

		static const DWORD FVF;
	};
};
