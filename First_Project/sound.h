#pragma once

#include <dsound.h>

////////////////////////////////////////////////////////////////////////////////

//��ʽ��Ƶ�ļ��ṹ��
typedef struct _STREAMDESC {
    LONG        lBufSection;
    LONG        lBufPos;
    LONG        lBufSize;
    LPDIRECTSOUNDBUFFER lpBuffer; //DirectSound�λ�����

    WAVEFORMATEX wfx;//WAV���ݸ�ʽ

    LONG        lDataPos;//�ļ��еĶ�ȡ�������ݵ��α�
    LONG        lDataLen;
    LPCSTR      filename;
    LONG        lSecondsLen;//�ļ����ݵ�ʱ�䳤��(��)
    LONG        lMillisecsHalfBuf;//��������һ���ܲ��ŵ�ʱ��(����)
    LONG        lReadLen;//һ�ζ�ȡ��������
    BOOL        bLoop;
    BOOL        bStaticBuffer;  //��ʽ/��̬ ������
} STREAMDESC, *LPSTREAMDESC;

class CDSound
{
protected:
    LPDIRECTSOUND8      lpDSound;
    BOOL                bSoundAble;//�����豸�Ƿ����

    DWORD               dwCoopLevel;//Э���ȼ�
    BOOL                bSetPrimaryFormat;//�Ƿ�����������������ĸ�ʽ

public:
    CDSound( DWORD dwNum, DWORD dwCL );
    CDSound( void );
    ~CDSound();

    //>>>>>>>>>> ���ó�Ա���� >>>>>>>>>>>>
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

    // �ⲿ���ó�Ա����
    LPDIRECTSOUND8 Get_lpDSound(void){return lpDSound;}
    DWORD Get_dwCoopLevel(void)      {return dwCoopLevel;}
    BOOL  Get_bSetPrimaryFormat(void){return bSetPrimaryFormat;}

    //>>>>>>>>>> ��̬����������ز�����Ա >>>>>>>>>>>>>>>
protected:
    LPDIRECTSOUNDBUFFER *ppBuffer;//�μ���̬������������׵�ַ
    DWORD    dwNumStatics;//���ж��ٸ���̬������
    DWORD    dwMaxBytesStatic;//��̬�������ռ�����ֵ

public:
    // ���þ�̬�������ĳ�Ա����
    void Set_dwMaxBytesStatic( DWORD dwBytes )
    {
        dwMaxBytesStatic = dwBytes;
    }
    void Set_dwNumStatics( DWORD dwNum );

    // ��̬�������Ĵ������ͷ�
    BOOL CreateStaticBuffer( DWORD dwIndex, LPCSTR filename);
    void ReleaseStaticBuffer( DWORD dwIndex );

    // ��̬���������ſ���
    void PlayStatic( DWORD dwIndex,BOOL bLoop,BOOL bCompulsory);
    void StopStatic( DWORD dwIndex );
    BOOL StaticIsPlaying( DWORD dwIndex );

    // ��̬����������������
    void SetStaticVolume( DWORD dwIndex, LONG lVolume );
    void AddStaticVolume( DWORD dwIndex, LONG lAdd );
    void SubStaticVolume( DWORD dwIndex, LONG lSub );

    // ��̬���������������Ե�����
    void SetStaticPan( DWORD dwIndex, LONG lPan );
    void AddStaticPan( DWORD dwIndex, LONG lAdd );
    void SubStaticPan( DWORD dwIndex, LONG lSub );

    // ��̬����������Ƶ�ʵ�����
    void SetStaticFreq( DWORD dwIndex, DWORD dwFreq );
    void AddStaticFreq( DWORD dwIndex, DWORD dwAdd );
    void SubStaticFreq( DWORD dwIndex, DWORD dwSub );

    // �ⲿ����
    LPDIRECTSOUNDBUFFER Get_pStaticBuffer( DWORD dwIndex );
    DWORD  Get_dwMaxBytesStatic(void){return dwMaxBytesStatic;}
    DWORD  Get_dwNumStatics( void )  {return dwNumStatics;}

    //>>>>>>>>>> ��ʽ����������ز�����Ա >>>>>>>>>>>>>>>
protected:
    LPSTREAMDESC *ppStream;//�μ���ʽ��������������׵�ַ
    DWORD        dwNumStreams;//��ʽ�������ĸ���
    DWORD        dwMaxBytesStream;//��ʽ�������ռ�����ֵ

public:
    void Set_dwMaxBytesStream( DWORD dwBytes )
    {
        dwMaxBytesStream = ( (DWORD)(dwBytes/8) )*8;
    }
    void Set_dwNumStreams( DWORD dwNum );

    // ��ʽ�������Ĵ�������պ��ͷ�
    BOOL CreateStreamBuffer( DWORD dwIndex, LPCSTR filename );
    void ClearStreamBuffer( DWORD dwIndex );
    void ReleaseStreamBuffer( DWORD dwIndex );

    // ��ʽ�������Ĳ���ֹͣ����
    void PlayStream( DWORD dwIndex, BOOL bLoop );
    void PauseStream( DWORD dwIndex );
    void StopStream( DWORD dwIndex );

    // �����������ݵĲ����α�
    void SetStreamPos( DWORD dwIndex, LONG lPos );
    void AddStreamPos( DWORD dwIndex, LONG lAdd );
    void SubStreamPos( DWORD dwIndex, LONG lSub );

    // ������ʽ������������
    void AddStreamVolume( DWORD dwIndex, LONG lAdd );
    void SubStreamVolume( DWORD dwIndex, LONG lSub );

    // ������ʽ������������,�Լ�������ʽ����
    BYTE *LoadStreamData( DWORD dwIndex );
    BOOL UpdateStreamBuffer( DWORD dwIndex );

    // �ⲿ����
    LPSTREAMDESC* Get_ppStream(void){return ppStream;}
    DWORD Get_dwMaxBytesStream(void){return dwMaxBytesStream;}
    DWORD Get_dwNumStreams(void)    {return dwNumStreams;}
};

///////////////////////////////////////////////////////////////////