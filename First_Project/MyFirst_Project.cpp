#define WIN32_LEAN_AND_MEAN

#include"Common.h"
LPDIRECT3D9		g_pD3D;
LPDIRECT3DDEVICE9		g_pDevice;
HWND			g_hWnd; //主窗口句柄
HINSTANCE	g_hInstance; //实例句柄
Terrain*	TheTerrian = 0; //地形的定义
CSkyBox*	SkyBox = 0;//天空盒的定义
Sprite*		Character = 0;//人物指针
Sprite*		Enemy = 0;//敌人
Camera TheCamera(Camera::LANDOBJECT); //camera
D3DXVECTOR3 ret;  //射线位置
RainParticle*	rain; //雨滴粒子系统
BoundingBox*	rainBox; //雨滴生成范围
BillBoard*		m_UI;//ui窗口
BillBoard*		m_Arrow;//鼠标
BillBoard*		m_EnemyPos;//敌人位置UI 死亡需要释放
BillBoard*		m_HeroPos;//英雄位置UI
ID3DXFont*		Font = 0;//字体
char			str[1024];//输出字体容器
BillBoard*		Loading;
bool			isReady = false;
HANDLE			hThread;
unsigned int __stdcall  OnLoad(LPVOID lpParameter);
bool isMiniMap(POINT MousePos)
{
	if( (MousePos.x >= 20 && MousePos.x <= 20 + 256 * 1.33f) && (MousePos.y >= (Height - 293) && MousePos.y <= (Height - 293 + 256 * 1.10f)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool InitD3D(HWND hwnd)
{
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9 caps;
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	int vp = 0;
	if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	D3DDISPLAYMODE d3ddm;
	if (g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm) != D3D_OK)
	{
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	if (g_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		vp,
		&d3dpp, &g_pDevice) != D3D_OK)
	{
		return false;
	}


	//set projection matrix
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI*0.5f,
		(float)Width / (float)Height,
		1.0f,
		1000.0f
	);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

#pragma region Input
	//init input
	DInput_Init();
	DInput_Init_Mouse();
	DInput_Init_Keyboard();
	SetCursorPos(Width / 2, Height / 2);
	//init camera position
#pragma endregion


	//set god view
	TheCamera.pitch(D3DX_PI / 3.5);
	TheCamera.setPosition(&D3DXVECTOR3(0.0f,120.0f,-40.0f));

	//Loading画面
	Loading = new BillBoard();
	Loading->Init(g_pDevice);
	Loading->LoadTexture(g_pDevice, 0, "Res\\UI\\LoadingSence.jpg");

	if (!isReady)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, OnLoad, NULL, 0, NULL);
	}
	return true;
}

void Render(float timeDelta)
{
	DInput_Read_Mouse_Buffer();
	DInput_Read_Keyboard();

	POINT MousePos;
	GetCursorPos(&MousePos);
	ScreenToClient(g_hWnd, &MousePos);
#pragma region CameraUpdte

	if (!isMiniMap(MousePos))
	{
		TheCamera.CameraUpdate(timeDelta);
	}
	if (::GetAsyncKeyState('W') & 0x8000f)
		TheCamera.pitch(-1.0f*timeDelta);
	if (::GetAsyncKeyState('S') & 0x8000f)
		TheCamera.pitch(1.0f*timeDelta);
	D3DXVECTOR3 pos;
	TheCamera.getPosition(&pos);
	if (pos.z > 310 * 4)
	{
		pos.z = 310 * 4;
	}
	if (pos.z < -310 * 4)
	{
		pos.z = -310 * 4;
	}
	if (pos.x > 310 * 4)
	{
		pos.x = 310 * 4;
	}
	if (pos.x < -310 * 4)
	{
		pos.x = -310 * 4;
	}

	TheCamera.setPosition(&pos);


#pragma endregion

	D3DXMATRIX V;
	TheCamera.getViewMatrix(&V);
	g_pDevice->SetTransform(D3DTS_VIEW, &V);


		//draw the scene;
		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

		g_pDevice->BeginScene();

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		if (!isReady)//画出主线程
		{
			Loading->SetSize(&D3DXVECTOR2(1.9, 1.05));
			Loading->SetPosition(&D3DXVECTOR2(0.0f, 0.0f));
			Loading->Render(0);
		}
	
		if (isReady)//画出子线程
		{



#pragma region SkyBox
			//skybox
			if (SkyBox)
			{
				TheCamera.getPosition(&pos);
				SkyBox->Render(pos);
			}
#pragma endregion

#pragma region terrian
			//terrian
			if (TheTerrian)
			{
				TheTerrian->draw(&I, true);
			}
#pragma endregion

#pragma region UI
			m_UI->SetSize(&D3DXVECTOR2(0.938f, 0.5f));
			RECT BottomUI;
			BottomUI.top = 208;
			BottomUI.bottom = Width - 30;
			BottomUI.left = 0;
			BottomUI.right = 2050;
			RECT TopUI;
			TopUI.top = 0;
			TopUI.bottom = 200;
			TopUI.left = 0;
			TopUI.right = 2050;
			//top UI
			m_UI->SetPosition(&D3DXVECTOR2(0, 0));
			m_UI->Render(0, TopUI);
			//bottom UI
			m_UI->SetPosition(&D3DXVECTOR2(0, Height - 410));
			m_UI->Render(0, BottomUI);

			//mini map
			m_UI->SetSize(&D3DXVECTOR2(1.33f, 1.10f)); //原图基础上x放大1.33倍 y放大1.10倍
			RECT miniMap;
			miniMap.top = 0;
			miniMap.bottom = 256;
			miniMap.left = 0;
			miniMap.right = 256;
			m_UI->SetPosition(&D3DXVECTOR2(20, Height - 293));
			m_UI->Render(1, miniMap);


			//mouse ui
			ShowCursor(false);
			POINT mouse;
			GetCursorPos(&mouse);
			m_Arrow->SetSize(&D3DXVECTOR2(1.0f, 1.0f));
			m_Arrow->SetPosition(&D3DXVECTOR2(mouse.x, mouse.y));
			RECT ArrowUI;
			ArrowUI.top = 96;
			ArrowUI.bottom = 128;
			ArrowUI.left = 224;
			ArrowUI.right = 256;
			if (!isMiniMap(MousePos))
			{

				if (mouse.x > Width - 100)
				{

					ArrowUI.right = 256;
					m_Arrow->SetAngle(0);
					m_Arrow->Render(0, ArrowUI);
				}
				else if (mouse.x < 100)
				{

					m_Arrow->SetAngle(D3DX_PI);
					m_Arrow->Render(0, ArrowUI);
				}
				else if (mouse.y < 100)
				{

					m_Arrow->SetAngle(-D3DX_PI / 2);
					m_Arrow->Render(0, ArrowUI);
				}
				else if (mouse.y > Height - 100)
				{
					m_Arrow->SetAngle(D3DX_PI / 2);
					m_Arrow->Render(0, ArrowUI);
				}
				else
				{
					ArrowUI.top = 0;
					ArrowUI.bottom = 34;
					ArrowUI.left = 0;
					ArrowUI.right = 34;
					m_Arrow->SetAngle(0);
					m_Arrow->Render(0, ArrowUI);

				}
			}
			else
			{
				ArrowUI.top = 0;
				ArrowUI.bottom = 34;
				ArrowUI.left = 0;
				ArrowUI.right = 34;
				m_Arrow->SetAngle(0);
				m_Arrow->Render(0, ArrowUI);
			}
			//人物头像
			m_UI->SetSize(&D3DXVECTOR2(1.0f, 1.0f));
			m_UI->SetPosition(&D3DXVECTOR2(20, 80));
			m_UI->Render(3);

			//血条魔法UI
			if (Character != NULL)
			{
				RECT Blood;
				Blood.top = 0;
				Blood.bottom = 50;
				Blood.left = 0;
				Blood.right = Character->m_iNowHp * 250 / Character->m_iMaxHp;
				m_UI->SetSize(&D3DXVECTOR2(1.5f, 0.5f));
				m_UI->SetPosition(&D3DXVECTOR2(95, 80));
				m_UI->Render(4, Blood);

				RECT Mp;
				Mp.top = 0;
				Mp.bottom = 50;
				Mp.left = 0;
				Mp.right = Character->m_iNowMp * 250 / Character->m_iMaxMp;
				m_UI->SetSize(&D3DXVECTOR2(1.5f, 0.5f));
				m_UI->SetPosition(&D3DXVECTOR2(95, 110));
				m_UI->Render(5, Mp);
			}
			//输出HP MP
			if (Character != NULL)
			{
				memset(str, 0, sizeof(str));
				sprintf(str, "%d", Character->m_iNowHp);
				string tempHp = str;
				LPCSTR Hp = tempHp.c_str();

				memset(str, 0, sizeof(str));
				sprintf(str, "%d", Character->m_iMaxHp);
				string tempMaxHp = str;
				LPCSTR MaxHp = tempMaxHp.c_str();

				memset(str, 0, sizeof(str));
				sprintf(str, "%d", Character->m_iNowMp);
				string tempMp = str;
				LPCSTR Mp = tempMp.c_str();

				memset(str, 0, sizeof(str));
				sprintf(str, "%d", Character->m_iMaxMp);
				string tempMaxMp = str;
				LPCSTR MaxMp = tempMaxMp.c_str();

				RECT rect = { 540, 1022, 600, 1080 };
				//Hp
				Font->DrawTextA(NULL, Hp, -1, &rect, DT_TOP | DT_LEFT, 0xff7FFFAA);
				rect = { 600, 1022, 650, 1080 };
				Font->DrawTextA(NULL, "/", -1, &rect, DT_TOP | DT_LEFT, 0xff7FFFAA);
				rect = { 630, 1022, 700, 1080 };
				Font->DrawTextA(NULL, MaxHp, -1, &rect, DT_TOP | DT_LEFT, 0xff7FFFAA);

				//MP
				rect = { 540, 1050, 600, 1080 };
				Font->DrawTextA(NULL, Mp, -1, &rect, DT_TOP | DT_LEFT, 0xff0000CD);

				rect = { 600, 1050, 650, 1080 };
				Font->DrawTextA(NULL, "/", -1, &rect, DT_TOP | DT_LEFT, 0xff0000CD);

				rect = { 630, 1050, 700, 1080 };
				Font->DrawTextA(NULL, MaxMp, -1, &rect, DT_TOP | DT_LEFT, 0xff0000CD);
			}
#pragma endregion

#pragma region Particle
			rain->Render();
#pragma endregion

#pragma region Mesh
			//mesh
#pragma region 主角
			WORD CharacterAction;
			if (Character)
			{
				g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
				Character->Render(timeDelta);
				g_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);


				//move
				if (IsRButtonPressed() && !(::GetAsyncKeyState('A') & 0x8000f))
				{
					//ray init
					POINT MousePos;
					GetCursorPos(&MousePos);
					ScreenToClient(g_hWnd, &MousePos);
					if (MousePos.y <= 750.0f&&MousePos.y >= 90)
					{
						sRay ray = calculate_picking_ray(MousePos.x, MousePos.y, g_pDevice);
						// transform the ray to world space
						D3DXMATRIX view_inverse_matrix;
						D3DXMatrixInverse(&view_inverse_matrix, NULL, &V);
						transform_ray(&ray, &view_inverse_matrix);

						TheTerrian->GetPressPointInTerrain(ret, ray.origin, ray.direction);
						if (MousePos.x > 100 && MousePos.x < Width - 100)
						{
							Character->SetEndPoint(ret);
						}
					}
				}

#pragma region 主角动画化切换
				//character animate_set
				CharacterAction = Character->GetAction();
				switch (CharacterAction)
				{
				case 0:
					Character->SetAnimationSet("stand_1", 1);
					break;
				case 1:
					Character->SetAnimationSet("walk", 1);
					break;
				case 3:
					Character->SetAnimationSet("Attack_1", 1);
					break;
				case 4:
					Character->SetAnimationSet("Spell", 1);
					break;
				case 5:
					Character->SetAnimationSet("Death", 1);
					break;
				case 6:
					Character->SetAnimationSet("Dissipate", 1);
					break;
				default:
					break;
				}
#pragma endregion
				//attack
				if ((::GetAsyncKeyState('A') & 0x8000f))
				{
					Character->SetAction(3);
				}
				//施法
				if ((::GetAsyncKeyState('1') & 0x8000f) && (Character->m_iNowHp < Character->m_iMaxHp))
				{
					Character->SetAction(4);
				}
				if (Character->m_iTime % 65 == 0 && CharacterAction == 4)
				{

					Character->m_iNowMp -= 50;
					Character->m_iNowHp += 100;
					if (Character->m_iNowHp > Character->m_iMaxHp)
					{
						Character->m_iNowHp = Character->m_iMaxHp;
					}
					Character->SetAction(0);
					Character->m_iTime == 0;
				}
				//lock camera
				if (::GetAsyncKeyState(VK_SPACE) & 0x8000f)
				{
					TheCamera.setPosition(&D3DXVECTOR3(Character->GetPosition().x, 120.0f, Character->GetPosition().y - 40.0f));
				}
				//update
				Character->UpDate();
			}
#pragma endregion

#pragma region 敌人

			if (Enemy)
			{
				g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
				Enemy->Render(timeDelta);
				g_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
			}


#pragma endregion

#pragma region 攻击
			if (Enemy != NULL&&Character != NULL)
			{

#pragma region 敌人角度
				D3DXVECTOR3 TargetPos; //目标位置


				if (Enemy->FieldOfView(Character->GetPosition()) && Enemy->m_iNowHp != 0)
				{
					//敌人不攻击
					if (!Enemy->AttackTest(Character->GetPosition()))
					{
						Enemy->SetAction(1);
						Enemy->SetAnimationSet("walk", 10);
						TargetPos.x = Character->GetPosition().x;
						TargetPos.z = Character->GetPosition().y;
						TargetPos.y = TheTerrian->getHeight(TargetPos.x, TargetPos.z);
						Enemy->SetEndPoint(TargetPos);
					}
					//敌人攻击
					if (Enemy->AttackTest(Character->GetPosition()))
					{
						if (Character->m_iNowHp > 0)
						{
							Enemy->SetAction(3);
							Enemy->SetAnimationSet("Attack", 1);
							if (Enemy->m_iAttackTime % 60 == 0)
							{
								Enemy->Attack(Character);
							}
							//死亡后不行动
							if (Character->m_iNowHp == 0)
							{
								Enemy->SetAction(1);
								Enemy->SetAnimationSet("stand", 1);
							}
						}
					}
					Enemy->UpDate();


				}
				//超出敌人视野范围
				else
				{
					if (Enemy->m_iNowHp != 0)
					{
						Enemy->SetAction(0);
						Enemy->SetAnimationSet("stand", 1);
					}
				}

#pragma endregion

#pragma region 主角角度
				if (Character->AttackTest(Enemy->GetPosition()))
				{
					if (CharacterAction == A_ATTACK)
					{
						if (Character->m_iAttackTime % 20 == 0)
						{
							Character->Attack(Enemy);
						}
					}
				}

				//主角死亡
				if (Character->m_iNowHp == 0)
				{

					if (CharacterAction != A_DEATH)
					{
						Character->SetAction(5);
					}
					if (CharacterAction == A_DISSIPATE)
					{
						Character->SetAction(6);
					}
					if (Character->m_iTime == 80)
					{
						SAFE_DELETE(Character);
					}
				}
				//怪物死亡
				if (Enemy->m_iNowHp == 0)
				{
					Enemy->SetAction(5);
					Enemy->SetAnimationSet("Death", 1);
					Enemy->UpDate();
					if (Enemy->m_iTime % 120 == 0)
					{
						SAFE_DELETE(Enemy);
					}
				}

#pragma endregion

			}

#pragma endregion

#pragma endregion

#pragma region 小地图拾取

			if (isMiniMap(MousePos) && IsLButtonPressed())
			{
				POINT Delta;
				Delta.x = (MousePos.x - 20) - 256 / 2 * 1.33f;
				Delta.y = (-(MousePos.y - Height + 293) + 256 / 2 * 1.10f);
				D3DXVECTOR3 CamaraPos;
				CamaraPos.x = Delta.x / 1.33f * 12;
				CamaraPos.z = Delta.y / 1.10f * 12 - 40.0f;;
				CamaraPos.y = 120.0f;
				TheCamera.setPosition(&CamaraPos);
			}
#pragma endregion

#pragma region 绘制小地图上的标记
			POINT CharaPos;
			POINT EnemyPos;
			POINT MapPos;
			if (Character != NULL)
			{
				CharaPos.x = Character->GetPosition().x;
				CharaPos.y = Character->GetPosition().y;
				//反算出小地图的位置
				MapPos.x = CharaPos.x / 12 * 1.33f + 20 + 256 / 2 * 1.33f;
				MapPos.y = Height - 293 + 256 / 2 * 1.10f - CharaPos.y / 12 * 1.10f;

				m_HeroPos->SetSize(&D3DXVECTOR2(1.0f*0.3, 1.0f*0.3));
				m_HeroPos->SetPosition(&D3DXVECTOR2(MapPos.x - 32 * 0.3 / 2, MapPos.y - 32 * 0.3 / 2));
				m_HeroPos->Render(0);
			}
			else
			{
				m_HeroPos->Destory();
			}
			if (Enemy != NULL)
			{
				EnemyPos.x = Enemy->GetPosition().x;
				EnemyPos.y = Enemy->GetPosition().y;

				MapPos.x = EnemyPos.x / 12 * 1.33f + 20 + 256 / 2 * 1.33f;
				MapPos.y = Height - 293 + 256 / 2 * 1.10f - EnemyPos.y / 12 * 1.10f;

				m_EnemyPos->SetSize(&D3DXVECTOR2(1.0f*0.3, 1.0f*0.3));
				m_EnemyPos->SetPosition(&D3DXVECTOR2(MapPos.x - 32 * 0.3 / 2, MapPos.y - 32 * 0.3 / 2));
				m_EnemyPos->Render(0);
			}
			else
			{
				m_EnemyPos->Destory();
			}

			//摄像机框
			D3DXVECTOR3 CameraPos;
			TheCamera.getPosition(&CameraPos);

			MapPos.x = CameraPos.x / 12 * 1.33f + 20 + 256 / 2 * 1.33f;
			MapPos.y = Height - 293 + 256 / 2 * 1.10f - (CameraPos.z + 40) / 12 * 1.10f;

			m_UI->SetSize(&D3DXVECTOR2(1.0f*0.7, 1.0f*0.7));
			m_UI->SetPosition(&D3DXVECTOR2(MapPos.x - 32 * 0.7 / 2, MapPos.y - 32 * 0.7 / 2));
			m_UI->Render(2);
#pragma endregion

		}

		g_pDevice->EndScene();
		g_pDevice->Present(0, 0, 0, 0);
	return;
}

void EndD3D()
{
	//release input
	DInput_Release_Mouse();
	DInput_Release_Keyboard();
	DInput_Shutdown();
	//release ui
	m_UI->Destory();
	SAFE_DELETE(m_Arrow);
	SAFE_DELETE(m_UI);
	SAFE_DELETE(m_HeroPos);
	SAFE_DELETE(m_EnemyPos);
	SAFE_DELETE(rain);
	SAFE_DELETE(rainBox);
	SAFE_DELETE(Character);
	SAFE_DELETE(Enemy);
	SAFE_DELETE(SkyBox);
	SAFE_DELETE(TheTerrian);
	

	SAFE_RELEASE(g_pDevice);

	//release d3d
	SAFE_RELEASE(g_pD3D);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;
	WNDCLASSEX winclass;
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = "GameWNDCLASS";
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winclass))
	{
		return 0;
	}

	
	if (!(g_hWnd = CreateWindowEx(NULL,
		"GameWNDCLASS",
		"Game Window",
		WS_POPUP | WS_VISIBLE,
		0, 0, Width, Height,
		NULL,
		NULL,
		hInstance,
		NULL)))
	{
		return 0;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	srand(GetTickCount());

	InitD3D(g_hWnd);

	MSG msg;
	while (true)
	{
		static float lastTime = (float)timeGetTime();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			if (KEYDOWN(VK_ESCAPE))
			{
				PostMessage(g_hWnd, WM_CLOSE, 0, 0);
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		float currTime = (float)timeGetTime();
		float timeDelta = (currTime - lastTime)*0.001f;
		Render(timeDelta);
		lastTime = currTime;
	}

	EndD3D();

	return msg.wParam;
}

unsigned int __stdcall OnLoad(LPVOID lpParameter)
{
	
	//light 
	D3DXVECTOR3 lightDirection(0.0f, 1.0f, 0.0f);

	//load terrain
	TheTerrian = new Terrain(g_pDevice, "Res\\Map\\map.raw", 256, 256, 12, 0.1);
	TheTerrian->loadTexture("Res\\Map\\floor.jpg");

	//set texture filters.
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//init skybox
	SkyBox = new CSkyBox(g_pDevice);
	SkyBox->InitSkyBox(10);
	//load sky texture
	SkyBox->SetTexture("Res\\Sky\\sky_RT.jpg", 0);
	SkyBox->SetTexture("Res\\Sky\\sky_LF.jpg", 1);
	SkyBox->SetTexture("Res\\Sky\\sky_UP.jpg", 2);
	SkyBox->SetTexture("Res\\Sky\\sky_DN.jpg", 3);
	SkyBox->SetTexture("Res\\Sky\\sky_FR.jpg", 4);
	SkyBox->SetTexture("Res\\Sky\\sky_BK.jpg", 5);

	//init character
	Character = new Sprite(g_pDevice, "Res\\Mesh\\Arthas.X", D3DXVECTOR3(0.3, 0.3, 0.3), "Stand_1", 10, 10, 1000);
	POINT position;
	position.x = Character->GetPosition().x;
	position.y = Character->GetPosition().y;
	Character->SetTransHeight(TheTerrian->getHeight(position.x, position.y) - 80);

	//init enemy
	Enemy = new Sprite(g_pDevice, "Res\\Mesh\\js.X", D3DXVECTOR3(0.25, 0.25, 0.25), "Stand", 10, 10, 500);
	position.x = 100;
	position.y = 100;
	Enemy->SetPosition(position);
	Enemy->SetTransHeight(TheTerrian->getHeight(position.x, position.y) - 110);

	//init rain
	rainBox = new BoundingBox();
	//box size
	rainBox->_max = D3DXVECTOR3(1500, 30, 1500);
	rainBox->_min = D3DXVECTOR3(-1500, 0, -1500);
	rain = new RainParticle(g_pDevice, rainBox, 3000, lightDirection);

	//init UI
	m_UI = new BillBoard();
	m_UI->Init(g_pDevice);
	m_UI->LoadTexture(g_pDevice, 0, "Res\\UI\\UI.BMP");
	m_UI->LoadTexture(g_pDevice, 1, "Res\\Map\\Minimap.bmp"); //mini map
	m_UI->LoadTexture(g_pDevice, 2, "Res\\Map\\Camara.png");//摄像机位置UI
	m_UI->LoadTexture(g_pDevice, 3, "Res\\UI\\BTNArthas.JPG");//人物头像
	m_UI->LoadTexture(g_pDevice, 4, "Res\\UI\\Blood.png"); //血条UI
	m_UI->LoadTexture(g_pDevice, 5, "Res\\UI\\MP.png"); //血条UI
														//mouse
	m_Arrow = new BillBoard();
	m_Arrow->Init(g_pDevice);
	m_Arrow->LoadTexture(g_pDevice, 0, "Res\\UI\\NightElfCursor.BMP");

	//position UI
	m_HeroPos = new BillBoard();
	m_HeroPos->Init(g_pDevice);
	m_HeroPos->LoadTexture(g_pDevice, 0, "Res\\Map\\minimap-hero.BMP");
	m_EnemyPos = new BillBoard();
	m_EnemyPos->Init(g_pDevice);
	m_EnemyPos->LoadTexture(g_pDevice, 0, "Res\\Map\\MinEnemy.png");

	//字体
	D3DXFONT_DESC df;
	ZeroMemory(&df, sizeof(D3DXFONT_DESC));
	df.Height = 25;
	df.Width = 12;
	df.MipLevels = D3DX_DEFAULT;
	df.Italic = false;
	df.CharSet = DEFAULT_CHARSET;
	df.OutputPrecision = 0;
	df.Quality = 0;
	df.PitchAndFamily = 0;
	strcpy_s(df.FaceName, "TIME NEW ROMAN");
	//创建ID3DXFont对象  
	D3DXCreateFontIndirect(g_pDevice, &df, &Font);

	isReady = true;
	return 0;
}