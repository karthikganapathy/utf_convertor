#ifndef _ATOMIC_LOCK_HPP__
#define _ATOMIC_LOCK_HPP__

#include <atomic>
#include <mutex>

namespace util
{

class AtomicLock
{
public:
    AtomicLock()
    {}

    AtomicLock(const AtomicLock&)               = delete;
    AtomicLock(AtomicLock&&)                    = delete;
    AtomicLock& operator=(const AtomicLock&)    = delete;
    AtomicLock& operator=(AtomicLock&&)         = delete;

    void lock()
    {
        while (std::atomic_flag_test_and_set_explicit(&is_locked_, std::memory_order_acquire))
            __asm__("nop");
    }

    void unlock()
    {
        std::atomic_flag_clear_explicit(&is_locked_, std::memory_order_release);
    }
private:
    static std::atomic_flag is_locked_;
};

}

#endif // _ATOMIC_LOCK_HPP__
