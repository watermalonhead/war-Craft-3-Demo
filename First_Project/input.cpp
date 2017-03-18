#define WIN32_LEAN_AND_MEAN  

#include "input.h"

// GLOBALS ////////////////////////////////////////////////

// directinput globals
LPDIRECTINPUT8       lpdi      = NULL;    // dinput object
LPDIRECTINPUTDEVICE8 lpdikey   = NULL;    // dinput keyboard
LPDIRECTINPUTDEVICE8 lpdimouse = NULL;    // dinput mouse
LPDIRECTINPUTDEVICE8 lpdijoy   = NULL;    // dinput joystick
GUID                 joystickGUID;        // guid for main joystick
char                 joyname[80];         // name of joystick

// these contain the target records for all di input packets
UCHAR keyboard_state[256]; // contains keyboard state table
DIMOUSESTATE mouse_state;  // contains state of mouse
DIJOYSTATE joy_state;      // contains state of joystick
int joystick_found = 0;    // tracks if joystick was found and inited

DIDEVICEOBJECTDATA m_MouseData[ITEMS_NUM];  //鼠标的缓冲区(缓冲模式)
long m_lMouseMoveX,m_lMouseMoveY;           //鼠标的X-Y轴总移动量

// FUNCTIONS /////////////////////////////////////////////////////////////////

int DInput_Init(void)
{
	if (FAILED(DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&lpdi, NULL)))
	   return (0);

	return(1);
}

void DInput_Shutdown(void)
{
	if (lpdi)
	{
		lpdi->Release();
		lpdi = NULL;
	}
}

BOOL CALLBACK DInput_Enum_Joysticks(LPCDIDEVICEINSTANCE lpddi,LPVOID guid_ptr) 
{
	// this function enumerates the joysticks, but stops at the first one and returns the instance guid of it, so we can create it

	*(GUID*)guid_ptr = lpddi->guidInstance; 

	// copy name into global
	strcpy(joyname, (char *)lpddi->tszProductName);

	// stop enumeration after one iteration
	return(DIENUM_STOP);
}

int DInput_Init_Joystick(int min_x, int max_x, int min_y, int max_y, int dead_zone)
{
	// this function initializes the joystick, it allows you to set the minimum and maximum x-y ranges 

	// first find the fucking GUID of your particular joystick
	lpdi->EnumDevices(DI8DEVCLASS_GAMECTRL, 
					  DInput_Enum_Joysticks, 
					  &joystickGUID, 
					  DIEDFL_ATTACHEDONLY); 

	if (lpdi->CreateDevice(joystickGUID, &lpdijoy, NULL) != DI_OK)
		return(0);

	// set cooperation level
	if (lpdijoy->SetCooperativeLevel(g_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return(0);

	// set data format
	if (lpdijoy->SetDataFormat(&c_dfDIJoystick) != DI_OK)
		return(0);

	// set the range of the joystick
	DIPROPRANGE joy_axis_range;

	// first x axis
	joy_axis_range.lMin = min_x;
	joy_axis_range.lMax = max_x;

	joy_axis_range.diph.dwSize       = sizeof(DIPROPRANGE); 
	joy_axis_range.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joy_axis_range.diph.dwObj        = DIJOFS_X;
	joy_axis_range.diph.dwHow        = DIPH_BYOFFSET;

	lpdijoy->SetProperty(DIPROP_RANGE,&joy_axis_range.diph);

	// now y-axis
	joy_axis_range.lMin = min_y;
	joy_axis_range.lMax = max_y;

	joy_axis_range.diph.dwSize       = sizeof(DIPROPRANGE); 
	joy_axis_range.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joy_axis_range.diph.dwObj        = DIJOFS_Y;
	joy_axis_range.diph.dwHow        = DIPH_BYOFFSET;

	lpdijoy->SetProperty(DIPROP_RANGE,&joy_axis_range.diph);

	// and now the dead band
	DIPROPDWORD dead_band;

	// scale dead zone by 100
	dead_zone *= 100;

	dead_band.diph.dwSize       = sizeof(dead_band);
	dead_band.diph.dwHeaderSize = sizeof(dead_band.diph);
	dead_band.diph.dwObj        = DIJOFS_X;
	dead_band.diph.dwHow        = DIPH_BYOFFSET;

	// deadband will be used on both sides of the range +/-
	dead_band.dwData            = dead_zone;

	// finally set the property
	lpdijoy->SetProperty(DIPROP_DEADZONE,&dead_band.diph);

	dead_band.diph.dwSize       = sizeof(dead_band);
	dead_band.diph.dwHeaderSize = sizeof(dead_band.diph);
	dead_band.diph.dwObj        = DIJOFS_Y;
	dead_band.diph.dwHow        = DIPH_BYOFFSET;

	// deadband will be used on both sides of the range +/-
	dead_band.dwData            = dead_zone;

	// finally set the property
	lpdijoy->SetProperty(DIPROP_DEADZONE,&dead_band.diph);

	// acquire the joystick
	if (lpdijoy->Acquire() != DI_OK)
		return(0);

	// set found flag
	joystick_found = 1;

	// return success
	return(1);

}

int DInput_Init_Mouse(void)
{
	// create a mouse device 
	if (lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL) != DI_OK)
		return(0);

	// set cooperation level
	// change to EXCLUSIVE FORGROUND for better control
	if (lpdimouse->SetCooperativeLevel(g_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return(0);

	// set data format
	if (lpdimouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
	   return(0);

	//设置鼠标设备的属性(使用缓冲区读数据)
	DIPROPDWORD dipROPWORD;
	dipROPWORD.diph.dwSize=sizeof(DIPROPDWORD);
	dipROPWORD.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	dipROPWORD.diph.dwObj=0;
	dipROPWORD.diph.dwHow=DIPH_DEVICE;
	dipROPWORD.dwData=ITEMS_NUM;  //#define ITEMS_NUM 10
	if(FAILED(lpdimouse->SetProperty(DIPROP_BUFFERSIZE,&dipROPWORD.diph)))
	{
		MessageBox(NULL,"设置鼠标设备属性失败.","警告",MB_OK|MB_ICONINFORMATION);
		DInput_Release_Mouse();
		return false;		
	}

	// acquire the mouse
	if (lpdimouse->Acquire()!=DI_OK)
		return(0);

	// return success
	return(1);

}

int DInput_Init_Keyboard(void)
{
	// create the keyboard device  
	if (lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL) != DI_OK)
	   return(0);

	// set cooperation level
	if (lpdikey->SetCooperativeLevel(g_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return(0);

	// set data format
	if (lpdikey->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
	   return(0);

	// acquire the keyboard
	if (lpdikey->Acquire()!=DI_OK)
	   return(0);

	// return success
	return(1);

}

int DInput_Read_Joystick(void)
{
	// make sure the joystick was initialized
	if (!joystick_found)
		return(0);

	if (lpdijoy)
	{
		lpdijoy->Poll();

		if (lpdijoy->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&joy_state) != DI_OK)
			return(0);
	}
	else
	{
		// joystick isn't plugged in, zero out state
		memset(&joy_state,0,sizeof(joy_state));

		// return error
		return(0);
	} // end else

	// return sucess
	return(1);

}

int DInput_Read_Keyboard(void)
{
	if (lpdikey)
	{
		if (lpdikey->GetDeviceState(256, (LPVOID)keyboard_state)!=DI_OK)
			return(0);
	}
	else
	{
		// keyboard isn't plugged in, zero out state
		memset(keyboard_state,0,sizeof(keyboard_state));

		// return error
		return(0);
	} // end else

	// return sucess
	return(1);
}

int DInput_Read_Mouse(void)
{
	if (lpdimouse)
	{
		if (lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state) != DI_OK)
			return(0);
	}
	else
	{
		// mouse isn't plugged in, zero out state
		memset(&mouse_state,0,sizeof(mouse_state));

		// return error
		return(0);
	} // end else

	// return sucess
	return(1);
} 


//捕捉鼠标按键和移动数据（基于缓冲模式的读取）
bool DInput_Read_Mouse_Buffer()
{
	DWORD dwReadNum=1;
	
	//每次读取数据前，一定要使鼠标缓冲区清零
	ZeroMemory(m_MouseData,sizeof(DIDEVICEOBJECTDATA)*ITEMS_NUM);
	//循环读取鼠标数据
	for(int i=0;i<ITEMS_NUM;i++)
	{
		if(DIERR_INPUTLOST == lpdimouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),&m_MouseData[i],&dwReadNum,0))
		{
			lpdimouse->Acquire();
			if(FAILED(lpdimouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),&m_MouseData[i],&dwReadNum,0)))
			{
				return false;
			}
		}

		if(m_MouseData[i].dwOfs == DIMOFS_X)
			m_lMouseMoveX += m_MouseData[i].dwData;
		
		if(m_MouseData[i].dwOfs==DIMOFS_Y)
			m_lMouseMoveY += m_MouseData[i].dwData;
	}
	return true;
}

//判断鼠标左键按下
bool IsLButtonPressed()
{
	for(int i=0;i<ITEMS_NUM;i++)
	{
		if((m_MouseData[i].dwOfs==DIMOFS_BUTTON0) && (m_MouseData[i].dwData & 0x80))
		{
			return true;
		}
	}
	return false;
}

//判断鼠标右键按下
bool IsRButtonPressed()
{
	for(int i=0;i<ITEMS_NUM;i++)
	{
		if((m_MouseData[i].dwOfs==DIMOFS_BUTTON1) && (m_MouseData[i].dwData & 0x80))
		{
			return true;
		}
	}
	return false;
}

//判断鼠标滚轮按下
bool IsMButtonPressed()
{
	for(int i=0;i<ITEMS_NUM;i++)
	{
		if((m_MouseData[i].dwOfs==DIMOFS_BUTTON2) && (m_MouseData[i].dwData & 0x80))
		{
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////

void DInput_Release_Joystick(void)
{
	if (lpdijoy)
    {    
		lpdijoy->Unacquire();
		lpdijoy->Release();
    }
}

void DInput_Release_Mouse(void)
{
	if (lpdimouse)
    {    
		lpdimouse->Unacquire();
		lpdimouse->Release();
    }
}

void DInput_Release_Keyboard(void)
{
	if (lpdikey)
    {
		lpdikey->Unacquire();
		lpdikey->Release();
    }
}