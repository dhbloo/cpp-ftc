#pragma once
#include <memory_resource>  // for std::memory_resource
#include <set>              // for std::pmr::set

namespace pmr {

class profile_resource : public std::pmr::memory_resource
{
public:
    explicit profile_resource(
        std::pmr::memory_resource *_upstream        = std::pmr::get_default_resource(),
        std::pmr::memory_resource *_record_upstream = std::pmr::get_default_resource())
        : upstream(_upstream)
        , alloc_rec(_record_upstream)
    {}
    ~profile_resource() = default;

    [[nodiscard]] std::pmr::memory_resource *upstream_resource() const noexcept { return upstream; }

    struct statistic
    {
        std::size_t bytes_allocated;
        std::size_t bytes_in_use;
        std::size_t bytes_highestest;
    };

    [[nodiscard]] statistic get_stat() const noexcept { return stat; }

private:
    void *do_allocate(std::size_t bytes, std::size_t align) override
    {
        void *ptr = upstream->allocate(bytes, align);
        alloc_rec.insert({ptr, bytes, align});

        stat.bytes_allocated += bytes;
        stat.bytes_in_use += bytes;
        if (stat.bytes_allocated > stat.bytes_highestest)
            stat.bytes_highestest = stat.bytes_allocated;

        return ptr;
    }

    void do_deallocate(void *ptr, std::size_t bytes, std::size_t align) override
    {
        auto recIt = alloc_rec.find({ptr, bytes, align});

        if (recIt == alloc_rec.end())
            throw std::invalid_argument("deallocate: invalid pointer");
        else if (recIt->size != bytes)
            throw std::invalid_argument("deallocate: size mismatch");
        else if (recIt->alignment != align)
            throw std::invalid_argument("deallocate: align mismatch");

        upstream->deallocate(ptr, bytes, align);

        alloc_rec.erase(recIt);
        stat.bytes_in_use -= bytes;
    }

    bool do_is_equal(const memory_resource &that) const noexcept override { return this == &that; }

    struct allocation_record
    {
        void *      ptr;
        std::size_t size;
        std::size_t alignment;

        friend bool operator<(const allocation_record &a, const allocation_record &b) noexcept
        {
            return a.ptr < b.ptr;
        }
    };

    memory_resource *                upstream;
    statistic                        stat;
    std::pmr::set<allocation_record> alloc_rec;
};

}  // namespace pmr