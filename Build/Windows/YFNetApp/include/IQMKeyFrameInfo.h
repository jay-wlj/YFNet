#pragma once

const GUID GUID_TIME_FORMAT_MEDIA_TIME = {0x7b785574, 0x8c82, 0x11cf, {0xbc, 0xc, 0x0, 0xaa, 0x0, 0xac, 0x74, 0xf6}};

interface 
 __declspec(uuid("01A5BBD3-FE71-487C-A2EC-F585918A8724"))
IQMKeyFrameInfo:IUnknown
{
	virtual HRESULT (GetKeyFrameCount) (UINT& nKFs) = 0; // returns S_FALSE when every frame is a keyframe
	virtual HRESULT (GetKeyFrames) (const GUID* pFormat, REFERENCE_TIME* pKFs, UINT& nKFs /* in, out*/) = 0;//pFormat 为：TIME_FORMAT_MEDIA_TIME， pKFs为关键帧时间戳数组
};
interface 
 __declspec(uuid("01A5BBD4-FE71-487C-A2EC-F585918A8724"))
IQMKeyFrameInfoEx:IUnknown
{
	virtual HRESULT (GetKeyFramesPos) (const GUID* pFormat, REFERENCE_TIME* pKFs, LONGLONG *pKFFP, UINT& nKFs) = 0; // return keyframe and keyframe's file pos
	virtual HRESULT (GetIndexPos) (LONGLONG* pIndexPos) = 0; // return index pos 
};