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
		int numVertsPerRow,  //每行的顶点数
		int numVertsPerCol,  //每列的顶点数
		int cellSpacing,    // space between cells
		float heightScale   //高度缩放比例
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
	int _width;  //地形宽度
	int _length; //地形长度
	int _numVertices;//总的顶点数
	int _numTriangles;//总的三角形面数

	float _heightScale;

	std::vector<int> _heightmap; //存放高度数据的数组

	// helper methods
	bool  readRawFile(std::string fileName); //从高度图中读取顶点高度信息
	bool  computeVertices();			//处理顶点
	bool  computeIndices();			//处理索引
	bool  lightTerrain(D3DXVECTOR3* directionToLight);			//地形阴影处理
	float computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight);		//计算方块的阴影值
	
	void GetPressPointInTerrain( D3DXVECTOR3 & pressp, D3DXVECTOR3 &Eyeat, D3DXVECTOR3 &pressonworld);//获取鼠标所点击的地形的位置
	
	
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
