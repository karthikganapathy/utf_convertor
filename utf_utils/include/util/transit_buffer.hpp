#ifndef _TRANSIT_BUFFER_HPP__
#define _TRANSIT_BUFFER_HPP__

#include <vector>
#include <memory>
#include <mutex>
#include <tuple>
#include <limits>
#include <type_traits>
#include <cassert>

#include "util/helper_functions.hpp"
#include "util/atomic_lock.hpp"

namespace util
{

// Goal is to provide maximum capacity for receiving
// as receiving operation can be expensive. In case of 
// circular buffer, two receive operation may be needed
// when the buffer wraps over.
template<typename Type, typename SizeType, SizeType t_Size>
class TransitBuffer
{
public:
    using value_type    = Type;

    struct Handle
    {
    public:
        explicit Handle(TransitBuffer* parent, Type* data, SizeType capacity) noexcept
            : parent_(parent)
            , data_(data)
            , capacity_(capacity)
            , size_(0)
        {}

        Handle()    = delete;

        Handle(Handle&& handle) noexcept
            : parent_(std::move(handle.parent_))
            , data_(std::move(handle.data_))
            , capacity_(std::move(handle.capacity_))
            , size_(std::move(handle.size_))
        {
            handle.size_        = 0;
            handle.capacity_    = 0;
        }

        Handle& operator=(Handle&& handle) noexcept
        {
            parent_             = std::move(handle.parent_);
            data_               = std::move(handle.data_);
            capacity_           = std::move(handle.capacity_);
            size_               = std::move(handle.size_);

            handle.size_        = 0;
            handle.capacity_    = 0;

            return *this;
        }

        [[nodiscard]] decltype(auto) data() noexcept
        {
            return data_;
        }

        [[nodiscard]] decltype(auto) capacity() noexcept
        {
            return capacity_;
        }

        void size(SizeType s) noexcept
        {
            size_   = std::move(s);
        }

        ~Handle() noexcept
        {
            if (capacity_ > 0)
                parent_->commit(*this);
        }

    private:
        TransitBuffer*  parent_;

    public:
        Type*           data_;
        SizeType        capacity_;
        SizeType        size_;
    };

    using ReadSizeStatus    = std::tuple<SizeType, bool>;

    template<typename... Args>
    TransitBuffer(Args&&... args)
        : buffer_(t_Size, Type(std::forward<Args>(args)...))
        , sizes_(t_Size)
        , pointers_(buffer_)
        , state_(State::Beginning)
        , operators_()
        , movable_block_()
        , lock_()
    {
        initialize();
    }

    decltype(auto) initialize() noexcept;

    [[nodiscard]] decltype(auto) buffer(SizeType size_requested = std::numeric_limits<SizeType>::max()) noexcept;
    [[nodiscard]] decltype(auto) move(Type* move_buffer, const SizeType move_size = 1) noexcept;
    [[nodiscard]] decltype(auto) commit(Handle& handle) noexcept;

    [[nodiscard]] decltype(auto) read(Type* destination_buffer, const SizeType read_size = 1) noexcept;

    using CheckDelimiter   = std::function<bool (const Type*, const Type*, const SizeType)>;
    [[nodiscard]] decltype(auto) read(Type* destination_buffer, const SizeType destination_buffer_size, const Type* delimiter, const SizeType delimiter_size, CheckDelimiter check_delimiter = [](const Type*, const Type*, const SizeType) { return true; } ) noexcept;

    [[nodiscard]] decltype(auto) read_fixed_size(Type* destination_buffer, const int64_t read_size) noexcept;

    [[nodiscard]] decltype(auto) read_block(SizeType size_requested = std::numeric_limits<SizeType>::max()) noexcept;
    [[nodiscard]] decltype(auto) release(Handle& handle) noexcept;

    ~TransitBuffer() noexcept
    {
        buffer_.clear();
    }

    [[nodiscard]] decltype(auto) data() noexcept
    {
        return buffer_.data();
    }

    decltype(auto) reset() noexcept
    {
        state_      = State::Beginning;
        sizes_.reset();
        pointers_.reset();
    }

protected:
    using Buffer                = std::vector<Type>;
    using Block                 = std::tuple<Type*, SizeType>;
    using ReserveOperator       = std::function<Block (SizeType)>;
    using CommitOperator        = std::function<void (Handle&)>;
    using ReleaseOperator       = std::function<void (SizeType)>;
    using ReadBlockOperator     = std::function<Block (SizeType)>;
    using Lock                  = util::AtomicLock;

    enum class State : std::uint8_t
    {
        Beginning   = 0,
        Progressed,
        Secondary
    };

    using ReserveOperators      = std::vector<ReserveOperator>;
    using CommitOperators       = std::vector<CommitOperator>;
    using ReleaseOperators      = std::vector<ReleaseOperator>;

    struct Sizes
    {
        SizeType    size_;
        SizeType    size_left_;
        SizeType    size_right_;

        Sizes()     = delete;

        explicit Sizes(SizeType size)   noexcept
            : size_(size)
            , size_left_(0)
            , size_right_(size)
        {}

        Sizes(Sizes&& sizes) noexcept
            : size_(std::move(sizes.size_))
            , size_left_(std::move(sizes.size_left_))
            , size_right_(std::move(sizes.size_right_))
        {}

        Sizes& operator=(Sizes&& sizes) noexcept
        {
            size_               = std::move(sizes.size_);
            size_left_          = std::move(sizes.size_left_);
            size_right_         = std::move(sizes.size_right_);

            return *this;
        }

        void reset()
        {
            size_left_  = 0;
            size_right_ = size_;
        }
    };

    struct Pointers
    {
        Type*   fixed_begin_;
        Type*   head_;
        Type*   tail_;
        Type*   second_tail_;
        Type*   promised_tail_;

        Pointers()  = delete;

        explicit Pointers(Buffer& buffer) noexcept
            : fixed_begin_(&buffer[0])
            , head_(&buffer[0])
            , tail_(&buffer[0])
            , second_tail_(&buffer[0])
            , promised_tail_(&buffer[0])
        {}

        Pointers(Pointers&& pointers) noexcept
            : fixed_begin_(std::move(pointers.fixed_begin_))
            , head_(std::move(pointers.head_))
            , tail_(std::move(pointers.tail_))
            , second_tail_(std::move(pointers.second_tail_))
            , promised_tail_(std::move(pointers.promised_tail_))
        {}

        Pointers& operator=(Pointers&& pointers) noexcept
        {
            fixed_begin_    = std::move(pointers.fixed_begin_);
            head_           = std::move(pointers.head_);
            tail_           = std::move(pointers.tail_);
            second_tail_    = std::move(pointers.second_tail_);
            promised_tail_  = std::move(pointers.promised_tail_);

            return *this;
        }

        void reset()
        {
            head_           = fixed_begin_;
            tail_           = fixed_begin_;
            second_tail_    = fixed_begin_;
            promised_tail_  = fixed_begin_;
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
        {
            reserve_operators_.clear();
            commit_operators_.clear();
            release_operators_.clear();
        }
    };

private:
    Buffer              buffer_;
    Sizes               sizes_;
    Pointers            pointers_;
    State               state_;
    Operators           operators_;
    ReadBlockOperator   movable_block_;
    Lock                lock_;
};

template<typename Type, typename SizeType, SizeType t_Size>
inline
decltype(auto) TransitBuffer<Type, SizeType, t_Size>::initialize() noexcept
{
    // reserve_operators_
    // State::Beginning
    operators_.reserve_operators_.emplace_back(
            [this](SizeType size_requested)
            {
                if (pointers_.promised_tail_ != pointers_.tail_)
                    return Block(pointers_.tail_, 0);

                SizeType space_available    = (pointers_.fixed_begin_ + sizes_.size_) - pointers_.tail_;
                SizeType capacity           = (size_requested >= space_available) ? space_available : size_requested;

                pointers_.promised_tail_    = pointers_.tail_ + capacity;
                sizes_.size_right_         -= capacity;

                return Block(pointers_.tail_, capacity);
            }
        );

    // State::Progressed
    operators_.reserve_operators_.emplace_back(
            [this](SizeType size_requested)
            {
                if (pointers_.promised_tail_ != pointers_.tail_)
                    return Block(pointers_.tail_, 0);

                SizeType capacity           = (size_requested >= sizes_.size_right_) ? sizes_.size_right_ : size_requested;

                pointers_.promised_tail_    = pointers_.tail_ + capacity;
                sizes_.size_right_         -= capacity;

                return Block(pointers_.tail_, capacity);
            }
        );

    // State::Secondary
    operators_.reserve_operators_.emplace_back(
            [this](SizeType size_requested)
            {
                if (    (pointers_.promised_tail_ != pointers_.tail_) &&
                        (pointers_.promised_tail_ != pointers_.second_tail_)    )
                    return Block(pointers_.second_tail_, 0);

                SizeType capacity           = (size_requested >= sizes_.size_left_) ? sizes_.size_left_ : size_requested;

                pointers_.promised_tail_    = pointers_.second_tail_ + capacity;
                sizes_.size_left_          -= capacity;

                return Block(pointers_.second_tail_, capacity);
            }
        );

    // commit_operators_
    // State::Beginning
    operators_.commit_operators_.emplace_back(
            [this](Handle& handle)
            {
                SizeType commit_size        = (handle.size_ < handle.capacity_) ? handle.size_ : handle.capacity_; 
                pointers_.tail_            += commit_size;
                sizes_.size_right_         += pointers_.promised_tail_ - pointers_.tail_;
                pointers_.promised_tail_    = pointers_.tail_;
                handle.data_                = pointers_.tail_;
                handle.capacity_            = 0;
                handle.size_                = 0;

                return;
            }
        );

    // State::Progressed
    operators_.commit_operators_.emplace_back(
            [this](Handle& handle)
            {
                SizeType commit_size        = (handle.size_ < handle.capacity_) ? handle.size_ : handle.capacity_; 
                pointers_.tail_            += commit_size;
                sizes_.size_right_         += pointers_.promised_tail_ - pointers_.tail_;
                pointers_.promised_tail_    = pointers_.tail_;
                handle.data_                = pointers_.tail_;
                handle.capacity_            = 0;
                handle.size_                = 0;

                if (pointers_.head_ == pointers_.tail_)
                {
                    state_                      = State::Beginning;
                    pointers_.head_             = pointers_.fixed_begin_;
                    pointers_.tail_             = pointers_.fixed_begin_;
                    pointers_.promised_tail_    = pointers_.fixed_begin_;
                    pointers_.second_tail_      = pointers_.fixed_begin_;
                    sizes_.size_left_           = 0;
                    sizes_.size_right_          = sizes_.size_;

                    return;
                }

                if (sizes_.size_left_ > sizes_.size_right_)
                    state_                  = State::Secondary;

                return;
            }
        );

    // State::Secondary
    operators_.commit_operators_.emplace_back(
            [this](Handle& handle)
            {
                if (pointers_.tail_ == pointers_.promised_tail_)
                    return;

                SizeType commit_size        = (handle.size_ < handle.capacity_) ? handle.size_ : handle.capacity_; 
                pointers_.second_tail_     += commit_size;
                sizes_.size_left_          += pointers_.promised_tail_ - pointers_.second_tail_;
                pointers_.promised_tail_    = pointers_.second_tail_;
                handle.data_                = pointers_.second_tail_;
                handle.capacity_            = 0;
                handle.size_                = 0;

                return;
            }
        );

    // release_operators_
    // State::Beginning
    operators_.release_operators_.emplace_back(
            [this](SizeType size_released)
            {
                if (size_released != 0)
                {
                    pointers_.head_            += size_released;
                    sizes_.size_left_          += size_released;

                    if (pointers_.head_ == pointers_.promised_tail_)
                    {
                        state_                      = State::Beginning;
                        pointers_.head_             = pointers_.fixed_begin_;
                        pointers_.tail_             = pointers_.fixed_begin_;
                        pointers_.promised_tail_    = pointers_.fixed_begin_;
                        pointers_.second_tail_      = pointers_.fixed_begin_;
                        sizes_.size_left_           = 0;
                        sizes_.size_right_          = sizes_.size_;

                        return;
                    }

                    if (pointers_.tail_ == pointers_.promised_tail_)
                    {
                        state_                      = (sizes_.size_right_ > sizes_.size_left_) ? State::Progressed : State::Secondary;

                        return;
                    }

                    if (pointers_.tail_ != pointers_.promised_tail_)
                        state_                      = State::Progressed;
                }

                return;
            }
        );

    // State::Progressed
    operators_.release_operators_.emplace_back(
            [this](SizeType size_released)
            {
                pointers_.head_            += size_released;
                sizes_.size_left_          += size_released;

                if (pointers_.head_ == pointers_.promised_tail_)
                {
                    state_                      = State::Beginning;
                    pointers_.head_             = pointers_.fixed_begin_;
                    pointers_.tail_             = pointers_.fixed_begin_;
                    pointers_.promised_tail_    = pointers_.fixed_begin_;
                    pointers_.second_tail_      = pointers_.fixed_begin_;
                    sizes_.size_left_           = 0;
                    sizes_.size_right_          = sizes_.size_;

                    return;
                }

                if (pointers_.tail_ == pointers_.promised_tail_ && sizes_.size_left_ > sizes_.size_right_)
                    state_                      = State::Secondary;

                return;
            }
        );

    // State::Secondary
    operators_.release_operators_.emplace_back(
            [this](SizeType size_released)
            {
                pointers_.head_            += size_released;
                sizes_.size_left_          += size_released;

                if (pointers_.head_ == pointers_.promised_tail_ && sizes_.size_left_ != 0)
                {
                    state_                      = State::Beginning;
                    pointers_.head_             = pointers_.fixed_begin_;
                    pointers_.tail_             = pointers_.fixed_begin_;
                    pointers_.promised_tail_    = pointers_.fixed_begin_;
                    pointers_.second_tail_      = pointers_.fixed_begin_;
                    sizes_.size_left_           = 0;
                    sizes_.size_right_          = sizes_.size_;

                    return;
                }

                if (pointers_.head_ == pointers_.tail_ && pointers_.head_ == pointers_.promised_tail_)
                {
                    state_                      = State::Beginning;
                    pointers_.head_             = pointers_.fixed_begin_;
                    pointers_.tail_             = pointers_.fixed_begin_;
                    pointers_.promised_tail_    = pointers_.fixed_begin_;
                    pointers_.second_tail_      = pointers_.fixed_begin_;
                    sizes_.size_left_           = 0;
                    sizes_.size_right_          = sizes_.size_;

                    return;
                }

                if (pointers_.head_ == pointers_.tail_)
                {
                    state_                      = State::Beginning;
                    pointers_.head_             = pointers_.fixed_begin_;
                    pointers_.tail_             = pointers_.second_tail_;
                    pointers_.second_tail_      = pointers_.fixed_begin_;
                    sizes_.size_right_          = (pointers_.fixed_begin_ + sizes_.size_) - pointers_.promised_tail_;
                    sizes_.size_left_           = 0;
                }

                return;
            }
        );

    movable_block_  =
        [this](SizeType size_requested)
        {
            SizeType    size = pointers_.tail_ - pointers_.head_;

            if (size > size_requested)
                size    = size_requested;

            return Block(pointers_.head_, size);
        };

    return;
}

template<typename Type, typename SizeType, SizeType t_Size>
inline
decltype(auto) TransitBuffer<Type, SizeType, t_Size>::buffer(SizeType size_requested) noexcept
{
    std::scoped_lock<Lock> lock(lock_);

    auto&& [movable_buffer, movable_size]   = operators_.reserve_operators_[to_index(state_)](size_requested);

    return Handle(this, movable_buffer, movable_size);
}

template<typename Type, typename SizeType, SizeType t_Size>
inline
decltype(auto) TransitBuffer<Type, SizeType, t_Size>::move(Type* move_buffer, const SizeType move_size) noexcept
{
    SizeType size = 0;

    do
    {
        SizeType    remaining_size  = move_size - size;
        Type*       buffer;
        SizeType    buffer_size;

        {
            std::scoped_lock<Lock> lock(lock_);

            auto&& [movable_buffer, movable_size] = operators_.reserve_operators_[to_index(state_)](remaining_size);

            buffer      = movable_buffer;
            buffer_size = movable_size;
        }

        if (buffer_size < remaining_size)
            remaining_size  = buffer_size;

        for (SizeType i = 0; i < remaining_size; ++i)
        {
            buffer[i]   = std::move(move_buffer[size++]);
        }

        // commit remaining_size
        Handle handle(this, buffer, buffer_size);
        handle.size_    = remaining_size;

        commit(handle);

    } while(size < move_size);

}

template<typename Type, typename SizeType, SizeType t_Size>
inline
decltype(auto) TransitBuffer<Type, SizeType, t_Size>::commit(Handle& handle) noexcept
{
    std::scoped_lock<Lock>  lock(lock_);
    operators_.commit_operators_[to_index(state_)](handle);
}

template<typename Type, typename SizeType, SizeType t_Size>
inline
decltype(auto) TransitBuffer<Type, SizeType, t_Size>::read(Type* destination_buffer, const SizeType read_size) noexcept
{
    auto&& [movable_buffer, movable_size]  = [this](auto size)
                {
                    std::scoped_lock<Lock>  lock(lock_);
                    return movable_block_(size);
                }(read_size);

    for (SizeType index = 0; index < movable_size; ++index)
        destination_buffer[index]   = std::move(movable_buffer[index]);

    [this](auto movable_size)
    {
        std::scoped_lock<Lock>  lock(lock_);
        operators_.release_operators_[to_index(state_)](movable_size);
    }(movable_size);

    return movable_size;
}

template<typename Type, typename SizeType, SizeType t_Size>
inline
decltype(auto) TransitBuffer<Type, SizeType, t_Size>::read(Type* destination_buffer, const SizeType destination_buffer_size, const Type* delimiter, const SizeType delimiter_size, CheckDelimiter check_delimiter) noexcept
{
    bool        delimiter_found     = false;
    SizeType    data_size           = 0;

    do
    {
        auto&& [movable_buffer, movable_size]  = [this]()
                    {
                        std::scoped_lock<Lock>  lock(lock_);
                        return movable_block_(std::numeric_limits<SizeType>::max());
                    }();

        SizeType index      = 0;

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

template<typename Type, typename SizeType, SizeType t_Size>
inline
decltype(auto) TransitBuffer<Type, SizeType, t_Size>::read_fixed_size(Type* destination_buffer, const int64_t read_size) noexcept
{
    SizeType    data_size           = 0;

    do
    {
        auto&& [movable_buffer, movable_size]  = [this]()
                    {
                        std::scoped_lock<Lock>  lock(lock_);
                        return movable_block_(std::numeric_limits<SizeType>::max());
                    }();

        SizeType index      = 0;

        while (index < movable_size && static_cast<int64_t>(data_size) < read_size)
        {
            destination_buffer[data_size++] = std::move(movable_buffer[index++]);
        }

        [this](auto index)
        {
            std::scoped_lock<Lock>  lock(lock_);
            operators_.release_operators_[to_index(state_)](index);
        }(index);

    } while(static_cast<int64_t>(data_size) < read_size);

    return;
}

template<typename Type, typename SizeType, SizeType t_Size>
inline
decltype(auto) TransitBuffer<Type, SizeType, t_Size>::read_block(SizeType size_requested) noexcept
{
    auto&& [movable_buffer, movable_size]  = [this](auto size)
                {
                    std::scoped_lock<Lock>  lock(lock_);
                    return movable_block_(size);
                }(size_requested);

    return Handle(this, movable_buffer, movable_size);
}

template<typename Type, typename SizeType, SizeType t_Size>
inline
decltype(auto) TransitBuffer<Type, SizeType, t_Size>::release(Handle& handle) noexcept
{
    std::scoped_lock<Lock>  lock(lock_);

    SizeType release_size   = (handle.size_ < handle.capacity_) ? handle.size_ : handle.capacity_;

    operators_.release_operators_[to_index(state_)](release_size);

    handle.capacity_        = 0;
    handle.size_            = 0;

    return;
}

}

#endif //_TRANSIT_BUFFER_HPP__
