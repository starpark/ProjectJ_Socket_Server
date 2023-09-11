#include "pch.h"
#include "Iocp.h"


OverlappedEx::OverlappedEx(EventType type) : eventType_(type)
{
	Init();
}

void OverlappedEx::Init()
{
	hEvent = nullptr;
	Internal = 0;
	InternalHigh = 0;
	Offset = 0;
	OffsetHigh = 0;
}

IocpMain::IocpMain()
{
	iocpHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
}

IocpMain::~IocpMain()
{
	CloseHandle(iocpHandle_);
}

bool IocpMain::RegisterHandle(HANDLE handle)
{
	return CreateIoCompletionPort(handle, iocpHandle_, 0, 0);
}

void IocpMain::WorkThread(unsigned int timeout)
{
	while (true)
	{
		DWORD numOfBytes = 0;
		ULONG_PTR key = 0;
		OverlappedEx* overlapped = nullptr;

		if (GetQueuedCompletionStatus(iocpHandle_, &numOfBytes, &key, reinterpret_cast<LPOVERLAPPED*>(&overlapped),
		                              timeout))
		{
			shared_ptr<IocpObject> iocpObject = overlapped->owner_;
			iocpObject->ProcessCompletePort(overlapped, numOfBytes);
		}
		else
		{
			int errorCode = WSAGetLastError();
			if (errorCode != WAIT_TIMEOUT)
			{
				// 비 정상적인 연결 끊김
				shared_ptr<IocpObject> iocpObject = overlapped->owner_;
				iocpObject->ProcessCompletePort(overlapped, numOfBytes);
			}
		}
	}
}
