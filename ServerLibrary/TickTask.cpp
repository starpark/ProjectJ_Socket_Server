#include "pch.h"
#include "TickTask.h"

TickTask::TickTask()
{
}

TickTask::~TickTask()
{
}

void TickTaskManager::DoTask()
{
	QueryPerformanceCounter(&LTickCounts.currentTick_);
	QueryPerformanceFrequency(&LTickCounts.frequency_);
	deltaSeconds_ = (LTickCounts.currentTick_.QuadPart - LTickCounts.lastTick_.QuadPart) / static_cast<double>(
		LTickCounts.frequency_.QuadPart);

	vector<weak_ptr<TickTask>> tasks;
	{
		WRITE_LOCK;
		while (!tickTaskQueue_.empty())
		{
			auto weakPtr = tickTaskQueue_.front();
			tickTaskQueue_.pop();

			tasks.push_back(weakPtr);
		}
	}

	for (auto task : tasks)
	{
		if (auto taskPtr = task.lock())
		{
			taskPtr->Tick(deltaSeconds_);
		}
	}

	{
		WRITE_LOCK;
		for (auto task : tasks)
		{
			if (task.expired() == false)
			{
				tickTaskQueue_.push(task);
			}
		}
	}

	LTickCounts.lastTick_ = LTickCounts.currentTick_;
}

void TickTaskManager::AddTask(const shared_ptr<TickTask>& task)
{
	WRITE_LOCK;
	tickTaskQueue_.push(task);
}
