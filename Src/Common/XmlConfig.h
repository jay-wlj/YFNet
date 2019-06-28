#ifndef COMMON_XMLCONFIG_H
#define COMMON_XMLCONFIG_H

#include "..\include\Xmlparse.h"
#include "..\share\QhtpEn_De.h"
#include "..\include\mytool.h"
#include "..\Common\File.h"

#ifdef _DEBUG_XMLCONFIG_UPDATER
#include "..\include\Console.h"
static ZConsole con1;
#define XmlConfigDebugOutString(str) do{con1 << str;}while(0)
#else
#define XmlConfigDebugOutString(str) 
#endif

namespace XMLConfig
{
	enum CipherType
	{
		Cipher_null,	// 未加密
		Cipher_Common,	
		Cipher_Qhtp,
	};
	class Tool
	{
	public:
		static BOOL LoadXml(CXMLDoc& doc, 
						    CString xmlFile, 
							CipherType cipherType = Cipher_null)
		{
			if(Cipher_Common == cipherType)
			{
				UINT fileSize = En_DeFile::GetFileSize(xmlFile)+1;
				if(fileSize > 1)
				{
					BYTE *buf = new BYTE[fileSize];
					ZeroMemory(buf,fileSize);
					UINT nBufInSize;
					if(En_DeFile::DecodeFile(xmlFile, buf, &nBufInSize))
					{
						if(doc.LoadXml_((TCHAR*)buf))
						{
							delete[]buf;
							return TRUE;
						}
					}
					delete[]buf;
				}
			}
			else if(Cipher_Qhtp == cipherType)
			{
				CQhtpEn_De decode;
				UINT nBufOutSize = 0;
				UINT byteSize = En_DeFile::GetFileSize(xmlFile)*2;
				void *buf = new char[byteSize];
				ZeroMemory(buf, byteSize);
				if(decode.DecodeFile(xmlFile, (char*)buf, &nBufOutSize))
				{
					if(doc.LoadXml_((char*)buf))
					{
						delete[]buf;
						return TRUE;
					}
				}
				delete[]buf;
			}
			else if(doc.Load_(xmlFile))
			{
				return TRUE;
			}
			return FALSE;
		}

		static BOOL SaveXml(CXMLDoc& doc, 
							CString xmlFile, 
							CipherType cipherType = Cipher_null)
		{
			if(Cipher_null == cipherType)
			{
				return doc.Save_(xmlFile);
			}
			else
			{
				CString content = doc.GetXml_();
				UINT nSize = content.GetLength()*sizeof(TCHAR);
				char* buf = new char[nSize+1];
				memcpy(buf,content,nSize);
				buf[nSize] = 0;
				if(En_DeFile::EncodeFile(xmlFile,buf,nSize))
				{
					delete []buf;
					return TRUE;
				}
				delete []buf;
			}
			return FALSE;
		}
	};

	class Updater
	{
	public:
		Updater(CString localFile,
				CString url, 
				int	updateInterval = 1,
				BOOL isLocalFileCipher = FALSE,
				BOOL isServerCipher = FALSE
				)
		{
			m_url = url;
			m_localFile = localFile;
			m_nUpdateInterval = updateInterval;
			m_bIsLocalFileCipher = isLocalFileCipher;
			m_bIsServerCipher = isServerCipher;
			m_tmpFile.Format(_T("%s_%s"),QFile::Tool::RemoveExtension(localFile), QFile::Tool::GetExtension(localFile));
		}
		BOOL Update()
		{
			XmlConfigDebugOutString(_T("Updater>>>>File:"));
			XmlConfigDebugOutString(m_localFile);
			XmlConfigDebugOutString(_T("\r\n"));
			if(IsNeedUpdate() && DownloadTmp())
			{				
				UpdateLocalFile();
				return TRUE;
			}
			return FALSE;
		}
	protected:
		virtual BOOL IsNeedUpdate()
		{
			CXMLDoc doc;
			if(!doc.Init_() || !Tool::LoadXml(doc,m_localFile,m_bIsLocalFileCipher?Cipher_Common:Cipher_null))
			{
				doc.Clear_();
				XmlConfigDebugOutString(_T("Updater>>>>NeedUpdate\r\n"));
				return TRUE;
			}
			CXMLElement xmlRoot;
			doc.GetDocumentElement_(xmlRoot);
			CString str = xmlRoot.GetAttribute_(_T("stamp"));
			if(str.GetLength() > 0)
			{	 
				SYSTEMTIME lastTime, nowTime;
				GetLocalTime(&nowTime);
				MyTool::Str2DateTime(str,lastTime);
				if(MyTool::DaysBetween(nowTime, lastTime) <= m_nUpdateInterval)
				{
					doc.Clear_();
					XmlConfigDebugOutString(_T("Updater>>>>Not NeedUpdate\r\n"));
					return FALSE;
				}
			}
			doc.Clear_();
			XmlConfigDebugOutString(_T("Updater>>>>NeedUpdate\r\n"));
			return TRUE;
		}
		CString GetLastModify()
		{
			CString lastModify; 
			CXMLDoc doc;
			if(doc.Init_() && Tool::LoadXml(doc,m_localFile,m_bIsLocalFileCipher?Cipher_Common:Cipher_null))
			{
				CXMLElement xmlRoot;
				doc.GetDocumentElement_(xmlRoot);
				lastModify = xmlRoot.GetAttribute_(_T("LastModified"));
			}
			doc.Clear_();
			XmlConfigDebugOutString("Updater>>>>LastModify:");
			XmlConfigDebugOutString(lastModify);
			XmlConfigDebugOutString("\r\n");
			return lastModify;
		}
		BOOL DownloadTmp()
		{
			net::HttpDownloader downloader(m_url,GetLastModify());
			if(downloader.DownloadFile(m_tmpFile))
			{   
				XmlConfigDebugOutString(_T("Updater>>>>DownloadTmp|**************Suceessr\n"));
				m_lastModifyFromServer = downloader.GetLastModify();
				XmlConfigDebugOutString("Updater>>>>ServerLastModify:");
				XmlConfigDebugOutString(m_lastModifyFromServer);
				XmlConfigDebugOutString("\r\n");
				return TRUE;
			}
			XmlConfigDebugOutString(_T("Updater>>>>DownloadTmp|*****************Failedr\n"));
			return FALSE;

		}
		void UpdateLocalFile()
		{
			XmlConfigDebugOutString(_T("Updater>>>>UpdateLocalFile\r\n"));
			SYSTEMTIME nowTime;
			GetLocalTime(&nowTime);
			if(!QFile::Tool::IsFileExist(m_tmpFile))
			{
				CXMLDoc doc;
				if(!doc.Init_() || !Tool::LoadXml(doc,m_localFile,m_bIsLocalFileCipher?Cipher_Common:Cipher_null))
				{
					doc.Clear_();
					return;
				}

				CXMLElement xmlRoot;
				doc.GetDocumentElement_(xmlRoot);
				xmlRoot.SetAttribute_(_T("stamp"),MyTool::DateTime2Str(nowTime));
				Tool::SaveXml(doc,m_localFile, m_bIsLocalFileCipher?Cipher_Common:Cipher_null);
				doc.Clear_();
			}
			else
			{
				CXMLDoc doc;
				if(!doc.Init_() || !Tool::LoadXml(doc,m_tmpFile,m_bIsServerCipher?Cipher_Qhtp:Cipher_null))
				{
					doc.Clear_();
					return;
				}
				CXMLElement xmlRoot;
				doc.GetDocumentElement_(xmlRoot);
				xmlRoot.SetAttribute_(_T("stamp"),MyTool::DateTime2Str(nowTime));
				xmlRoot.SetAttribute_(_T("LastModified"),m_lastModifyFromServer);
				Tool::SaveXml(doc,m_localFile, m_bIsLocalFileCipher?Cipher_Common:Cipher_null);
				doc.Clear_();
				QFile::Tool::DeleteFile(m_tmpFile);
			}
		}
	protected:
		CString m_url;
		CString m_localFile;
		CString m_tmpFile;
		CString m_lastModifyFromServer;
		BOOL m_bIsLocalFileCipher;
		BOOL m_bIsServerCipher;
		int	m_nUpdateInterval;   // 单位：天
	};
	class UpdaterEveryday : public Updater
	{
	public:
		UpdaterEveryday(CString localFile,
			            CString url, 
			            int updateInterval = 1,
			            BOOL isLocalFileCipher = FALSE,
			            BOOL isServerCipher = FALSE
			           ): Updater(localFile,url,updateInterval,isLocalFileCipher,isServerCipher)
		{

		}

	protected:
		virtual BOOL IsNeedUpdate()
		{
			CXMLDoc doc;
			if(!doc.Init_() || !Tool::LoadXml(doc,m_localFile,m_bIsLocalFileCipher?Cipher_Common:Cipher_null))
			{
				doc.Clear_();
				XmlConfigDebugOutString(_T("Updater>>>>NeedUpdate\r\n"));
				return TRUE;
			}
			CXMLElement xmlRoot;
			doc.GetDocumentElement_(xmlRoot);
			CString str = xmlRoot.GetAttribute_(_T("stamp"));
			if(str.GetLength() > 0)
			{	 
				SYSTEMTIME lastTime, nowTime;
				GetLocalTime(&nowTime);
				MyTool::Str2DateTime(str,lastTime);
				if(IsSameDay(nowTime, lastTime))
				{
					doc.Clear_();
					XmlConfigDebugOutString(_T("Updater>>>>Not NeedUpdate\r\n"));
					return FALSE;
				}
			}
			doc.Clear_();
			XmlConfigDebugOutString(_T("Updater>>>>NeedUpdate\r\n"));
			return TRUE;
		}
		BOOL IsSameDay(const SYSTEMTIME &sysTime1, const SYSTEMTIME &sysTime2)
		{
			if(sysTime1.wYear == sysTime2.wYear
				&& sysTime1.wMonth == sysTime2.wMonth
				&& sysTime1.wDay == sysTime2.wDay)
			{
				return TRUE;
			}
			return FALSE;
		}
	};
}

#endif