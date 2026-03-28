#ifndef TYR_COMMON_RAW_VECTOR_SET_HPP_
#define TYR_COMMON_RAW_VECTOR_SET_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/raw_vector_pool.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

namespace tyr
{

template<std::unsigned_integral Size, typename T, size_t FirstSegmentBytes = 1024>
    requires std::is_trivially_copyable_v<T>
class RawVectorSet
{
public:
    RawVectorSet() : m_pool(std::make_shared<RawVectorPool<Size, T, FirstSegmentBytes>>()), m_set(0, IndexableHash(m_pool), IndexableEqualTo(m_pool)) {}

    RawVectorSet(const RawVectorSet&) = delete;
    RawVectorSet& operator=(const RawVectorSet&) = delete;
    RawVectorSet(RawVectorSet&&) = default;
    RawVectorSet& operator=(RawVectorSet&&) = default;

    std::optional<uint_t> find(const std::vector<T>& value) const
    {
        if (auto it = m_set.find(value); it != m_set.end())
            return *it;
        return std::nullopt;
    }

    uint_t insert(const std::vector<T>& value)
    {
        if (auto it = m_set.find(value); it != m_set.end())
            return *it;

        const auto idx = m_pool->insert(value);
        m_set.emplace(idx);
        return idx;
    }

    RawVectorView<Size, T> operator[](uint_t idx) noexcept { return (*m_pool)[idx]; }

    RawVectorView<const Size, const T> operator[](uint_t idx) const noexcept { return (*m_pool)[idx]; }

    size_t size() const noexcept { return m_pool->size(); }

    void clear() noexcept
    {
        m_pool->clear();
        m_set.clear();
    }

private:
    struct IndexableHash
    {
        using is_transparent = void;

        std::shared_ptr<RawVectorPool<Size, T, FirstSegmentBytes>> pool;

        IndexableHash() noexcept : pool(nullptr) {}
        explicit IndexableHash(std::shared_ptr<RawVectorPool<Size, T, FirstSegmentBytes>> pool) noexcept : pool(std::move(pool)) {}

        static size_t hash(const T* data, size_t len) noexcept
        {
            size_t seed = len;
            for (size_t i = 0; i < len; ++i)
                tyr::hash_combine(seed, data[i]);
            return seed;
        }

        size_t operator()(uint_t idx) const noexcept
        {
            const auto view = (*pool)[idx];
            return hash(view.data(), view.size());
        }

        size_t operator()(const std::vector<T>& value) const noexcept { return hash(value.data(), value.size()); }
    };

    struct IndexableEqualTo
    {
        using is_transparent = void;

        std::shared_ptr<RawVectorPool<Size, T, FirstSegmentBytes>> pool;

        IndexableEqualTo() noexcept : pool(nullptr) {}
        explicit IndexableEqualTo(std::shared_ptr<RawVectorPool<Size, T, FirstSegmentBytes>> pool) noexcept : pool(std::move(pool)) {}

        static bool equal_to(const T* lhs, size_t lhs_size, const T* rhs, size_t rhs_size) noexcept
        {
            return lhs_size == rhs_size && std::equal(lhs, lhs + lhs_size, rhs);
        }

        bool operator()(uint_t lhs, uint_t rhs) const noexcept
        {
            const auto lhs_view = (*pool)[lhs];
            const auto rhs_view = (*pool)[rhs];
            return equal_to(lhs_view.data(), lhs_view.size(), rhs_view.data(), rhs_view.size());
        }

        bool operator()(const std::vector<T>& lhs, uint_t rhs) const noexcept
        {
            const auto rhs_view = (*pool)[rhs];
            return equal_to(lhs.data(), lhs.size(), rhs_view.data(), rhs_view.size());
        }

        bool operator()(uint_t lhs, const std::vector<T>& rhs) const noexcept
        {
            const auto lhs_view = (*pool)[lhs];
            return equal_to(lhs_view.data(), lhs_view.size(), rhs.data(), rhs.size());
        }

        bool operator()(const std::vector<T>& lhs, const std::vector<T>& rhs) const noexcept
        {
            return equal_to(lhs.data(), lhs.size(), rhs.data(), rhs.size());
        }
    };

    std::shared_ptr<RawVectorPool<Size, T, FirstSegmentBytes>> m_pool;
    gtl::flat_hash_set<uint_t, IndexableHash, IndexableEqualTo> m_set;
};

}  // namespace tyr

#endif