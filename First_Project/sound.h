#pragma once

#include <dsound.h>

////////////////////////////////////////////////////////////////////////////////

//流式音频文件结构体
typedef struct _STREAMDESC {
    LONG        lBufSection;
    LONG        lBufPos;
    LONG        lBufSize;
    LPDIRECTSOUNDBUFFER lpBuffer; //DirectSound次缓冲区

    WAVEFORMATEX wfx;//WAV数据格式

    LONG        lDataPos;//文件中的读取声音数据的游标
    LONG        lDataLen;
    LPCSTR      filename;
    LONG        lSecondsLen;//文件数据的时间长度(秒)
    LONG        lMillisecsHalfBuf;//缓冲区的一半能播放的时间(毫秒)
    LONG        lReadLen;//一次读取的数据量
    BOOL        bLoop;
    BOOL        bStaticBuffer;  //流式/静态 缓冲区
} STREAMDESC, *LPSTREAMDESC;

class CDSound
{
protected:
    LPDIRECTSOUND8      lpDSound;
    BOOL                bSoundAble;//声音设备是否可用

    DWORD               dwCoopLevel;//协作等级
    BOOL                bSetPrimaryFormat;//是否可以设置主缓冲区的格式

public:
    CDSound( DWORD dwNum, DWORD dwCL );
    CDSound( void );
    ~CDSound();

    //>>>>>>>>>> 设置成员变量 >>>>>>>>>>>>
    void Set_dwCoopLevel( DWORD dwCL );
    void Set_bSetPrimaryFormat( BOOL bSetFormat );

    BOOL InitDSound( HWND hWnd );
    BOOL SetPrimaryBufferFormat( DWORD dwChannels,
                                 DWORD dwSamplesPerSec,
                                 DWORD dwBitsPerSample );
    void ReleaseDSound(void);
    void RestoreBuffer( LPDIRECTSOUNDBUFFER lpBuf );
    BYTE *LoadWaveFile(WAVEFORMATEX *format,
                        DWORD        *pdwDataLen,
                        LPCSTR       filename,
                        BOOL         bReadData);

    // 外部调用成员变量
    LPDIRECTSOUND8 Get_lpDSound(void){return lpDSound;}
    DWORD Get_dwCoopLevel(void)      {return dwCoopLevel;}
    BOOL  Get_bSetPrimaryFormat(void){return bSetPrimaryFormat;}

    //>>>>>>>>>> 静态缓冲区的相关操作成员 >>>>>>>>>>>>>>>
protected:
    LPDIRECTSOUNDBUFFER *ppBuffer;//次级静态缓冲区数组的首地址
    DWORD    dwNumStatics;//共有多少个静态缓冲区
    DWORD    dwMaxBytesStatic;//静态缓冲区空间的最大值

public:
    // 设置静态缓冲区的成员变量
    void Set_dwMaxBytesStatic( DWORD dwBytes )
    {
        dwMaxBytesStatic = dwBytes;
    }
    void Set_dwNumStatics( DWORD dwNum );

    // 静态缓冲区的创建与释放
    BOOL CreateStaticBuffer( DWORD dwIndex, LPCSTR filename);
    void ReleaseStaticBuffer( DWORD dwIndex );

    // 静态缓冲区播放控制
    void PlayStatic( DWORD dwIndex,BOOL bLoop,BOOL bCompulsory);
    void StopStatic( DWORD dwIndex );
    BOOL StaticIsPlaying( DWORD dwIndex );

    // 静态缓冲区音量的设置
    void SetStaticVolume( DWORD dwIndex, LONG lVolume );
    void AddStaticVolume( DWORD dwIndex, LONG lAdd );
    void SubStaticVolume( DWORD dwIndex, LONG lSub );

    // 静态缓冲区声音均衡性的设置
    void SetStaticPan( DWORD dwIndex, LONG lPan );
    void AddStaticPan( DWORD dwIndex, LONG lAdd );
    void SubStaticPan( DWORD dwIndex, LONG lSub );

    // 静态缓冲区声音频率的设置
    void SetStaticFreq( DWORD dwIndex, DWORD dwFreq );
    void AddStaticFreq( DWORD dwIndex, DWORD dwAdd );
    void SubStaticFreq( DWORD dwIndex, DWORD dwSub );

    // 外部调用
    LPDIRECTSOUNDBUFFER Get_pStaticBuffer( DWORD dwIndex );
    DWORD  Get_dwMaxBytesStatic(void){return dwMaxBytesStatic;}
    DWORD  Get_dwNumStatics( void )  {return dwNumStatics;}

    //>>>>>>>>>> 流式缓冲区的相关操作成员 >>>>>>>>>>>>>>>
protected:
    LPSTREAMDESC *ppStream;//次级流式缓冲描述数组的首地址
    DWORD        dwNumStreams;//流式缓冲区的个数
    DWORD        dwMaxBytesStream;//流式缓冲区空间的最大值

public:
    void Set_dwMaxBytesStream( DWORD dwBytes )
    {
        dwMaxBytesStream = ( (DWORD)(dwBytes/8) )*8;
    }
    void Set_dwNumStreams( DWORD dwNum );

    // 流式缓冲区的创建、清空和释放
    BOOL CreateStreamBuffer( DWORD dwIndex, LPCSTR filename );
    void ClearStreamBuffer( DWORD dwIndex );
    void ReleaseStreamBuffer( DWORD dwIndex );

    // 流式缓冲区的播放停止控制
    void PlayStream( DWORD dwIndex, BOOL bLoop );
    void PauseStream( DWORD dwIndex );
    void StopStream( DWORD dwIndex );

    // 调节整个数据的播放游标
    void SetStreamPos( DWORD dwIndex, LONG lPos );
    void AddStreamPos( DWORD dwIndex, LONG lAdd );
    void SubStreamPos( DWORD dwIndex, LONG lSub );

    // 设置流式缓冲区的音量
    void AddStreamVolume( DWORD dwIndex, LONG lAdd );
    void SubStreamVolume( DWORD dwIndex, LONG lSub );

    // 更新流式缓冲区的数据,以及载入流式数据
    BYTE *LoadStreamData( DWORD dwIndex );
    BOOL UpdateStreamBuffer( DWORD dwIndex );

    // 外部调用
    LPSTREAMDESC* Get_ppStream(void){return ppStream;}
    DWORD Get_dwMaxBytesStream(void){return dwMaxBytesStream;}
    DWORD Get_dwNumStreams(void)    {return dwNumStreams;}
};

///////////////////////////////////////////////////////////////////