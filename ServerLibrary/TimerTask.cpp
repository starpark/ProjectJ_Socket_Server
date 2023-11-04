#include "pch.h"
#include "TimerTask.h"


void TimerTaskManager::Execute()
{
	UINT64 nowTick = GetTickCount64();
	if (nowTick >= LEndTickCount)
	{
		return;
	}

	vector<TimerItem> items;
	{
		WRITE_LOCK;
		while (timerTaskQueue_.empty() == false)
		{
			const TimerItem& timerItem = timerTaskQueue_.top();
			if (timerItem.executeTick_ > nowTick)
			{
				break;
			}

			items.push_back(timerItem);
			timerTaskQueue_.pop();
		}
	}

	for (auto item : items)
	{
		auto taskPtr = item.task_;

		taskPtr->Execute();
	}
}
