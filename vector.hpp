#pragma once

#include <eoslib/types.hpp>
#include <eoslib/system.h>
#include <eoslib/memory.hpp>

using namespace eosio;

template<typename T>
class vector {

private:
  T* begin = nullptr;
  T* end = nullptr;
  T* capacity_end = nullptr;

  void allocate(size_t size) {
    if (size > 0) {
      begin = malloc(size * sizeof(T));
      assert(begin != nullptr, "malloc failed");
      capacity_end = begin + size;
      end = begin;
    }
  }

  void reallocate(size_t size) {
    free();
    allocate(size);
  }

  void free() {
    if (begin != nullptr) {
      free(begin);
      begin = end = capacity_end = nullptr;
    }
  }

  size_t is_full() {
    return end == capacity_end;
  }

  void push_back_(const T& val) {
    //assert( !is_full(), "No capacity left");
    *end = val;
    end += 1;
  }

  void pop_back_() {
    *(end - 1).~T();
    end--;
  }

  void fill(size_t n, const T& val) {
    assert(get_capacity() >= n, "Capacity too small to fill that many elements");
    for (size_t i = 0; i < n; i++) {
      push_back_(val);
    }
  }

  void copy(const vector<T>& x) {
    size_t size = x.get_size();
    if (get_capacity() < size)
      reallocate(size);

    for (size_t i = 0; i < size; i++) {
      push_back_(x[i]);
    }
  }

public:
  /**
   * Default constructor.
   * Constructs empty container with no elements
   */
  vector() {};

  /**
   * Fill constructor.
   * Constructs container with n elements, each element is a copy of val
   */
  vector(size_t n, const T& val) {
    allocate(n);
    fill(n, val);
  }

  vector(size_t n) : vector(n, T()) {}

  //TODO:
  /**
   * Range constructor.
   * Constructs a container with as many elements as the range [first,last),
   * with each element emplace-constructed from its corresponding element in that range,
   * in the same order.
   */
  //vector()

  /**
   * Copy constructor.
   * Constructs a container with a copy of each of the elements in x, in the same order.
   */
  vector(const vector<T>& x) {
    copy(x);
  }

  /**
   * Move constructor.
   * Constructs a container that acquires the elements of x.
   */
  vector(vector<T>&& x) = default;

  vector operator=(const vector& vec);

  vector operator=(vector&& vec);


  size_t get_size() const {
    return (begin != nullptr) ? end - begin : 0;
  }

  size_t get_capacity() const {
    return (begin != nullptr) ? capacity_end - begin : 0;
  }

  void resize(size_t s) {

  }

  void resize(size_t s, const T& val) {
    resize(s);
  }


  bool is_empty() const {
    return get_size() == 0;
  }

  void reserve(size_t capacity);

  void shrink_to_fit();


  const T& operator[](size_t index) const {
    return at(index);
  }

  T& operator[](size_t index) {
    return at(index);
  }

  const T& at(size_t index) const;

  T& at(size_t index);

  const T& front() const {
    return at(0);
  }

  T& front() {
    return at(0);
  }

  const T& back() const;

  T& back();

  const T* get_data() const {
    return data;
  }

  T* get_data() {
    return data;
  }


  void assign(size_t n, const T* val);

  void push_back(const T& val);

  void push_back(T&& val);

  void pop_back();

  void clear() {
    for (size_t i = 0; i < get_size(); i++) {
      pop_back_();
    }
  }

  //TODO: inserts, erase, swap, emplace, emplace_back
};
