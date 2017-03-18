#include "sound.h"

CDSound::CDSound( DWORD dwNum, DWORD dwCL )
{
    lpDSound     = NULL;
    bSoundAble   = FALSE;
    dwCoopLevel  = dwCL;
    if( dwCoopLevel == DSSCL_NORMAL )
        bSetPrimaryFormat = FALSE;
    else 
        bSetPrimaryFormat = TRUE;

    // Ϊ��̬���������ݿ��ٿռ�,��������ÿ��Ԫ��
    ppBuffer = new LPDIRECTSOUNDBUFFER[dwNum];
    ZeroMemory( ppBuffer, dwNum*sizeof(LPDIRECTSOUNDBUFFER) );
    // Ĭ�ϵľ�̬�������Ĵ�С����Ϊ1M
    dwMaxBytesStatic = 1024000;
    dwNumStatics  = dwNum;

    // Ϊ��ʽ�����������ٿռ�,��������ÿ��Ԫ��
    // Ĭ�������ֻ�ܴ���2����ʽ������
    ppStream = new LPSTREAMDESC[2];
    ZeroMemory( ppStream, 2*sizeof(LPSTREAMDESC) );
    // Ĭ�ϵ���ʽ�������Ĵ�С����Ϊ1M
    dwMaxBytesStream = 1024000;
    dwNumStreams = 2;
}

CDSound::CDSound( void )
{
    lpDSound            = NULL;
    bSoundAble          = FALSE;
    // Ĭ�������Э���ȼ���һ��ȼ�,���ܸı�����������ʽ
    dwCoopLevel         = DSSCL_NORMAL;
    bSetPrimaryFormat   = FALSE;

    // Ϊ��̬���������ٿռ�,��������ÿ��Ԫ��
    // Ĭ�������ֻ�ܴ���5����̬�μ�������
    ppBuffer      = new LPDIRECTSOUNDBUFFER[5];
    ZeroMemory( ppBuffer, 5*sizeof( LPDIRECTSOUNDBUFFER ) );
    // Ĭ�ϵľ�̬�������Ĵ�С������Ϊ1M
    dwMaxBytesStatic = 1024000;
    // Ĭ�������ֻ�ܴ���5����̬�μ�������
    dwNumStatics        = 5;

    // Ϊ��ʽ�����������ٿռ�,��������ÿ��Ԫ��
    // Ĭ�������ֻ�ܴ���2����ʽ������
    ppStream = new LPSTREAMDESC[2];
    ZeroMemory( ppStream, 2*sizeof(LPSTREAMDESC) );
    // Ĭ�ϵ���ʽ�������Ĵ�С����Ϊ1M
    dwMaxBytesStream = 1024000;
    dwNumStreams = 2;
}

CDSound::~CDSound()
{
    // �ͷž�̬�μ��������ռ�
    if( ppBuffer )  
	{
        delete[] ppBuffer;
        ppBuffer = NULL;
    }
    // �ͷ���ʽ�μ����������ռ�
    if( ppStream ) 
	{
        for (DWORD dw=0; dw< dwNumStreams; dw++ )
        {
            if(ppStream[dw]) 
			{
                delete ppStream[dw];
                ppStream[dw] = NULL;
            }
        }
        delete[] ppStream;
        ppStream = NULL;
    }
    // �ͷ�DirectSound����
    ReleaseDSound();
}

void CDSound::ReleaseDSound(void)
{
	if( lpDSound != NULL )
	{
		lpDSound->Release();
		lpDSound = NULL;
	}
}

/***************************************************************
* ������: Set_dwCoopLevel(...)
* ����: ����DirectSoundЭ���ȼ�
****************************************************************/
void CDSound::Set_dwCoopLevel( DWORD dwCL )
{
    dwCoopLevel = dwCL;
    if( dwCoopLevel == DSSCL_NORMAL )
        bSetPrimaryFormat = FALSE;
    else 
        bSetPrimaryFormat = TRUE;
}

/***************************************************************
* ������: Set_bSetPrimaryFormat(...)
* ����: ����DirectSound�Ƿ���������������ʽ
****************************************************************/
void CDSound::Set_bSetPrimaryFormat( BOOL bSetFormat )
{
    if( bSetFormat )
    {
        dwCoopLevel       = DSSCL_PRIORITY;
        bSetPrimaryFormat = TRUE;
    }
    else 
	{
        dwCoopLevel       = DSSCL_NORMAL;
        bSetPrimaryFormat = FALSE;
    }
}

/***************************************************************
* ������: InitDSound(...)
* ����: ��ʼ��DirectSound����
****************************************************************/
BOOL CDSound::InitDSound( HWND hWnd )
{
    // �ͷ���ǰ�����Ѿ������˵�DirectSound����
    ReleaseDSound();

	// ����DirectSound����
    HRESULT hr = DirectSoundCreate8( NULL, &lpDSound, NULL );
	if( FAILED( hr ) )
	{
        MessageBox( hWnd, TEXT("����DirectSound����ʧ��\n")
                          TEXT("�����޷���������\n"),
                          TEXT("DirectSoundCreate8"), MB_OK );
		return FALSE;
	}

	// ����DirectSoundЭ���ȼ�
    hr = lpDSound->SetCooperativeLevel(hWnd,dwCoopLevel);
	if( FAILED( hr ) )
	{
        MessageBox( hWnd, TEXT("����DirectSoundЭ���ȼ�ʧ��\n")
                          TEXT("�����޷���������\n"),
                          TEXT("SetCooperativeLevel"), MB_OK );
		return FALSE;
	}

    // ��ʼ���ɹ�
    bSoundAble = TRUE;

	return TRUE; 
}

/***************************************************************
* ������: SetPrimaryBufferFormat(...)
* ����: ������������������ʽ
****************************************************************/
BOOL CDSound::SetPrimaryBufferFormat( DWORD dwChannels,
                                      DWORD dwSamplesPerSec,
                                      DWORD dwBitsPerSample)
{
    // Э���ȼ���������������������ʽ�������豸��Ч�򷵻�
    if( !bSetPrimaryFormat||!bSoundAble )
        return FALSE;

    HRESULT  hr;
    // ��ʱ��������ָ��
    LPDIRECTSOUNDBUFFER lpPrimaryBuffer = NULL;

    // ���� DSBUFFERDESC �ṹ
    DSBUFFERDESC	dsbd;
	ZeroMemory( &dsbd, sizeof(dsbd) );
    dsbd.dwSize         = sizeof(DSBUFFERDESC);
    dsbd.dwFlags        = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes  = 0; 
    dsbd.lpwfxFormat    = NULL;
    // ������ʱ��������
    hr = lpDSound->CreateSoundBuffer( &dsbd, &lpPrimaryBuffer, NULL );
	if( FAILED( hr ) )
        return FALSE;

    // ����wave��ʽ�ṹ
	WAVEFORMATEX	wfx;
	ZeroMemory( &wfx, sizeof(wfx) );
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = (WORD)dwChannels;
    wfx.nSamplesPerSec  = dwSamplesPerSec;
    wfx.wBitsPerSample  = (WORD)dwBitsPerSample;
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    // ����wfx�ṹ��ֵ��������������ʽ
    hr = lpPrimaryBuffer->SetFormat( &wfx );
	if( FAILED( hr ) )
        return FALSE;

    // �ͷ���ʱ��������
    if( lpPrimaryBuffer )
    {
        lpPrimaryBuffer->Release();
        lpPrimaryBuffer = NULL;
    }

    return TRUE;
}



/***************************************************************
* ������: RestoreBuffer(...)
* ����: �ָ�ָ���Ļ������ռ�
****************************************************************/
void CDSound::RestoreBuffer( LPDIRECTSOUNDBUFFER lpBuf )
{
    HRESULT hr;
    if( lpBuf )
    {
        hr = lpBuf->Restore();
        while( hr == DSBSTATUS_BUFFERLOST )
            lpBuf->Restore();
    }
}

/***************************************************************
* ������: LoadWaveFile(...)
* ����: ��ȡ�����ļ�������
* ע��: ��bReadData������ֵΪ��ʱ��������һ��pDataָ��ָ����������ݻ�������ʹ�ú�����ͷ���
****************************************************************/
BYTE* CDSound::LoadWaveFile( WAVEFORMATEX  *format,
                              DWORD         *pdwDataLen,
                              LPCSTR        filename,
                              BOOL          bReadData)
{
	if( filename==NULL )
		return NULL;

	// ��ֻ��ģʽ��������WAVE�ļ���
	HMMIO file = mmioOpen((LPSTR)filename,NULL,MMIO_READ);
	if( file==NULL )
		return NULL;

	// �жϸ��ļ��Ƿ�Ϊ"WAVE"�ļ���ʽ
	char ch[4];
	mmioSeek( file, 8L, SEEK_SET );
	mmioRead( file, ch, 4L );
    if(ch[0]!='W'||ch[1]!='A'||ch[2]!='V'||ch[3]!='E')
	{
		mmioClose( file, 0 );
		return NULL;
	}

	// ��ȡWAVE�ļ���WAVEFORMATEX�ṹ������
	if( format )
	{
		mmioSeek( file, 20L, SEEK_SET );
		mmioRead( file, (HPSTR)format, sizeof(WAVEFORMATEX) );
	}

    // ��ȡ�������ݵĳ���
    DWORD dwDataLen;
    mmioSeek( file, 40L, SEEK_SET );
    mmioRead( file, (char*)&dwDataLen, sizeof(DWORD) );

    // �Ƿ�Ҫ��¼�������ݵĳ���
    if( pdwDataLen )
        (*pdwDataLen) = dwDataLen;

    // �Ƿ��ȡ��������
    BYTE *pData = NULL;
    if( bReadData )
    {
        // ����һ����������ݵĿռ�
        pData = new BYTE[ dwDataLen ];
        // ��ȡ��������
        mmioSeek( file, 44L, SEEK_SET );
        mmioRead( file, (char*)pData, (LONG)dwDataLen );
    }

    mmioClose( file, 0);
	return pData;
}

/***************************************************************
* ������: Set_dwNumStatics(...)
* ����: ����CDSound����ɴ����ľ�̬�μ���������Ŀ
****************************************************************/
void CDSound::Set_dwNumStatics( DWORD dwNum )
{
    // ���ôμ��������ĸ��������ڴ���DirectSound����֮ǰ����
    if( lpDSound )
        return;

    // �ͷ���ǰ�����Ŀռ�
    if( ppBuffer )  
	{
        delete[] ppBuffer;
        ppBuffer  = NULL;
    }

    // �����µĿռ�
    ppBuffer = new LPDIRECTSOUNDBUFFER[dwNum];
    // ����
    ZeroMemory( ppBuffer, dwNum*sizeof(LPDIRECTSOUNDBUFFER) );
    dwNumStatics = dwNum;
}

/***************************************************************
* ������: CreateStaticBuffer(...)
* ����: ����һ���μ�������,���ҽ�filenameָ����������ݵ���û�����
****************************************************************/
BOOL CDSound::CreateStaticBuffer( DWORD dwIndex, LPCSTR filename )
{
    // �����Ч��
    if( !bSoundAble )             return FALSE;
    if( dwIndex >= dwNumStatics ) return FALSE;
    if( ppBuffer == NULL )        return FALSE;

    HRESULT hr;
	BYTE *pData; 		//��ʱ���ݻ�����ָ��
	WAVEFORMATEX format;//���ݵĸ�ʽ
	DWORD dwDataLen;	//�������ݴ�С

    // ע: LoadWaveFile����������һ����pDataָ��Ŀռ�,����Ҫ�ͷ���
	pData = LoadWaveFile( &format, &dwDataLen, filename, TRUE );

	if( pData == NULL )
        return FALSE ;

    // �޶��������Ĵ�С
    if( dwDataLen > dwMaxBytesStatic )
        dwDataLen = dwMaxBytesStatic;

    // ���� DSBUFFERDESC �ṹ,���Ҹ��ݸ����Դ����μ���̬������
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(dsbd) );
	dsbd.dwSize		   = sizeof(dsbd);
	dsbd.dwFlags	   = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN |	
					     DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
	dsbd.dwBufferBytes = dwDataLen;
	dsbd.lpwfxFormat   = &format;
	hr = lpDSound->CreateSoundBuffer(&dsbd,&ppBuffer[dwIndex],NULL);
	if( FAILED( hr ))  {
		delete[] pData;
        return FALSE;
	}

	LPVOID lpvPtr;
	DWORD dwBytes;

    // �������������Ա�����д������
	hr = ppBuffer[dwIndex]->Lock(0,0,&lpvPtr,&dwBytes,NULL,NULL, DSBLOCK_ENTIREBUFFER);
	if( FAILED(hr) ) {
        RestoreBuffer( ppBuffer[dwIndex] );
        delete[] pData;
        return FALSE;
    }

    // �����ݴ��ݵ�������
	memcpy( lpvPtr, pData, dwBytes );

    // �������������������
    hr = ppBuffer[dwIndex]->Unlock( lpvPtr, dwBytes, NULL, 0 );
	if( FAILED(hr) ) {
        RestoreBuffer( ppBuffer[dwIndex] );
		delete[] pData;
        return FALSE;
	}

	delete[] pData;
	return TRUE;
}

/***************************************************************
* ������: ReleaseStaticBuffer(...)
* ����: ���ָ���Ĵμ�������
****************************************************************/
void CDSound::ReleaseStaticBuffer( DWORD dwIndex )
{
    // �����Ч��
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    ppBuffer[dwIndex]->Release();
    ppBuffer[dwIndex] = NULL;
}

/***************************************************************
* ������: PlayStatic(...)
* ����: ���ž�̬����������������
****************************************************************/
void CDSound::PlayStatic( DWORD dwIndex, BOOL bLoop, BOOL bCompulsory )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    // ��ȡ��������״̬
	DWORD dwStatus;
	HRESULT hr = ppBuffer[dwIndex]->GetStatus( &dwStatus );
	if( FAILED( hr ) )
        return;

    // ��ʧ�˻����������»��
    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        RestoreBuffer( ppBuffer[dwIndex] );
        return;
    }

    // ����ǿ���Դ�ͷ����
    if( !bCompulsory ) 
    {
        // �Ѿ������򷵻�
        if( dwStatus & DSBSTATUS_PLAYING )
            return;
        // ��δ�����򲥷�
        else 
		{
            ppBuffer[dwIndex]->SetCurrentPosition(0);
		    ppBuffer[dwIndex]->Play(0,0,(bLoop?DSBPLAY_LOOPING:0));
        }
    }
    // ǿ���Դ�ͷ����
    else 
	{
        ppBuffer[dwIndex]->SetCurrentPosition(0);
		ppBuffer[dwIndex]->Play(0,0,(bLoop?DSBPLAY_LOOPING:0));
    }
}

/***************************************************************
* ������: StopStatic(...)
* ����: ֹͣ���ž�̬������
****************************************************************/
void CDSound::StopStatic( DWORD dwIndex )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    ppBuffer[dwIndex]->Stop();
}

/***************************************************************
* ������: StaticIsPlaying(...)
* ����: ��̬�������Ƿ����ڲ���
****************************************************************/
BOOL CDSound::StaticIsPlaying( DWORD dwIndex )
{
    // �����Ч��
    if( !bSoundAble )             return FALSE;
    if( dwIndex >= dwNumStatics ) return FALSE;
    if( ppBuffer == NULL )        return FALSE;
    if( ppBuffer[dwIndex]==NULL ) return FALSE;

    // ��ȡ��������״̬
	DWORD dwStatus;
	HRESULT hr = ppBuffer[dwIndex]->GetStatus( &dwStatus );
	if( FAILED( hr ) )
        return FALSE;

    // ��ʧ�˻����������»�� 
    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        RestoreBuffer( ppBuffer[dwIndex] );
        return FALSE;
    }
	else if( dwStatus & DSBSTATUS_PLAYING )
		return TRUE;
	else
		return FALSE;
}

/***************************************************************
* ������: SetStaticVolume(...)
* ����: ����ָ����̬������������
****************************************************************/
void CDSound::SetStaticVolume( DWORD dwIndex, LONG lVolume )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    if( lVolume > DSBVOLUME_MAX )   lVolume = DSBVOLUME_MAX;
    if( lVolume < DSBVOLUME_MIN )   lVolume = DSBVOLUME_MIN;
    ppBuffer[dwIndex]->SetVolume( lVolume );
}

/***************************************************************
* ������: AddStaticVolume(...)
* ����: ��ָ���ľ�̬����������lAddָ��������
****************************************************************/
void CDSound::AddStaticVolume( DWORD dwIndex, LONG lAdd )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    LONG  lVolume;
    if( ppBuffer[dwIndex] )
    {
        ppBuffer[dwIndex]->GetVolume( &lVolume );
        lVolume += lAdd;

        if( lVolume > DSBVOLUME_MAX )   lVolume = DSBVOLUME_MAX;
        if( lVolume < DSBVOLUME_MIN )   lVolume = DSBVOLUME_MIN;
        ppBuffer[dwIndex]->SetVolume( lVolume );
    }
}

/***************************************************************
* ������: SubStaticVolume(...)
* ����: ��ָ���ľ�̬����������lSubָ��������
****************************************************************/
void CDSound::SubStaticVolume( DWORD dwIndex, LONG lSub )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    LONG  lVolume;
    if( ppBuffer[dwIndex] )
    {
        ppBuffer[dwIndex]->GetVolume( &lVolume );
        lVolume -= lSub;

        if( lVolume > DSBVOLUME_MAX )   lVolume = DSBVOLUME_MAX;
        if( lVolume < DSBVOLUME_MIN )   lVolume = DSBVOLUME_MIN;
        ppBuffer[dwIndex]->SetVolume( lVolume );
    }
}

/***************************************************************
* ������: SetStaticPan(...)
* ����: ����ָ����̬������������������ΪlPanָ����ֵ
****************************************************************/
void CDSound::SetStaticPan( DWORD dwIndex, LONG lPan )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    if( lPan > DSBPAN_RIGHT )  lPan = DSBPAN_RIGHT;
    if( lPan < DSBPAN_LEFT  )  lPan = DSBPAN_LEFT;
    ppBuffer[dwIndex]->SetPan( lPan );
}

/***************************************************************
* ������: AddStaticPan(...)
* ����: ��ָ���ľ�̬�������ľ���������lAddָ����ֵ
****************************************************************/
void CDSound::AddStaticPan( DWORD dwIndex, LONG lAdd )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    LONG  lPan;
    if( ppBuffer[dwIndex] )
    {
        ppBuffer[dwIndex]->GetPan( &lPan );
        lPan += lAdd;

        if( lPan > DSBPAN_RIGHT )  lPan = DSBPAN_RIGHT;
        if( lPan < DSBPAN_LEFT  )  lPan = DSBPAN_LEFT;
        ppBuffer[dwIndex]->SetPan( lPan );
    }
}

/***************************************************************
* ������: SubStaticPan(...)
* ����: ��ָ���ľ�̬�������ľ���������lSubָ����ֵ
****************************************************************/
void CDSound::SubStaticPan( DWORD dwIndex, LONG lSub )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    LONG  lPan;
    if( ppBuffer[dwIndex] )
    {
        ppBuffer[dwIndex]->GetPan( &lPan );
        lPan -= lSub;

        if( lPan > DSBPAN_RIGHT )  lPan = DSBPAN_RIGHT;
        if( lPan < DSBPAN_LEFT  )  lPan = DSBPAN_LEFT;
        ppBuffer[dwIndex]->SetPan( lPan );
    }
}

/***************************************************************
* ������: SetStaticFreq(...)
* ����: ����ָ����̬������������Ƶ��ΪdwFreqָ����ֵ
****************************************************************/
void CDSound::SetStaticFreq( DWORD dwIndex, DWORD dwFreq )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    // ����Ƶ�ʷ�Χ����ΪԭʼƵ��
    if( dwFreq < DSBFREQUENCY_MIN || dwFreq > DSBFREQUENCY_MAX )
        dwFreq = DSBFREQUENCY_ORIGINAL;
    ppBuffer[dwIndex]->SetFrequency( dwFreq );
}

/***************************************************************
* ������: AddStaticFreq(...)
* ����: ��ָ���ľ�̬������������Ƶ�����dwAddָ����ֵ
****************************************************************/
void CDSound::AddStaticFreq( DWORD dwIndex, DWORD dwAdd )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    DWORD dwFreq;
    if( ppBuffer[dwIndex] )
    {
        ppBuffer[dwIndex]->GetFrequency( &dwFreq );
        dwFreq += dwAdd;

        if( dwFreq < DSBFREQUENCY_MIN ) dwFreq = DSBFREQUENCY_MIN;
        if( dwFreq > DSBFREQUENCY_MAX ) dwFreq = DSBFREQUENCY_MAX;
        ppBuffer[dwIndex]->SetFrequency( dwFreq );
    }
}

/***************************************************************
* ������: SubStaticFreq(...)
* ����: ��ָ���ľ�̬������������Ƶ�ʽ���dwSubָ����ֵ
****************************************************************/
void CDSound::SubStaticFreq( DWORD dwIndex, DWORD dwSub )
{
    // �����Ч��
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    DWORD dwFreq;
    if( ppBuffer[dwIndex] )
    {
        ppBuffer[dwIndex]->GetFrequency( &dwFreq );
        dwFreq -= dwSub;

        if( dwFreq < DSBFREQUENCY_MIN ) dwFreq = DSBFREQUENCY_MIN;
        if( dwFreq > DSBFREQUENCY_MAX ) dwFreq = DSBFREQUENCY_MAX;
        ppBuffer[dwIndex]->SetFrequency( dwFreq );
    }
}

/***************************************************************
* ������: Get_pStaticBuffer(...)
* ����: ��ȡָ���ľ�̬������ָ��
****************************************************************/
LPDIRECTSOUNDBUFFER CDSound::Get_pStaticBuffer( DWORD dwIndex )
{
    // �����Ч��
    if( dwIndex >= dwNumStatics ) return NULL;
    if( ppBuffer == NULL )        return NULL;

    return ppBuffer[dwIndex];
}

/***************************************************************
* ������: Set_dwNumStreams(...)
* ����: ����CDSound����ɴ�������ʽ�μ���������Ŀ
****************************************************************/
void CDSound::Set_dwNumStreams( DWORD dwNum )
{
    // ���ôμ��������ĸ��������ڴ���DirectSound����֮ǰ����
    if( lpDSound )
        return;

    // �ͷ���ǰ����ʽ�μ����������ռ�
    if( ppStream ) 
	{
        for (DWORD dw=0; dw< dwNumStreams; dw++ )
        {
            if(ppStream[dw]) {
                delete ppStream[dw];
                ppStream[dw] = NULL;
            }
        }
        delete[] ppStream;
        ppStream = NULL;
    }

    // ���µĸ��������µĿռ䣬������
    ppStream = new LPSTREAMDESC[dwNum];
    ZeroMemory( ppStream, dwNum*sizeof(LPSTREAMDESC) );
    dwNumStreams = dwNum;
}

/***************************************************************
* ��������CreateStreamBuffer(...)
* ���ܣ�����һ��ʽ������������ָ�����ļ��������ݵĿ�ͷһ�������뻺����
****************************************************************/
BOOL CDSound::CreateStreamBuffer( DWORD dwIndex, LPCSTR filename )
{
    // �����Ч��
    if( !bSoundAble )             return FALSE;
    if( dwIndex >= dwNumStreams ) return FALSE;
    if( ppStream == NULL )        return FALSE;

    ppStream[dwIndex] = new STREAMDESC;
    ZeroMemory( ppStream[dwIndex],sizeof(STREAMDESC) );
    ppStream[dwIndex]->filename = filename;

    // ��ȡ�ļ�����Ϣ
	LoadWaveFile( &ppStream[dwIndex]->wfx,
                  (DWORD*)&ppStream[dwIndex]->lDataLen,
                  ppStream[dwIndex]->filename, FALSE );

    if(ppStream[dwIndex]->lDataLen==0) 
	{
        delete ppStream[dwIndex];
        ppStream[dwIndex] = NULL;
        return FALSE;
    }

    // �޶��������Ĵ�С
    if( ppStream[dwIndex]->lDataLen > (LONG)dwMaxBytesStream )
    {
        ppStream[dwIndex]->lBufSize      = (LONG)dwMaxBytesStream;
        ppStream[dwIndex]->bStaticBuffer = FALSE;
    }
    else 
	{
        ppStream[dwIndex]->lBufSize   = ppStream[dwIndex]->lDataLen;
        ppStream[dwIndex]->bStaticBuffer = TRUE;
    }

    HRESULT hr;
    // ���� DSBUFFERDESC �ṹ,���Ҹ��ݸ����Դ���������
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(dsbd) );
	dsbd.dwSize		   = sizeof(dsbd);
	dsbd.dwFlags	   = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN |	
					     DSBCAPS_CTRLVOLUME ;
	dsbd.dwBufferBytes = (DWORD)ppStream[dwIndex]->lBufSize;
	dsbd.lpwfxFormat   = &ppStream[dwIndex]->wfx;
	hr = lpDSound->CreateSoundBuffer( &dsbd, &ppStream[dwIndex]->lpBuffer, NULL);
    if( FAILED( hr )) 
	{
        delete ppStream[dwIndex];
        ppStream[dwIndex] = NULL;
        return FALSE;
    }

    // �����Wave�ļ�ʱ���ϵĳ���(����Ϊ��λ)
    ppStream[dwIndex]->lSecondsLen = (LONG) (ppStream[dwIndex]->lDataLen /ppStream[dwIndex]->wfx.nAvgBytesPerSec );
    // ���㻺������һ���ʱ�䳤��(�Ժ���Ϊ��λ)
    ppStream[dwIndex]->lMillisecsHalfBuf =(LONG)((FLOAT)ppStream[dwIndex]->lBufSize * 500.0F/(FLOAT)ppStream[dwIndex]->wfx.nAvgBytesPerSec );

    // ��ʼ����ʽ����ĸ��ֲ���
    ppStream[dwIndex]->lBufPos  = 0;
    ppStream[dwIndex]->lBufSection = 0;
    ppStream[dwIndex]->lDataPos = 0;
    ppStream[dwIndex]->lReadLen = ppStream[dwIndex]->lBufSize;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lReadLen = (LONG)(ppStream[dwIndex]->lBufSize/2);

    return TRUE;
}

/***************************************************************
* ������: ClearStreamBuffer(...)
* ����: ���ָ������ʽ�������������,�����Ϊ����
****************************************************************/
void CDSound::ClearStreamBuffer( DWORD dwIndex )
{
    // �����Ч��
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    //>>>>>>>>>>>> ������д����� >>>>>>>>>>>>>>>
	LPVOID lpvPtr;
	DWORD dwBytes;
    HRESULT hr;

    // �������������Ա�����д������
	hr = ppStream[dwIndex]->lpBuffer->Lock( 0,0,
                                            &lpvPtr, &dwBytes,
                                            NULL, NULL,
                                            DSBLOCK_ENTIREBUFFER);
	if( FAILED(hr) ) {
        RestoreBuffer( ppStream[dwIndex]->lpBuffer );
        return;
    }

    // ���������ݴ��ݵ�������
    ZeroMemory( lpvPtr, dwBytes );

    // �������������������
    hr = ppStream[dwIndex]->lpBuffer->Unlock( lpvPtr, dwBytes,
                                              NULL, 0 );
	if( FAILED(hr) ) 
	{
        RestoreBuffer( ppStream[dwIndex]->lpBuffer );
        return;
	}
}

/***************************************************************
* ������: ReleaseStreamBuffer(...)
* ����: �ͷ�ָ������ʽ�������Ŀռ�
****************************************************************/
void CDSound::ReleaseStreamBuffer( DWORD dwIndex )
{
    // �����Ч��
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    ppStream[dwIndex]->lpBuffer->Release();
    ppStream[dwIndex]->lpBuffer = NULL;

    delete ppStream[dwIndex];
    ppStream[dwIndex] = NULL;
}

/***************************************************************
* ������: PlayStream(...)
* ����: ��ָ���ķ�ʽ����ָ������ʽ����������������
* ע: ���ñ�����ʱ��ֻ�е��ﵽ���»�������ͷ���ֻ��β���ֵ����ݵ������󣬲Ż���ļ���ȡ���ݡ�
*     ÿ�δ��ļ���ȡ���ݺ��ļ����ݵĲ����α��Զ����ƣ������ѭ�������ļ��������α��Զ����ơ�
*     ǰ�����δ��ļ���ȡ���ݵ�ʱ��������ʽ����������ԱlMillisecsHalfBuf������
****************************************************************/
void CDSound::PlayStream( DWORD dwIndex, BOOL bLoop )
{
    // �����Ч��
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    HRESULT hr;
    // ��ȡ������Ŀǰ״̬
	DWORD dwStatus; 
	hr = ppStream[dwIndex]->lpBuffer->GetStatus(&dwStatus);
	if( FAILED( hr ) )
        return;

    // ��ʧ�˻����������»�ȡ
    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        RestoreBuffer( ppStream[dwIndex]->lpBuffer );
        return;
    }

    //>>>>> Դ���ݵĴ�СС�ڻ������Ĵ�С,���ʵ�־�̬���� >>>>>>
    if( ppStream[dwIndex]->bStaticBuffer )
    {
        // �Ѿ������򷵻�
        if( dwStatus & DSBSTATUS_PLAYING )
            return;
        // ��δ�����򲥷�
        else {
            ppStream[dwIndex]->lpBuffer->SetCurrentPosition(0);
            ppStream[dwIndex]->lpBuffer->Play(
                        0,0,(bLoop?DSBPLAY_LOOPING:0));
        }
        return;
    }
    //>>>>>>>>>>> ��ʽ���� >>>>>>>>>>>>>>>
    // ���ڲ���
    if( dwStatus & DSBSTATUS_PLAYING )
    {
        ppStream[dwIndex]->lpBuffer->GetCurrentPosition(
                    (LPDWORD)&ppStream[dwIndex]->lBufPos,NULL );
        if(ppStream[dwIndex]->lBufPos >= ppStream[dwIndex]->lReadLen)
        {
            // �����α굽�ﻺ�������м��,����¿�ͷ��������
            if( ppStream[dwIndex]->lBufSection==0 ) 
            {
                UpdateStreamBuffer( dwIndex );
                ppStream[dwIndex]->lBufSection = 1;
            }
        }
        // �����α굽�ﻺ�����Ľ�����(Ҳ�ǿ�ʼ��),����½�β��������
        else if( ppStream[dwIndex]->lBufSection==1 )
        {
            UpdateStreamBuffer( dwIndex );
            ppStream[dwIndex]->lBufSection = 0;
        }
    }
    // ��ʼ����
    else 
	{
        ppStream[dwIndex]->lpBuffer->SetCurrentPosition(0);
        ppStream[dwIndex]->bLoop = bLoop;
        ppStream[dwIndex]->lpBuffer->Play(0,0,1);
    }
}

/***************************************************************
* ������: PauseStream(...)
* ����: ��ָͣ������ʽ������������������ļ���ȡ����
****************************************************************/
void CDSound::PauseStream( DWORD dwIndex )
{
    // �����Ч��
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    ppStream[dwIndex]->lpBuffer->Stop();
}

/***************************************************************
* ������: StopStream(...)
* ����: ֹͣ��ʽ�������Ĳ��ţ��������������ļ����ݵĲ����α����ڿ�ͷ��Ȼ���ļ����ݵĿ�ͷ�������뻺������
* ���ԣ�����StopStreamһ������ļ���ȡ����һ�Σ�ʹ��ʱҪ����Ƶ����ʹ��StopStream�����������������ʱ�����
****************************************************************/
void CDSound::StopStream( DWORD dwIndex )
{
    // �����Ч��
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    ppStream[dwIndex]->lpBuffer->Stop();
    ppStream[dwIndex]->lBufPos      = 0;
    ppStream[dwIndex]->lBufSection  = 0;
    ppStream[dwIndex]->lDataPos     = 0;

    // ���¸���ǰ�����λ�����������
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection  = 1;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection  = 0;
}

/***************************************************************
* ������: SetStreamPos(...)
* ����: ����ָ����ʽ�����Ĳ����α�ΪlPosָ����λ��
****************************************************************/
void CDSound::SetStreamPos( DWORD dwIndex, LONG lPos )
{
    // �����Ч��
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    // ��ͣ������ʽ����
    PauseStream( dwIndex );

    LONG Pos = ppStream[dwIndex]->wfx.nAvgBytesPerSec * lPos;
    ppStream[dwIndex]->lDataPos = Pos;

    // �α�ص��������Ŀ�ʼλ��
    ppStream[dwIndex]->lBufPos = 0;
    ppStream[dwIndex]->lBufSection = 0;

    // �޶��α��λ��
    if( ppStream[dwIndex]->lDataPos >= ppStream[dwIndex]->lDataLen )
        ppStream[dwIndex]->lDataPos = ppStream[dwIndex]->lDataLen-1;
    else if( ppStream[dwIndex]->lDataPos < 0 )
        ppStream[dwIndex]->lDataPos = 0;

    // ���¸��»�������ǰ�����ε�����
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 1;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 0;
}

/***************************************************************
* ������: AddStreamPos(...)
* ����: ��ָ������ʽ�����Ĳ����α�λ��������lAddָ����ֵ
****************************************************************/
void CDSound::AddStreamPos( DWORD dwIndex, LONG lAdd )
{
    // �����Ч��
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    // ��ͣ������ʽ����
    PauseStream( dwIndex );

    LONG Add = ppStream[dwIndex]->wfx.nAvgBytesPerSec * lAdd;
    ppStream[dwIndex]->lDataPos += Add;

    // �α�ص��������Ŀ�ʼλ��
    ppStream[dwIndex]->lBufPos = 0;
    ppStream[dwIndex]->lBufSection = 0;

    // �޶��α�λ��
    if( ppStream[dwIndex]->lDataPos >= ppStream[dwIndex]->lDataLen )
        ppStream[dwIndex]->lDataPos = ppStream[dwIndex]->lDataLen;

    // ���¸��»�������ǰ�������ֵ�����
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 1;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 0;
}

/***************************************************************
* ������: SubStreamPos(...)
* ����: ��ָ������ʽ�ļ����ݵĲ����α�λ�ú�����lSubָ����ֵ
****************************************************************/
void CDSound::SubStreamPos( DWORD dwIndex, LONG lSub )
{
    // �����Ч��
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    // ��ͣ������ʽ����
    PauseStream( dwIndex );

    LONG Sub = ppStream[dwIndex]->wfx.nAvgBytesPerSec * lSub;
    ppStream[dwIndex]->lDataPos -= Sub;

    // �α�ص��������Ŀ�ʼλ��
    ppStream[dwIndex]->lBufPos = 0;
    ppStream[dwIndex]->lBufSection = 0;

    // �α�λ���޶�
    if( ppStream[dwIndex]->lDataPos < 0 )
        ppStream[dwIndex]->lDataPos = 0;

    // ���»�����ǰ�����ε�����
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 1;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 0;
}

/***************************************************************
* ������: AddStreamVolume(...)
* ����: ��ָ������ʽ����������λ�������lAddָ����ֵ
****************************************************************/
void CDSound::AddStreamVolume( DWORD dwIndex, LONG lAdd )
{
    // �����Ч��
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    LONG lVolume;
    // ��ȡ��ǰ����
    ppStream[dwIndex]->lpBuffer->GetVolume( &lVolume );
    lVolume += lAdd;

    // ���������޶�
    if( lVolume > DSBVOLUME_MAX )   lVolume = DSBVOLUME_MAX;
    if( lVolume < DSBVOLUME_MIN )   lVolume = DSBVOLUME_MIN;
    ppStream[dwIndex]->lpBuffer->SetVolume( lVolume );
}

/***************************************************************
* ������: SubStreamVolume(...)
* ����: ��ָ������ʽ����������λ�ý�����lSubָ����ֵ
****************************************************************/
void CDSound::SubStreamVolume( DWORD dwIndex, LONG lSub )
{
    // �����Ч��
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    LONG lVolume;
    // ��ȡ��ǰ����
    ppStream[dwIndex]->lpBuffer->GetVolume( &lVolume );
    lVolume -= lSub;

    // ���������޶�
    if( lVolume > DSBVOLUME_MAX )   lVolume = DSBVOLUME_MAX;
    if( lVolume < DSBVOLUME_MIN )   lVolume = DSBVOLUME_MIN;
    ppStream[dwIndex]->lpBuffer->SetVolume( lVolume );
}

/***************************************************************
* ������: LoadStreamData(...)
* ����: ���ļ���������
* ע��: ���ñ������󣬽�����һ��pDataָ��ָ����������ݻ�������ʹ�ú�����ͷ���
****************************************************************/
BYTE * CDSound::LoadStreamData( DWORD dwIndex )
{
    // �����Ч��
    if( !bSoundAble )               return NULL;
    if( dwIndex >= dwNumStreams )   return NULL;
    if( ppStream == NULL )          return NULL;
    if( ppStream[dwIndex] == NULL ) return NULL;
	if( ppStream[dwIndex]->lReadLen <=0 ) return NULL;

	BYTE *pData = NULL;  LONG lNext;  HMMIO file;

    // ��ֻ����ʽ��������WAVE�ļ�
	file = mmioOpen((LPSTR)ppStream[dwIndex]->filename,
                           NULL,MMIO_READ);
    if( file==NULL ) {
        ppStream[dwIndex]->lDataLen=0;
		return NULL;
    }

	//>>>>>>>>>>>>>>>> ��ȡ��ʽ���� >>>>>>>>>>>>>>
    // ������ж������ݿ��Զ�ȡ
    lNext =ppStream[dwIndex]->lDataLen -
            ppStream[dwIndex]->lDataPos;

    // ����һ����������ݵĿռ�
    pData = new BYTE[ ppStream[dwIndex]->lReadLen ];

    // ���ļ���λ��Ŀǰ��λ��
    mmioSeek( file, 44+ppStream[dwIndex]->lDataPos, SEEK_SET);
    // ��ȡ��������
    if( ppStream[dwIndex]->lReadLen > lNext )
    {
        mmioRead( file, (char*)pData, lNext );
        if( ppStream[dwIndex]->bLoop ) {
            // ������ļ����ݵĿ�ͷӦ�ö�ȡ��������
            LONG lSurplus = ppStream[dwIndex]->lReadLen -lNext;
            mmioSeek( file, 44, SEEK_SET );
            mmioRead( file,(char*)(pData+lNext),lSurplus );
            // ���α���Ƶ��ļ����ݵĿ�ͷ
            ppStream[dwIndex]->lDataPos = lSurplus;
        }
        else
            ppStream[dwIndex]->lDataPos += lNext;
    }
    else {
        mmioRead(file,(char*)pData,ppStream[dwIndex]->lReadLen );
        // �α�����
        ppStream[dwIndex]->lDataPos+=ppStream[dwIndex]->lReadLen;
    }

    mmioClose( file, 0 );
	return pData;
}

/********************************************************
* ������: UpdateStreamBuffer(...)
* ����: ���ļ���ȡ���ݸ�����ʽ��������(ǰ�λ���)
*********************************************************/
BOOL CDSound::UpdateStreamBuffer( DWORD dwIndex )
{
    // �����Ч��
    if( !bSoundAble )                     return FALSE;
    if( dwIndex >= dwNumStreams )         return FALSE;
    if( ppStream == NULL )                return FALSE;
    if( ppStream[dwIndex]==NULL )         return FALSE;
    if( ppStream[dwIndex]->lpBuffer==NULL)return FALSE;

    HRESULT hr;
	BYTE *pData =NULL;//��ʱ������
    // ע: LoadStreamData����������һ����pDataָ��
    //     �Ŀռ�,����Ҫ�ͷ���
	pData = LoadStreamData( dwIndex );
	if( pData == NULL )
        return FALSE;

    //>>>>>>>>>>>> ������д����� >>>>>>>>>>>>>>>
	LPVOID lpvPtr1, lpvPtr2;//ָ��1	//ָ��2
	DWORD dwBytes1, dwBytes2;
    DWORD dwOffset;
    if( ppStream[dwIndex]->lBufSection==0 )
        dwOffset = 0;
    else
        dwOffset = (DWORD)ppStream[dwIndex]->lReadLen;

    // �������������Ա�����д������
	hr = ppStream[dwIndex]->lpBuffer->Lock( dwOffset,
                    (DWORD)ppStream[dwIndex]->lReadLen,
                    &lpvPtr1, &dwBytes1,
                    &lpvPtr2, &dwBytes2, 0);
	if( FAILED(hr) ) {
        RestoreBuffer( ppStream[dwIndex]->lpBuffer );
        delete[] pData;
        return FALSE;
    }

    // �����ݴ��ݵ�������
	memcpy( lpvPtr1, pData, dwBytes1 );
	if( dwBytes2 )
		memcpy( lpvPtr2, pData+dwBytes1, dwBytes2 );

    // �������������������
    hr = ppStream[dwIndex]->lpBuffer->Unlock(lpvPtr1,
                                             dwBytes1,
                                             lpvPtr2,
                                             dwBytes2 );
	if( FAILED(hr) ) {
        RestoreBuffer( ppStream[dwIndex]->lpBuffer );
		delete[] pData;
        return FALSE;
	}

	delete[] pData;
	return TRUE;
}
