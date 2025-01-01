#ifndef SPC_MYSET_H
#define SPC_MYSET_H

#include <iostream>
#include "myvec.h"
#include <algorithm>

namespace spc {

/**
 * @class myset
 * @brief A custom implementation of a set container that prevents duplicate elements.
 * @tparam T The type of elements stored in the set.
 */
template <typename T>
class myset {
private:
    spc::myvec<T> elements; ///< Internal storage for elements in the set.

public:
    /**
     * @brief Default constructor for myset.
     */
    myset() = default;

    /**
     * @brief Inserts a value into the set if it is not already present.
     * @param value The value to insert.
     */
    void insert(const T& value) {
        if (std::find(elements.begin(), elements.end(), value) == elements.end()) {
            elements.push_back(value);
        }
    }

    /**
     * @brief Checks if a value exists in the set.
     * @param value The value to search for.
     * @return True if the value exists in the set, false otherwise.
     */
    bool find(const T& value) const {
        return std::find(elements.begin(), elements.end(), value) != elements.end();
    }

    /**
     * @brief Gets the number of elements in the set.
     * @return The size of the set.
     */
    size_t size() const {
        return elements.size();
    }

    /**
     * @brief Checks if the set is empty.
     * @return True if the set is empty, false otherwise.
     */
    bool empty() const {
        return elements.empty();
    }

    /**
     * @brief Clears all elements from the set.
     */
    void clear() {
        elements.clear();
    }

    /**
     * @brief Returns an iterator to the beginning of the set.
     * @return Pointer to the first element in the set.
     */
    T* begin() { return elements.begin(); }  

    /**
     * @brief Returns an iterator to the end of the set.
     * @return Pointer to one past the last element in the set.
     */
    T* end() { return elements.end(); }

    /**
     * @brief Returns a constant iterator to the beginning of the set.
     * @return Constant pointer to the first element in the set.
     */
    const T* begin() const { return elements.begin(); }  

    /**
     * @brief Returns a constant iterator to the end of the set.
     * @return Constant pointer to one past the last element in the set.
     */
    const T* end() const { return elements.end(); } 

};

} // namespace spc

#endif // SPC_MYSET_H
