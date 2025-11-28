#ifndef MYVEC_H
#define MYVEC_H

#include <stdexcept>
#include <iterator>

#include <iostream>

namespace spc
{

    /**
     * @class myvec
     * @brief A custom dynamic array implementation similar to std::vector.
     * @tparam T The type of elements stored in the vector.
     */
    template <typename T>
    class myvec
    {
    public:
        /**
         * @brief Constructor to initialize the vector with a specified capacity.
         * @param cap The initial capacity of the vector (default is 10).
         * @throws std::invalid_argument if the capacity is not positive.
         */
        explicit myvec(int cap = 10)
            : size(0), capacity(cap)
        {
            if (capacity <= 0)
                throw std::invalid_argument("Capacity must be positive");
            data = new T[capacity];
        }

        /**
         * @brief Destructor to release allocated memory.
         */
        ~myvec()
        {
            delete[] data;
        }

        /**
         * @brief Copy constructor to create a new vector as a copy of another.
         * @param other The vector to copy from.
         */
        myvec(const myvec &other)
            : size(other.size), capacity(other.capacity)
        {
            data = new T[capacity];
            for (int i = 0; i < size; ++i)
            {
                data[i] = other.data[i];
            }
        }

        /**
         * @brief Move constructor to transfer ownership of resources.
         * @param other The vector to move from.
         */
        myvec(myvec &&other) noexcept
            : data(other.data), size(other.size), capacity(other.capacity)
        {
            other.data = nullptr;
            other.size = 0;
            other.capacity = 0;
        }

        /**
         * @brief Copy assignment operator to copy elements from another vector.
         * @param other The vector to copy from.
         * @return A reference to this vector.
         */
        myvec &operator=(const myvec &other)
        {
            if (this != &other)
            {
                delete[] data;
                size = other.size;
                capacity = other.capacity;
                data = new T[capacity];
                for (int i = 0; i < size; ++i)
                {
                    data[i] = other.data[i];
                }
            }
            return *this;
        }

        /**
         * @brief Move assignment operator to transfer resources from another vector.
         * @param other The vector to move from.
         * @return A reference to this vector.
         */
        myvec &operator=(myvec &&other) noexcept
        {
            if (this != &other)
            {
                delete[] data;
                data = other.data;
                size = other.size;
                capacity = other.capacity;
                other.data = nullptr;
                other.size = 0;
                other.capacity = 0;
            }
            return *this;
        }

        /**
         * @brief Adds an element to the end of the vector (move version).
         * @param val The value to move into the vector.
         */
        void push_back(T &&val)
        {
            if (size == capacity)
            {
                resize();
            }
            data[size++] = std::move(val);
        }

        /**
         * @brief Adds an element to the end of the vector (copy version).
         * @param val The value to copy into the vector.
         */
        void push_back(const T &val)
        {
            if (size == capacity)
            {
                resize();
            }
            data[size++] = val;
        }

        /**
         * @brief Access operator to get or modify an element by index.
         * @param index The index of the element to access.
         * @return A reference to the element.
         */
        T &operator[](int index)
        {
            return data[index];
        }

        /**
         * @brief Access operator to get an element by index (const version).
         * @param index The index of the element to access.
         * @return A const reference to the element.
         */
        const T &operator[](int index) const
        {
            return data[index];
        }

        /**
         * @brief Get the current size of the vector.
         * @return The number of elements in the vector.
         */
        int get_size() const { return size; }

        /**
         * @brief Get the current capacity of the vector.
         * @return The maximum number of elements the vector can hold.
         */
        int get_capacity() const { return capacity; }

        /**
         * @brief Range-based constructor to initialize the vector from iterators.
         * @tparam InputIterator The type of the input iterator.
         * @param first The beginning of the range.
         * @param last The end of the range.
         */
        template <typename InputIterator>
        myvec(InputIterator first, InputIterator last)
        {
            size = std::distance(first, last);
            capacity = size;
            data = new T[capacity];
            int index = 0;
            for (auto it = first; it != last; ++it)
            {
                data[index++] = *it;
            }
        }

        /**
         * @brief Clear all elements from the vector.
         */
        void clear()
        {
            size = 0;
        }

        /**
         * @brief Check if the vector is empty.
         * @return True if the vector is empty, false otherwise.
         */
        bool empty()
        {
            return size == 0;
        }

        /**
         * @brief Get an iterator to the beginning of the vector.
         * @return A pointer to the first element.
         */
        T *begin() { return data; }

        /**
         * @brief Get an iterator to the end of the vector.
         * @return A pointer to one past the last element.
         */
        T *end() { return data + size; }

        /**
         * @brief Get a const iterator to the beginning of the vector.
         * @return A const pointer to the first element.
         */
        const T *begin() const { return data; }

        /**
         * @brief Get a const iterator to the end of the vector.
         * @return A const pointer to one past the last element.
         */
        const T *end() const { return data + size; }

    private:
        T *data;        /**< Pointer to the array storing the elements. */
        int size;       /**< The current number of elements in the vector. */
        int capacity;   /**< The current capacity of the vector. */

        /**
         * @brief Resize the vector to accommodate more elements.
         */
        void resize()
        {
            capacity *= 2;
            T *new_data = new T[capacity];
            for (int i = 0; i < size; ++i)
            {
                new_data[i] = std::move(data[i]);
            }
            delete[] data;
            data = new_data;
        }
    };

} // namespace spc

#endif // MYVEC_H
