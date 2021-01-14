#pragma once
#include "helpers.h"

template <typename ...Types>
struct variant_traits {
  template<size_t I, typename ...Rest>
  struct nth {
    using type = void;
  };

  template<size_t I, typename First, typename ...Rest>
  struct nth<I, First, Rest...> {
    using nth<I - 1, Rest...>::type;
  };

  template<typename First, typename... Rest>
  struct nth<0, First, Rest...> {
    using type = First;
  };

  template <size_t I, typename ...F_Types>
  using nth_t = typename nth<I, F_Types...>::type;


  template<typename T>
  struct is_in_place {
    static constexpr bool value = true;
  };

  template<typename T>
  struct is_in_place<in_place_type_t<T>> {
    static constexpr bool value = false;
  };

  template<size_t I>
  struct is_in_place<in_place_index_t<I>> {
    static constexpr bool value = false;
  };

  template <typename T>
  static constexpr bool is_in_place_v = is_in_place<T>::value;


  template <typename T, typename ...F_Types>
  struct count_type {
    static constexpr size_t value = 0;
  };

  template <typename T, typename First, typename ...Rest>
  struct count_type<T, First, Rest...> {
    static constexpr size_t value = std::is_same_v<T, First> + count_type<T, Rest...>::value;
  };

  template <typename T, typename ...F_Types>
  static constexpr size_t count_type_v = count_type<T, F_Types...>::value;

  template <size_t I, typename First, typename ...Rest>
  static constexpr size_t get_type_index(in_place_type_t<First>) {
    return I;
  }

  /**
   * traits
   */
  static constexpr bool default_constructible = std::is_default_constructible_v<nth_t<0, Types...>>;
  static constexpr bool copy_constructible = (std::is_copy_constructible_v<Types> && ...);
  static constexpr bool move_constructible = (std::is_move_constructible_v<Types> && ...);
  template <typename T, typename ...F_Types>
  static constexpr bool in_place_type_constructible = std::is_constructible_v<T, F_Types...> && (count_type_v<T, Types...> == 1);
  template <size_t I, typename ...F_Types>
  static constexpr bool in_place_index_constructible = (I < sizeof...(Types)) &&
                                                       std::is_constructible_v<variant_alternative_t<I, variant<Types...>>, F_Types...>;
  template <typename T>
  static constexpr bool converting_constructible = (sizeof...(Types) > 0) &&
                                          (!std::is_same_v<variant<Types...>, std::decay_t<T>>) &&
                                          (is_in_place_v<T>) &&
                                          (std::is_constructible_v<variant_alternative_t<index_chooser_v<T, variant<Types...>>, variant<Types...>>, T>);


  static constexpr bool copy_assignable = copy_constructible && (std::is_copy_assignable_v<Types> && ...);
  static constexpr bool move_assignable = move_constructible && (std::is_move_assignable_v<Types> && ...);
  template <typename T>
  static constexpr bool converting_assignable = converting_constructible<T> &&
      std::is_assignable_v<variant_alternative_t<index_chooser_v<T, variant<Types...>>, variant<Types...>>, T>;


  static constexpr bool nothrow_default_constructible = std::is_nothrow_default_constructible_v<nth_t<0, Types...>>;
  static constexpr bool nothrow_move_constructible = (std::is_nothrow_move_constructible_v<Types> && ...);
  static constexpr bool nothrow_move_assignable =
      nothrow_move_constructible && (std::is_nothrow_move_assignable_v<Types> && ...);
  template <typename T>
  static constexpr bool nothrow_converting_constructible = std::is_nothrow_constructible_v<variant_alternative_t<index_chooser_v<T, variant<Types...>>, variant<Types...>>, T>;
  template <typename T>
  static constexpr bool nothrow_converting_assignable = nothrow_converting_constructible<T> &&
      std::is_nothrow_assignable_v<variant_alternative_t<index_chooser_v<T, variant<Types...>>, variant<Types...>>, T>;


  static constexpr bool trivially_destructible = (std::is_trivially_destructible_v<Types> && ...);
  static constexpr bool trivially_copy_constructible = (std::is_trivially_copy_constructible_v<Types> && ...);
  static constexpr bool trivially_move_constructible = (std::is_trivially_move_constructible_v<Types> && ...);
  static constexpr bool trivially_copy_assignable = trivially_copy_constructible && (std::is_trivially_copy_assignable_v<Types> && ...) && trivially_destructible;
  static constexpr bool trivially_move_assignable = trivially_move_constructible && (std::is_trivially_move_assignable_v<Types> && ...) && trivially_destructible;
};
