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

    // 为静态缓冲区数据开辟空间,并且清零每个元素
    ppBuffer = new LPDIRECTSOUNDBUFFER[dwNum];
    ZeroMemory( ppBuffer, dwNum*sizeof(LPDIRECTSOUNDBUFFER) );
    // 默认的静态缓冲区的大小上限为1M
    dwMaxBytesStatic = 1024000;
    dwNumStatics  = dwNum;

    // 为流式缓冲描述开辟空间,并且清零每个元素
    // 默认情况下只能创建2个流式缓冲区
    ppStream = new LPSTREAMDESC[2];
    ZeroMemory( ppStream, 2*sizeof(LPSTREAMDESC) );
    // 默认的流式缓冲区的大小上限为1M
    dwMaxBytesStream = 1024000;
    dwNumStreams = 2;
}

CDSound::CDSound( void )
{
    lpDSound            = NULL;
    bSoundAble          = FALSE;
    // 默认情况下协作等级是一般等级,不能改变主缓冲区格式
    dwCoopLevel         = DSSCL_NORMAL;
    bSetPrimaryFormat   = FALSE;

    // 为静态缓冲区开辟空间,并且清零每个元素
    // 默认情况下只能创建5个静态次级缓冲区
    ppBuffer      = new LPDIRECTSOUNDBUFFER[5];
    ZeroMemory( ppBuffer, 5*sizeof( LPDIRECTSOUNDBUFFER ) );
    // 默认的静态缓冲区的大小上限设为1M
    dwMaxBytesStatic = 1024000;
    // 默认情况下只能创建5个静态次级缓冲区
    dwNumStatics        = 5;

    // 为流式缓冲描述开辟空间,并且清零每个元素
    // 默认情况下只能创建2个流式缓冲区
    ppStream = new LPSTREAMDESC[2];
    ZeroMemory( ppStream, 2*sizeof(LPSTREAMDESC) );
    // 默认的流式缓冲区的大小上限为1M
    dwMaxBytesStream = 1024000;
    dwNumStreams = 2;
}

CDSound::~CDSound()
{
    // 释放静态次级缓冲区空间
    if( ppBuffer )  
	{
        delete[] ppBuffer;
        ppBuffer = NULL;
    }
    // 释放流式次级缓冲描述空间
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
    // 释放DirectSound对象
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
* 函数名: Set_dwCoopLevel(...)
* 功能: 设置DirectSound协作等级
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
* 函数名: Set_bSetPrimaryFormat(...)
* 功能: 设置DirectSound是否可以设置主缓冲格式
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
* 函数名: InitDSound(...)
* 功能: 初始化DirectSound环境
****************************************************************/
BOOL CDSound::InitDSound( HWND hWnd )
{
    // 释放以前可能已经创建了的DirectSound对象
    ReleaseDSound();

	// 创建DirectSound对象
    HRESULT hr = DirectSoundCreate8( NULL, &lpDSound, NULL );
	if( FAILED( hr ) )
	{
        MessageBox( hWnd, TEXT("创建DirectSound对象失败\n")
                          TEXT("程序将无法播放声音\n"),
                          TEXT("DirectSoundCreate8"), MB_OK );
		return FALSE;
	}

	// 设置DirectSound协作等级
    hr = lpDSound->SetCooperativeLevel(hWnd,dwCoopLevel);
	if( FAILED( hr ) )
	{
        MessageBox( hWnd, TEXT("设置DirectSound协作等级失败\n")
                          TEXT("程序将无法播放声音\n"),
                          TEXT("SetCooperativeLevel"), MB_OK );
		return FALSE;
	}

    // 初始化成功
    bSoundAble = TRUE;

	return TRUE; 
}

/***************************************************************
* 函数名: SetPrimaryBufferFormat(...)
* 功能: 设置主声音缓冲区格式
****************************************************************/
BOOL CDSound::SetPrimaryBufferFormat( DWORD dwChannels,
                                      DWORD dwSamplesPerSec,
                                      DWORD dwBitsPerSample)
{
    // 协作等级不允许设置主缓冲区格式或声音设备无效则返回
    if( !bSetPrimaryFormat||!bSoundAble )
        return FALSE;

    HRESULT  hr;
    // 临时主缓冲区指针
    LPDIRECTSOUNDBUFFER lpPrimaryBuffer = NULL;

    // 设置 DSBUFFERDESC 结构
    DSBUFFERDESC	dsbd;
	ZeroMemory( &dsbd, sizeof(dsbd) );
    dsbd.dwSize         = sizeof(DSBUFFERDESC);
    dsbd.dwFlags        = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes  = 0; 
    dsbd.lpwfxFormat    = NULL;
    // 创建临时主缓冲区
    hr = lpDSound->CreateSoundBuffer( &dsbd, &lpPrimaryBuffer, NULL );
	if( FAILED( hr ) )
        return FALSE;

    // 设置wave格式结构
	WAVEFORMATEX	wfx;
	ZeroMemory( &wfx, sizeof(wfx) );
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = (WORD)dwChannels;
    wfx.nSamplesPerSec  = dwSamplesPerSec;
    wfx.wBitsPerSample  = (WORD)dwBitsPerSample;
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    // 根据wfx结构的值设置主缓冲区格式
    hr = lpPrimaryBuffer->SetFormat( &wfx );
	if( FAILED( hr ) )
        return FALSE;

    // 释放临时主缓冲区
    if( lpPrimaryBuffer )
    {
        lpPrimaryBuffer->Release();
        lpPrimaryBuffer = NULL;
    }

    return TRUE;
}



/***************************************************************
* 函数名: RestoreBuffer(...)
* 功能: 恢复指定的缓冲区空间
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
* 函数名: LoadWaveFile(...)
* 功能: 读取波形文件的数据
* 注意: 当bReadData参数的值为真时，将分配一个pData指针指向的声音数据缓冲区。使用后必须释放它
****************************************************************/
BYTE* CDSound::LoadWaveFile( WAVEFORMATEX  *format,
                              DWORD         *pdwDataLen,
                              LPCSTR        filename,
                              BOOL          bReadData)
{
	if( filename==NULL )
		return NULL;

	// 以只读模式打开所给的WAVE文件；
	HMMIO file = mmioOpen((LPSTR)filename,NULL,MMIO_READ);
	if( file==NULL )
		return NULL;

	// 判断该文件是否为"WAVE"文件格式
	char ch[4];
	mmioSeek( file, 8L, SEEK_SET );
	mmioRead( file, ch, 4L );
    if(ch[0]!='W'||ch[1]!='A'||ch[2]!='V'||ch[3]!='E')
	{
		mmioClose( file, 0 );
		return NULL;
	}

	// 读取WAVE文件的WAVEFORMATEX结构的数据
	if( format )
	{
		mmioSeek( file, 20L, SEEK_SET );
		mmioRead( file, (HPSTR)format, sizeof(WAVEFORMATEX) );
	}

    // 读取声音数据的长度
    DWORD dwDataLen;
    mmioSeek( file, 40L, SEEK_SET );
    mmioRead( file, (char*)&dwDataLen, sizeof(DWORD) );

    // 是否要记录声音数据的长度
    if( pdwDataLen )
        (*pdwDataLen) = dwDataLen;

    // 是否读取声音数据
    BYTE *pData = NULL;
    if( bReadData )
    {
        // 开辟一存放声音数据的空间
        pData = new BYTE[ dwDataLen ];
        // 读取声音数据
        mmioSeek( file, 44L, SEEK_SET );
        mmioRead( file, (char*)pData, (LONG)dwDataLen );
    }

    mmioClose( file, 0);
	return pData;
}

/***************************************************************
* 函数名: Set_dwNumStatics(...)
* 功能: 设置CDSound对象可创建的静态次级缓冲区数目
****************************************************************/
void CDSound::Set_dwNumStatics( DWORD dwNum )
{
    // 设置次级缓冲区的个数必须在创建DirectSound对象之前进行
    if( lpDSound )
        return;

    // 释放以前创建的空间
    if( ppBuffer )  
	{
        delete[] ppBuffer;
        ppBuffer  = NULL;
    }

    // 开辟新的空间
    ppBuffer = new LPDIRECTSOUNDBUFFER[dwNum];
    // 清零
    ZeroMemory( ppBuffer, dwNum*sizeof(LPDIRECTSOUNDBUFFER) );
    dwNumStatics = dwNum;
}

/***************************************************************
* 函数名: CreateStaticBuffer(...)
* 功能: 创建一个次级缓冲区,并且将filename指向的声音数据调入该缓冲区
****************************************************************/
BOOL CDSound::CreateStaticBuffer( DWORD dwIndex, LPCSTR filename )
{
    // 检测有效性
    if( !bSoundAble )             return FALSE;
    if( dwIndex >= dwNumStatics ) return FALSE;
    if( ppBuffer == NULL )        return FALSE;

    HRESULT hr;
	BYTE *pData; 		//临时数据缓冲区指针
	WAVEFORMATEX format;//数据的格式
	DWORD dwDataLen;	//声音数据大小

    // 注: LoadWaveFile函数创建了一个由pData指向的空间,用完要释放它
	pData = LoadWaveFile( &format, &dwDataLen, filename, TRUE );

	if( pData == NULL )
        return FALSE ;

    // 限定缓冲区的大小
    if( dwDataLen > dwMaxBytesStatic )
        dwDataLen = dwMaxBytesStatic;

    // 设置 DSBUFFERDESC 结构,并且根据该属性创建次级静态缓冲区
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

    // 锁定缓冲区，以便向它写入数据
	hr = ppBuffer[dwIndex]->Lock(0,0,&lpvPtr,&dwBytes,NULL,NULL, DSBLOCK_ENTIREBUFFER);
	if( FAILED(hr) ) {
        RestoreBuffer( ppBuffer[dwIndex] );
        delete[] pData;
        return FALSE;
    }

    // 将数据传递到缓冲区
	memcpy( lpvPtr, pData, dwBytes );

    // 解除缓冲区的锁定操作
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
* 函数名: ReleaseStaticBuffer(...)
* 功能: 清空指定的次级缓冲区
****************************************************************/
void CDSound::ReleaseStaticBuffer( DWORD dwIndex )
{
    // 检测有效性
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    ppBuffer[dwIndex]->Release();
    ppBuffer[dwIndex] = NULL;
}

/***************************************************************
* 函数名: PlayStatic(...)
* 功能: 播放静态缓冲区的声音数据
****************************************************************/
void CDSound::PlayStatic( DWORD dwIndex, BOOL bLoop, BOOL bCompulsory )
{
    // 检测有效性
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    // 获取缓冲区的状态
	DWORD dwStatus;
	HRESULT hr = ppBuffer[dwIndex]->GetStatus( &dwStatus );
	if( FAILED( hr ) )
        return;

    // 丢失了缓冲区则重新获得
    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        RestoreBuffer( ppBuffer[dwIndex] );
        return;
    }

    // 不是强制性从头播放
    if( !bCompulsory ) 
    {
        // 已经播放则返回
        if( dwStatus & DSBSTATUS_PLAYING )
            return;
        // 还未播放则播放
        else 
		{
            ppBuffer[dwIndex]->SetCurrentPosition(0);
		    ppBuffer[dwIndex]->Play(0,0,(bLoop?DSBPLAY_LOOPING:0));
        }
    }
    // 强制性从头播放
    else 
	{
        ppBuffer[dwIndex]->SetCurrentPosition(0);
		ppBuffer[dwIndex]->Play(0,0,(bLoop?DSBPLAY_LOOPING:0));
    }
}

/***************************************************************
* 函数名: StopStatic(...)
* 功能: 停止播放静态缓冲区
****************************************************************/
void CDSound::StopStatic( DWORD dwIndex )
{
    // 检测有效性
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    ppBuffer[dwIndex]->Stop();
}

/***************************************************************
* 函数名: StaticIsPlaying(...)
* 功能: 静态缓冲区是否正在播放
****************************************************************/
BOOL CDSound::StaticIsPlaying( DWORD dwIndex )
{
    // 检测有效性
    if( !bSoundAble )             return FALSE;
    if( dwIndex >= dwNumStatics ) return FALSE;
    if( ppBuffer == NULL )        return FALSE;
    if( ppBuffer[dwIndex]==NULL ) return FALSE;

    // 获取缓冲区的状态
	DWORD dwStatus;
	HRESULT hr = ppBuffer[dwIndex]->GetStatus( &dwStatus );
	if( FAILED( hr ) )
        return FALSE;

    // 丢失了缓冲区则重新获得 
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
* 函数名: SetStaticVolume(...)
* 功能: 设置指定静态缓冲区的音量
****************************************************************/
void CDSound::SetStaticVolume( DWORD dwIndex, LONG lVolume )
{
    // 检测有效性
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    if( lVolume > DSBVOLUME_MAX )   lVolume = DSBVOLUME_MAX;
    if( lVolume < DSBVOLUME_MIN )   lVolume = DSBVOLUME_MIN;
    ppBuffer[dwIndex]->SetVolume( lVolume );
}

/***************************************************************
* 函数名: AddStaticVolume(...)
* 功能: 给指定的静态缓冲区增加lAdd指定的音量
****************************************************************/
void CDSound::AddStaticVolume( DWORD dwIndex, LONG lAdd )
{
    // 检测有效性
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
* 函数名: SubStaticVolume(...)
* 功能: 给指定的静态缓冲区减少lSub指定的音量
****************************************************************/
void CDSound::SubStaticVolume( DWORD dwIndex, LONG lSub )
{
    // 检测有效性
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
* 函数名: SetStaticPan(...)
* 功能: 设置指定静态缓冲区的声音均衡性为lPan指出的值
****************************************************************/
void CDSound::SetStaticPan( DWORD dwIndex, LONG lPan )
{
    // 检测有效性
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    if( lPan > DSBPAN_RIGHT )  lPan = DSBPAN_RIGHT;
    if( lPan < DSBPAN_LEFT  )  lPan = DSBPAN_LEFT;
    ppBuffer[dwIndex]->SetPan( lPan );
}

/***************************************************************
* 函数名: AddStaticPan(...)
* 功能: 将指定的静态缓冲区的均衡性右移lAdd指出的值
****************************************************************/
void CDSound::AddStaticPan( DWORD dwIndex, LONG lAdd )
{
    // 检测有效性
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
* 函数名: SubStaticPan(...)
* 功能: 将指定的静态缓冲区的均衡性左移lSub指出的值
****************************************************************/
void CDSound::SubStaticPan( DWORD dwIndex, LONG lSub )
{
    // 检测有效性
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
* 函数名: SetStaticFreq(...)
* 功能: 设置指定静态缓冲区的声音频率为dwFreq指出的值
****************************************************************/
void CDSound::SetStaticFreq( DWORD dwIndex, DWORD dwFreq )
{
    // 检测有效性
    if( !bSoundAble )             return;
    if( dwIndex >= dwNumStatics ) return;
    if( ppBuffer == NULL )        return;
    if( ppBuffer[dwIndex]==NULL ) return;

    // 超出频率范围的设为原始频率
    if( dwFreq < DSBFREQUENCY_MIN || dwFreq > DSBFREQUENCY_MAX )
        dwFreq = DSBFREQUENCY_ORIGINAL;
    ppBuffer[dwIndex]->SetFrequency( dwFreq );
}

/***************************************************************
* 函数名: AddStaticFreq(...)
* 功能: 将指定的静态缓冲区的声音频率提高dwAdd指出的值
****************************************************************/
void CDSound::AddStaticFreq( DWORD dwIndex, DWORD dwAdd )
{
    // 检测有效性
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
* 函数名: SubStaticFreq(...)
* 功能: 将指定的静态缓冲区的声音频率降低dwSub指出的值
****************************************************************/
void CDSound::SubStaticFreq( DWORD dwIndex, DWORD dwSub )
{
    // 检测有效性
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
* 函数名: Get_pStaticBuffer(...)
* 功能: 获取指定的静态缓冲区指针
****************************************************************/
LPDIRECTSOUNDBUFFER CDSound::Get_pStaticBuffer( DWORD dwIndex )
{
    // 检测有效性
    if( dwIndex >= dwNumStatics ) return NULL;
    if( ppBuffer == NULL )        return NULL;

    return ppBuffer[dwIndex];
}

/***************************************************************
* 函数名: Set_dwNumStreams(...)
* 功能: 设置CDSound对象可创建的流式次级缓冲区数目
****************************************************************/
void CDSound::Set_dwNumStreams( DWORD dwNum )
{
    // 设置次级缓冲区的个数必须在创建DirectSound对象之前进行
    if( lpDSound )
        return;

    // 释放以前的流式次级缓冲描述空间
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

    // 以新的个数开辟新的空间，并清零
    ppStream = new LPSTREAMDESC[dwNum];
    ZeroMemory( ppStream, dwNum*sizeof(LPSTREAMDESC) );
    dwNumStreams = dwNum;
}

/***************************************************************
* 函数名：CreateStreamBuffer(...)
* 功能：创建一流式缓冲区，并将指定的文件声音数据的开头一部分载入缓冲区
****************************************************************/
BOOL CDSound::CreateStreamBuffer( DWORD dwIndex, LPCSTR filename )
{
    // 检测有效性
    if( !bSoundAble )             return FALSE;
    if( dwIndex >= dwNumStreams ) return FALSE;
    if( ppStream == NULL )        return FALSE;

    ppStream[dwIndex] = new STREAMDESC;
    ZeroMemory( ppStream[dwIndex],sizeof(STREAMDESC) );
    ppStream[dwIndex]->filename = filename;

    // 读取文件的信息
	LoadWaveFile( &ppStream[dwIndex]->wfx,
                  (DWORD*)&ppStream[dwIndex]->lDataLen,
                  ppStream[dwIndex]->filename, FALSE );

    if(ppStream[dwIndex]->lDataLen==0) 
	{
        delete ppStream[dwIndex];
        ppStream[dwIndex] = NULL;
        return FALSE;
    }

    // 限定缓冲区的大小
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
    // 设置 DSBUFFERDESC 结构,并且根据该属性创建缓冲区
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

    // 估算该Wave文件时间上的长度(以秒为单位)
    ppStream[dwIndex]->lSecondsLen = (LONG) (ppStream[dwIndex]->lDataLen /ppStream[dwIndex]->wfx.nAvgBytesPerSec );
    // 估算缓冲区的一半的时间长度(以毫秒为单位)
    ppStream[dwIndex]->lMillisecsHalfBuf =(LONG)((FLOAT)ppStream[dwIndex]->lBufSize * 500.0F/(FLOAT)ppStream[dwIndex]->wfx.nAvgBytesPerSec );

    // 初始化流式缓冲的各种参数
    ppStream[dwIndex]->lBufPos  = 0;
    ppStream[dwIndex]->lBufSection = 0;
    ppStream[dwIndex]->lDataPos = 0;
    ppStream[dwIndex]->lReadLen = ppStream[dwIndex]->lBufSize;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lReadLen = (LONG)(ppStream[dwIndex]->lBufSize/2);

    return TRUE;
}

/***************************************************************
* 函数名: ClearStreamBuffer(...)
* 功能: 清空指定的流式缓冲区里的数据,将其改为静音
****************************************************************/
void CDSound::ClearStreamBuffer( DWORD dwIndex )
{
    // 检测有效性
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    //>>>>>>>>>>>> 缓冲区写入操作 >>>>>>>>>>>>>>>
	LPVOID lpvPtr;
	DWORD dwBytes;
    HRESULT hr;

    // 锁定缓冲区，以便向它写入数据
	hr = ppStream[dwIndex]->lpBuffer->Lock( 0,0,
                                            &lpvPtr, &dwBytes,
                                            NULL, NULL,
                                            DSBLOCK_ENTIREBUFFER);
	if( FAILED(hr) ) {
        RestoreBuffer( ppStream[dwIndex]->lpBuffer );
        return;
    }

    // 将静音数据传递到缓冲区
    ZeroMemory( lpvPtr, dwBytes );

    // 解除缓冲区的锁定操作
    hr = ppStream[dwIndex]->lpBuffer->Unlock( lpvPtr, dwBytes,
                                              NULL, 0 );
	if( FAILED(hr) ) 
	{
        RestoreBuffer( ppStream[dwIndex]->lpBuffer );
        return;
	}
}

/***************************************************************
* 函数名: ReleaseStreamBuffer(...)
* 功能: 释放指定的流式缓冲区的空间
****************************************************************/
void CDSound::ReleaseStreamBuffer( DWORD dwIndex )
{
    // 检测有效性
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
* 函数名: PlayStream(...)
* 功能: 以指定的方式播放指定的流式缓冲区的声音数据
* 注: 调用本函数时，只有当达到更新缓冲区开头部分或结尾部分的数据的条件后，才会从文件读取数据。
*     每次从文件读取数据后，文件数据的播放游标自动下移；如果是循环播放文件数据则游标自动回绕。
*     前后两次从文件读取数据的时间间隔由流式缓冲描述成员lMillisecsHalfBuf决定。
****************************************************************/
void CDSound::PlayStream( DWORD dwIndex, BOOL bLoop )
{
    // 检测有效性
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    HRESULT hr;
    // 获取缓冲区目前状态
	DWORD dwStatus; 
	hr = ppStream[dwIndex]->lpBuffer->GetStatus(&dwStatus);
	if( FAILED( hr ) )
        return;

    // 丢失了缓冲区则重新获取
    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        RestoreBuffer( ppStream[dwIndex]->lpBuffer );
        return;
    }

    //>>>>> 源数据的大小小于缓冲区的大小,则可实现静态播放 >>>>>>
    if( ppStream[dwIndex]->bStaticBuffer )
    {
        // 已经播放则返回
        if( dwStatus & DSBSTATUS_PLAYING )
            return;
        // 还未播放则播放
        else {
            ppStream[dwIndex]->lpBuffer->SetCurrentPosition(0);
            ppStream[dwIndex]->lpBuffer->Play(
                        0,0,(bLoop?DSBPLAY_LOOPING:0));
        }
        return;
    }
    //>>>>>>>>>>> 流式播放 >>>>>>>>>>>>>>>
    // 正在播放
    if( dwStatus & DSBSTATUS_PLAYING )
    {
        ppStream[dwIndex]->lpBuffer->GetCurrentPosition(
                    (LPDWORD)&ppStream[dwIndex]->lBufPos,NULL );
        if(ppStream[dwIndex]->lBufPos >= ppStream[dwIndex]->lReadLen)
        {
            // 播放游标到达缓冲区的中间点,则更新开头部分数据
            if( ppStream[dwIndex]->lBufSection==0 ) 
            {
                UpdateStreamBuffer( dwIndex );
                ppStream[dwIndex]->lBufSection = 1;
            }
        }
        // 播放游标到达缓冲区的结束点(也是开始点),则更新结尾部分数据
        else if( ppStream[dwIndex]->lBufSection==1 )
        {
            UpdateStreamBuffer( dwIndex );
            ppStream[dwIndex]->lBufSection = 0;
        }
    }
    // 开始播放
    else 
	{
        ppStream[dwIndex]->lpBuffer->SetCurrentPosition(0);
        ppStream[dwIndex]->bLoop = bLoop;
        ppStream[dwIndex]->lpBuffer->Play(0,0,1);
    }
}

/***************************************************************
* 函数名: PauseStream(...)
* 功能: 暂停指定的流式声音。本函数不会从文件读取数据
****************************************************************/
void CDSound::PauseStream( DWORD dwIndex )
{
    // 检测有效性
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    ppStream[dwIndex]->lpBuffer->Stop();
}

/***************************************************************
* 函数名: StopStream(...)
* 功能: 停止流式缓冲区的播放，并将缓冲区和文件数据的播放游标置于开头。然后将文件数据的开头部分载入缓冲区。
* 所以，调用StopStream一次则从文件读取数据一次；使用时要避免频繁的使用StopStream，以免程序的运行速率变慢。
****************************************************************/
void CDSound::StopStream( DWORD dwIndex )
{
    // 检测有效性
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    ppStream[dwIndex]->lpBuffer->Stop();
    ppStream[dwIndex]->lBufPos      = 0;
    ppStream[dwIndex]->lBufSection  = 0;
    ppStream[dwIndex]->lDataPos     = 0;

    // 重新更新前后两段缓冲区的数据
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection  = 1;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection  = 0;
}

/***************************************************************
* 函数名: SetStreamPos(...)
* 功能: 设置指定流式声音的播放游标为lPos指出的位置
****************************************************************/
void CDSound::SetStreamPos( DWORD dwIndex, LONG lPos )
{
    // 检测有效性
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    // 暂停播放流式声音
    PauseStream( dwIndex );

    LONG Pos = ppStream[dwIndex]->wfx.nAvgBytesPerSec * lPos;
    ppStream[dwIndex]->lDataPos = Pos;

    // 游标回到缓冲区的开始位置
    ppStream[dwIndex]->lBufPos = 0;
    ppStream[dwIndex]->lBufSection = 0;

    // 限定游标的位置
    if( ppStream[dwIndex]->lDataPos >= ppStream[dwIndex]->lDataLen )
        ppStream[dwIndex]->lDataPos = ppStream[dwIndex]->lDataLen-1;
    else if( ppStream[dwIndex]->lDataPos < 0 )
        ppStream[dwIndex]->lDataPos = 0;

    // 重新更新缓冲区的前后两段的数据
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 1;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 0;
}

/***************************************************************
* 函数名: AddStreamPos(...)
* 功能: 将指定的流式声音的播放游标位置下移了lAdd指定的值
****************************************************************/
void CDSound::AddStreamPos( DWORD dwIndex, LONG lAdd )
{
    // 检测有效性
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    // 暂停播放流式声音
    PauseStream( dwIndex );

    LONG Add = ppStream[dwIndex]->wfx.nAvgBytesPerSec * lAdd;
    ppStream[dwIndex]->lDataPos += Add;

    // 游标回到缓冲区的开始位置
    ppStream[dwIndex]->lBufPos = 0;
    ppStream[dwIndex]->lBufSection = 0;

    // 限定游标位置
    if( ppStream[dwIndex]->lDataPos >= ppStream[dwIndex]->lDataLen )
        ppStream[dwIndex]->lDataPos = ppStream[dwIndex]->lDataLen;

    // 重新更新缓冲区的前后两部分的数据
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 1;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 0;
}

/***************************************************************
* 函数名: SubStreamPos(...)
* 功能: 将指定的流式文件数据的播放游标位置后退了lSub指定的值
****************************************************************/
void CDSound::SubStreamPos( DWORD dwIndex, LONG lSub )
{
    // 检测有效性
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    // 暂停播放流式声音
    PauseStream( dwIndex );

    LONG Sub = ppStream[dwIndex]->wfx.nAvgBytesPerSec * lSub;
    ppStream[dwIndex]->lDataPos -= Sub;

    // 游标回到缓冲区的开始位置
    ppStream[dwIndex]->lBufPos = 0;
    ppStream[dwIndex]->lBufSection = 0;

    // 游标位置限定
    if( ppStream[dwIndex]->lDataPos < 0 )
        ppStream[dwIndex]->lDataPos = 0;

    // 更新缓冲区前后两段的数据
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 1;
    UpdateStreamBuffer( dwIndex );
    ppStream[dwIndex]->lBufSection = 0;
}

/***************************************************************
* 函数名: AddStreamVolume(...)
* 功能: 将指定的流式声音的音量位置提高了lAdd指定的值
****************************************************************/
void CDSound::AddStreamVolume( DWORD dwIndex, LONG lAdd )
{
    // 检测有效性
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    LONG lVolume;
    // 获取当前音量
    ppStream[dwIndex]->lpBuffer->GetVolume( &lVolume );
    lVolume += lAdd;

    // 音量上限限定
    if( lVolume > DSBVOLUME_MAX )   lVolume = DSBVOLUME_MAX;
    if( lVolume < DSBVOLUME_MIN )   lVolume = DSBVOLUME_MIN;
    ppStream[dwIndex]->lpBuffer->SetVolume( lVolume );
}

/***************************************************************
* 函数名: SubStreamVolume(...)
* 功能: 将指定的流式声音的音量位置降低了lSub指定的值
****************************************************************/
void CDSound::SubStreamVolume( DWORD dwIndex, LONG lSub )
{
    // 检测有效性
    if( !bSoundAble )                       return;
    if( dwIndex >= dwNumStreams )           return;
    if( ppStream == NULL )                  return;
    if( ppStream[dwIndex]==NULL )           return;
    if( ppStream[dwIndex]->lpBuffer==NULL ) return;

    LONG lVolume;
    // 获取当前音量
    ppStream[dwIndex]->lpBuffer->GetVolume( &lVolume );
    lVolume -= lSub;

    // 音量上限限定
    if( lVolume > DSBVOLUME_MAX )   lVolume = DSBVOLUME_MAX;
    if( lVolume < DSBVOLUME_MIN )   lVolume = DSBVOLUME_MIN;
    ppStream[dwIndex]->lpBuffer->SetVolume( lVolume );
}

/***************************************************************
* 函数名: LoadStreamData(...)
* 功能: 从文件载入数据
* 注意: 调用本函数后，将分配一个pData指针指向的声音数据缓冲区。使用后必须释放它
****************************************************************/
BYTE * CDSound::LoadStreamData( DWORD dwIndex )
{
    // 检测有效性
    if( !bSoundAble )               return NULL;
    if( dwIndex >= dwNumStreams )   return NULL;
    if( ppStream == NULL )          return NULL;
    if( ppStream[dwIndex] == NULL ) return NULL;
	if( ppStream[dwIndex]->lReadLen <=0 ) return NULL;

	BYTE *pData = NULL;  LONG lNext;  HMMIO file;

    // 以只读方式打开所给的WAVE文件
	file = mmioOpen((LPSTR)ppStream[dwIndex]->filename,
                           NULL,MMIO_READ);
    if( file==NULL ) {
        ppStream[dwIndex]->lDataLen=0;
		return NULL;
    }

	//>>>>>>>>>>>>>>>> 读取流式数据 >>>>>>>>>>>>>>
    // 求出还有多少数据可以读取
    lNext =ppStream[dwIndex]->lDataLen -
            ppStream[dwIndex]->lDataPos;

    // 开辟一存放声音数据的空间
    pData = new BYTE[ ppStream[dwIndex]->lReadLen ];

    // 对文件定位到目前的位置
    mmioSeek( file, 44+ppStream[dwIndex]->lDataPos, SEEK_SET);
    // 读取声音数据
    if( ppStream[dwIndex]->lReadLen > lNext )
    {
        mmioRead( file, (char*)pData, lNext );
        if( ppStream[dwIndex]->bLoop ) {
            // 求出在文件数据的开头应该读取多少数据
            LONG lSurplus = ppStream[dwIndex]->lReadLen -lNext;
            mmioSeek( file, 44, SEEK_SET );
            mmioRead( file,(char*)(pData+lNext),lSurplus );
            // 将游标回绕到文件数据的开头
            ppStream[dwIndex]->lDataPos = lSurplus;
        }
        else
            ppStream[dwIndex]->lDataPos += lNext;
    }
    else {
        mmioRead(file,(char*)pData,ppStream[dwIndex]->lReadLen );
        // 游标下移
        ppStream[dwIndex]->lDataPos+=ppStream[dwIndex]->lReadLen;
    }

    mmioClose( file, 0 );
	return pData;
}

/********************************************************
* 函数名: UpdateStreamBuffer(...)
* 功能: 从文件读取数据更新流式缓冲区段(前段或后段)
*********************************************************/
BOOL CDSound::UpdateStreamBuffer( DWORD dwIndex )
{
    // 检测有效性
    if( !bSoundAble )                     return FALSE;
    if( dwIndex >= dwNumStreams )         return FALSE;
    if( ppStream == NULL )                return FALSE;
    if( ppStream[dwIndex]==NULL )         return FALSE;
    if( ppStream[dwIndex]->lpBuffer==NULL)return FALSE;

    HRESULT hr;
	BYTE *pData =NULL;//临时缓冲区
    // 注: LoadStreamData函数创建了一个由pData指向
    //     的空间,用完要释放它
	pData = LoadStreamData( dwIndex );
	if( pData == NULL )
        return FALSE;

    //>>>>>>>>>>>> 缓冲区写入操作 >>>>>>>>>>>>>>>
	LPVOID lpvPtr1, lpvPtr2;//指针1	//指针2
	DWORD dwBytes1, dwBytes2;
    DWORD dwOffset;
    if( ppStream[dwIndex]->lBufSection==0 )
        dwOffset = 0;
    else
        dwOffset = (DWORD)ppStream[dwIndex]->lReadLen;

    // 锁定缓冲区，以便向它写入数据
	hr = ppStream[dwIndex]->lpBuffer->Lock( dwOffset,
                    (DWORD)ppStream[dwIndex]->lReadLen,
                    &lpvPtr1, &dwBytes1,
                    &lpvPtr2, &dwBytes2, 0);
	if( FAILED(hr) ) {
        RestoreBuffer( ppStream[dwIndex]->lpBuffer );
        delete[] pData;
        return FALSE;
    }

    // 将数据传递到缓冲区
	memcpy( lpvPtr1, pData, dwBytes1 );
	if( dwBytes2 )
		memcpy( lpvPtr2, pData+dwBytes1, dwBytes2 );

    // 解除缓冲区的锁定操作
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
