#pragma once

namespace msgcenter
{
	struct Message
	{
		unsigned __int64 ID;
		const char *content;
		int  contentSize;
	};

	interface IBackend;
	interface ICallback;

	IBackend* GetBackend(ICallback* pCallback);

	interface ICallback
	{
		virtual int PushMsg(Message* msg, int msgSize)=0;
	};

	interface IBackend
	{
		virtual void Start(unsigned __int64 userID, const char* sessionID , int sessionIDLen)=0;
		virtual void Stop()=0;
	};
}

