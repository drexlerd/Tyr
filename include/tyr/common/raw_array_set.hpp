/*
 * Copyright (C) 2025 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef TYR_COMMON_RAW_ARRAY_SET_HPP_
#define TYR_COMMON_RAW_ARRAY_SET_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/raw_array_pool.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

namespace tyr
{

template<typename T, size_t ArraysPerSegment = 1024>
    requires std::is_trivially_copyable_v<T>
class RawArraySet
{
public:
    explicit RawArraySet(size_t array_size) :
        m_pool(std::make_shared<RawArrayPool<T, ArraysPerSegment>>(array_size)),
        m_array_size(array_size),
        m_set(0, IndexableHash(m_pool, array_size), IndexableEqualTo(m_pool, array_size))
    {
    }

    RawArraySet(const RawArraySet&) = delete;
    RawArraySet& operator=(const RawArraySet&) = delete;
    RawArraySet(RawArraySet&&) = default;
    RawArraySet& operator=(RawArraySet&&) = default;

    std::optional<uint_t> find(const std::vector<T>& value) const
    {
        assert(value.size() == m_array_size);

        if (auto it = m_set.find(value); it != m_set.end())
            return *it;

        return std::nullopt;
    }

    uint_t insert(const std::vector<T>& value)
    {
        assert(value.size() == m_array_size);

        if (auto it = m_set.find(value); it != m_set.end())
            return *it;

        const uint_t idx = static_cast<uint_t>(m_pool->size());
        auto* arr = m_pool->allocate();
        std::memcpy(arr, value.data(), m_array_size * sizeof(T));
        m_set.emplace(idx);
        return idx;
    }

    T* operator[](uint_t idx) noexcept { return (*m_pool)[idx]; }

    const T* operator[](uint_t idx) const noexcept { return (*m_pool)[idx]; }

    size_t size() const noexcept { return m_pool->size(); }
    size_t array_size() const noexcept { return m_pool->array_size(); }

    void clear() noexcept
    {
        m_pool->clear();
        m_set.clear();
    }

private:
    struct IndexableHash
    {
        using is_transparent = void;

        std::shared_ptr<RawArrayPool<T, ArraysPerSegment>> pool;
        size_t array_size;

        IndexableHash() noexcept : pool(nullptr), array_size(0) {}
        explicit IndexableHash(std::shared_ptr<RawArrayPool<T, ArraysPerSegment>> pool, size_t array_size) noexcept :
            pool(std::move(pool)),
            array_size(array_size)
        {
        }

        static size_t hash(const T* arr, size_t len) noexcept
        {
            size_t seed = len;
            for (size_t i = 0; i < len; ++i)
                tyr::hash_combine(seed, arr[i]);
            return seed;
        }

        size_t operator()(uint_t el) const noexcept { return hash((*pool)[el], array_size); }

        size_t operator()(const std::vector<T>& el) const noexcept
        {
            assert(el.size() == array_size);
            return hash(el.data(), array_size);
        }
    };

    struct IndexableEqualTo
    {
        using is_transparent = void;

        std::shared_ptr<RawArrayPool<T, ArraysPerSegment>> pool;
        size_t array_size;

        IndexableEqualTo() noexcept : pool(nullptr), array_size(0) {}
        explicit IndexableEqualTo(std::shared_ptr<RawArrayPool<T, ArraysPerSegment>> pool, size_t array_size) noexcept :
            pool(std::move(pool)),
            array_size(array_size)
        {
        }

        static bool equal_to(const T* lhs, const T* rhs, size_t len) { return std::equal(lhs, lhs + len, rhs); }

        bool operator()(uint_t lhs, uint_t rhs) const noexcept { return equal_to((*pool)[lhs], (*pool)[rhs], array_size); }

        bool operator()(const std::vector<T>& lhs, uint_t rhs) const noexcept
        {
            assert(lhs.size() == array_size);
            return equal_to(lhs.data(), (*pool)[rhs], array_size);
        }

        bool operator()(uint_t lhs, const std::vector<T>& rhs) const noexcept
        {
            assert(rhs.size() == array_size);
            return equal_to((*pool)[lhs], rhs.data(), array_size);
        }

        bool operator()(const std::vector<T>& lhs, const std::vector<T>& rhs) const noexcept
        {
            assert(lhs.size() == array_size);
            assert(rhs.size() == array_size);
            return equal_to(lhs.data(), rhs.data(), array_size);
        }
    };

    std::shared_ptr<RawArrayPool<T, ArraysPerSegment>> m_pool;
    size_t m_array_size;
    gtl::flat_hash_set<uint_t, IndexableHash, IndexableEqualTo> m_set;
};

}  // namespace tyr

#endif