#ifndef _GDIPLUSTOOL_H_H__
#define _GDIPLUSTOOL_H_H__
#include <Windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#include <atlstr.h>
#include <vector>
#include "StringTool.h"
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

class GdiPlusTool
{
public:
	//克隆一个新的Bitmap对象，不用时，需使用delete释放。
	static Gdiplus::Bitmap *CloneBtimap(Gdiplus::Bitmap *pBitmap)
	{
		Gdiplus::Bitmap* pCloneObj = NULL;
		if(pBitmap)
		{
			int nW = pBitmap->GetWidth();
			int nH = pBitmap->GetHeight();
			if(nW > 0 &&  nH> 0)
			{
				Gdiplus::Rect rt(0, 0, nW, nH);
				pCloneObj = new Gdiplus::Bitmap(rt.Width, rt.Height);
				Gdiplus::Graphics gra(pCloneObj);
				gra.DrawImage(pBitmap, rt);
			}
		}
		return pCloneObj;
	}

	static Bitmap* LoadFile(const CString& strFile)
	{
		Bitmap *pBitmap = NULL;
		CStringW wstrFile = Convert::ToWString(strFile);
		if(GetFileAttributesW(wstrFile) != INVALID_FILE_ATTRIBUTES)
		{
			Bitmap*	pTmp = Bitmap::FromFile(wstrFile);
			pBitmap = GdiPlusTool::CloneBtimap(pTmp);
			delete pTmp;
		}
		return pBitmap;
	}

	static Bitmap* LoadResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType)
	{
		Bitmap *pBmp = NULL;
		HRSRC hSrc = ::FindResource(hModule, lpName, lpType);
		if (! hSrc)
		{
			return pBmp;
		}
		HGLOBAL hRes = ::LoadResource(hModule, hSrc);
		if (! hRes)
		{
			return pBmp;
		}
		DWORD dwSize = SizeofResource(hModule, hSrc);
		LPVOID pVoid = LockResource(hRes);
		HGLOBAL hGblobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
		if (hGblobal && pVoid)
		{
			LPVOID pGlobal = GlobalLock(hGblobal);
			memcpy(pGlobal, pVoid, dwSize);

			IStream *pStream = NULL;
			if (CreateStreamOnHGlobal(hGblobal, TRUE, &pStream) == S_OK)
			{
				pBmp = Bitmap::FromStream(pStream);
				pStream->Release();

				if (Ok != pBmp->GetLastStatus())
				{
					delete pBmp;
					pBmp = NULL;
				}
			}
			GlobalUnlock(hGblobal);
			GlobalFree(hGblobal);
		}
		UnlockResource(hSrc);
		return pBmp;
	}
	static BOOL LoadImages(const CString& strFile,std::vector<Bitmap*> &vpBitmap, std::vector<int>& vDelay)
	{
		Image *pImage = Image::FromFile(strFile);
		Status sRet = pImage->GetLastStatus();
		if (sRet == OutOfMemory || sRet == FileNotFound)
		{
			SafeDelete(pImage);
			return FALSE;
		}

		//对图标进行处理，解决XP下不透明问题
		if (0 == _tcsnicmp(PathFindExtension(strFile), _T(".ico"), _tcslen(_T(".ico"))))
		{
			Bitmap *pTmpBmp = Bitmap::FromFile(strFile);

			Rect rectBounds(0, 0, pTmpBmp->GetWidth(), pTmpBmp->GetHeight() ); 
			Graphics grap(pImage);

			Gdiplus::BitmapData bmData;  
			pTmpBmp->LockBits(&rectBounds, ImageLockModeRead, PixelFormat32bppARGB, &bmData); 
			Bitmap bmpAlpha(bmData.Width, bmData.Height, bmData.Stride, PixelFormat32bppARGB,(BYTE*)bmData.Scan0); 
			pTmpBmp->UnlockBits(&bmData);  

			grap.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality); 
			grap.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality); 
			grap.SetCompositingMode(Gdiplus::CompositingModeSourceOver); 

			SolidBrush br(Color(255, 255, 255, 255));
			grap.FillRectangle(&br, rectBounds);				//先填充白色背景
			ImageAttributes ia;
			Color clr;
			pTmpBmp->GetPixel(0, 0, &clr);
			if (clr.GetValue() == Color(255, 0, 0, 0).GetValue())
			{
				ia.SetColorKey(clr, clr);								//设置透明色
			}
			grap.DrawImage(&bmpAlpha, rectBounds, 0, 0, rectBounds.Width, rectBounds.Height, UnitPixel, &ia);
			SafeDelete(pTmpBmp);
		}


		UINT count = pImage->GetFrameDimensionsCount();
		if(count <= 0)
		{
			return FALSE;
		}

		GUID *pDimensionIDs=(GUID*)new GUID[count];
		ZeroMemory(pDimensionIDs ,sizeof(GUID)*count);
		pImage->GetFrameDimensionsList(pDimensionIDs, count);
		UINT nFrameCount = pImage->GetFrameCount(&pDimensionIDs[0]);
		if (nFrameCount > 1)
		{
			UINT bufferSize = pImage->GetPropertyItemSize(PropertyTagFrameDelay);
			BYTE *pBuff = new BYTE[bufferSize];
			PropertyItem* pItems = (PropertyItem*)pBuff;
			pImage->GetPropertyItem(PropertyTagFrameDelay, bufferSize, pItems);
			long* pValue = (long*)pItems->value;
			for (int i = 0; i < nFrameCount; i++)
			{	
				vDelay.push_back(*pValue * 10);
				++pValue;
			}
			delete []pBuff;
		}

		for(int i = 0; i < nFrameCount; i++)
		{
			pImage->SelectActiveFrame(&FrameDimensionTime, i);
			vpBitmap.push_back( GdiPlusTool::CloneBtimap((Bitmap*)pImage));
		}
		delete pImage;
		delete []pDimensionIDs;
		return TRUE;
	}
	static Bitmap* FromIconFile(const CString& strFile, int size = 48)
	{
		Bitmap *pBitmap = NULL;
		HICON hIcon = GetHIconFromFile(strFile,size);
		if(hIcon)
		{
			pBitmap = Bitmap::FromHICON(hIcon);
			DestroyIcon(hIcon);
		}
		return pBitmap;
	}
	/*************************************************
	Function:       // GetHIconFromFile
	Description:    // 得到一个文件的icon句柄,默认是48*48,
	Input:          // strFile，文件路径
	Output:         // 无
	Return:         // icon句柄
	*************************************************/
	static HICON GetHIconFromFile(const CString& strFile, UINT nsize = 48)
	{
		CStringW wstrFile = Convert::ToWString(strFile);
		if(GetFileAttributesW(wstrFile) != INVALID_FILE_ATTRIBUTES)
		{
			HICON hIcon = NULL;
			UINT nIconID = 0;
			PrivateExtractIcons(strFile, 0, nsize, nsize, &hIcon, &nIconID, 1, LR_LOADFROMFILE);
			if(hIcon == NULL)
			{
				PrivateExtractIcons(strFile, 0, 32, 32, &hIcon, &nIconID, 1, LR_LOADFROMFILE);
			}
			if(hIcon == NULL)
			{
				PrivateExtractIcons(strFile, 0, 16, 16, &hIcon, &nIconID, 1, LR_LOADFROMFILE);
			}
			return hIcon;
		}
		return NULL;
	}

	static int MeasureStringWidth(HWND hWnd, const CString& strTxt, LPCTSTR lpFarmaly, UINT uSize)
	{
		Gdiplus::Graphics gra(hWnd);
		Gdiplus::PointF origin;
		Font nameFont(lpFarmaly, uSize, 0, UnitPixel);
		RectF boundingBox;
		gra.MeasureString(Convert::ToWString(strTxt), -1, &nameFont, origin, &boundingBox);
		return (long)boundingBox.Width;
	}

	static int MeasureStringHeight(HWND hWnd, const CString& strTxt, const CString &strfontfamily, int nFontSize)
	{
		Gdiplus::Graphics gra(hWnd);
		Gdiplus::PointF origin;
		Font(strfontfamily,nFontSize) ;
		RectF boundingBox;
		gra.MeasureString(Convert::ToWString(strTxt), -1, &Gdiplus::Font(strfontfamily,nFontSize), origin, &boundingBox);
		return (long)boundingBox.Height;
	}

	static BOOL SaveBmp(const CString& path, Bitmap* pBmp)
	{
		if (pBmp)
		{
			CLSID clsid;
			if(GetEncoderClsid(L"image/bmp", &clsid))
			{
				if(pBmp->Save(Convert::ToWString(path), &clsid, 0) == Ok)
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	static BOOL SaveJpgByQuality(const CString& path, Image *pBmp,long lQuality = 90)
	{
		BOOL bRet = FALSE;
		if ( pBmp)
		{
			CLSID clsid;
			Gdiplus::EncoderParameters eps;
			eps.Count = 1;
			eps.Parameter[0].Guid = Gdiplus::EncoderQuality;
			eps.Parameter[0].NumberOfValues = 1;
			eps.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			eps.Parameter[0].Value = &lQuality;		
			if(GetEncoderClsid(L"image/jpeg", &clsid))
			{
				bRet = pBmp->Save(path, &clsid, &eps) == Gdiplus::Ok;
			}
		}
		return bRet;
	}

	static BOOL SaveDataImage(LPCTSTR path, BYTE *pData, long lQuality = 90)
	{
		BOOL bRet = FALSE;
		CString str = path;
		CString strp4 = str.Right(4);
		BOOL bBMP = TRUE;
		if(strp4.CompareNoCase(_T(".jpg")) == 0)
		{
			bBMP = FALSE;
		}
		Bitmap *pBmp = Bitmap::FromBITMAPINFO((BITMAPINFO*)pData, pData+sizeof(BITMAPINFO));

		if (bBMP)
		{
			bRet = SaveBmp(str, pBmp);
		}
		else
		{
			bRet = SaveJpgByQuality(str, pBmp, lQuality);
		}
		SafeDelete(pBmp);
		return bRet;
	}
	
	static BOOL SaveJpgByCompression(const CString& path, Image *pBmp,long Compression = 0)
	{
		BOOL bRet = FALSE;
		if ( pBmp)
		{
			CLSID clsid;
			EncoderParameters eps;
			eps.Count = 1;
			eps.Parameter[0].Guid = Gdiplus::EncoderCompression;
			eps.Parameter[0].NumberOfValues = 1;
			eps.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			eps.Parameter[0].Value = &Compression;
			if(GetEncoderClsid(L"image/jpeg", &clsid))
			{
				bRet = pBmp->Save(Convert::ToWString(path), &clsid, &eps) == Gdiplus::Ok;
			}
		}
		return bRet;
	}

	static BOOL GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		if(format == NULL) return FALSE;
		UINT  numEncoders = 0;        
		UINT  size = 0;        
		Gdiplus::GetImageEncodersSize(&numEncoders, &size);
		if(size == 0) return FALSE;  

		Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(new char[size]);
		if(pImageCodecInfo == NULL) return FALSE;  

		Gdiplus::GetImageEncoders(numEncoders, size, pImageCodecInfo);
		for(UINT j = 0; j < numEncoders; ++j)
		{
			if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				delete []pImageCodecInfo;
				return TRUE;  
			}
		}
		delete []pImageCodecInfo;
		return FALSE; 
	}
	static Bitmap * GrayImage(Image* pImg)
	{
		Bitmap *pBmp = NULL;
		if (pImg)
		{
			pBmp = new Bitmap(pImg->GetWidth(), pImg->GetHeight());
			Graphics p(pBmp); /*= Graphics::FromImage(pImg);*/
			ColorMatrix ClrMatrix =
			{
				0.299f, 0.299f, 0.299f, 0.0f, 0.0f,
				0.587f, 0.587f, 0.587f, 0.0f, 0.0f,
				0.114f, 0.114f, 0.114f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f
			};
			ImageAttributes ImgAttr;
			ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
			p.DrawImage(pImg, Rect(0, 0, pImg->GetWidth(), pImg->GetHeight()),
				0, 0, pImg->GetWidth(), pImg->GetHeight(), UnitPixel, &ImgAttr);
		}
		return pBmp;
	}
	static Image* ScaleImage(Image *pImg, INT nMaxW, INT nMaxH)
	{
		Image *pRet = 0;
		if (pImg)
		{
			int w = pImg->GetWidth();
			int h = pImg->GetHeight();
			if (w > nMaxW || h > nMaxH)
			{
				float ws = float(nMaxW) / float(w);
				float hs = float(nMaxH) / float(h);
				if (ws > hs)
				{
					h *= hs;
					w *= hs;
				}
				else
				{
					h *= ws;
					w *= hs;
				}
			}
			pRet = new Bitmap(w, h, PixelFormat24bppRGB);
			Graphics *p = Graphics::FromImage(pRet);
			p->DrawImage(pImg, Rect(0, 0, w, h), 0,0, pImg->GetWidth(), pImg->GetHeight(), UnitPixel);
			delete p;
		}
		return pRet;
	}

	static void GetRoundRectPath(Rect rect, int radius, GraphicsPath& path)
	{			
		if (radius > 0)
		{
			path.StartFigure();

			int width = rect.Width - 1 ;
			int height = rect.Height - 1 ;

			Rect r = rect ;
			int dia = radius;

			if(dia > r.Width)     
				dia = r.Width;
			if(dia > r.Height)     
				dia = r.Height;

			// define a corner 
			Rect Corner(r.X, r.Y, dia, dia);

			// top left
			path.AddArc(Corner, 180, 90);                    

			if(dia == 20)
			{
				Corner.Width += 1; 
				Corner.Height += 1; 
				r.Width -=1; 
				r.Height -= 1;
			}

			//top right
			Corner.X += (r.Width - dia - 1);
			path.AddArc(Corner, 270, 90);    

			// bottom right
			Corner.Y += (r.Height - dia - 1);
			path.AddArc(Corner, 0, 90);    

			// bottom left
			Corner.X -= (r.Width - dia - 1);
			path.AddArc(Corner, 90, 90);

			path.CloseFigure();    
		}
		else
		{
			path.AddRectangle(rect);
		}
	}

	//画圆角矩形
	static void Draw3DRoundRect(Graphics* gr, Rect rect, int radius, DWORD rtColor, DWORD fillColor, bool IsFill, int nBorder=0)
	{
		if ( radius > 0 )
		{
			GraphicsPath path;
			GetRoundRectPath(rect, radius, path);

			SmoothingMode smode = gr->GetSmoothingMode() ;
			gr->SetSmoothingMode(SmoothingModeAntiAlias);

			if(IsFill && fillColor > 0 )
			{
				SolidBrush solidBrush(fillColor);
				gr->FillPath(&solidBrush, &path);
			}

			if ( rtColor > 0 && nBorder > 0  )
			{
				Pen pen(rtColor, nBorder);
				gr->DrawPath(&pen, &path);
			}
			else if( fillColor > 0) 
			{
				Pen pen(fillColor, 1);
				gr->DrawPath(&pen, &path);
			}

			gr->SetSmoothingMode(smode);
		}
		else
		{
			if(IsFill && fillColor > 0)
			{
				SolidBrush solidBrush(fillColor);
				gr->FillRectangle(&solidBrush, rect);
			}

			if ( rtColor > 0 )
			{
				if ( nBorder <= 0 )
				{
					nBorder = 1 ;
				}

				Gdiplus::Pen pen(rtColor,nBorder);
				Point pt1(rect.X, rect.Y);
				Point pt2(rect.X+rect.Width-nBorder, rect.Y);
				Point pt3(rect.X+rect.Width-nBorder, rect.Y+rect.Height-nBorder);
				Point pt4(rect.X,rect.Y+rect.Height-nBorder);

				gr->DrawLine(&pen,pt1,pt2);
				gr->DrawLine(&pen,pt2,pt3);
				gr->DrawLine(&pen,pt3,pt4);
				gr->DrawLine(&pen,pt4,pt1);
			}
		}
	}

	//画圆角矩形
	static void Draw3DRoundImage(Graphics* pGraph, Rect rect, int radius, Bitmap *pImage)
	{
		if ( radius > 0 )
		{
			Bitmap bmp(rect.Width, rect.Height) ;			
			Gdiplus::TextureBrush brush(pImage) ;

			Graphics gra(&bmp);
			gra.SetSmoothingMode(SmoothingModeHighQuality);

			double fScaleX = (double)rect.Width/pImage->GetWidth() ;
			double fScaleY = (double)rect.Height/pImage->GetHeight() ;

			brush.ScaleTransform(fScaleX, fScaleY,MatrixOrderAppend) ;

			Rect rt(0,0,rect.Width,rect.Height); 
			GraphicsPath path;
			GetRoundRectPath(rt, radius, path);

			gra.FillPath(&brush, &path);

			pGraph->DrawImage(&bmp, rect, 0, 0, rect.Width, rect.Height, UnitPixel);
		}
	}

	//将图像分三段绘图  左中右， 上中下   中间平铺
	static void DrawThreePartImg(Graphics *gra, Bitmap* pBmp, const Rect& rect, BOOL bHorizontal = TRUE)
	{
		if(gra && pBmp)
		{
			int nW = pBmp->GetWidth();
			int nH = pBmp->GetHeight();

			if(bHorizontal) // 水平
			{
				int nPerW = nW / 3;
				int nY = rect.Y+(rect.Height - nH)/2;

				int w = rect.Width/3;
				w = w<nPerW? w: nPerW;

				if (w <= 0)
				{
					w = rect.Width;
					Rect rcLeft(rect.X, nY, w, nH);

					ImageAttributes  imAtt;
					imAtt.SetWrapMode(WrapModeTile);												//平铺中间段图片
					gra->DrawImage(pBmp, rcLeft , 0, 0, nW, nH, UnitPixel, &imAtt);		//画左边
					return;
				}

				Rect rcLeft(rect.X, nY, w, nH);
				gra->DrawImage(pBmp, rcLeft , 0, 0, w, nH, UnitPixel);		//画左边

				Rect rcMid(rect.X+w, nY, rect.Width-2*w, nH);
				ImageAttributes  imAtt;
				imAtt.SetWrapMode(WrapModeTile);												//平铺中间段图片
				gra->DrawImage(pBmp, rcMid , nPerW, 0, nPerW, nH, UnitPixel, &imAtt);

				Rect rcRight(rect.GetRight()-w, nY, w, nH);
				gra->DrawImage(pBmp, rcRight , nW-w, 0, w, nH, UnitPixel);	//画右边
			}
			else
			{
				int nPerH = nH / 3;
				int nX = rect.X+(rect.Width - nW)/2;

				int h = rect.Height/3;
				h = h<nPerH? h: nPerH;

				if (h <= 0)
				{
					h = rect.Height;
					Rect rcTop(nX, rect.Y, nW, h);

					ImageAttributes  imAtt;
					imAtt.SetWrapMode(WrapModeTile);												//平铺中间段图片
					gra->DrawImage(pBmp, rcTop , 0, 0, nW, nH, UnitPixel, &imAtt);		//画上边
					return;
				}

				Rect rcTop(nX, rect.Y, nW, h);
				gra->DrawImage(pBmp, rcTop , 0, 0, nW, h, UnitPixel);		//画上边

				Rect rcMid(nX, rect.Y+h, nW, rect.Height-2*h);
				ImageAttributes  imAtt;
				imAtt.SetWrapMode(WrapModeTile);												//平铺中间段图片
				gra->DrawImage(pBmp, rcMid , 0, nPerH, nW, nPerH, UnitPixel, &imAtt);

				Rect rcBottom(nX, rect.GetBottom()-h, nW, h);
				gra->DrawImage(pBmp, rcBottom ,0, nH-h, nW, h, UnitPixel);	//画下面
			}
		}

	}

	static BOOL SetClipboardBmp(HWND hWnd, Bitmap *pBmp)		//将Bitmap对象拷贝到粘贴板
	{
		BOOL bRet = FALSE;
		if (NULL == pBmp || Ok != pBmp->GetLastStatus())
		{
			return bRet;
		}

		HDC hDC  = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);			//为屏幕创建设备描述表
		HDC hMemDC = CreateCompatibleDC(hDC);							//为屏幕设备描述表创建兼容的内存设备描述表

		int cx = pBmp->GetWidth();
		int cy = pBmp->GetHeight();

		HBITMAP hBmp = ::CreateCompatibleBitmap(hDC, cx, cy);
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBmp);
		Graphics gra(hMemDC);
		gra.DrawImage(pBmp, Rect(0, 0, cx, cy),  0, 0, cx, cy, UnitPixel);
		gra.ReleaseHDC(hMemDC);
		hBmp = (HBITMAP)::SelectObject(hMemDC, hOldBmp);
		SafeDelObj(hMemDC);
		SafeDelObj(hDC);

		if (OpenClipboard(hWnd)) 
		{
			EmptyClipboard();			//清空剪贴板
			SetClipboardData(CF_BITMAP, hBmp);        //把屏幕内容粘贴到剪贴板上, hBitmap 为刚才的屏幕位图句柄
			CloseClipboard();			 //关闭剪贴板

			bRet = TRUE;
		}
		SafeDelObj(hBmp);
		return bRet;
	}

	//////////////////////////////////////////////////////////////////////////
	//本地采用GDI截图
	static BOOL SaveHWNDToFile(HWND hWnd, LPCTSTR lpFileName)   
	{   
		CRect rect;
		::GetWindowRect(hWnd, rect);
		if ( rect.Width() < 0 || rect.Height() <= 0 )
		{
			return FALSE;
		}

		HDC hScrDC = ::GetDC(hWnd) ;
		HBITMAP hBmp = SaveDCToBitmap(hScrDC, rect.Width(), rect.Height()) ;
		if( hBmp )
		{
			SaveBitmapToFile( hBmp, lpFileName ) ;
			DeleteObject( hBmp ) ;
		}
		::ReleaseDC(hWnd, hScrDC);

		return TRUE;   
	}

	static BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName)   
	{   
		HDC      hDC;									//设备描述表   
		int      iBits;                                 //当前显示分辨率下每个像素所占字节数   
		WORD     wBitCount;								//位图中每个像素所占字节数   
		DWORD    dwPaletteSize=0,						//定义调色板大小，   位图中像素字节大小   ，位图文件大小   ，   写入文件字节数   
			dwBmBitsSize,   
			dwDIBSize, dwWritten;   
		BITMAP                 Bitmap;							  //位图属性结构   
		BITMAPFILEHEADER       bmfHdr;							  //位图文件头结构   
		BITMAPINFOHEADER       bi;								  //位图信息头结构     
		LPBITMAPINFOHEADER     lpbi;							  //指向位图信息头结构   

		HANDLE                 fh, hDib, hPal, hOldPal = NULL ;   //定义文件，分配内存句柄，调色板句柄   

		//计算位图文件每个像素所占字节数   
		//hDC   =   CreateDC("DISPLAY",NULL,NULL,NULL);  

		hDC=::CreateCompatibleDC( NULL ) ;   
		if( !hDC )
		{
			return FALSE ;
		}
		iBits=GetDeviceCaps(hDC,BITSPIXEL)*GetDeviceCaps(hDC,PLANES);   
		DeleteDC(hDC);   

		if(iBits<=1)   
		{
			wBitCount=1;
		}
		else if(iBits<=4)
		{
			wBitCount=4;
		}
		else if(iBits<=8)   
		{
			wBitCount=8;   
		}
		else if(iBits<=24)   
		{
			wBitCount=24;   
		}
		else if(iBits<=32)   
		{
			wBitCount=32;  
		}
		else  
		{
			wBitCount=24;   
		}
		//计算调色板大小   
		if(wBitCount<=8)   
		{
			dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);   
		}
		//设置位图信息头结构   
		GetObject(hBitmap,sizeof(BITMAP),(LPSTR)&Bitmap);   
		bi.biSize=sizeof(BITMAPINFOHEADER);   
		bi.biWidth=Bitmap.bmWidth;   
		bi.biHeight=Bitmap.bmHeight;   
		bi.biPlanes=1;   
		bi.biBitCount=wBitCount;   
		bi.biCompression=BI_RGB;   
		bi.biSizeImage=0;   
		bi.biXPelsPerMeter=0;   
		bi.biYPelsPerMeter=0;   
		bi.biClrUsed=0;   
		bi.biClrImportant=0;   

		dwBmBitsSize=((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;   

		//为位图内容分配内存   
		hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));

		lpbi=(LPBITMAPINFOHEADER)GlobalLock(hDib);   
		*lpbi=bi;   

		//   处理调色板       
		hPal=GetStockObject(DEFAULT_PALETTE);   

		if( hPal )   
		{   
			hDC=::GetDC(NULL);   
			hOldPal=::SelectPalette(hDC,(HPALETTE)hPal,FALSE);   
			RealizePalette(hDC);   
		}

		//   获取该调色板下新的像素值   
		GetDIBits(hDC,
			hBitmap,
			0,
			(UINT)Bitmap.bmHeight,   
			(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize,
			(LPBITMAPINFO)lpbi,
			DIB_RGB_COLORS);   

		if( hOldPal )   
		{   
			SelectPalette(hDC,(HPALETTE)hOldPal,TRUE);   
			RealizePalette(hDC);   	  
		}

		::ReleaseDC(NULL,hDC); 


		//创建位图文件   
		fh = CreateFile(lpFileName, GENERIC_WRITE,0,NULL,CREATE_ALWAYS,   
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);   

		if( !fh || fh == INVALID_HANDLE_VALUE )
		{
			GlobalUnlock(hDib);   
			GlobalFree(hDib);   
			CloseHandle(fh);  

			return FALSE ;
		}

		//   设置位图文件头   
		bmfHdr.bfType=0x4D42;     //   "BM"   
		dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;       
		bmfHdr.bfSize=dwDIBSize;   
		bmfHdr.bfReserved1=0;   
		bmfHdr.bfReserved2 =0;   
		bmfHdr.bfOffBits=(DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;   

		//  写入位图文件头   
		WriteFile(fh,(LPSTR)&bmfHdr,sizeof(BITMAPFILEHEADER),&dwWritten,NULL);
		//  写入位图文件其余内容   
		WriteFile(fh,(LPSTR)lpbi,dwDIBSize,&dwWritten,NULL);  

		//清除       
		GlobalUnlock(hDib);   
		GlobalFree(hDib);   
		CloseHandle(fh);   

		return TRUE;   
	}

	static HBITMAP SaveDCToBitmap(HDC hScrDC, int nWidth, int nHeight)
	{
		HDC hMemDC;					//屏幕设备描述表 和 内存设备描述表   
		HBITMAP  hBitmap, hOldBitmap;		//hBitmap位图句柄用来保存rect所对应的图片    

		hMemDC=CreateCompatibleDC(NULL);	//创建内存设备描述表

		BITMAPINFOHEADER BMM={sizeof(BITMAPINFOHEADER)};
		BMM.biWidth = nWidth ;
		BMM.biHeight= nHeight ;
		BMM.biPlanes=1;
		BMM.biCompression=BI_RGB;
		BMM.biSizeImage=0;
		BMM.biXPelsPerMeter=0;
		BMM.biYPelsPerMeter=0;
		BMM.biClrUsed=0;
		BMM.biClrImportant=0;
		BMM.biBitCount=32;
		BMM.biSize=sizeof(BITMAPINFOHEADER);

		BYTE * Pbits = NULL;
		hBitmap=CreateDIBSection(hScrDC,(BITMAPINFO*)&BMM,DIB_RGB_COLORS,(VOID **)&Pbits,NULL,0);

		if (NULL == hBitmap)
		{
			return NULL ;
		}

		hOldBitmap=(HBITMAP)SelectObject(hMemDC,hBitmap);
		if(!hOldBitmap)
		{
			DeleteObject( hBitmap ) ;
			return NULL ;
		}

		BitBlt(hMemDC,0,0,nWidth,nHeight,hScrDC,0,0,SRCCOPY) ;
		hBitmap=(HBITMAP)SelectObject(hMemDC,hOldBitmap) ;

		::DeleteDC(hMemDC);  

		return hBitmap; 
	}

	static HRESULT SaveIcon(HICON hIcon, const wchar_t* path)
	{
		// Create the IPicture intrface
		PICTDESC desc = { sizeof(PICTDESC) };
		desc.picType = PICTYPE_ICON;
		desc.icon.hicon = hIcon;

		IPicture* pPicture = 0;
		HRESULT hr = OleCreatePictureIndirect(&desc, IID_IPicture, FALSE, (void**)&pPicture);
		if (FAILED(hr)) return hr;
   
		// Create a stream and save the image
		IStream* pStream = 0;
		CreateStreamOnHGlobal(0, TRUE, &pStream);
		LONG cbSize = 0;
		hr = pPicture->SaveAsFile(pStream, TRUE, &cbSize);
    
		// Write the stream content to the file
		if (!FAILED(hr)) 
		{
			HGLOBAL hBuf = 0;
			GetHGlobalFromStream(pStream, &hBuf);
			void* buffer = GlobalLock(hBuf);
			HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
			if (!hFile) 
				hr = HRESULT_FROM_WIN32(GetLastError());
			else 
			{
				DWORD written = 0;
				WriteFile(hFile, buffer, cbSize, &written, 0);
				CloseHandle(hFile);
			}
			GlobalUnlock(buffer);
		}

		// Cleanup
		pStream->Release();
		pPicture->Release();

		return hr;
	}

	static BOOL CreateIcon(Gdiplus::Bitmap* pBitmap, LPCTSTR lpPath)
	{
		HICON hIcon=NULL;

		if(Gdiplus::Ok == pBitmap->GetHICON(&hIcon))
		{
			return ( S_OK == SaveIcon( hIcon, lpPath) ) ;

		}
		
		return FALSE;
	}
};
#endif
