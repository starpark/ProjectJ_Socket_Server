#pragma once

#define USE_MANY_LOCKS(count)			Lock locks_[count]
#define USE_LOCK						USE_MANY_LOCKS(1)
#define	READ_LOCK_IDX(idx)				ReadLockGuard readLockGuard_##idx(locks_[idx], typeid(this).name())
#define READ_LOCK						READ_LOCK_IDX(0)
#define	WRITE_LOCK_IDX(idx)				WriteLockGuard writeLockGuard_##idx(locks_[idx], typeid(this).name())
#define WRITE_LOCK						WRITE_LOCK_IDX(0)
#define ADOPT_WRITE_LOCK				WriteLockGuard writeLockGuard_##idx(locks_[idx], typeid(this).name(), adopt_lock_t)
#define USE_MULTIPLE_LOCK				Lock& GetLockRef() {return std::ref(locks_[0]);}
#define GET_CLASS_NAME					typeid(this).name()

#define CRASH(cause)						\
{											\
	uint32_t* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}
