/*
 * Copyright (C) 2025 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_SHARED_OBJECT_POOL_HPP_
#define TYR_COMMON_SHARED_OBJECT_POOL_HPP_

#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>
#include <stack>
#include <utility>
#include <vector>

namespace tyr
{

/**
 * A thread-safe shared object pool
 */

template<typename T>
class SharedObjectPool;

template<typename T>
struct SharedObjectPoolEntry
{
    std::atomic_size_t refcount;
    T object;

    SharedObjectPoolEntry() : refcount(0), object() {}

    template<typename... Args>
    explicit SharedObjectPoolEntry(Args&&... args) : refcount(0), object(std::forward<Args>(args)...)
    {
    }
};

template<typename T>
class SharedObjectPoolPtr
{
private:
    using Entry = SharedObjectPoolEntry<T>;

    SharedObjectPool<T>* m_pool;
    Entry* m_entry;

private:
    void deallocate()
    {
        assert(m_pool && m_entry);

        m_pool->free(m_entry);
        m_pool = nullptr;
        m_entry = nullptr;
    }

    Entry* release() noexcept
    {
        Entry* temp = m_entry;
        m_entry = nullptr;
        m_pool = nullptr;
        return temp;
    }

    void inc_ref_count() noexcept
    {
        assert(m_entry);

        m_entry->refcount.fetch_add(1, std::memory_order_relaxed);
    }

    void dec_ref_count()
    {
        assert(m_entry);

        auto old = m_entry->refcount.fetch_sub(1, std::memory_order_acq_rel);

        assert(old > 0);
        if (old == 1)
        {
            deallocate();  // returns to pool with mutex
        }
    }

public:
    SharedObjectPoolPtr() noexcept : SharedObjectPoolPtr(nullptr, nullptr) {}

    SharedObjectPoolPtr(SharedObjectPool<T>* pool, Entry* object) noexcept : m_pool(pool), m_entry(object)
    {
        if (m_pool && m_entry)
            inc_ref_count();
    }

    SharedObjectPoolPtr(const SharedObjectPoolPtr& other) noexcept : SharedObjectPoolPtr()
    {
        m_pool = other.m_pool;
        m_entry = other.m_entry;

        if (m_pool && m_entry)
            inc_ref_count();
    }

    SharedObjectPoolPtr& operator=(const SharedObjectPoolPtr& other)
    {
        if (this != &other)
        {
            if (m_pool && m_entry)
                dec_ref_count();

            m_pool = other.m_pool;
            m_entry = other.m_entry;

            if (m_pool && m_entry)
                inc_ref_count();
        }
        return *this;
    }

    // Movable
    SharedObjectPoolPtr(SharedObjectPoolPtr&& other) noexcept : m_pool(other.m_pool), m_entry(other.m_entry)
    {
        other.m_pool = nullptr;
        other.m_entry = nullptr;
    }

    SharedObjectPoolPtr& operator=(SharedObjectPoolPtr&& other) noexcept
    {
        if (this != &other)
        {
            if (m_pool && m_entry)
                dec_ref_count();

            m_pool = other.m_pool;
            m_entry = other.m_entry;

            other.m_pool = nullptr;
            other.m_entry = nullptr;
        }
        return *this;
    }

    ~SharedObjectPoolPtr()
    {
        if (m_pool && m_entry)
            dec_ref_count();
    }

    SharedObjectPoolPtr clone() const
        requires std::is_copy_assignable_v<T>
    {
        if (m_pool && m_entry)
        {
            SharedObjectPoolPtr pointer = m_pool->get_or_allocate();
            *pointer = this->operator*();  // copy-assign T
            return pointer;
        }
        else
        {
            return SharedObjectPoolPtr();
        }
    }

    T& operator*() const noexcept
    {
        assert(m_entry);
        return m_entry->object;
    }

    T* operator->() const noexcept
    {
        assert(m_entry);
        return &m_entry->object;
    }

    size_t ref_count() const noexcept
    {
        assert(m_entry);
        return m_entry->refcount.load(std::memory_order_acquire);
    }

    explicit operator bool() const noexcept { return m_entry != nullptr; }
};

template<typename T>
class SharedObjectPool
{
private:
    using Entry = SharedObjectPoolEntry<T>;

    std::vector<std::unique_ptr<Entry>> m_storage;
    std::stack<Entry*> m_stack;
    mutable std::mutex m_mutex;

    template<typename... Args>
    void allocate(Args&&... args)
    {
        m_storage.push_back(std::make_unique<Entry>(std::forward<Args>(args)...));
        m_stack.push(m_storage.back().get());
    }

    void free(Entry* element)
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        m_stack.push(element);
    }

    friend class SharedObjectPoolPtr<T>;

public:
    // Non-copyable to prevent dangling memory pool pointers.
    SharedObjectPool() noexcept = default;
    SharedObjectPool(const SharedObjectPool& other) = delete;
    SharedObjectPool& operator=(const SharedObjectPool& other) = delete;
    SharedObjectPool(SharedObjectPool&& other) noexcept = delete;
    SharedObjectPool& operator=(SharedObjectPool&& other) noexcept = delete;

    [[nodiscard]] SharedObjectPoolPtr<T> get_or_allocate() { return get_or_allocate<>(); }

    template<typename... Args>
    [[nodiscard]] SharedObjectPoolPtr<T> get_or_allocate(Args&&... args)
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        if (m_stack.empty())
            allocate(std::forward<Args>(args)...);
        Entry* element = m_stack.top();
        m_stack.pop();
        return SharedObjectPoolPtr<T>(this, element);
    }

    [[nodiscard]] size_t get_size() const noexcept
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        return m_storage.size();
    }

    [[nodiscard]] size_t get_num_free() const noexcept
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        return m_stack.size();
    }
};

}

#endif