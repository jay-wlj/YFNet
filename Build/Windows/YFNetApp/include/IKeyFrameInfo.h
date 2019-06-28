/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once
const GUID GUID_TIME_FORMAT_MEDIA_TIME = {0x7b785574, 0x8c82, 0x11cf, {0xbc, 0xc, 0x0, 0xaa, 0x0, 0xac, 0x74, 0xf6}};
//IKeyFrameInfo
const GUID IID_IKeyFrameInfo = {0x01A5BBD3, 0xFE71, 0x487C, {0xA2, 0xEC, 0xF5, 0x85, 0x91, 0x8A, 0x87, 0x24}};
[uuid("01A5BBD3-FE71-487C-A2EC-F585918A8724")]
interface IKeyFrameInfo : public IUnknown
{
	STDMETHOD (GetKeyFrameCount) (UINT& nKFs) = 0; // returns S_FALSE when every frame is a keyframe
	STDMETHOD (GetKeyFrames) (const GUID* pFormat, LONGLONG* pKFs, UINT& nKFs /* in, out*/) = 0;//pFormat 为：TIME_FORMAT_MEDIA_TIME， pKFs为关键帧时间戳数组
};

//[uuid("01A5BBD4-FE71-487C-A2EC-F585918A8724")]
const GUID IID_IKeyFrameInfoEx = {0x01A5BBD4, 0xFE71, 0x487C, {0xA2, 0xEC, 0xF5, 0x85, 0x91, 0x8A, 0x87, 0x24}};
interface IKeyFrameInfoEx : public IKeyFrameInfo
{
	STDMETHOD (GetKeyFramesPos) (const GUID* pFormat, LONGLONG* pKFs, LONGLONG *pKFFP, UINT& nKFs) = 0; // return keyframe and keyframe's file pos
	STDMETHOD (GetIndexPos) (LONGLONG* pIndexPos) = 0; // return index pos 
};
