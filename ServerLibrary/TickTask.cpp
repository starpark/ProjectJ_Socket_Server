#include "pch.h"
#include "TickTask.h"
#include "CommandTask.h"

TickTask::TickTask()
{
}

TickTask::~TickTask()
{
}

void TickTaskManager::DoTask(UINT64 currentTick)
{
	vector<TickItem> items;
	{
		WRITE_LOCK;
		while (tickTaskQueue_.empty() == false)
		{
			const TickItem& tickItem = tickTaskQueue_.top();
			if (tickItem.executeTick_ > currentTick)
			{
				break;
			}

			items.push_back(tickItem);
			tickTaskQueue_.pop();
		}
	}

	for (auto item : items)
	{
		if (auto taskPtr = item.task_.lock())
		{
			if (FindPendingRemove(taskPtr))
			{
				continue;
			}

			UINT64 nowTick = GetTickCount64();
			UINT64 elapsedTick = nowTick - item.lastTick_;

			if (elapsedTick > 0)
			{
				double deltaSeconds = static_cast<double>(elapsedTick) / 1000;
				taskPtr->Tick(deltaSeconds);

				WRITE_LOCK;
				tickTaskQueue_.push(TickItem{nowTick, nowTick + NEXT_TICK, taskPtr});
			}
			else
			{
				WRITE_LOCK;
				tickTaskQueue_.push(item);
			}
		}
	}
}

void TickTaskManager::AddTask(const shared_ptr<TickTask>& task)
{
	const UINT64 lastTick = GetTickCount64();
	const UINT64 executeTick = lastTick + NEXT_TICK;

	WRITE_LOCK;
	tickTaskQueue_.push(TickItem{lastTick, executeTick, task});
	workingTasks_.insert(task);
}

void TickTaskManager::RemoveTask(const shared_ptr<TickTask>& task)
{
	WRITE_LOCK;
	if (workingTasks_.find(task) != workingTasks_.end())
	{
		workingTasks_.erase(task);
	}
}

bool TickTaskManager::FindPendingRemove(shared_ptr<TickTask> task)
{
	WRITE_LOCK;
	if (workingTasks_.find(task) == workingTasks_.end())
	{
		return true;
	}

	return false;
}
