#ifndef _CHAR_STREAM_HPP__
#define _CHAR_STREAM_HPP__

namespace util
{

template<typename Buffer>
class CharStream
{
public:
    using BType = typename Buffer::value_type;

    CharStream(Buffer& buff)
        : buffer_(buff)
    {}

    CharStream& operator<<(const char* const str)
    {
        char* ptr       = const_cast<char*>(str);
        auto handle     = buffer_.buffer();
        auto capacity   = handle.capacity_;

        while (capacity > 0 && *ptr != '\0')
        {
            handle.data()[handle.size_++]   = *(reinterpret_cast<BType*>(ptr));

            ptr++;
            capacity--;

            if (capacity == 0)
            {
                buffer_.commit(handle);

                handle      = buffer_.buffer();
                capacity    = handle.capacity_;
            }
        }

        buffer_.commit(handle);

        return *this;
    }

    CharStream(const CharStream&)               = delete;
    CharStream& operator=(const CharStream&)    = delete;

private:
    Buffer&     buffer_;
};

template<>
class CharStream<char*>
{
public:
    CharStream(char* buff, int size)
        : buffer_(buff)
        , size_(size)
        , index_(0)
    {}

    CharStream& operator<<(const char* const str)
    {
        const char* ptr = str;
        auto capacity   = size_ - index_;

        while (capacity > 0 && *ptr != '\0')
        {
            buffer_[index_++]   = *ptr++;
            capacity--;
        }

        return *this;
    }

    char* data()
    {
        return buffer_;
    }

    int length()
    {
        return index_;
    }

    CharStream(const CharStream&)               = delete;
    CharStream& operator=(const CharStream&)    = delete;

private:
    char*   buffer_;
    int     size_;
    int     index_;
};

}

#endif //_CHAR_STREAM_HPP__
