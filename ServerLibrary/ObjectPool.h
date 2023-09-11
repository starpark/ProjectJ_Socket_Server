#pragma once

template <typename ObjectType>
struct SlistItem : public SLIST_ENTRY
{
	ObjectType item_;
};


template <typename ObjectType>
struct SlistHeaderWrapper
{
public:
	SlistHeaderWrapper()
	{
		InitializeSListHead(&header_);
	}

	~SlistHeaderWrapper()
	{
		while (auto object = InterlockedPopEntrySList(&header_))
		{
			_aligned_free(object);
		}
	}

	PSLIST_HEADER operator&() { return &header_; }

	SLIST_HEADER header_;
};

template <typename ObjectType>
class ObjectPool
{
public: // 외부 사용
	template <typename... Args>
	static shared_ptr<ObjectType> MakeShared(Args&&... args)
	{
		return shared_ptr<ObjectType>{Pop(forward<Args>(args)...), ObjectDeleter};
	}

private:
	template <typename... Args>
	static ObjectType* Pop(Args&&... args)
	{
		auto object = InterlockedPopEntrySList(&header_);

		if (object == nullptr)
		{
			object = static_cast<PSLIST_ENTRY>(_aligned_malloc(sizeof(SlistItem<ObjectType>),
			                                                   MEMORY_ALLOCATION_ALIGNMENT));
		}

		++object;
		new(object)ObjectType(forward<Args>(args)...);
		return reinterpret_cast<ObjectType*>(object);
	}

	static void ObjectDeleter(ObjectType* object)
	{
		PSLIST_ENTRY entry = reinterpret_cast<PSLIST_ENTRY>(object) - 1;
		InterlockedPushEntrySList(&header_, entry);
	}

private:
	static SlistHeaderWrapper<ObjectType> header_;
	static int allocSize_;
};

template <typename ObjectType>
SlistHeaderWrapper<ObjectType> ObjectPool<ObjectType>::header_{};

template <typename ObjectType>
int ObjectPool<ObjectType>::allocSize_ = sizeof(SlistItem<ObjectType>);
