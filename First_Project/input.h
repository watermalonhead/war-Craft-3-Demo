#pragma once

#include <dinput.h>

// DEFINES ////////////////////////////////////////////////

#define ITEMS_NUM 10

// PROTOTYPES /////////////////////////////////////////////

// input
int DInput_Init(void);
void DInput_Shutdown(void);

int DInput_Init_Mouse(void);
int DInput_Init_Keyboard(void);
int DInput_Init_Joystick(int min_x=-256, int max_x=256,int min_y=-256, int max_y=256, int dead_band = 10);

int DInput_Read_Mouse(void);
int DInput_Read_Keyboard(void);
int DInput_Read_Joystick(void);

void DInput_Release_Mouse(void);
void DInput_Release_Keyboard(void);
void DInput_Release_Joystick(void);

bool DInput_Read_Mouse_Buffer();	//���ڻ���ģʽ��������
bool IsLButtonPressed();            //�ж�����������
bool IsRButtonPressed();            //�ж�����Ҽ�����
bool IsMButtonPressed();            //�ж�����м们�ְ���

// GLOBALS ////////////////////////////////////////////////

// EXTERNALS //////////////////////////////////////////////

extern HWND			g_hWnd; //�����ھ��
extern HINSTANCE	g_hInstance; //ʵ�����

// directinput globals
extern LPDIRECTINPUT8       lpdi;       // dinput object
extern LPDIRECTINPUTDEVICE8 lpdikey;    // dinput keyboard
extern LPDIRECTINPUTDEVICE8 lpdimouse;  // dinput mouse
extern LPDIRECTINPUTDEVICE8 lpdijoy;    // dinput joystick 

extern GUID joystickGUID; // guid for main joystick
extern char joyname[80];  // name of joystick

// these contain the target records for all di input packets
extern UCHAR keyboard_state[256]; // contains keyboard state table
extern DIMOUSESTATE mouse_state;  // contains state of mouse
extern DIJOYSTATE joy_state;      // contains state of joystick

extern DIDEVICEOBJECTDATA m_MouseData[ITEMS_NUM];  //���Ļ�����(����ģʽ�´洢���״̬)
extern long m_lMouseMoveX,m_lMouseMoveY;           //����X-Y�����ƶ���

extern int joystick_found;        // tracks if stick is plugged in

//////////////////////////////////////////////////////////////////////////////////