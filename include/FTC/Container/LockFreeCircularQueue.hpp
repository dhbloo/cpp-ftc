/**
 * @file LockFreeCircularQueue.hpp
 * A wait-free concurrent circular queue.
 *
 * Bounded concurrent queue implementation for SPSC, MPSC, SPMC, MPMC scenario.
 */

#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <memory>
#include <optional>
#include <thread>

namespace ftc {

template <typename T, std::size_t Size, bool SP, bool SC> class LockFreeCircularQueue
{
    static_assert(Size >= 32, "Size must be at least 32");
    static_assert(Size & (Size - 1) == 0, "Size must be a power of 2");

#ifdef __cpp_lib_hardware_interference_size
    static constexpr std::size_t cacheLineSize = std::hardware_destructive_interference_size;
#else
    static constexpr std::size_t cacheLineSize = 64;
#endif

    static constexpr std::size_t numMaxThreads = 16;

public:
    LockFreeCircularQueue();
    ~LockFreeCircularQueue();

    /// @brief Try to construct a value into the queue (non-blocking).
    /// @return Whether the emplacement operation is success.
    /// Returns immediately if emplacement fails due to contention or buffer is full.
    template <typename... Args> bool TryEmplace(Args &&... args);

    /// @brief Construct a value into the queue (blocking).
    /// Blocks due to contention or buffer is full until emplacement succeeds.
    template <typename... Args> void Emplace(Args &&... args);

    /// @brief Try to copy a value into the queue (non-blocking).
    /// @return Whether the push operation is success.
    /// Returns immediately if push fails due to contention or buffer is full.
    bool TryPush(const T &value);

    /// @brief Try to move a value into the queue (non-blocking).
    /// @return Whether the push operation is success.
    /// Returns immediately if push fails due to contention or buffer is full.
    bool TryPush(T &&value);

    /// @brief Copy a value into the queue (blocking).
    /// Blocks due to contention or buffer is full until push succeeds.
    void Push(const T &value);

    /// @brief Move a value into the queue (blocking).
    /// Blocks due to contention or buffer is full until push succeeds.
    void Push(T &&value);

    /// @brief Try to pop a value out of the queue (non-blocking).
    /// @return Value if the pop operation is success, otherwise std::nullopt.
    /// Returns immediately if pop fails due to contention or buffer is empty.
    std::optional<T> TryPop();

    /// @brief Pop a value out of the queue (blocking).
    /// Blocks due to contention or buffer is empty until pop succeeds.
    T Pop();

    /// Returns the estimated count of current elements.
    std::size_t Count() const;

    /// Checks if the queue is empty.
    bool Empty() const;

    /// Checks if the queue is full.
    bool Full() const;

private:
    struct Tag
    {
        std::size_t cycle : sizeof(std::size_t) * 8 - 1;
        bool        full : 1;
    };
    static_assert(std::atomic<Tag>::is_always_lock_free);

    // @brief Slot contains a tag and a value (default as uninitialized storage).
    // Slot is aligned as the cache line size, to avoid false sharing.
    struct alignas(cacheLineSize) Slot
    {
        std::atomic<Tag>                              tag;
        std::aligned_storage_t<sizeof(T), alignof(T)> storage;
    } slots_[Size];

    /// Index of the front of the queue.
    alignas(cacheLineSize) std::atomic<std::size_t> head_;

    /// Index of the back of the queue.
    alignas(cacheLineSize) std::atomic<std::size_t> tail_;

    static inline std::size_t IndexOf(std::size_t p) { return p % Size; }
    static inline std::size_t CycleOf(std::size_t p) { return p / Size; }
};

}  // namespace ftc

template <typename T, std::size_t Size, bool SP, bool SC>
inline ftc::LockFreeCircularQueue<T, Size, SP, SC>::LockFreeCircularQueue() : head_()
                                                                            , tail_()
{
    std::memset(slots_, 0, sizeof(slots_));
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline ftc::LockFreeCircularQueue<T, Size, SP, SC>::~LockFreeCircularQueue()
{}

template <typename T, std::size_t Size, bool SP, bool SC>
template <typename... Args>
inline bool ftc::LockFreeCircularQueue<T, Size, SP, SC>::TryEmplace(Args &&... args)
{
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    std::size_t head = head_.load(std::memory_order_relaxed);

    if constexpr (SP) {
        if (tail - head < Size) {
            tail_.store(tail + 1, std::memory_order_relaxed);
            std::size_t index = IndexOf(tail);
            Tag         tag   = slots_[index].tag.load(std::memory_order_acquire);
            if (!tag.full) {
                new (&slots_[index].storage) T(std::forward<Args>(args)...);
                slots_[index].tag.store(Tag {CycleOf(tail), true}, std::memory_order_release);
                return true;
            }
        }
    }
    else {
        std::ptrdiff_t count = tail - head;
        if (count + numMaxThreads < Size) {
            tail              = tail_.fetch_add(1, std::memory_order_relaxed);
            std::size_t index = IndexOf(tail);
            Tag         tag   = slots_[index].tag.load(std::memory_order_acquire);
            if (!tag.full && tag.cycle == CycleOf(tail) - 1) {
                new (&slots_[index].storage) T(std::forward<Args>(args)...);
                slots_[index].tag.store(Tag {CycleOf(tail), true}, std::memory_order_release);
                return true;
            }
        }
        else if (count < Size) {
            if (tail_.compare_exchange_strong(tail, tail + 1, std::memory_order_relaxed)) {
                std::size_t index = IndexOf(tail);
                new (&slots_[index].storage) T(std::forward<Args>(args)...);
                slots_[index].tag.store(Tag {CycleOf(tail), true}, std::memory_order_release);
                return true;
            }
        }
    }

    return false;
}

template <typename T, std::size_t Size, bool SP, bool SC>
template <typename... Args>
inline void ftc::LockFreeCircularQueue<T, Size, SP, SC>::Emplace(Args &&... args)
{
    while (!TryEmplace(std::forward<Args>(args)...)) {
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline bool ftc::LockFreeCircularQueue<T, Size, SP, SC>::TryPush(const T &value)
{
    return TryEmplace(value);
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline bool ftc::LockFreeCircularQueue<T, Size, SP, SC>::TryPush(T &&value)
{
    return TryEmplace(std::move(value));
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline void ftc::LockFreeCircularQueue<T, Size, SP, SC>::Push(const T &value)
{
    Emplace(value);
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline void ftc::LockFreeCircularQueue<T, Size, SP, SC>::Push(T &&value)
{
    Emplace(std::move(value));
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline std::optional<T> ftc::LockFreeCircularQueue<T, Size, SP, SC>::TryPop()
{
    return std::optional<T>();
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline T ftc::LockFreeCircularQueue<T, Size, SP, SC>::Pop()
{
    return T();
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline std::size_t ftc::LockFreeCircularQueue<T, Size, SP, SC>::Count() const
{
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    return tail > head ? tail - head > Size ? Size : tail - head : 0;
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline bool ftc::LockFreeCircularQueue<T, Size, SP, SC>::Empty() const
{
    return Count() == 0;
}

template <typename T, std::size_t Size, bool SP, bool SC>
inline bool ftc::LockFreeCircularQueue<T, Size, SP, SC>::Full() const
{
    return Count() == Size;
}
