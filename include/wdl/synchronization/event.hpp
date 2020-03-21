// event.hpp
//
// Class Implementation: wdl::synchronization::event
//
// Windows Event object wrapper.

#pragma once

#include <windows.h>

#include <wdl/debug.hpp>
#include <wdl/handle.hpp>

namespace wdl::synchronization
{
	// wdl::synchronization::event_type
	//
	// Windows event type enumeration.

	enum class event_type
	{
		auto_reset,
		manual_reset
	};

	// wdl::synchronization::event
	//
	// WDL wrapper around Windows events implementing RAII.

	class event
	{	
		using null_handle_t = wdl::handle::null_handle;

		null_handle_t m_handle;

	public:
		explicit event(event_type type)
			: m_handle{ ::CreateEvent(nullptr, static_cast<bool>(type), false, nullptr) }
		{}

		// rely on destructor for null_handle to clean up
		~event() = default;

		event(event&)            = delete;
		event& operator=(event&) = delete;

		event(event&& other) noexcept
			: m_handle{ other.m_handle.release() }
		{}

		event& operator=(event&& rhs) noexcept
		{
			if (this != &rhs)
			{
				m_handle = std::move(rhs.m_handle);
			}
			
			return *this;
		}

		void set() noexcept
		{
			VERIFY(::SetEvent(m_handle.get()));
		}

		void reset() noexcept
		{
			VERIFY(::ResetEvent(m_handle.get()));
		}

		bool wait(const unsigned long ms = INFINITE) noexcept
		{
			const auto res = ::WaitForSingleObject(m_handle.get(), ms);

			ASSERT(res == WAIT_OBJECT_0 || res == WAIT_TIMEOUT);

			return res == WAIT_OBJECT_0;
		}

		HANDLE get() const noexcept
		{
			return m_handle.get();
		}

		bool valid() const noexcept
		{
			return m_handle ? true : false;
		}

		explicit operator bool()
		{
			return m_handle ? true : false; 
		}
	};
}