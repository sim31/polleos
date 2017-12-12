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
    if ( size > 0 ) {
      begin = malloc(size * sizeof(T));
      assert(begin != nullptr, "malloc failed");
      capacity_end = begin + size;
      end = begin;
    }
  }

  void reallocate(size_t capacity, bool save) {
    if ( save && !is_empty() ) {
      T* nbegin = malloc(capacity * sizeof(T));
      assert(nbegin != nullptr, "malloc failed");
      size_t new_size = min(capacity, get_size());
      move_to(nbegin, new_size);
      free();
      begin = nbegin;
      end = begin + new_size;
      capacity_end = begin + capacity;
    }
    else {
      free();
      allocate(capacity);
    }
  }

  void move_to(T* dest, size_t n) {
    auto it = begin;
    for (size_t i = 0; i < n; i++, it++, dest++) {
      new(dest) T(it);
    }
  }

  void reallocate_if_full() {
    if ( is_full() ) {
      reallocate(get_capacity() * 2, true);
    }
  }

  void free() {
    if ( begin != nullptr ) {
      clear();
      free(begin);
      begin = end = capacity_end = nullptr;
    }
  }

  size_t is_full() const {
    return end == capacity_end;
  }

  void _push_back(const T& val) {
    //assert( !is_full(), "No capacity left");
    new(end++) T(val);
  }

  void _push_back(const T&& val) {
    //assert( !is_full(), "No capacity left");
    new(end++) T( static_cast<T&&>(val) );
  }

  void _push_back(const T& val, size_t n) {
    assert(get_capacity() >= n, "Capacity too small to fill that many elements");
    for (size_t i = 0; i < n; i++) {
      _push_back(val);
    }
  }

  void _pop_back() {
    *(--end).~T();
  }

  void _pop_back(size_t n) {
    for (size_t i = 0; i < n; i++) {
      _pop_back();
    }
  }

  void copy(const vector<T>& x) {
    for (size_t i = 0; i < x.get_size(); i++) {
      _push_back(x[i]);
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
    _push_back(val, n);
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
    allocate(x.get_capacity());
    copy(x);
  }

  /**
   * Move constructor.
   * Constructs a container that acquires the elements of x.
   */
  vector(vector<T>&& x) = default;

  ~vector() {
    free();
  }

  vector operator=(const vector& vec) {
    if ( get_capacity() < vec.get_size() )
      reallocate(vec.get_capacity(), false);
    else
      clear();

    copy(vec);
  }

  vector operator=(vector&& vec) {
    free();
    begin = vec.begin;
    end = vec.end;
    capacity_end = vec.capacity_end;
  }

  size_t get_size() const {
    return (begin != nullptr) ? end - begin : 0;
  }

  size_t get_capacity() const {
    return (begin != nullptr) ? capacity_end - begin : 0;
  }

  void resize(size_t s) {
    resize(s, T());
  }

  void resize(size_t s, const T& val) {
    size_t current_s = get_size();
    if ( s < current_s ) {
      _pop_back(current_s - s);
    }
    else if ( s > current_s) {
      reserve(s);
      _push_back(val, current_s - s);
    }
  }

  bool is_empty() const {
    return get_size() == 0;
  }

  void reserve(size_t capacity) {
    if ( capacity > get_capacity() ) {
      reallocate(capacity, true);
    }
  }

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
    return begin;
  }

  T* get_data() {
    return begin;
  }


  void assign(size_t n, const T* val);

  void push_back(const T& val) {
    reallocate_if_full();
    _push_back(val);
  }

  void push_back(T&& val) {
    reallocate_if_full();
    //TODO: implement std::move and std::forward
    _push_back(static_cast<T&&>(val));
  }

  void pop_back();

  // Destroys objects in the container, does not free allocated capacity
  void clear() {
    for (size_t i = 0; i < get_size(); i++) {
      _pop_back();
    }
  }

  //TODO: inserts, erase, swap, emplace, emplace_back
};
