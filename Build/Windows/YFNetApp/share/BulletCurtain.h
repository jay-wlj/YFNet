#pragma once

#include <string>
#include "../include/mscrypto.h"
#include "../include/json/json.h"
#include "../include/IBarrageSubtitle.h"
#ifdef _DEBUG
#pragma comment(lib,"../../lib/json_mtd_d.lib")
#else
#pragma comment(lib,"../../lib/json_mtd.lib")
#endif

namespace bullte_curtain
{
    const WORD          BC_CMD_Port             = 8031;
    const char * const  BC_CMD_DNS              = "barr.upload.kbimg.cn";

#ifndef ASSERT
#define ASSERT(expr) _ASSERTE(expr)
#endif

#pragma pack(push)
#pragma pack(1)
    // tag length value结构体。
    typedef struct atom_item_t
    {
        WORD tag;	//类型
        WORD len;	//长度。(value的长度)
        union
        {
            BYTE u8;
            WORD u16;
            DWORD u32;
            UINT64 u64;
            char str[1];
            unsigned char ustr[1];
        }v;
    }atom_item_t;
#pragma pack(pop)
#define atom_len(item) (sizeof(item->tag)+sizeof(item->len)+item->len)

    struct BCItem
    {
        CString text;
        DWORD color;
        WORD size;
        BulletStyle style;
        LONGLONG showTime, lifeTime;
        SIZE wh;
        CString imgid;
        CString sid;
        INT64 bcid;
        BCItem(void)
        {
            Empty();
        }
        void Empty(void)
        {
            bcid = 0;
            sid.Empty();
            text.Empty();
            imgid.Empty();
            color = 0;
            size = BULLET_SIZE_MIDDLE;
            style = BULLET_STYLE_ROLL;
            lifeTime = showTime = 0;
            wh.cx = wh.cy = 0;
        }
        bool IsPushDataValid(void)
        {
            return ( text.GetLength() && 0 < showTime && 0 < lifeTime );
        }
        bool IsSendDataValid(void)
        {
            return ( IsPushDataValid() && 0 < wh.cx && 0 < wh.cy );
        }
    };

    class CStreamMgr
    {
    public:
        CStreamMgr(void) : m_pIs(0), m_hMem(0), m_pData(0)
        {
            Init();
        }
        ~CStreamMgr(void)
        {
            Release();
        }
        void Init(void)
        {
            Release();
            if ( m_hMem = GlobalAlloc(GMEM_MOVEABLE, 0) )
            {
                CreateStreamOnHGlobal(m_hMem, TRUE, &m_pIs);
            }
            ASSERT(m_pIs && "Failed to Init IStream");
        }
        operator IStream * (void)
        {
            return m_pIs;
        }
        BYTE * Data(void)
        {
            if ( m_pIs && !m_pData )
            {
                LARGE_INTEGER liBegin = {0};
                m_pIs->Seek(liBegin, STREAM_SEEK_SET, 0);
                m_pData = (BYTE *)GlobalLock(m_hMem);
            }
            return m_pData;
        }
        DWORD Size(void)
        {
            return m_pIs ? GlobalSize(m_hMem) : 0;
        }
        void Release(void)
        {
            if ( m_pIs )
            {
                m_pIs->Release();
                m_pIs = 0;
            }
            if ( m_hMem )
            {
                GlobalUnlock(m_hMem);
                GlobalFree(m_hMem);
                m_hMem = 0;
            }
            m_pData = 0;
        }

    private:
        IStream * m_pIs;
        HGLOBAL m_hMem;
        BYTE * m_pData;
    };

    template<class T>
    class CThisFunToStaticFun
    {
    public:
        // 联合类，用于转换类成员方法指针到普通函数指针（试过编译器不允许在这两种函数之间强制转换），不知道有没有更好的方法。
        union
        {
            DWORD (WINAPI *ThreadProc)(void *);
            DWORD (WINAPI T::*MemberProc)(void);
        }Proc;
    };

	class CBulletCurtain : public CThisFunToStaticFun<CBulletCurtain>
    {
    public:
        //#define DEF_FAMILY		_T("宋体")
        CBulletCurtain(void) : m_Socket(0), m_playMediaCtrl(0), m_hUpLoad(0), m_pBack(0), m_bSwitch(true)
        {
            memset(&m_SockAddrIn, 0, sizeof(m_SockAddrIn));
        }
        ~CBulletCurtain(void)
        {
            if ( m_hUpLoad )
            {
                TerminateThread(m_hUpLoad, 0);
                m_hUpLoad = 0;
            }
            UnInitNetTransferDetails();
        }
		void Init(IQvodCallBack * pmaster, HWND hwnd)
		{
			m_playMediaCtrl = 0;
			m_pBack = pmaster;
            m_hWndHost = hwnd;
			m_playMediaCtrl = GetPlayMedia();
            Switch( m_bSwitch );
		}
        void Switch(bool bOn)
        {
            if ( !m_playMediaCtrl || !IsPlaying() ) return;
			m_playMediaCtrl->Bullet_SetEnable((m_bSwitch = bOn)?TRUE:FALSE);
        }
        void Render(
            LONGLONG llID,
            LPCTSTR szText,
            WORD size,
            BulletStyle style,
            DWORD dwColor,
            LONGLONG llStartTime,
            LONGLONG llEndTime,
            bool bMyOwn = false
            )
        {
            if ( !m_playMediaCtrl || !IsPlaying() ) return;
            BulletText btxt;
            btxt.llID = llID;
            wcsncpy_s( btxt.szText, _countof( btxt.szText ), szText, _TRUNCATE );
            btxt.dwFontSize = size;
            btxt.dwStyle = style;
            btxt.dwColor = dwColor;
            btxt.llStartTime = llStartTime;
            btxt.llEndTime = llEndTime;
            btxt.dwAuthor = bMyOwn ? BULLET_AUTHOR_OWN : BULLET_AUTHOR_OTHERUSER;
			m_playMediaCtrl->Bullet_PutText(&btxt);
        }

        /*
        ** 接口名：Push，服务器推送的弹幕数据
        ** 参数：
        ** jsonString：
        ** {
        **   data:
        **   [
        **     {comment:'xxx',font_color:f8f8f8,film_position:1212122,life_time:1212125,show_type:3,display_position:'U'},
        **     {comment:'yyy',font_color:f8f8f8,film_position:1212122,life_time:1212125,show_type:2}
        **   ]
        ** }
        ** show_type：从右至左-1；从右至左-2；悬停-3
        ** display_position：固定上-U；固定中-M；固定下-L
        */
        void Push(CString jsonString)
        {
            if ( !m_playMediaCtrl || !IsPlaying() ) return;
            PSTR szU8 = Fun::FromUnicode(CP_UTF8, jsonString);
            if ( !szU8 ) return;
            std::vector<BCItem> data;
            DecodeJson( szU8, data );
            for ( std::vector<BCItem>::iterator it = data.begin(); it != data.end(); ++it )
            {
                if ( it->IsPushDataValid() && 0 < it->bcid )
                {
                    DebugCode(con<<__FUNCTION__<<"\n";);
                    Render(it->bcid, it->text, /*DEF_FAMILY, */it->size, it->style, it->color, it->showTime, it->showTime + it->lifeTime);
                }
            }
            delete [] szU8;
        }

        /*
        ** 接口名：Send，当前视频中，用户发送的数据，渲染出来的同时，还需要上传
        ** 参数：
        ** jsonString：
        ** {
        **   data:
        **   [
        **     {comment:'xxx',font_color:f8f8f8,film_position:1212122,life_time:1212125,show_type:3,display_position:'U',width:64,height:64,sid:'H6DqVMPt'},
        **     {comment:'yyy',font_color:f8f8f8,film_position:1212122,life_time:1212125,show_type:2,width:64,height:64,sid:'H6DqVMPt'}
        **   ]
        ** }
        ** show_type：从右至左-1；从右至左-2；悬停-3
        ** display_position：固定上-U；固定中-M；固定下-L
        */
        void Send(CString jsonString)
        {
            DebugCode(con<<__FUNCTION__<<" begin......\n";);
            if ( 4 > jsonString.GetLength() || !m_pBack || m_curUpLoad.IsSendDataValid() || !IsPlaying() )
            {
                return;
            }
            PSTR szU8 = Fun::FromUnicode(CP_UTF8, jsonString);
            if ( !szU8 ) return;
            std::vector<BCItem> data;
            DecodeJson( szU8, data );
            BCItem send;
            if ( 1 == data.size() )
            {
                send = data[0];
            }
            if ( !send.IsSendDataValid() && send.sid.GetLength() )
            {
                m_sid = send.sid;
                DebugCode(con<<__FUNCTION__<<" sid="<<m_sid<<"......\n";);
            }
            else if ( send.IsSendDataValid() && m_sid.GetLength() )
            {
                m_dwMs = 0;
                m_curUpLoad = send;
                m_curSend = jsonString;
                m_imgStream.Init();
                if ( m_imgStream )
                {
                    GetShortcut(m_curUpLoad.wh, m_dwMs);
                }
                Proc.MemberProc = &CBulletCurtain::UpLoad;
                m_hUpLoad = CreateThread(0, 0, Proc.ThreadProc, this, 0, 0);
                DebugCode(con<<__FUNCTION__<<"\n";);
                Render(m_curUpLoad.bcid, m_curUpLoad.text, /*DEF_FAMILY, */m_curUpLoad.size, m_curUpLoad.style, m_curUpLoad.color, m_curUpLoad.showTime, m_curUpLoad.showTime + m_curUpLoad.lifeTime, true);
            }
            delete [] szU8;
            DebugCode(con<<__FUNCTION__<<" end......\n";);
        }
        void PopBCID(CString & p1, CString & p2)
        {
            //if ( m_curUpLoad.imgid.GetLength() )
            {
                p1 = m_curSend;
                p2 = m_curUpLoad.imgid;
            }
            m_curSend.Empty();
            m_curUpLoad.Empty();
            DebugCode(con<<__FUNCTION__<<"\n";);
        }
        CString GetSID(void)
        {
            return m_sid;
        }
        void PostSeek(void)
        {
            if ( !m_playMediaCtrl ) return;
			m_playMediaCtrl->Bullet_RemoveAll();
            ::PostMessage(m_hWndHost, WMU_BC_NOTIFY_WEB_SEEK, 0, 0);
            DebugCode(con<<__FUNCTION__<<"RemoveAllBulletText\n";);
        }
    private:
        PlayCtrl * GetPlayMedia(void)
        {
            PlayCtrl * ppc = 0;
            if ( m_pBack )
            {
                ppc = ( PlayCtrl * )m_pBack->EventNotify_(QCM_BC_GET_PM_INTERFACE);
            }
            return ppc;
        }

        bool IsPlaying(void)
        {
            PlayCtrl * ppc = GetPlayMedia();
            if ( !ppc ) return false;
            e_PlayState s = _PS_Init;
            ppc->GetPlayState(&s);
            return _PS_Stop < s;
        }
        // 解析域名和初始Socket
        bool PreWork(void)
        {
            // 解析host
            SOCKADDR_IN sockAddrIn = {0};
            if ( !memcmp( &sockAddrIn, &m_SockAddrIn, sizeof(SOCKADDR_IN) ) && !GetHostSockAddr() )
            {
                DebugCode(::MessageBox(0, _T("UpLoad: GetHostSockAddr Failed!"), _T("Error!"), 0););
                return false;
            }

            // 初始网络
            UnInitNetTransferDetails();
            if ( !InitNetTransferDetails() )
            {
                DebugCode(::MessageBox(0, _T("UpLoad: InitNetTransferDetails Failed!"), _T("Error!"), 0););
                return false;
            }
            return true;
        }

        // 服务器信息-最好不要放到主线程调用
        bool GetHostSockAddr(void)
        {
            struct hostent *phost = gethostbyname(BC_CMD_DNS);
            if ( !phost || ( AF_INET != phost->h_addrtype ) )
            {
                return false;
            }

            m_SockAddrIn.sin_family = AF_INET;
            m_SockAddrIn.sin_port = htons(BC_CMD_Port);
            struct in_addr *addr = (struct in_addr *)phost->h_addr_list[0];
            m_SockAddrIn.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*addr));
            return true;
        }

        // 初始网络传输相关的一些资源
        bool InitNetTransferDetails(void)
        {
            int timeout = 5000;
            if ( INVALID_SOCKET == ( m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) )
                || SOCKET_ERROR == setsockopt( m_Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(int) )
                || SOCKET_ERROR == connect( m_Socket, (sockaddr *)&m_SockAddrIn, sizeof(SOCKADDR_IN) ) )
            {
                closesocket(m_Socket);
                m_Socket = 0;
                return false;
            }
            return true;
        }

        // 卸载网络传输相关的一些资源
        void UnInitNetTransferDetails(void)
        {
            if ( m_Socket )
            {
                closesocket(m_Socket);
                m_Socket = 0;
            }
        }

        void Add2Package(string & qbuf, void* data, DWORD len)
        {
            if ( !len ) return;
            qbuf.append( (const char *)data, len );
        }

        void DecodeJson(PCSTR szU8, std::vector<BCItem> & data)
        {
            BCItem item;
            Json::Reader read;
            Json::Value root, dataNode;
            if ( !read.parse( szU8, root, false ) ) return;
            dataNode = root["data"];
            for ( int i = 0, nCount = dataNode.size(); i < nCount; ++i )
            {
                string utf8 = dataNode[i]["nid"].asString();
                item.bcid = _strtoi64(utf8.c_str(), 0, 10);
                utf8 = dataNode[i]["comment"].asString();
                item.text = Fun::Utf8ToUnicode( utf8 );
                utf8 = dataNode[i]["font_color"].asString();
                item.color = 0xff000000 + strtoul(utf8.c_str(), 0, 16);
                utf8 = dataNode[i]["font_size"].asString();
                if ( !utf8.compare("S") ) item.size = BULLET_SIZE_SMALL;
                else if ( !utf8.compare("M") ) item.size = BULLET_SIZE_MIDDLE;
                else if ( !utf8.compare("L") ) item.size = BULLET_SIZE_LARGE;
                utf8 = dataNode[i]["film_position"].asString();
                item.showTime = 10000 * ((LONGLONG)strtoul(utf8.c_str(), 0, 10));
                utf8 = dataNode[i]["life_time"].asString();
                item.lifeTime = 10000 * ((LONGLONG)strtoul(utf8.c_str(), 0, 10));
                string stype = dataNode[i]["show_type"].asString();
                utf8 = dataNode[i]["display_position"].asString();
                if ( !stype.compare("1") )			item.style = BULLET_STYLE_ROLL;
                else if ( !stype.compare("2") )		item.style = BULLET_STYLE_REVERSE_ROLL;
                else if ( !stype.compare("3") )
                {
                    if ( !utf8.compare("U") )		item.style = BULLET_STYLE_TOP;
                    else if ( !utf8.compare("M") )	item.style = BULLET_STYLE_MIDDLE;
                    else if ( !utf8.compare("L") )	item.style = BULLET_STYLE_BOTTOM;
                }
                utf8 = dataNode[i]["width"].asString();
                item.wh.cx = strtoul(utf8.c_str(), 0, 10);
                utf8 = dataNode[i]["height"].asString();
                item.wh.cy = strtoul(utf8.c_str(), 0, 10);
                utf8 = dataNode[i]["sid"].asString();
                item.sid = CA2CT(utf8.c_str());
                data.push_back(item);
            }
        }
        RECT AdjustImgRatio(const SIZE & szDes, const SIZE & szSrc)
        {
            RECT offset;
            SetRectEmpty( &offset );
            if ( !szDes.cx || !szDes.cy || !szSrc.cx || !szSrc.cy )
            {
                return offset;
            }
            float fixRatio = szDes.cx*1.0f/szDes.cy;
            float imgRatio = szSrc.cx*1.0f/szSrc.cy;
            if ( fixRatio > imgRatio ) // 竖条图片，top和bottom不为0
            {
                offset.top = (szSrc.cy*1.0f-szSrc.cx/fixRatio)/2;
                offset.bottom = szSrc.cy-offset.top;
                offset.right = szSrc.cx;
            }
            else // 横条图片，left和right不为0
            {
                offset.left = (szSrc.cx*1.0f-szSrc.cy*fixRatio)/2;
                offset.right = szSrc.cx-offset.left;
                offset.bottom = szSrc.cy;
            }
            return offset;
        }
        void GetShortcut(const SIZE & sz, DWORD & ms )
        {
            BYTE * pData = 0;
            REFERENCE_TIME nCurTime = 0;
            PlayCtrl * ppc = GetPlayMedia();
            if ( !ppc ) return;
            if ( ppc->GetCurrentImage( &pData, _E_CM_CURRENT, &nCurTime ) )
            {
                ms = nCurTime/10000;
                Bitmap * pdes = new Bitmap( sz.cx, sz.cy );
                Bitmap * psrc = Bitmap::FromBITMAPINFO( (BITMAPINFO*)pData, pData+sizeof(BITMAPINFO) );
                if ( psrc && pdes )
                {
                    Graphics g(pdes);
                    RECT offset = AdjustImgRatio( sz, CSize( psrc->GetWidth(), psrc->GetHeight() ) );
                    g.DrawImage(psrc, Rect(0, 0, sz.cx, sz.cy), offset.left, offset.top, offset.right - offset.left, offset.bottom - offset.top, UnitPixel );
                    CLSID clsid;
                    if ( 0 > GetEncoderClsid( L"image/jpeg", &clsid ) || Ok != pdes->Save( m_imgStream, &clsid ) )
                    {
                        DebugCode(con<<__FUNCTION__<<" Save Failed......\n";);
                    }
                }
                else DebugCode(con<<__FUNCTION__<<" FromBITMAPINFO Failed......\n";);
                SafeDelete(pdes);
                SafeDelete(psrc);
            }
            else DebugCode(con<<__FUNCTION__<<" GetCurrentImage Failed......\n";);
        }
        DWORD FileHash(string & filebuf, char * pHash, WORD len)
        {
            if ( !pHash || 32 > len ) return 0;
            DWORD dwSize = m_imgStream.Size();
            if ( !dwSize ) return 0;
            char md5[61] = {0};
            if ( !CryptTool::GetMD5(m_imgStream.Data(), dwSize, md5) )
                return 0;
            filebuf.append((char *)m_imgStream.Data(), dwSize);
            memcpy(pHash, md5, 32);
            return dwSize;
        }

        bool base16_decode(const char* base16, int length, char* bin)
        {
            if ( length % 2 )
                return false;
            const char BAD = -1;
            const char base16val[128] =
            {
                BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
                BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
                BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
                0,    1,  2,  3,   4,  5,  6,  7,   8,  9,BAD,BAD, BAD,BAD,BAD,BAD,
                BAD, 10, 11, 12,  13, 14, 15,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
                BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
                BAD, 10, 11, 12,  13, 14, 15,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
                BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
            };
#define decode16(ch) ( ( (unsigned char)ch ) < 128 ? base16val[ch] : BAD )

            char ch = 0;
            for ( int i = 0; i < length; i += 2 )
            {
                // 高四位
                if ( BAD == ( ch = decode16(base16[i]) ) )
                    return false; // 非法字符
                bin[i/2] = ch << 4;

                // 低四位
                if ( BAD == ( ch = decode16(base16[i + 1]) ) )
                    return false; // 非法字符
                bin[i/2] |= ch;
            }
            return true;
        }
        bool is_big_endian(void)
        {
            union
            {
                char c;
                short s;
            } u;
            u.s = 1;
            return 0 == u.c;
        }

        WORD reserve_short(WORD val)
        {
            union
            {
                WORD w;
                char c[2];
            } u, r;
            u.w = val;
            r.c[0] = u.c[1];
            r.c[1] = u.c[0];
            return r.w;
        }

        DWORD reserve_ulong(DWORD val)
        {
            union
            {
                DWORD l;
                char c[4];
            } u, r;
            u.l = val;
            r.c[0] = u.c[3];
            r.c[1] = u.c[2];
            r.c[2] = u.c[1];
            r.c[3] = u.c[0];
            return r.l;
        }

        DWORD WINAPI UpLoad(void)
        {
            DebugCode(con<<__FUNCTION__<<" begin......\n";);
            do
            {
                if ( !m_imgStream )
                {
                    DebugCode(con<<__FUNCTION__<<" break1......\n";);
                    break;
                }

                // 预处理
                if ( !PreWork() )
                {
                    DebugCode(con<<__FUNCTION__<<" break2......\n";);
                    break;
                }

                /*
                该文档定义不同系统通过socket上传文件的协议。
                上传生产环境地址：mobile.kuaibo.com:28080
                下载生产环境地址：img.mobile.kuaibo.com
                •修改hosts：183.60.46.144 
                注意事项
                1.客户端每次上传必须打开和关闭套接字，不能共用套接字 
                上传流程
                上传使用两步方式： 1、请求上传 2、发送文件内容 
                1.请求上传，客户端发送如下数据： 
                packet_length + app_id + file_size + file_hash + TLV ... 包头定义：
                packet_length	4字节	包的总长度 
                app_id			2字节	应用ID
                file_size		4字节	文件大小
                file_hash		16字节	文件hash
                TAG				2字节	TLV的TAG
                LENGTH			2字节	TLV的LENGTH

                目前不同应用的取值：
                app_id 应用名称
                1 弹幕截图上传
                2 移动互传图片上传
                */
                string filebuf;
                WORD appid = 1;
                char val[41] = {0};
                DWORD pacLen = 26;
                DWORD fileSize = FileHash(filebuf, val, 33);
                char fileHash[16] = {0};
                if ( !fileSize || !base16_decode(val, 32, fileHash) )
                {
                    DebugCode(con<<__FUNCTION__<<" break3......\n";);
                    break;
                }
                string sendBuf;
                appid = reserve_short(appid);
                DWORD fileSizeB = reserve_ulong(fileSize);

                // sid的tlv
                string sid = CT2CA(m_sid);
                atom_item_t tlv_sid = {1, 8, 0};
                DWORD tlv_sidLen = atom_len((&tlv_sid));
                tlv_sid.tag = reserve_short(tlv_sid.tag);
                tlv_sid.len = reserve_short(tlv_sid.len);
                memcpy(tlv_sid.v.str, sid.c_str(), 8);
                pacLen += tlv_sidLen;

                // file_name的tlv
                val[40] = 0;
                m_pBack->EventNotify_(QCM_BC_GET_HASH, (long)val, 41);
                string fname = val;
                fname += '#';
                fname += ultoa(m_dwMs, val, 10);
                atom_item_t appItem = {0, fname.size(), 0};
                string fname_tlv;
                fname_tlv.append( atom_len( (&appItem) ), 0 );
                atom_item_t* fname_tlv_body = (atom_item_t*)(&fname_tlv[0]);
                DWORD fname_tlvLen = fname_tlv.size();
                fname_tlv_body->tag = reserve_short(2);
                fname_tlv_body->len = reserve_short(appItem.len);
                memcpy(fname_tlv_body->v.str, fname.c_str(), appItem.len);
                pacLen += fname_tlvLen;
                DWORD pacLenB = pacLen;
                pacLenB = reserve_ulong(pacLenB);
                Add2Package(sendBuf, &pacLenB, sizeof(pacLenB));
                Add2Package(sendBuf, &appid, sizeof(appid));
                Add2Package(sendBuf, &fileSizeB, sizeof(fileSizeB));
                Add2Package(sendBuf, &fileHash, sizeof(fileHash));
                Add2Package(sendBuf, &tlv_sid, tlv_sidLen);
                Add2Package(sendBuf, &fname_tlv[0], fname_tlvLen);

                int ret = sendto( m_Socket, sendBuf.data(), pacLen, 0, (struct sockaddr*)&m_SockAddrIn, sizeof(m_SockAddrIn) );
                if ( SOCKET_ERROR == ret )
                {
                    DebugCode(::MessageBox(0, _T("UpLoad: sendto 1st Failed!"), _T("Error!"), 0););
                    break;
                }

                /*
                2.请求完成后，服务器返回2个字节结果result：
                0 允许上传
                1 系统繁忙
                2 认证错误
                3 文件太大
                4 非法参数
                5 文件类型不正确
                6 文件已经存在
                ps：文件已存在，返回文件信息：packet_length + TLV
                */
                WORD rsp2 = 0;
                ret = recv( m_Socket, (char *)&rsp2, sizeof(rsp2), 0 );
                if ( sizeof(rsp2) != ret )
                {
                    DebugCode(::MessageBox(0, _T("UpLoad: recv 2nd Failed!\n"), _T("Error!"), 0););
                    break;
                }
                if ( rsp2 )
                {
                    DebugCode(con<<__FUNCTION__<<" break4|"<<(short)rsp2<<"......\n";);
                    break;
                }

                /*
                3.客户端发送文件数据
                */
                ret = 0;
                int sendCount = 0;
                while ( sendCount < fileSize )
                {
                    ret = sendto( m_Socket, filebuf.data(), fileSize, 0, (struct sockaddr*)&m_SockAddrIn, sizeof(m_SockAddrIn) );
                    if ( !ret || SOCKET_ERROR == ret )
                    {
                        DebugCode(::MessageBox(0, _T("UpLoad: sendto 3th Failed!\n"), _T("Error!"), 0););
                        break;
                    }
                    sendCount += ret;
                }
                if ( sendCount < fileSize )
                {
                    DebugCode(::MessageBox(0, _T("UpLoad: sendto 3th Failed!\n"), _T("Error!"), 0););
                    break;
                }

                /*
                4.文件完全上传完毕后，服务器返回结果：
                packet_length + result + TLV 定义如下：
                package_length	4字节	包的总长度
                result			2字节	上传结果
                TAG				2字节	TLV的TAG
                LENGTH			2字节	TLV的LENGTH

                2个字节result：
                10 上传成功
                11 校验错误
                12 文件系统错误
                */
                char rsp4[39] = {0};
                ret = recv( m_Socket, rsp4, sizeof(rsp4), 0 );
                if ( !ret || SOCKET_ERROR == ret )
                {
                    DebugCode(::MessageBox(0, _T("UpLoad: recv 4th Failed!\n"), _T("Error!"), 0););
                    break;
                }
                *(DWORD *)rsp4 = reserve_ulong(*(DWORD *)rsp4);
                *(WORD *)(rsp4 + 4) = reserve_short(*(WORD *)(rsp4 + 4));
                atom_item_t * imgid_tlv = (atom_item_t *)(rsp4 + 6);
                imgid_tlv->tag = reserve_short(imgid_tlv->tag);
                imgid_tlv->len = reserve_short(imgid_tlv->len);
                if ( !*(DWORD *)rsp4 || 10 != *(WORD *)(rsp4 + 4) )
                {
                    DebugCode(con<<__FUNCTION__<<" break5|"<<(long(*(DWORD *)rsp4))<<"|"<<((short)*(WORD *)(rsp4 + 4))<<"......\n";);
                    break;
                }
                m_curUpLoad.imgid = CA2CT(rsp4 + 10);
            } while ( 0 );
            m_imgStream.Release();
            UnInitNetTransferDetails();
            if ( m_curUpLoad.imgid.IsEmpty() )
            {
                m_curUpLoad.Empty();
                DebugCode(con<<__FUNCTION__<<" Failed......\n";);
            }
            m_pBack->EventNotify_(QCM_BC_SET_UPLOAD_SHORTCUT_OK);
            SafeDelHandle(m_hUpLoad);
            DebugCode(con<<__FUNCTION__<<"|imgid="<<m_curUpLoad.imgid<<" end......\n";);
            return 0;
        }
        SOCKET				m_Socket;
        SOCKADDR_IN			m_SockAddrIn;
        CString				m_sid;
        CString				m_curSend;
        HANDLE				m_hUpLoad;
        BCItem				m_curUpLoad;
        IQvodCallBack *		m_pBack;
		PlayCtrl *			m_playMediaCtrl;
        bool				m_bSwitch;
        DWORD				m_dwMs;
        CStreamMgr			m_imgStream;
        HWND                m_hWndHost;
        //CSysDpi				m_dpi;
    };
}
