//
// Created by archivist on 2/2/26.
//

#ifndef MAGIC_INSERTION_SORT_HPP
#define MAGIC_INSERTION_SORT_HPP

#include <algorithm>

namespace RedMagic {
    namespace details {
        enum class force_implementation {
            any,
            swap,
            rotate,
            mode_default = any
        };
    }

    using impl_t = details::force_implementation;

    template<
        typename Iter,
        typename IterE,
        typename Pred,
        impl_t implementation = impl_t::mode_default
    >
    void insertion_sort(Iter begin, IterE end, Pred pred) {
        if (std::distance(begin, end) <= 1) return;

        for (auto it = std::next(begin); it != end; ++it) {
            auto trailer = it;
            // The naive implementation is much faster on Clang for some reason
            // and the "fast" implementation is slower than the naive one by around 3x
#ifdef __clang__
            constexpr bool clang = true;
#else
            constexpr bool clang = false;
#endif
            constexpr bool easy_to_move =
                std::is_move_assignable_v<decltype(*begin)> or std::is_trivially_copy_assignable_v<decltype(*begin)>;
            constexpr bool cheap_to_swap = sizeof(*begin) < 32 and easy_to_move;

            if constexpr (
                implementation == impl_t::swap
                or (
                    clang
                    and cheap_to_swap
                    and implementation != impl_t::rotate
                )
            ) {
                while (trailer != begin && pred(*trailer, *std::prev(trailer))) {
                    std::swap(*trailer, *std::prev(trailer));
                    --trailer;
                }
            } else {
                auto insertion_point = trailer;
                while (insertion_point != begin && pred(*trailer, *std::prev(insertion_point))) {
                    --insertion_point;
                }
                if (insertion_point != trailer) {
                    std::rotate(insertion_point, trailer, std::next(trailer));
                }
            }
        }
    }
}

#endif //MAGIC_INSERTION_SORT_HPP