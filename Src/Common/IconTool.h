#pragma once

namespace IconTool{

	struct TCursorOrIcon 
	{ 
		WORD Reserved; 
		WORD wType; 
		WORD Count; 
	}; 

	struct TIconRec 
	{ 
		BYTE Width; 
		BYTE Height; 
		WORD Colors; 
		WORD Reserved1; 
		WORD Reserved2; 
		long DIBSize; 
		long DIBOffset; 
	}; 

	static void InitBmpInfoHeader(HBITMAP Bitmap,BITMAPINFOHEADER& BI, int nBit) 
	{ 
		int Bytes; 
		DIBSECTION DS; 

		DS.dsBmih.biSize = 0; 
		Bytes = GetObject(Bitmap, sizeof(DS), &DS); 
		
		if ( Bytes>=sizeof(DS.dsBm)+sizeof(DS.dsBmih) && DS.dsBmih.biSize>=sizeof(DS.dsBmih) ) 
			memcpy(&BI, &DS.dsBmih, sizeof(BITMAPINFOHEADER)); 
		else 
		{ 
			memset(&BI, 0, sizeof(BI)); 
			BI.biSize = sizeof(BI); 
			BI.biWidth = DS.dsBm.bmWidth; 
			BI.biHeight = DS.dsBm.bmHeight; 
			BI.biBitCount = DS.dsBm.bmPlanes 
			* DS.dsBm.bmBitsPixel; 
		} 

		if (nBit!=0) 
			BI.biBitCount = nBit;
 
		if (BI.biBitCount <= 8) 
			BI.biClrUsed = 1 << BI.biBitCount; 

		BI.biPlanes = 1;
 
		if (BI.biClrImportant > BI.biClrUsed) 
			BI.biClrImportant = BI.biClrUsed; 

		if (BI.biSizeImage == 0) 
			BI.biSizeImage = ((BI.biWidth*BI.biBitCount+31) / 32) * 4 * BI.biHeight; 
	};

	static void GetDIBSizes(HBITMAP Bitmap,DWORD& InfoSize, DWORD& ImageSize,int nBit) 
	{ 
		BITMAPINFOHEADER BI; 

		InitBmpInfoHeader(Bitmap, BI, nBit); 
		InfoSize = sizeof(BITMAPINFOHEADER); 
		if (BI.biBitCount > 8)
		{ 
			if (BI.biCompression&BI_BITFIELDS) 
				InfoSize += 12; 
		} 
		else 
			InfoSize += sizeof(RGBQUAD) * (BI.biClrUsed!=0 ? BI.biClrUsed :(1 << BI.biBitCount)); 
		
		ImageSize = BI.biSizeImage; 
	};

	static void GetDIB(HBITMAP Bitmap,BITMAPINFO* BmpInfo, void* Bits, int nBit) 
	{ 
		HDC DC; 
		DC = CreateCompatibleDC(NULL); 
		InitBmpInfoHeader(Bitmap, BmpInfo-> bmiHeader, nBit); 
		GetDIBits(DC, Bitmap, 0, BmpInfo-> bmiHeader.biHeight, Bits,BmpInfo, DIB_RGB_COLORS);
		DeleteDC(DC); 
	}; 

	static BOOL _stdcall SaveIcon(HICON Icon, LPCTSTR FileName, int nBit) 
	{ 
		BOOL ret; 
		void *MonoBits; 
		void *ColorBits; 
		DWORD dwWrite; 
		HANDLE hFile; 
		TIconRec List; 
		ICONINFO IconInfo; 
		TCursorOrIcon CI; 
		DWORD MonoInfoSize; 
		DWORD ColorInfoSize; 
		DWORD MonoBitsSize; 
		DWORD ColorBitsSize; 
		BITMAPINFO *MonoInfo; 
		BITMAPINFO *ColorInfo; 

		switch(nBit) 
		{ 
			case 0: 
			case 1: 
			case 4: 
			case 8: 
			case 16: 
			case 24: 
			case 32: 
				break; 
			default: 
				return FALSE; 
		} 

		if (Icon==NULL || !FileName) 
			return FALSE; 

		hFile = CreateFile(FileName,GENERIC_WRITE, FILE_SHARE_READ,0, CREATE_ALWAYS, 0, 0); 
		
		if (hFile==INVALID_HANDLE_VALUE) 
			return false; 

		memset(&CI, 0, sizeof(CI)); 
		memset(&List, 0, sizeof(List)); 
		GetIconInfo(Icon, &IconInfo); 

		GetDIBSizes(IconInfo.hbmMask,MonoInfoSize, MonoBitsSize, 1); 
		GetDIBSizes(IconInfo.hbmColor,ColorInfoSize, ColorBitsSize,nBit); 
		
		MonoInfo = (BITMAPINFO*)malloc(MonoInfoSize); 
		ColorInfo = (BITMAPINFO*)malloc(ColorInfoSize); 
		MonoBits = malloc(MonoBitsSize); 
		ColorBits = malloc(ColorBitsSize); 
		
		GetDIB(IconInfo.hbmMask, MonoInfo,MonoBits, 1); 
		GetDIB(IconInfo.hbmColor, ColorInfo,ColorBits, nBit); 
		
		CI.wType = 0x10001; 
		CI.Count = 1; 
		
		ret &= WriteFile(hFile, &CI, sizeof(CI),&dwWrite, NULL); 
		
		List.Width = ColorInfo-> bmiHeader.biWidth; 
		List.Height = ColorInfo-> bmiHeader.biHeight; 
		List.Colors = ColorInfo-> bmiHeader.biPlanes * ColorInfo-> bmiHeader.biBitCount; 
		List.DIBSize = ColorInfoSize + ColorBitsSize + MonoBitsSize; 
		List.DIBOffset = sizeof(CI) + sizeof(List); 
		
		ret &= WriteFile(hFile, &List, sizeof(List), &dwWrite, NULL); 
		
		ColorInfo-> bmiHeader.biHeight *= 2; 
		
		ret &= WriteFile(hFile, ColorInfo,ColorInfoSize, &dwWrite, NULL); 
		ret &= WriteFile(hFile, ColorBits,ColorBitsSize, &dwWrite, NULL); 
		ret &= WriteFile(hFile, MonoBits,MonoBitsSize, &dwWrite, NULL); 
		
		free(ColorInfo); 
		free(MonoInfo); 
		free(ColorBits); 
		free(MonoBits); 
		
		DeleteObject(IconInfo.hbmColor); 
		DeleteObject(IconInfo.hbmMask); 

		CloseHandle(hFile); 

		return TRUE; 
	};

}