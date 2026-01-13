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

#ifndef TYR_COMMON_UNIQUE_OBJECT_POOL_HPP
#define TYR_COMMON_UNIQUE_OBJECT_POOL_HPP

#include <cassert>
#include <concepts>
#include <memory>
#include <mutex>
#include <stack>
#include <vector>

namespace tyr
{
template<typename T>
class UniqueObjectPool;

template<typename T>
class UniqueObjectPoolPtr
{
private:
    UniqueObjectPool<T>* m_pool;
    T* m_entry;

private:
    void deallocate()
    {
        assert(m_pool && m_entry);

        m_pool->free(m_entry);
        m_pool = nullptr;
        m_entry = nullptr;
    }

    T* release() noexcept
    {
        T* temp = m_entry;
        m_entry = nullptr;
        m_pool = nullptr;
        return temp;
    }

public:
    UniqueObjectPoolPtr() noexcept : UniqueObjectPoolPtr(nullptr, nullptr) {}

    UniqueObjectPoolPtr(UniqueObjectPool<T>* pool, T* object) noexcept : m_pool(pool), m_entry(object) {}

    UniqueObjectPoolPtr(const UniqueObjectPoolPtr& other) = delete;

    UniqueObjectPoolPtr& operator=(const UniqueObjectPoolPtr& other) = delete;

    // Movable
    UniqueObjectPoolPtr(UniqueObjectPoolPtr&& other) noexcept : m_pool(other.m_pool), m_entry(other.m_entry)
    {
        other.m_pool = nullptr;
        other.m_entry = nullptr;
    }

    UniqueObjectPoolPtr& operator=(UniqueObjectPoolPtr&& other) noexcept
    {
        if (this != &other)
        {
            if (m_pool && m_entry)
                deallocate();

            m_pool = other.m_pool;
            m_entry = other.m_entry;

            other.m_pool = nullptr;
            other.m_entry = nullptr;
        }
        return *this;
    }

    UniqueObjectPoolPtr clone() const
        requires std::is_copy_assignable_v<T>
    {
        if (m_pool && m_entry)
        {
            UniqueObjectPoolPtr pointer = m_pool->get_or_allocate();
            *pointer = this->operator*();  // copy-assign T
            return pointer;
        }
        else
        {
            return UniqueObjectPoolPtr();
        }
    }

    ~UniqueObjectPoolPtr()
    {
        if (m_pool && m_entry)
            deallocate();
    }

    T& operator*() const noexcept
    {
        assert(m_entry);
        return *m_entry;
    }

    T* operator->() const noexcept
    {
        assert(m_entry);
        return m_entry;
    }

    T* get() const noexcept { return m_entry; }

    explicit operator bool() const noexcept { return m_entry != nullptr; }
};

template<typename T>
class UniqueObjectPool
{
private:
    std::vector<std::unique_ptr<T>> m_storage;
    std::stack<T*> m_stack;
    mutable std::mutex m_mutex;

    template<typename... Args>
    void allocate()
    {
        m_storage.push_back(std::make_unique<T>());
        m_stack.push(m_storage.back().get());
    }

    void free(T* element)
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        m_stack.push(element);
    }

    friend class UniqueObjectPoolPtr<T>;

public:
    // Non-copyable to prevent dangling memory pool pointers.
    UniqueObjectPool() noexcept = default;
    UniqueObjectPool(const UniqueObjectPool& other) = delete;
    UniqueObjectPool& operator=(const UniqueObjectPool& other) = delete;
    UniqueObjectPool(UniqueObjectPool&& other) noexcept = delete;
    UniqueObjectPool& operator=(UniqueObjectPool&& other) noexcept = delete;

    [[nodiscard]] UniqueObjectPoolPtr<T> get_or_allocate()
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        if (m_stack.empty())
            allocate();
        T* element = m_stack.top();
        m_stack.pop();
        return UniqueObjectPoolPtr<T>(this, element);
    }

    template<typename... Args>
    [[nodiscard]] UniqueObjectPoolPtr<T> get_or_allocate(Args&&... args)
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        if (m_stack.empty())
            allocate();
        T* element = m_stack.top();
        m_stack.pop();
        element->initialize(std::forward<Args>(args)...);
        return UniqueObjectPoolPtr<T>(this, element);
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