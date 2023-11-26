#include "pch.h"
#include "TimerTask.h"


TimerTaskManager::~TimerTaskManager()
{
}

UINT32 TimerTaskManager::GetThreadId(UINT64 state)
{
	return static_cast<UINT32>(state >> 32);
}

void TimerTaskManager::RemoveTimer(TimerHandle handle)
{
	if (TimerElementPtr element = handle.lock())
	{
		UINT64 prevState = element->state.fetch_or(RemovedState, std::memory_order_acquire);
		int executingThreadId = GetThreadId(prevState);

		while (executingThreadId != 0 && LThreadID != executingThreadId)
		{
			this_thread::yield();
			executingThreadId = GetThreadId(element->state.load(std::memory_order_relaxed));
		}
	}
}

void TimerTaskManager::Distribute()
{
	if (distributing_.exchange(true, memory_order_acquire) == true)
	{
		return;
	}

	const UINT64 currentThreadId = static_cast<UINT64>(LThreadID) << 32;

	auto PumpAddedElementsQueue = [this]()
	{
		while (TimerElementPtr addedElement = addedElements_.Pop())
		{
			UINT64 state = addedElement->state.load(std::memory_order_relaxed);
			if (GetThreadId(state) == 0 && state == DefaultState)
			{
				elements_.push_back(move(addedElement));
			}
		}
	};

	PumpAddedElementsQueue();

	if (elements_.empty())
	{
		distributing_.store(false);
		return;
	}

	currentTick = GetTickCount64();

	vector<TimerElementPtr> timerElements;
	int elementIdx = 0;

	do
	{
		for (; elementIdx < elements_.size(); ++elementIdx)
		{
			TimerElementPtr& element = elements_[elementIdx];
			UINT64 prevState = element->state.fetch_add(currentThreadId, std::memory_order_acquire);

			ASSERT_CRASH(GetThreadId(prevState) == 0);

			auto ClearExecutionFlag = [currentThreadId](TimerElementPtr& element)
			{
				return static_cast<UINT8>(element->state.fetch_sub(currentThreadId, std::memory_order_release) - currentThreadId);
			};

			if (prevState == RemovedState)
			{
				ClearExecutionFlag(element);

				continue;
			}

			if (element->executeTick > currentTick)
			{
				ClearExecutionFlag(element);
				timerElements.push_back(move(element));

				continue;
			}

			if (auto object = element->taskData->object.lock())
			{
				object->Push(element->taskData->task, true);

				prevState = ClearExecutionFlag(element);
				if (prevState != RemovedState && element->isLoop == true)
				{
					ASSERT_CRASH(prevState == DefaultState);
					element->executeTick = currentTick + element->delayTick;
					timerElements.push_back(move(element));
				}
			}
			else
			{
				prevState = ClearExecutionFlag(element);
				ASSERT_CRASH(prevState == DefaultState || prevState == RemovedState);
			}
		}
		PumpAddedElementsQueue();
	}
	while (elementIdx < elements_.size());

	elements_.clear();
	elements_ = move(timerElements);

	distributing_.store(false, memory_order_release);
}
