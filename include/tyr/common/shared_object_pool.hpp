/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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
class SharedObjectPoolPtr
{
private:
    using RefCount = std::atomic_size_t;
    using Entry = std::pair<RefCount, T>;

    SharedObjectPool<T>* m_pool;
    Entry* m_object;

private:
    void deallocate()
    {
        assert(m_pool && m_object);

        m_pool->free(m_object);
        m_pool = nullptr;
        m_object = nullptr;
    }

    Entry* release() noexcept
    {
        Entry* temp = m_object;
        m_object = nullptr;
        m_pool = nullptr;
        return temp;
    }

    void inc_ref_count() noexcept
    {
        assert(m_object);

        m_object->first.fetch_add(1, std::memory_order_relaxed);
    }

    void dec_ref_count()
    {
        assert(m_object);

        auto old = m_object->first.fetch_sub(1, std::memory_order_acq_rel);

        assert(old > 0);
        if (old == 1)
        {
            deallocate();  // returns to pool with mutex
        }
    }

public:
    SharedObjectPoolPtr() noexcept : SharedObjectPoolPtr(nullptr, nullptr) {}

    SharedObjectPoolPtr(SharedObjectPool<T>* pool, Entry* object) noexcept : m_pool(pool), m_object(object)
    {
        if (m_pool && m_object)
            inc_ref_count();
    }

    SharedObjectPoolPtr(const SharedObjectPoolPtr& other) noexcept : SharedObjectPoolPtr()
    {
        m_pool = other.m_pool;
        m_object = other.m_object;

        if (m_pool && m_object)
            inc_ref_count();
    }

    SharedObjectPoolPtr& operator=(const SharedObjectPoolPtr& other)
    {
        if (this != &other)
        {
            if (m_pool && m_object)
                dec_ref_count();

            m_pool = other.m_pool;
            m_object = other.m_object;

            if (m_pool && m_object)
                inc_ref_count();
        }
        return *this;
    }

    // Movable
    SharedObjectPoolPtr(SharedObjectPoolPtr&& other) noexcept : m_pool(other.m_pool), m_object(other.m_object)
    {
        other.m_pool = nullptr;
        other.m_object = nullptr;
    }

    SharedObjectPoolPtr& operator=(SharedObjectPoolPtr&& other) noexcept
    {
        if (this != &other)
        {
            if (m_pool && m_object)
                dec_ref_count();

            m_pool = other.m_pool;
            m_object = other.m_object;

            other.m_pool = nullptr;
            other.m_object = nullptr;
        }
        return *this;
    }

    ~SharedObjectPoolPtr()
    {
        if (m_pool && m_object)
            dec_ref_count();
    }

    SharedObjectPoolPtr clone() const
    {
        if (m_pool && m_object)
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
        assert(m_object);
        return m_object->second;
    }

    T* operator->() const noexcept
    {
        assert(m_object);
        return &m_object->second;
    }

    size_t ref_count() const noexcept
    {
        assert(m_object);
        return m_object->first.load(std::memory_order_acquire);
    }

    explicit operator bool() const noexcept { return m_object != nullptr; }
};

template<typename T>
class SharedObjectPool
{
private:
    using RefCount = std::atomic_size_t;
    using Entry = std::pair<RefCount, T>;

    std::vector<std::unique_ptr<Entry>> m_storage;
    std::stack<Entry*> m_stack;
    std::mutex m_mutex;

    template<typename... Args>
    void allocate(Args&&... args)
    {
        m_storage.push_back(std::make_unique<Entry>(RefCount { 0 }, T(std::forward<Args>(args)...)));
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
    SharedObjectPool(const SharedObjectPool& other) noexcept = delete;
    SharedObjectPool& operator=(const SharedObjectPool& other) noexcept = delete;
    SharedObjectPool(SharedObjectPool&& other) noexcept = delete;
    SharedObjectPool& operator=(SharedObjectPool&& other) noexcept = delete;

    [[nodiscard]] SharedObjectPoolPtr<T> get_or_allocate() { return get_or_allocate<>(); }

    template<typename... Args>
    [[nodiscard]] SharedObjectPoolPtr<T> get_or_allocate(Args&&... args)
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        if (m_stack.empty())
        {
            allocate(std::forward<Args>(args)...);
        }
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