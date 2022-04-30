//----------------------------------------------------------------------------------------------
// MFWaveWriter.h
//----------------------------------------------------------------------------------------------
#ifndef MFWAVEWRITER_H
#define MFWAVEWRITER_H

const UINT32 WAVE_HEAD_LEN		= 44;
const UINT32 WAVE_HEAD_EXT_LEN	= 80;

#define SWAP32(val) (UINT32)((((UINT32)(val)) & 0x000000FF)<<24 | (((UINT32)(val)) & 0x0000FF00)<<8 | (((UINT32)(val)) & 0x00FF0000)>>8 | (((UINT32)(val)) & 0xFF000000)>>24)

#pragma pack(push, 1)

struct RIFFCHUNK
{
	UINT32 fcc;
	UINT32 cb;
};

struct RIFFLIST
{
	UINT32 fcc;
	UINT32 cb;
	UINT32 fccListType;
};

struct WAVEFORM
{
	UINT32 fcc;
	UINT32 cb;
	UINT16 wFormatTag;
	UINT16 nChannels;
	UINT32 nSamplesPerSec;
	UINT32 nAvgBytesPerSec;
	UINT16 nBlockAlign;
	UINT16 wBitsPerSample;
};

struct WAVEFORM_EXT : public WAVEFORM
{
	UINT16 cbSize;
	UINT16 wValidBitsPerSample;
	UINT32 dwChannelMask;
	GUID SubFormat;
};

struct FACT
{
	UINT32 fcc;
	UINT32 cb;
	UINT32 lenght;
};

#pragma pack(pop)

class CMFWaveWriter
{
public:

	CMFWaveWriter() : m_hFile(INVALID_HANDLE_VALUE){}
	~CMFWaveWriter(){ CLOSE_HANDLE_IF(m_hFile); }

	BOOL Initialize(const WCHAR*, const BOOL);
	BOOL WriteWaveData(const BYTE*, const DWORD);
	BOOL FinalizeHeader(WAVEFORMATEX*, const UINT32, const BOOL);

private:

	HANDLE m_hFile;

	BOOL SetWaveHeader(const WAVEFORMATEX*, const UINT32, BYTE*);
	BOOL SetWaveHeaderExt(WAVEFORMATEX*, const UINT32, BYTE*);
};

#endif