
#include <common.h>
#include <intrin.h>

#pragma intrinsic(_BitScanForward)

unsigned char scanForwardBits(char max_val, unsigned long arg) {
    unsigned long index;
    unsigned char b = _BitScanForward(&index, arg);
    index = max_val - index;
    if (!b) index = 0;
    return (unsigned char)index;
}

#define new_arr(TYPE, LEN) new TYPE[(LEN)]

std::vector<unsigned char> createChannelsMap(const std::vector<int>& vec) {
    /* parse arguments */

    int int_list_len = 0;

    int_list_len = (int)vec.size();
    if (int_list_len <= 0) return {};

    long* int_arr = new_arr(long, int_list_len);

    int max_e = 0;
    int min_e = vec[0];
    for (int idx = 0; idx < int_list_len; idx++) {
        int e = vec[idx];
        if (max_e < e) max_e = e;
        if (e < min_e) min_e = e;

        int_arr[idx] = e;
    }

    //////////////////
    // redefine
    unsigned long freq_arr_size = int_list_len;
    long* freq_arr = int_arr;
    //////////////////

    max_e -= min_e;
    long idx_arr_size = 0;
    long time_ponts_size = 1 << (char)max_e;

    for (long *e_ptr = freq_arr, *e_ptr_end = (freq_arr + freq_arr_size); e_ptr < e_ptr_end; e_ptr++) {
        *e_ptr -= min_e;
        idx_arr_size += 1 << (char)*e_ptr;
    }

    std::vector<unsigned char> ret_vector(idx_arr_size, 0);
    ret_vector.reserve(idx_arr_size);
    auto* idx_arr = ret_vector.data();

    {  // isolated namespace
        unsigned char* e_ptr = idx_arr;
        for (long pos = 0; pos < time_ponts_size; pos++) {
            unsigned char pos_val = scanForwardBits((char)max_e, pos);
            for (unsigned char fr_idx = 0; fr_idx < (unsigned char)freq_arr_size; fr_idx++) {
                if (pos_val <= freq_arr[fr_idx]) {
                    *(e_ptr++) = fr_idx;
                }
            }
        }
    }
    delete[] int_arr;

    return ret_vector;
}