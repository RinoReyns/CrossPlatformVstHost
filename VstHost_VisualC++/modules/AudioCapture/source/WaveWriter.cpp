#include "WaveWriter.h"

CMFWaveWriter::CMFWaveWriter(uint8_t verbose) :
	wave_file_handler_(INVALID_HANDLE_VALUE),
	verbose_(verbose)
{}

CMFWaveWriter::~CMFWaveWriter()
{
	CLOSE_HANDLE_IF(wave_file_handler_); 
}

VST_ERROR_STATUS CMFWaveWriter::Initialize(const WCHAR* wszFile, WAVEFORMATEX* pwfx)
{
	RETURN_ERROR_IF_NOT_SUCCESS(this->ReadEndpointFormat(pwfx));

	this->header_length_ = this->is_extensible_format_header_ ? WAVE_HEAD_EXT_LEN : WAVE_HEAD_LEN;
	CLOSE_HANDLE_IF(wave_file_handler_);

	wave_file_handler_ = CreateFile(wszFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (wave_file_handler_ == INVALID_HANDLE_VALUE)
	{
		LOG(ERROR) << "Handler in WaveReader not created.";
		return VST_ERROR_STATUS::OPEN_FILE_ERROR;
	}

	BYTE WavHeader[WAVE_HEAD_EXT_LEN];
	memset(WavHeader, 0, sizeof(WavHeader));

	// TODO:
	// create one function for write to file and check status
	DWORD dwWritten;

	if(!WriteFile(wave_file_handler_, (LPCVOID)WavHeader, this->header_length_, &dwWritten, 0) || 
	   dwWritten != this->header_length_)
	{
		return VST_ERROR_STATUS::READ_WRITE_ERROR;
	}

	return VST_ERROR_STATUS::SUCCESS;
}

VST_ERROR_STATUS CMFWaveWriter::ReadEndpointFormat(WAVEFORMATEX* pwfx)
{
	switch (pwfx->wFormatTag)
	{
		case WAVE_FORMAT_PCM:
			LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"WAVE_FORMAT_PCM";
			break;

		case WAVE_FORMAT_IEEE_FLOAT:
			LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"WAVE_FORMAT_IEEE_FLOAT";
			break;

		case WAVE_FORMAT_EXTENSIBLE:
			LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"WAVE_FORMAT_EXTENSIBLE";
			this->is_extensible_format_header_ = TRUE;

			WAVEFORMATEXTENSIBLE* pWaveFormatExtensible = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx);

			if (pWaveFormatExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
			{
				LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"KSDATAFORMAT_SUBTYPE_PCM";
			}
			else if (pWaveFormatExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
			{
				LOG_IF(verbose_ > LogLevelType::LOG_LEVEL::ERROR_LOG, INFO) << L"KSDATAFORMAT_SUBTYPE_IEEE_FLOAT";
			}
			break;
	}

	return VST_ERROR_STATUS::SUCCESS;
}

BOOL CMFWaveWriter::WriteWaveData(const BYTE* pData, const DWORD dwLength)
{
	BOOL bRet = FALSE;
	DWORD dwWritten;

	if(!WriteFile(wave_file_handler_, (LPCVOID)pData, dwLength, &dwWritten, 0) || dwWritten != dwLength)
	{
		IF_ERROR_RETURN(bRet);
	}

	return bRet = TRUE;
}

BOOL CMFWaveWriter::FinalizeHeader(WAVEFORMATEX* pwfx, const UINT32 uiFileLength)
{
	BOOL bRet = FALSE;
	DWORD dwMove;
	DWORD dwWritten;

	BYTE WavHeader[WAVE_HEAD_EXT_LEN];
	memset(WavHeader, 0, sizeof(WavHeader));

	if((dwMove = SetFilePointer(wave_file_handler_, 0, NULL, FILE_BEGIN)) == INVALID_SET_FILE_POINTER)
	{
		IF_ERROR_RETURN(bRet);
	}

	if(this->is_extensible_format_header_)
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

	if(!WriteFile(wave_file_handler_, (LPCVOID)WavHeader, this->header_length_, &dwWritten, 0) || 
		dwWritten != this->header_length_)
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
	if (uiDataLen == 0)
	{
		return FALSE;
	}

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