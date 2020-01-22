// condition_variable.h
// Windows Condition Variable wrapper.

#include <windows.h>

#include "debug.h"
#include "slim_lock.h"
#include "critical_section.h"

namespace wdl
{
	// condition_variable
	// Windows Condition Variable wrapper.

	class condition_variable
	{
	public:
		condition_variable() noexcept
			: m_cv{ CONDITION_VARIABLE_INIT }
		{}

		condition_variable(const condition_variable& other)          = delete;
		condition_variable(condition_variable&& other)               = delete;
		condition_variable& operator=(const condition_variable& rhs) = delete;
		condition_variable& operator=(condition_variable&& rhs)      = delete;

		template <typename T>
		void wait_while(slim_lock& lock, T predicate) noexcept
		{
			while (predicate())
			{
				VERIFY(::SleepConditionVariableSRW(
					&m_cv,
					lock.native_handle(),
					INFINITE,
					0
				));
			}
		}

		template <typename T>
		void wait_while(critical_section& lock, T predicate) noexcept
		{
			while (predicate())
			{
				VERIFY(
					::SleepConditionVariableCS(
						&m_cv,
						lock.native_handle(),
						INFINITE
				));
			}
		}

		void wake_one() noexcept
		{
			::WakeConditionVariable(&m_cv);

			SleepCondition
		}

		void wake_all() noexcept
		{
			::WakeAllConditionVariable(&m_cv);
		}

	private:
		CONDITION_VARIABLE m_cv;
	};
}