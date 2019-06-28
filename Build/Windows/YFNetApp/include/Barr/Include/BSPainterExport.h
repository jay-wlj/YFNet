#ifndef BS_PAINTER_EXPORT_H_
#define BS_PAINTER_EXPORT_H_

//#define LAZY_DLL_EXPORT

#ifdef LAZY_DLL_EXPORT /*happyhuang x*/
#define LAZY_WIN32_DLL_API __declspec(dllexport)
#else
#define LAZY_WIN32_DLL_API __declspec(dllimport)
#endif
#include "IBulletSubtitlePainter.h"

typedef LAZY_WIN32_DLL_API HRESULT (InitBulletSubtilePainter)(IBulletSubtitlePainter ** ppIBulletSubtitlePainter);
typedef LAZY_WIN32_DLL_API HRESULT	(DeinitBulletSubtilePainter)(IBulletSubtitlePainter ** ppIBulletSubtitlePainter);
#endif