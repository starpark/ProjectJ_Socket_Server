#include "pch.h"
#include "TickTask.h"

TickTask::TickTask()
	: lastTick_(GetTickCount64())
{
}

TickTask::~TickTask()
{
}

void TickTaskManager::DoTask(UINT64 currentTick)
{
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
			UINT64 elapsedTick = currentTick - taskPtr->lastTick_;
			if (elapsedTick > 0)
			{
				double deltaSeconds = elapsedTick / static_cast<double>(1000.0f);

				if (deltaSeconds >= 0.01)
				{
					taskPtr->Tick(deltaSeconds);

					taskPtr->lastTick_ = currentTick;
				}
			}
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
}

void TickTaskManager::AddTask(const shared_ptr<TickTask>& task)
{
	WRITE_LOCK;
	tickTaskQueue_.push(task);
}
