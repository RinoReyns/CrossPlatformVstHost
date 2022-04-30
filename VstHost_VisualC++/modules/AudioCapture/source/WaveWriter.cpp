//----------------------------------------------------------------------------------------------
// MFWaveWriter.cpp
//----------------------------------------------------------------------------------------------
#include "Stdafx.h"
#include "WaveWriter.h"

BOOL CMFWaveWriter::Initialize(const WCHAR* wszFile, const BOOL bExtensibleFormat)
{
	BOOL bRet = FALSE;
	const UINT32 bHeaderLenght = bExtensibleFormat ? WAVE_HEAD_EXT_LEN : WAVE_HEAD_LEN;
	CLOSE_HANDLE_IF(m_hFile);

	m_hFile = CreateFile(wszFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		IF_ERROR_RETURN(bRet);
	}

	BYTE WavHeader[WAVE_HEAD_EXT_LEN];
	memset(WavHeader, 0, sizeof(WavHeader));

	DWORD dwWritten;

	if(!WriteFile(m_hFile, (LPCVOID)WavHeader, bHeaderLenght, &dwWritten, 0) || dwWritten != bHeaderLenght)
	{
		IF_ERROR_RETURN(bRet);
	}

	return bRet = TRUE;
}

BOOL CMFWaveWriter::WriteWaveData(const BYTE* pData, const DWORD dwLength)
{
	BOOL bRet = FALSE;
	DWORD dwWritten;

	if(!WriteFile(m_hFile, (LPCVOID)pData, dwLength, &dwWritten, 0) || dwWritten != dwLength)
	{
		IF_ERROR_RETURN(bRet);
	}

	return bRet = TRUE;
}

BOOL CMFWaveWriter::FinalizeHeader(WAVEFORMATEX* pwfx, const UINT32 uiFileLength, const BOOL bExtensibleFormat)
{
	BOOL bRet = FALSE;
	DWORD dwMove;
	DWORD dwWritten;
	const UINT32 bHeaderLenght = bExtensibleFormat ? WAVE_HEAD_EXT_LEN : WAVE_HEAD_LEN;

	BYTE WavHeader[WAVE_HEAD_EXT_LEN];
	memset(WavHeader, 0, sizeof(WavHeader));

	if((dwMove = SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN)) == INVALID_SET_FILE_POINTER)
	{
		IF_ERROR_RETURN(bRet);
	}

	if(bExtensibleFormat)
	{
		if(!SetWaveHeaderExt(pwfx, uiFileLength, WavHeader))
		{
			IF_ERROR_RETURN(bRet);
		}
	}
	else
	{
		if(!SetWaveHeader(pwfx, uiFileLength, WavHeader))
		{
			IF_ERROR_RETURN(bRet);
		}
	}

	if(!WriteFile(m_hFile, (LPCVOID)WavHeader, bHeaderLenght, &dwWritten, 0) || dwWritten != bHeaderLenght)
	{
		IF_ERROR_RETURN(bRet);
	}

	return bRet = TRUE;
}

BOOL CMFWaveWriter::SetWaveHeaderExt(WAVEFORMATEX* pwfx, const UINT32 uiDataLen, BYTE* head)
{
	if (uiDataLen == 0)
	{
		return FALSE;
	}

	assert((uiDataLen * pwfx->nBlockAlign) % 2 == 0);

	RIFFCHUNK* pch;
	RIFFLIST  *priff;
	WAVEFORM_EXT *pwaveExt;
	FACT* pFact;
	WAVEFORMATEXTENSIBLE *pWaveFormatExtensible = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(pwfx);

	priff = (RIFFLIST*)head;
	priff->fcc = SWAP32('RIFF');

	priff->cb = (uiDataLen * pwfx->nBlockAlign) + WAVE_HEAD_EXT_LEN - sizeof(RIFFCHUNK);
	priff->fccListType = SWAP32('WAVE');

	pwaveExt = (WAVEFORM_EXT*)(priff + 1);
	pwaveExt->fcc = SWAP32('fmt ');
	pwaveExt->cb = sizeof(WAVEFORM_EXT) - sizeof(RIFFCHUNK);
	pwaveExt->wFormatTag = pwfx->wFormatTag;
	pwaveExt->nChannels = pwfx->nChannels;
	pwaveExt->nSamplesPerSec = pwfx->nSamplesPerSec;
	pwaveExt->nAvgBytesPerSec = pwfx->nAvgBytesPerSec;
	pwaveExt->nBlockAlign = pwfx->nBlockAlign;
	pwaveExt->wBitsPerSample = pwfx->wBitsPerSample;
	pwaveExt->cbSize = pwfx->cbSize;
	pwaveExt->wValidBitsPerSample = pWaveFormatExtensible->Samples.wValidBitsPerSample;
	pwaveExt->dwChannelMask = pWaveFormatExtensible->dwChannelMask;
	pwaveExt->SubFormat = pWaveFormatExtensible->SubFormat;

	pFact = (FACT*)(pwaveExt + 1);
	pFact->fcc = SWAP32('fact');
	pFact->cb = 4;
	pFact->lenght = uiDataLen * pwaveExt->nChannels;

	pch = (RIFFCHUNK*)(pFact + 1);
	pch->fcc = SWAP32('data');
	pch->cb = (uiDataLen * pwfx->nBlockAlign);

	return TRUE;
}

BOOL CMFWaveWriter::SetWaveHeader(const WAVEFORMATEX* pwfx, const UINT32 uiDataLen, BYTE* head)
{
	if(uiDataLen == 0)
		return FALSE;

	RIFFCHUNK* pch;
	RIFFLIST* priff;
	WAVEFORM* pwave;

	priff = (RIFFLIST*)head;
	priff->fcc = SWAP32('RIFF');

	priff->cb = (uiDataLen * pwfx->nBlockAlign) + WAVE_HEAD_LEN - sizeof(RIFFCHUNK);
	priff->fccListType = SWAP32('WAVE');

	pwave = (WAVEFORM*)(priff + 1);
	pwave->fcc = SWAP32('fmt ');
	pwave->cb = sizeof(WAVEFORM) - sizeof(RIFFCHUNK);
	pwave->wFormatTag = pwfx->wFormatTag;
	pwave->nChannels = pwfx->nChannels;
	pwave->nSamplesPerSec = pwfx->nSamplesPerSec;
	pwave->nAvgBytesPerSec = pwfx->nAvgBytesPerSec;
	pwave->nBlockAlign = pwfx->nBlockAlign;
	pwave->wBitsPerSample = pwfx->wBitsPerSample;

	pch = (RIFFCHUNK*)(pwave + 1);
	pch->fcc = SWAP32('data');
	pch->cb = (uiDataLen * pwfx->nBlockAlign);

	return TRUE;
}