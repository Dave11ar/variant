#pragma once

#include <type_traits>
#include <utility>
#include "helpers.h"
#include "variadic_storage.h"

namespace var {
template <bool, typename ...Types>
union variadic_union {
};

template <typename First,  typename ...Rest>
union variadic_union<false, First, Rest...> {
  ~variadic_union() {}

  constexpr variadic_union() : rest() {}

  template <size_t I, typename ...Args>
  constexpr variadic_union(in_place_index_t<I>, Args&&... args)
      : rest(in_place_index<I - 1>, std::forward<Args>(args)...) {}

  template <typename ...Args>
  constexpr variadic_union(in_place_index_t<0>, Args&&... args)
      : first(std::forward<Args>(args)...) {}


  template<size_t I>
  constexpr auto &get(in_place_index_t<I>) {
    return rest.get(in_place_index<I - 1>);
  }
  constexpr auto &get(in_place_index_t<0>) {
    return first;
  }

  template<size_t I>
  constexpr auto const &get(in_place_index_t<I>) const {
    return rest.get(in_place_index<I - 1>);
  }
  constexpr auto const &get(in_place_index_t<0>) const {
    return first;
  }

  template<size_t I>
  constexpr void destroy(in_place_index_t<I>) {
    rest.destroy(in_place_index<I - 1>);
  }
  void destroy(in_place_index_t<0>) {
    first.~First();
  }

  template <size_t I, typename ...Args>
  void construct(in_place_index_t<I>, Args &&...args) {
    rest.construct(in_place_index<I - 1>, std::forward<Args>(args)...);
  }
  template <typename ...Args>
  void construct(in_place_index_t<0>, Args &&...args) {
    new(const_cast<std::remove_cv_t<First>*>(&first)) First(std::forward<Args>(args)...);
  }

  template <size_t index, typename T>
  static constexpr size_t get_type_index(in_place_type_t<T>) {
    return variadic_union<false, Rest...>::template get_type_index<index + 1>(in_place_type<T>);
  }

  template <size_t index>
  static constexpr size_t get_type_index(in_place_type_t<First>) {
    return index;
  }

  constexpr variadic_union(variadic_union const &other) = default;
  constexpr variadic_union (variadic_union &&other) = default;
  constexpr variadic_union &operator=(variadic_union const &other) = default;
  constexpr variadic_union &operator=(variadic_union &&other) = default;
 private:
  First first;
  variadic_union<false, Rest...> rest;
};

template <typename First,  typename ...Rest>
union variadic_union<true, First, Rest...> {
  ~variadic_union() = default;

  constexpr variadic_union() : rest() {}

  template <size_t I, typename ...Args>
  constexpr variadic_union(in_place_index_t<I>, Args&&... args)
      : rest(in_place_index<I - 1>, std::forward<Args>(args)...) {}

  template <typename ...Args>
  constexpr variadic_union(in_place_index_t<0>, Args&&... args)
      : first(std::forward<Args>(args)...) {
  }


  template<size_t I>
  constexpr auto &get(in_place_index_t<I>) {
    return rest.get(in_place_index<I - 1>);
  }
  constexpr auto &get(in_place_index_t<0>) {
    return first;
  }

  template<size_t I>
  constexpr auto const &get(in_place_index_t<I>) const {
    return rest.get(in_place_index<I - 1>);
  }
  constexpr auto const &get(in_place_index_t<0>) const {
    return first;
  }

  template<size_t I>
  constexpr void destroy(in_place_index_t<I>) {
    rest.destroy(in_place_index<I - 1>);
  }
  void destroy(in_place_index_t<0>) {
    first.~First();
  }

  template <size_t I, typename ...Args>
  void construct(in_place_index_t<I>, Args &&...args) {
    rest.construct(in_place_index<I - 1>, std::forward<Args>(args)...);
  }
  template <typename ...Args>
  void construct(in_place_index_t<0>, Args &&...args) {
    new(const_cast<std::remove_cv_t<First>*>(&first)) First(std::forward<Args>(args)...);
  }

  constexpr variadic_union(variadic_union const &other) = default;
  constexpr variadic_union (variadic_union &&other) = default;
  constexpr variadic_union &operator=(variadic_union const &other) = default;
  constexpr variadic_union &operator=(variadic_union &&other) = default;
private:
  First first;
  variadic_union<true, Rest...> rest;
};
} // end of var namespace
