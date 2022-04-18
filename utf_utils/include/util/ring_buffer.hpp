#ifndef _RING__BUFFER_HPP__
#define _RING__BUFFER_HPP__

#include <vector>
#include <memory>
#include <limits>

#include "util/bool_lock.hpp"
#include "util/helper_functions.hpp"

namespace util
{

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
class RingBuffer
{
public:
    using value_type    = Type;
    struct Handle
    {
    public:
        explicit Handle(RingBuffer* parent, SizeType begin, SizeType capacity) noexcept
            : parent_(parent)
            , begin_(begin)
            , capacity_(capacity)
            , size_(0)
        {}

        Handle()    = delete;

        Handle(Handle&& handle) noexcept
            : parent_(std::move(handle.parent_))
            , begin_(std::move(handle.begin_))
            , capacity_(std::move(handle.capacity_))
            , size_(std::move(handle.size_))
        {
            handle.size_        = 0;
            handle.capacity_    = 0;
        }

        Handle& operator=(Handle&& handle) noexcept
        {
            parent_             = std::move(handle.parent_);
            begin_              = std::move(handle.begin_);
            capacity_           = std::move(handle.capacity_);
            size_               = std::move(handle.size_);

            handle.size_        = 0;
            handle.capacity_    = 0;

            return *this;
        }

        [[nodiscard]] decltype(auto) data() noexcept
        {
            return parent_->data() + begin_;
        }

        [[nodiscard]] decltype(auto) capacity() noexcept
        {
            return capacity_;
        }

        void size(SizeType s) noexcept
        {
            size_   = s;
        }

        ~Handle() noexcept
        {
            if (capacity_ > 0)
                parent_->commit(*this);
        }

    private:
        RingBuffer*     parent_;

    public:
        SizeType        begin_;
        SizeType        capacity_;
        SizeType        size_;
    };

    using ReadSizeStatus    = std::tuple<SizeType, bool>;

    RingBuffer() noexcept
        : buffer_(t_Size, Type())
        , sizes_(t_Size)
        , state_(State::Beginning)
        , operators_()
        , lock_()
    {
        initialize();
    }

    ~RingBuffer() noexcept
    {
        buffer_.clear();
    }

    RingBuffer(const RingBuffer&)             = delete;
    RingBuffer& operator=(const RingBuffer&)  = delete;

    [[nodiscard]] decltype(auto) buffer(SizeType size_requested = std::numeric_limits<SizeType>::max()) noexcept;
    [[nodiscard]] decltype(auto) read(Type* destination_buffer, const SizeType read_size = 1) noexcept;
    [[nodiscard]] decltype(auto) read_block(SizeType size_requested = std::numeric_limits<SizeType>::max()) noexcept;

    using CheckDelimiter   = std::function<bool (const Type*, const Type*, const SizeType)>;
    [[nodiscard]] decltype(auto) read(Type* destination_buffer, const SizeType destination_buffer_size, const Type* delimiter, const SizeType delimiter_size, CheckDelimiter check_delimiter = [](const Type*, const Type*, const SizeType) { return true; } ) noexcept;

    [[nodiscard]] decltype(auto) read_fixed_size(Type* destination_buffer, const int64_t read_size) noexcept;

    decltype(auto) move(const Type* const move_buffer, const SizeType move_size) noexcept;
    decltype(auto) commit(Handle& handle) noexcept;

    [[nodiscard]] decltype(auto) release(Handle& handle) noexcept;

    [[nodiscard]] decltype(auto) data() noexcept
    {
        return buffer_.data();
    }

protected:
    decltype(auto) initialize() noexcept;
    [[nodiscard]] decltype(auto) reserve_back(SizeType size_requested) noexcept;
    [[nodiscard]] decltype(auto) reserve_front(SizeType size_requested) noexcept;
    [[nodiscard]] decltype(auto) commit_back(Handle& handle) noexcept;
    [[nodiscard]] decltype(auto) commit_front(Handle& handle) noexcept;

    using StorageType           = std::vector<Type>;
    using Lock                  = BoolLock;
    using Block                 = std::tuple<SizeType, SizeType>;
    using ReserveOperator       = std::function<Block (SizeType)>;
    using CommitOperator        = std::function<void (Handle&)>;
    using ReleaseOperator       = std::function<void (SizeType)>;
    using ReadBlockOperator     = std::function<Block (SizeType)>;
    using ReserveOperators      = std::vector<ReserveOperator>;
    using CommitOperators       = std::vector<CommitOperator>;
    using ReleaseOperators      = std::vector<ReleaseOperator>;

    enum class State : std::uint8_t
    {
        Beginning   = 0,
        Progressed,
        Rolledover
    };

    struct Sizes
    {
        SizeType    last_;
        SizeType    head_;
        SizeType    tail_;
        SizeType    end_;
        SizeType    capacity_;

        Sizes()     = delete;

        explicit Sizes(SizeType size)   noexcept
            : last_(0)
            , head_(0)
            , tail_(0)
            , end_(0)
            , capacity_(size)
        {}

        Sizes(Sizes&& sizes) noexcept
            : last_(std::move(sizes.last_))
            , head_(std::move(sizes.head_))
            , tail_(std::move(sizes.tail_))
            , end_(std::move(sizes.end_))
            , capacity_(std::move(sizes.capacity_))
        {}

        Sizes& operator=(Sizes&& sizes) noexcept
        {
            last_       = std::move(sizes.last_);
            head_       = std::move(sizes.head_);
            tail_       = std::move(sizes.tail_);
            end_        = std::move(sizes.end_);
            capacity_   = std::move(sizes.capacity_);

            return *this;
        }
    };

    struct Operators
    {
        ReserveOperators    reserve_operators_;
        CommitOperators     commit_operators_;
        ReleaseOperators    release_operators_;

        Operators() noexcept
            : reserve_operators_()
            , commit_operators_()
            , release_operators_()
        {}

        Operators(Operators&& operators) noexcept
            : reserve_operators_(std::move(operators.reserve_operators_))
            , commit_operators_(std::move(operators.commit_operators_))
            , release_operators_(std::move(operators.release_operators_))
        {}

        Operators& operator=(Operators&& operators) noexcept
        {
            reserve_operators_  = std::move(operators.reserve_operators_);
            commit_operators_   = std::move(operators.commit_operators_);
            release_operators_  = std::move(operators.release_operators_);

            return *this;
        }

        ~Operators() noexcept
        {}
    };

private:
    StorageType         buffer_;
    Sizes               sizes_;
    State               state_;
    Operators           operators_;
    ReadBlockOperator   movable_block_;
    Lock                lock_;
};

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::initialize() noexcept
{
    // reserve - State::Beginning
    operators_.reserve_operators_.emplace_back(
            [this](SizeType size_requested)
            {
                return reserve_back(size_requested);
            }
        );

    // reserve - State::Progressed
    operators_.reserve_operators_.emplace_back(
            [this](SizeType size_requested)
            {
                return reserve_back(size_requested);
            }
        );

    // reserve - State::Rolledover
    operators_.reserve_operators_.emplace_back(
            [this](SizeType size_requested)
            {
                return reserve_front(size_requested);
            }
        );

    // commit - State::Beginning
    operators_.commit_operators_.emplace_back(
            [this](Handle& handle)
            {
                commit_back(handle);

                return;
            }
        );

    // commit - State::Progressed
    operators_.commit_operators_.emplace_back(
            [this](Handle& handle)
            {
                commit_back(handle);

                if (sizes_.head_ == sizes_.tail_)
                {
                    state_  = State::Beginning;
                    sizes_  = std::move(Sizes(sizes_.capacity_));

                    return;
                }

                if (sizes_.tail_ == sizes_.capacity_)
                {
                    sizes_.end_ = 0;
                    state_  = State::Rolledover;

                    return;
                }

                return;
            }
        );

    // commit - State::Rolledover
    operators_.commit_operators_.emplace_back(
            [this](Handle& handle)
            {
                commit_front(handle);

                return;
            }
        );

    // release - State::Beginning
    operators_.release_operators_.emplace_back(
            [this](SizeType size_released)
            {
                if (size_released != 0)
                {
                    sizes_.head_   += size_released;

                    if (sizes_.head_ == sizes_.end_)
                    {
                        sizes_      = std::move(Sizes(sizes_.capacity_));
                        return;
                    }

                    state_          = (sizes_.tail_ == sizes_.capacity_) ? State::Rolledover : State::Progressed;
                }

                return;
            }
        );

    // release - State::Progressed
    operators_.release_operators_.emplace_back(
            [this](SizeType size_released)
            {
                sizes_.head_   += size_released;

                if (sizes_.head_ == sizes_.end_)
                {
                    state_      = State::Beginning;
                    sizes_      = std::move(Sizes(sizes_.capacity_));

                    return;
                }

                if (sizes_.tail_ == sizes_.capacity_)
                    state_  = State::Rolledover;

                return;
            }
        );

    // release - State::Rolledover
    operators_.release_operators_.emplace_back(
            [this](SizeType size_released)
            {
                sizes_.head_   += size_released;

                if (sizes_.head_ == sizes_.tail_)
                {
                    state_          = State::Beginning;
                    sizes_.head_    = 0;
                    sizes_.tail_    = sizes_.last_;
                    sizes_.last_    = 0;
                }

                return;
            }
        );

    movable_block_  =
        [this](SizeType size_requested)
        {
            SizeType    size = sizes_.tail_ - sizes_.head_;

            if (size > size_requested)
                size    = size_requested;

            return Block(sizes_.head_, size);
        };

}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::reserve_back(SizeType size_requested) noexcept
{
    if (sizes_.end_ != sizes_.tail_)
        return Block(sizes_.tail_, 0);

    SizeType space_available    = sizes_.capacity_ - sizes_.tail_;
    SizeType reserve_size       = (size_requested >= space_available) ? space_available : size_requested;
    sizes_.end_                 = sizes_.tail_ + reserve_size;

    return Block(sizes_.tail_, reserve_size);
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::reserve_front(SizeType size_requested) noexcept
{
    if (sizes_.end_ != sizes_.last_)
        return Block(sizes_.last_, 0);

    if (sizes_.last_ == sizes_.head_)
        return Block(sizes_.last_, 0);

    SizeType space_available    = sizes_.head_ - sizes_.last_;
    SizeType reserve_size       = (size_requested >= space_available) ? space_available : size_requested;
    sizes_.end_                 = sizes_.last_ + reserve_size;

    return Block(sizes_.last_, reserve_size);
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::commit_back(Handle& handle) noexcept
{
    SizeType commit_size        = (handle.size_ < handle.capacity_) ? handle.size_ : handle.capacity_; 
    sizes_.tail_               += commit_size;
    sizes_.end_                 = sizes_.tail_;
    handle.begin_               = sizes_.tail_;
    handle.capacity_            = 0;
    handle.size_                = 0;

    return;
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::commit_front(Handle& handle) noexcept
{
    SizeType commit_size        = (handle.size_ < handle.capacity_) ? handle.size_ : handle.capacity_; 
    sizes_.last_               += commit_size;
    sizes_.end_                 = sizes_.last_;
    handle.begin_               = sizes_.last_;
    handle.capacity_            = 0;
    handle.size_                = 0;

    return;
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::buffer(SizeType size_requested) noexcept
{
    std::scoped_lock<Lock> lock(lock_);

    auto&& [begin, capacity]    = operators_.reserve_operators_[to_index(state_)](size_requested);

    return Handle(this, begin, capacity);
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::move(const Type* const move_buffer, const SizeType move_size) noexcept
{
    SizeType size = 0;

    do
    {
        SizeType    remaining_size  = move_size - size;
        Type*       buffer;
        SizeType    buffer_begin;
        SizeType    buffer_size;

        {
            std::scoped_lock<Lock> lock(lock_);

            auto&& [begin, capacity] = operators_.reserve_operators_[to_index(state_)](remaining_size);

            buffer_begin    = begin;
            buffer_size     = capacity;
        }

        buffer  = data() + buffer_begin;

        if (buffer_size < remaining_size)
            remaining_size  = buffer_size;

        for (SizeType i = 0; i < remaining_size; ++i)
            buffer[i]   = std::move(move_buffer[size++]);

        // commit remaining_size
        Handle handle(this, buffer_begin, buffer_size);
        handle.size_    = remaining_size;

        commit(handle);

    } while(size < move_size);

    return;
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::commit(Handle& handle) noexcept
{
    std::scoped_lock<Lock>  lock(lock_);
    operators_.commit_operators_[to_index(state_)](handle);
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::read(Type* destination_buffer, const SizeType read_size) noexcept
{
    auto&& [begin, capacity]  = [this](auto size)
                {
                    std::scoped_lock<Lock>  lock(lock_);
                    return movable_block_(size);
                }(read_size);

    Type* buffer    = data() + begin;

    for (SizeType index = 0; index < capacity; ++index)
        destination_buffer[index]   = std::move(buffer[index]);

    [this](auto capacity)
    {
        std::scoped_lock<Lock>  lock(lock_);
        operators_.release_operators_[to_index(state_)](capacity);
    }(capacity);

    return capacity;
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::read(Type* destination_buffer, const SizeType destination_buffer_size, const Type* delimiter, const SizeType delimiter_size, CheckDelimiter check_delimiter) noexcept
{
    bool        delimiter_found     = false;
    SizeType    data_size           = 0;

    do
    {
        auto&& [movable_offset, movable_size]  = [this]()
                    {
                        std::scoped_lock<Lock>  lock(lock_);
                        return movable_block_(std::numeric_limits<SizeType>::max());
                    }();

        Type* movable_buffer    = data() + movable_offset;
        SizeType index          = 0;

        for (index = 0; index < movable_size && data_size < destination_buffer_size; ++index)
        {
            destination_buffer[data_size++] = std::move(movable_buffer[index]);

            if (index > delimiter_size)
            {
                bool    delimiter_match = true;
                for (SizeType i = 0; i < delimiter_size; ++i)
                {
                    if (destination_buffer[data_size - 1 - i] != delimiter[delimiter_size - 1 - i])
                    {
                        delimiter_match = false;
                        break;
                    }
                }

                if (delimiter_match && check_delimiter(destination_buffer, delimiter, delimiter_size))
                {
                    delimiter_found = true;
                    ++index;
                    break;
                }
            }
        }

        [this](auto index)
        {
            std::scoped_lock<Lock>  lock(lock_);
            operators_.release_operators_[to_index(state_)](index);
        }(index);

    } while(!delimiter_found && data_size < destination_buffer_size);

    return ReadSizeStatus(data_size, delimiter_found);
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::read_block(SizeType size_requested) noexcept
{
    auto&& [begin, size]  = [this](auto size)
                {
                    std::scoped_lock<Lock>  lock(lock_);
                    return movable_block_(size);
                }(size_requested);

    return Handle(this, begin, size);
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::read_fixed_size(Type* destination_buffer, const int64_t read_size) noexcept
{
    SizeType    data_size   = 0;

    do
    {
        auto&& [offset, capacity]  = [this]()
                    {
                        std::scoped_lock<Lock>  lock(lock_);
                        return movable_block_(std::numeric_limits<SizeType>::max());
                    }();

        SizeType    index   = 0;
        Type*       buffer  = data() + offset;

        while (index < capacity && static_cast<int64_t>(data_size) < read_size)
        {
            destination_buffer[data_size++] = std::move(buffer[index++]);
        }

        [this](auto index)
        {
            std::scoped_lock<Lock>  lock(lock_);
            operators_.release_operators_[to_index(state_)](index);
        }(index);

    } while(data_size < read_size);

    return;
}

template<typename Type, typename SizeType, SizeType t_Size, typename... Args>
inline
decltype(auto) RingBuffer<Type, SizeType, t_Size, Args...>::release(Handle& handle) noexcept
{
    std::scoped_lock<Lock>  lock(lock_);

    SizeType release_size   = (handle.size_ < handle.capacity_) ? handle.size_ : handle.capacity_;

    operators_.release_operators_[to_index(state_)](release_size);

    handle.capacity_        = 0;
    handle.size_            = 0;

    return;
}

}

#endif // _RING__BUFFER_HPP__
