#ifndef MAGIC_INDEXED_VECTOR_HPP
#define MAGIC_INDEXED_VECTOR_HPP
#include <algorithm>
#include <cstdint>
#include <ranges>
#include <vector>

#include "cassert"

namespace RedMagic {
    /**
     * This is a contiguous container where data is not addressed by order of
     * insertion, but by an arbitrary index. The index remains stable even when
     * other indices are removed, or when the iteration order is changed.
     *
     * Unless the `indexed_vector` needs to be resized, or elements are sorted, the
     * data structure is pointer stable. Insertion within capacity doesn't change
     * the position of other elements.
     *
     * @tparam T The data to store
     * @tparam index_t The type of the index to use, using a full size_t is
     * probably overkill depending on the use-case
     */
    template<typename T, typename index_t = uint32_t>
    class indexed_vector {
        std::vector<T> data;
        std::vector<index_t> indices;
        std::vector<index_t> back_indices;
        std::vector<index_t> marked_for_deletion;
    public:
        using value_type = T;
        using index_type = index_t;

        auto begin() {
            return data.begin();
        }

        auto end() {
            return data.end();
        }

        auto cbegin() const {
            return data.cbegin();
        }

        auto cend() const {
            return data.cbegin();
        }

        auto begin() const {
            return data.cbegin();
        }

        auto end() const {
            return data.cbegin();
        }

        [[nodiscard]] bool has_element(index_t idx) const {
            return idx < indices.size();
        }

        /**
         * Inserts a new element inside the backing array
         * @note This is one of the operations that may break pointer stability, but only if `size() == capacity()`
         * @param value The value to insert
         * @return The index used to access the element
         */
        index_t push_back(const T& value) {
            if (data.size() < back_indices.size()) {
                index_t index = data.size();
                index_t back = back_indices[index];
                index_t forward = indices[back];
                data.push_back(value);
                return forward;
            } else {
                index_t index = data.size();
                data.push_back(value);
                indices.push_back(index);
                back_indices.push_back(index);
                return index;
            }
        }

        /**
         * Inserts a new element inside the backing array
         * @note This is one of the operations that may break pointer stability, but only if `size() == capacity()`
         * @param value The value to insert
         * @return The index used to access the element
         */
        index_t push_back(T value) {
            index_t index = data.size();
            if (data.size() < back_indices.size()) {
                index_t back = back_indices[index];
                index_t forward = indices[back];
                data.emplace_back(std::move(value));
                return forward;
            } else {
                data.push_back(std::move(value));
                indices.push_back(index);
                back_indices.push_back(index);
                return index;
            }
        }

        /**
         * Constructs a new element in place inside the backing array
         * @note This is one of the operations that may break pointer stability, but only if `size() == capacity()`
         * @param args arguments for the constructor of the type, perfect forwarded.
         * @return The index used to access the element
         */
        index_t emplace_back(auto&&... args) {
            index_t index = data.size();
            if (data.size() < back_indices.size()) {
                index_t back = back_indices[index];
                index_t forward = indices[back];
                data.emplace_back(std::forward<decltype(args)>(args)...);
                return forward;
            } else {
                data.emplace_back(std::forward<decltype(args)>(args)...);
                indices.push_back(index);
                back_indices.push_back(index);
                return index;
            }
        }

        /**
         * Remove the element with the provided index.
         * @note internal arrays are not cleared, but are swapped with the last
         * element instead. That is with the notable exception of the array that
         * holds the actual stored type which is cleared
         * @note This is one of the operations that may break pointer stability
         * @param index The index of the element to remove
         */
        void remove(index_t index) {
            assert(not data.empty() && "indexed_array::remove: no data to remove");
            assert(index < indices.size() && "indexed_array::remove: index beyond the scope");
            index_t elem = indices[index];
            assert(elem < data.size() && "indexed_array::remove: index does not exist");
            if (elem >= data.size()) return;
            if (elem == data.size() - 1) { data.pop_back(); return; }
            std::swap(data[elem], data[data.size() - 1]);
            std::swap(back_indices[elem], back_indices[data.size() - 1]);
            std::swap(indices[back_indices[elem]], indices[back_indices[data.size() - 1]]);
            data.pop_back();
        }

        /**
         * @invariant pred is evaluated exactly once on each element. Elements are valid at least until the predicate returns
         * @note This is one of the operations that may break pointer stability
         * @param pred a binary predicate that returns true for every element that should be deleted
         */
        void remove_if(auto pred) {
            // We iterate backwards, this way we do not invalidate iterators we have not reached yed
            for (auto idx : back_indices | std::views::reverse) {
                auto indexed_as = indices[idx];
                if (indexed_as < data.size() and pred(data[indexed_as])) {
                    remove(indexed_as);
                }
            }
        }

        /**
         * Mark an index to be removed later
         * @param index the index to mark
         */
        void mark_for_discard(index_t index) {
            marked_for_deletion.push_back(index);
        }

        /**
         * Clears all indexed marked for removal
         * @note This is one of the operations that may break pointer stability
         * @return whether any work was actually performed
         */
        bool collect_all_marks() {
            const bool did_delete = not marked_for_deletion.empty();
            std::ranges::sort(marked_for_deletion, std::less{});
            while (not marked_for_deletion.empty()) {
                remove(marked_for_deletion.back());
                marked_for_deletion.pop_back();
            }
            return did_delete;
        }

        auto& operator[](index_t index) {
            return data[indices[index]];
        }

        const auto& operator[](index_t index) const {
            return data[indices[index]];
        }

        index_t reverse_search(T* ptr) const {
            assert(ptr - data.data() < data.size() && "indexed_array::reverse_search: Pointer out of bounds. Did we move?");
            return back_indices[ptr - data.data()];
        }

        [[nodiscard]] bool empty() const {return size() == 0;}
        [[nodiscard]] bool full() const {return false;}
        [[nodiscard]] size_t size() const {return data.size();}
        [[nodiscard]] size_t capacity() const {return data.capacity();}
        [[nodiscard]] std::span<T> span() {return {data.data(), data.size()};}

        /**
         * Reserves space for future insertions.
         * @warning Do not use in a loop. Make a heuristic once and then rely on
         * normal resizing, or just do not use this.
         * @note This is one of the operations that may break pointer stability
         * @param capacity The minimal new capacity to be matched
         */
        void reserve(size_t capacity) {
            data.reserve(capacity);
            indices.reserve(capacity);
            back_indices.reserve(capacity);
        }

        /**
         * This sorts the data for when it gets *iterated*, this operates under the assumption that data is already nearly sorted
         * @param Pred The predicate by which to sort
         * @note This is one of the operations that may break pointer stability
         * @internal
         * This uses insertion sort, specifically a swap based implementation. I could have used a rotate based one,
         * but I would still have had to swap the indices array all over the place.
         * @endinternal
         */
        void sort(std::invocable<const T&, const T&> auto Pred) {
            if (size() < 2) { return; }
            for (auto it = 1; it != data.size(); ++it) {
                auto trailer = it;

                while (trailer != 0 && Pred(data[trailer], data[trailer - 1])) {
                    std::swap(data[trailer], data[trailer - 1]);
                    std::swap(back_indices[trailer], back_indices[trailer - 1]);
                    std::swap(indices[back_indices[trailer]], indices[back_indices[trailer - 1]]);
                    --trailer;
                }
            }
        }

        /**
         * This sorts the data for when it gets *iterated*, this operates under the assumption that data is already sorted
         * @param Pred The predicate by which to sort
         * @param Proj A projection to use to sort the values
         * @note All the values are projected once, and we then swap the projections with the data. This avoids thousands of
         * stupid projections
         * @note This is one of the operations that may break pointer stability
         * @internal
         * This uses insertion sort, specifically a swap based implementation. I could have used a rotate based one,
         * but I would still have had to swap the indices array all over the place.
         * @endinternal
         */
        void sort(auto Pred, auto Proj) {
            if (size() < 2) { return; }
            using projected_t = std::invoke_result_t<decltype(Proj), const T&>;
            std::vector<projected_t> P;
            P.reserve(size());
            for (const auto& elem : data) {
                P.emplace_back(Proj(elem));
            }
            for (auto it = 1; it != data.size(); ++it) {
                auto trailer = it;

                while (trailer != 0 && Pred(P[trailer], P[trailer - 1])) {
                    std::swap(P[trailer], P[trailer - 1]);
                    std::swap(data[trailer], data[trailer - 1]);
                    std::swap(back_indices[trailer], back_indices[trailer - 1]);
                    std::swap(indices[back_indices[trailer]], indices[back_indices[trailer - 1]]);
                    --trailer;
                }
            }
        }
    };
}

#endif //MAGIC_INDEXED_VECTOR_HPP