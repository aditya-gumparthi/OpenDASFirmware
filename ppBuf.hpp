#ifndef PP_BUF_HPP
#define PP_BUF_HPP
#include <stddef.h>
#include "pico/mutex.h"

template <typename T, const size_t N>
class ppBuf
{
public:
    ppBuf()
    {
        mutex_init(&buffer_mutex);
    }
    ~ppBuf() { };

    size_t Size() const { return N; }
    size_t get_curr_w_ptr_loc() const { return write_ptr; }
    long unsigned int iteration_counter = 0;

    int current_buffer() const
    {
        return w_act_buf == m_data1;
    }

    size_t set_buffer_swap_threshold(size_t _n)
    {
        mutex_enter_blocking(&buffer_mutex);
        buf_limit = _n;
        size_t current_write_ptr = write_ptr;
        mutex_exit(&buffer_mutex);
        return current_write_ptr;
    }

    size_t append(T dt)
    {
        size_t retVal;
        mutex_enter_blocking(&buffer_mutex);
        if (write_ptr < buf_limit)
        {
            w_act_buf[write_ptr++] = dt;
            retVal = write_ptr;
        }
        else
        {
            swap_buffers();
            w_act_buf[write_ptr++] = dt;
            retVal = write_ptr;
        }
        mutex_exit(&buffer_mutex);
        return retVal;
    }

    T read_active_buf(size_t loc) const
    {
        if (loc < N)
            return r_act_buf[loc];
        else
            return r_act_buf[0];
    }

    T read_inactive_buf(size_t loc) const
    {
        if (loc < N)
            return w_act_buf[loc];
        else
            return w_act_buf[0];
    }

private:
    T m_data0[N];
    T m_data1[N];
    T *volatile w_act_buf = m_data0;
    size_t volatile write_ptr = 0;
    T *volatile r_act_buf = m_data1;
    size_t volatile buf_limit = N;
    mutable mutex_t buffer_mutex;

    void swap_buffers()
    {
        T *temp = w_act_buf;
        w_act_buf = r_act_buf;
        r_act_buf = temp;
        write_ptr = 0;
        iteration_counter++;
    }
};

#endif /* PP_BUF_HPP */
