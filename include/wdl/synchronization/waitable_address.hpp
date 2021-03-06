// waitable_address.hpp
//
// Class Implementation: cdl::synchronization::waitable_address
//
// Wrapper class around Windows WaitForAddress() synchronization mechanism.

#pragma once

#include <windows.h>
#include <stdint.h>

namespace wdl::synchronization
{
    namespace detail
    {
        template <size_t addr_size>
        void interlocked_update(
            void* target_address, 
            void* new_value_address
            );

        template<>
        void interlocked_update<sizeof(uint8_t)>(
            void* target_address, 
            void* new_value_address
            )
        {
            InterlockedExchange8(
                static_cast<char*>(target_address), 
                *static_cast<char*>(new_value_address)
                );
        }

        template<>
        void interlocked_update<sizeof(uint16_t)>(
            void* target_address, 
            void* new_value_address
            )
        {
            InterlockedExchange16(
                static_cast<SHORT*>(target_address), 
                *static_cast<SHORT*>(new_value_address)
                );
        }

        template<>
        void interlocked_update<sizeof(uint32_t)>(
            void* target_address, 
            void* new_value_address
            )
        {
            InterlockedExchange(
                static_cast<LONG*>(target_address), 
                *static_cast<LONG*>(new_value_address)
                );
        }

        template<>
        void interlocked_update<sizeof(uint64_t)>(
            void* target_address, 
            void* new_value_address
            )
        {
            InterlockedExchange64(
                static_cast<LONG64*>(target_address), 
                *static_cast<LONG64*>(new_value_address)
                );
        }
    }

    template <typename TargetAddressType>
    class waitable_address
    {
        static_assert(
            (sizeof(TargetAddressType) == 1) |
            (sizeof(TargetAddressType) == 2) |
            (sizeof(TargetAddressType) == 4) |
            (sizeof(TargetAddressType) == 8)
            );

        TargetAddressType* m_target_address;

    public:
        waitable_address(TargetAddressType* target_address)
            : m_target_address{ target_address }
        {}

        // non-copyable
        waitable_address(waitable_address& other)          = delete;
        waitable_address& operator=(waitable_address& rhs) = delete;

        // non-movable
        waitable_address(waitable_address&& other)          = delete;
        waitable_address& operator=(waitable_address&& rhs) = delete;

        void wait_until_not(
            TargetAddressType undesired, 
            unsigned long timeout = INFINITE
            )
        {
            TargetAddressType captured = *m_target_address;
            while (captured == undesired)
            {
                ::WaitOnAddress(
                    m_target_address, 
                    static_cast<void*>(&undesired),
                    sizeof(TargetAddressType),
                    timeout
                    );
                    
                captured = *m_target_address;
            }
        }

        void write_and_wake_one(TargetAddressType new_value)
        {
            detail::interlocked_update<sizeof(TargetAddressType)>(m_target_address, &new_value);
            ::WakeByAddressSingle(m_target_address);
        }

        void write_and_wake_all(TargetAddressType new_value)
        {
            detail::interlocked_update<sizeof(TargetAddressType)>(m_target_address, &new_value);
            ::WakeByAddressAll(m_target_address);
        }
    };
}