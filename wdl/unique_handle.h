// unique_handle.h
// RAII wrapper class for raw Windows handles that 
// ensure automatic, safe resource management.

#pragma once

#include <windows.h>
#include <winhttp.h>

#include "debug.h"

namespace wdl
{
    // unique_handle
    //
    // Resource management wrapper for raw Windows
    // handle values; semantics similar to C++ std::unique_ptr

    template <typename Traits>
    class unique_handle
    {
        using pointer = typename Traits::pointer;

    public:
        explicit unique_handle(pointer value = Traits::invalid()) noexcept
            : m_value(value)
        {
        }

        ~unique_handle() noexcept
        {
            close();
        }

        unique_handle(unique_handle const& other) = delete;

        auto operator=(unique_handle const& rhs)->unique_handle & = delete;

        unique_handle(unique_handle&& other) noexcept
            : m_value{ other.release() }
        {
        }

        unique_handle& operator=(unique_handle&& rhs) noexcept
        {
            if (this != &rhs)
            {
                reset(rhs.release());
            }

            return *this;
        }

        explicit operator bool() const noexcept
        {
            return m_value != Traits::invalid();
        }

        pointer* operator& () noexcept
        {
            close();
            m_value = Traits::invalid();
            return &m_value;
        }

        pointer get() const noexcept
        {
            return m_value;
        }

        pointer* put() noexcept
        {
            close();
            m_value = Traits::invalid();
            return &m_value;
        }

        pointer release() noexcept
        {
            auto value = m_value;
            m_value = Traits::invalid();
            return value;
        }

        bool reset(pointer value = Traits::invalid()) noexcept
        {
            if (m_value != value)
            {
                close();
                m_value = value;
            }

            return static_cast<bool>(*this);
        }

        pointer* addressof() const noexcept
        {
            return &m_value;
        }

    private:
        pointer m_value;

        auto close() noexcept -> void
        {
            if (*this)
            {
                Traits::close(m_value);
            }
        }
    };

    // null_handle_traits
    // 
    // Traits definition appropriate for raw Windows handles
    // for which the return value on initilization failure 
    // is a null value (i.e. NULL or nullptr)

    struct null_handle_traits
    {
        using pointer = HANDLE;

        constexpr static pointer invalid() noexcept
        {
            return nullptr;
        }

        static void close(pointer value) noexcept
        {
            ::CloseHandle(value);
        }
    };

    // invalid_handle_traits
    //
    // Traits definition appropriate for raw Windows handles
    // for which the return value on initilization failure 
    // is a Windows constant #define INVALID_HANDLE_VALUE

    struct invalid_handle_traits
    {
        using pointer = HANDLE;

        constexpr static pointer invalid() noexcept
        {
            return INVALID_HANDLE_VALUE;
        }

        static void close(pointer value) noexcept
        {
            ::CloseHandle(value);
        }
    };

    // registry_handle_traits
    //
    // Traits definition appropriate for Windows
    // handles returned by Registry API calls.

    struct registry_handle_traits
    {
        using pointer = HKEY;

        constexpr static pointer invalid() noexcept
        {
            return nullptr;
        }

        static void close(pointer value) noexcept
        {
            ::RegCloseKey(value);
        }
    };

    // winhttp_traits
    //
    // Traits definition appropriate for use
    // with the WinHTTP API.

    struct winhttp_handle_traits
    {
        using pointer = HINTERNET;

        constexpr static pointer invalid() noexcept
        {
            return nullptr;
        }

        static void close(pointer value) noexcept
        {
            VERIFY(::WinHttpCloseHandle(value));
        }
    };

    // pool_handle_traits
    //
    // Traits definition appropriate for use
    // with Windows thread pool object

    struct pool_handle_traits
    {
        using pointer = PTP_POOL;

        constexpr static pointer invalid() noexcept
        {
            return nullptr;
        }

        static void close(pointer value) noexcept
        {
            ::CloseThreadpool(value);
        }
    };

    // work_handle_traits
    //
    // Traits definition appropriate for use
    // with Windows thread pool work object.

    struct work_handle_traits
    {
        using pointer = PTP_WORK;

        constexpr static pointer invalid() noexcept
        {
            return nullptr;
        }

        static void close(pointer value) noexcept
        {
            ::CloseThreadpoolWork(value);
        }
    };

    using null_handle    = unique_handle<null_handle_traits>;
    using invalid_handle = unique_handle<invalid_handle_traits>;
    using reg_handle     = unique_handle<registry_handle_traits>;
    using winhttp_handle = unique_handle<winhttp_handle_traits>;
    using pool_handle    = unique_handle<pool_handle_traits>;
    using work_handle    = unique_handle<work_handle_traits>;
}
