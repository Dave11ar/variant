#pragma once

#include "variadic_union.h"
#include "variant_traits.h"
#include <bits/enable_special_members.h>
/**
 * bases for destructor
 */
template <bool is_trivial, typename First, typename ...Rest>
struct variadic_storage_destructor_base {
protected:
  using var_union = variadic_union<false, First, Rest...>;

  ~variadic_storage_destructor_base() {
    variant_cast(*this).reset();
  }

  template <typename T/*, std::enable_if_t<std::is_constructible_v<T, T>, int> = 0*/ >
  constexpr variadic_storage_destructor_base(T &&t)
      : index_val(index_chooser_v<T, variant<First, Rest...>>), value(in_place_index<index_chooser_v<T, variant<First, Rest...>>>, std::forward<T>(t)) {}

  template <typename T, typename ...Args/*, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0*/>
  constexpr variadic_storage_destructor_base(in_place_type_t<T>, Args&&... args)
      : index_val(type_index<T, First, Rest...>), value(in_place_index<type_index<T, First, Rest...>>, std::forward<Args>(args)...) {}

  template <size_t I, typename ...Args>
  constexpr variadic_storage_destructor_base(in_place_index_t<I>, Args &&...args)
      : index_val(I), value(in_place_index<I>, std::forward<Args>(args)...) {}


  variadic_storage_destructor_base() = default;
  constexpr variadic_storage_destructor_base(variadic_storage_destructor_base const &other) = default;
  constexpr variadic_storage_destructor_base(variadic_storage_destructor_base &&other) = default;
  constexpr variadic_storage_destructor_base &operator=(variadic_storage_destructor_base const &other) = default;
  constexpr variadic_storage_destructor_base& operator=(variadic_storage_destructor_base &&other) = default;

  size_t index_val;
  var_union value;
};

template <typename First, typename ...Rest>
struct variadic_storage_destructor_base<true, First, Rest...> {
protected:
  using var_union = variadic_union<true, First, Rest...>;

  template <typename T/*, std::enable_if_t<std::is_constructible_v<T, T>, int> = 0*/ /*,
      std::enable_if_t<std::is_same_v<T, First> || (std::is_same_v<T, Rest> || ...), int> = 0*/>
  constexpr variadic_storage_destructor_base(T &&t)
      : index_val(index_chooser_v<T, variant<First, Rest...>>), value(in_place_index<index_chooser_v<T, variant<First, Rest...>>>, std::forward<T>(t)) {}

  template <typename T, typename ...Args/*, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0*/>
  constexpr variadic_storage_destructor_base(in_place_type_t<T>, Args&&... args)
      : index_val(type_index<T, First, Rest...>), value(in_place_index<type_index<T, First, Rest...>>, std::forward<Args>(args)...) {}

  template <size_t I, typename ...Args>
  constexpr variadic_storage_destructor_base(in_place_index_t<I>, Args&&... args)
      : index_val(I), value(in_place_index<I>, std::forward<Args>(args)...) {}


  variadic_storage_destructor_base() = default;
  ~variadic_storage_destructor_base() = default;
  constexpr variadic_storage_destructor_base(variadic_storage_destructor_base const &other) = default;
  constexpr variadic_storage_destructor_base(variadic_storage_destructor_base &&other) = default;
  constexpr variadic_storage_destructor_base &operator=(variadic_storage_destructor_base const &other) = default;
  constexpr variadic_storage_destructor_base& operator=(variadic_storage_destructor_base &&other) = default;

  size_t index_val;
  variadic_union<true, First, Rest...> value;
};

/**
 * bases for copy_ctor
 */
template <bool is_trivial, typename ...Types>
struct variadic_storage_copy_constructor_base :
    variadic_storage_destructor_base<variant_traits<Types...>::trivially_destructible, Types...> {
  using base = variadic_storage_destructor_base<variant_traits<Types...>::trivially_destructible, Types...>;
  using base::base;

  constexpr variadic_storage_copy_constructor_base(variadic_storage_copy_constructor_base const &other) {
    variant<Types...> const &other_variant = variant_cast(other);
    variant<Types...> &this_variant = variant_cast(*this);

    if ((this->index_val = other.index_val)  == variant_npos) {
      return;
    }

    this_variant.emplace_variant(other_variant);
  }


  constexpr variadic_storage_copy_constructor_base(variadic_storage_copy_constructor_base &&other) = default;
  constexpr variadic_storage_copy_constructor_base &operator=(variadic_storage_copy_constructor_base const &other) = default;
  constexpr variadic_storage_copy_constructor_base& operator=(variadic_storage_copy_constructor_base &&other) = default;
};

template <typename ...Types>
struct variadic_storage_copy_constructor_base<true, Types...> :
    variadic_storage_destructor_base<variant_traits<Types...>::trivially_destructible, Types...> {
  using base = variadic_storage_destructor_base<variant_traits<Types...>::trivially_destructible, Types...>;
  using base::base;

  constexpr variadic_storage_copy_constructor_base(variadic_storage_copy_constructor_base const &other) = default;
  constexpr variadic_storage_copy_constructor_base(variadic_storage_copy_constructor_base &&other) = default;
  constexpr variadic_storage_copy_constructor_base &operator=(variadic_storage_copy_constructor_base const &other) = default;
  constexpr variadic_storage_copy_constructor_base& operator=(variadic_storage_copy_constructor_base &&other) = default;
};

/**
 * bases for move_ctor
 */
template <bool is_trivial, typename ...Types>
struct variadic_storage_move_constructor_base :
    variadic_storage_copy_constructor_base<variant_traits<Types...>::trivially_copy_constructible, Types...> {
  using base = variadic_storage_copy_constructor_base<variant_traits<Types...>::trivially_copy_constructible, Types...>;
  using base::base;

  constexpr variadic_storage_move_constructor_base(variadic_storage_move_constructor_base &&other)
    noexcept(variant_traits<Types...>::nothrow_move_constructible) {
    variant<Types...> &&other_variant = variant_cast(std::forward<decltype(other)>(other));
    variant<Types...> &this_variant = variant_cast(*this);

    if ((this->index_val = other.index_val)  == variant_npos) {
      return;
    }

    this_variant.emplace_variant(std::forward<decltype(other_variant)>(other_variant));
  }


  constexpr variadic_storage_move_constructor_base(variadic_storage_move_constructor_base const &other) = default;
  constexpr variadic_storage_move_constructor_base &operator=(variadic_storage_move_constructor_base const &other) = default;
  constexpr variadic_storage_move_constructor_base& operator=(variadic_storage_move_constructor_base &&other) = default;
};

template <typename ...Types>
struct variadic_storage_move_constructor_base<true, Types...> :
    variadic_storage_copy_constructor_base<variant_traits<Types...>::trivially_copy_constructible, Types...> {
  using base = variadic_storage_copy_constructor_base<variant_traits<Types...>::trivially_copy_constructible, Types...>;
  using base::base;


  constexpr variadic_storage_move_constructor_base &operator=(variadic_storage_move_constructor_base const &other) = default;
  constexpr variadic_storage_move_constructor_base& operator=(variadic_storage_move_constructor_base &&other) = default;
  constexpr variadic_storage_move_constructor_base(variadic_storage_move_constructor_base const &other) = default;
  constexpr variadic_storage_move_constructor_base(variadic_storage_move_constructor_base &&other) = default;
};


/**
 * bases for copy assignment
 */
template <bool is_trivial, typename ...Types>
struct variadic_storage_copy_assignment_base :
    variadic_storage_move_constructor_base<variant_traits<Types...>::trivially_move_constructible, Types...> {
  using base = variadic_storage_move_constructor_base<variant_traits<Types...>::trivially_move_constructible, Types...>;
  using base::base;

  constexpr variadic_storage_copy_assignment_base& operator=(variadic_storage_copy_assignment_base const &other) {
    variant<Types...> const &other_variant = variant_cast(other);
    variant<Types...> &this_variant = variant_cast(*this);

    if (other_variant.valueless_by_exception()) {
      if (!this_variant.valueless_by_exception()) {
        this_variant.reset();
      }
    } else {
      visit_index<void>([&this_variant, &other_variant](auto this_index, auto other_index) {
        if constexpr (this_index == other_index) {
          get<this_index>(this_variant) = get<other_index>(other_variant);
        } else {
          this_variant.emplace_variant(other_variant);
        }
      }, this_variant, other_variant);
    }

    this->index_val = other.index_val;

    return *this;
  }


  constexpr variadic_storage_copy_assignment_base& operator=(variadic_storage_copy_assignment_base &&other) = default;
  constexpr variadic_storage_copy_assignment_base(variadic_storage_copy_assignment_base &&other) = default;
  constexpr variadic_storage_copy_assignment_base(variadic_storage_copy_assignment_base const &other) = default;
};

template <typename ...Types>
struct variadic_storage_copy_assignment_base<true, Types...> :
    variadic_storage_move_constructor_base<variant_traits<Types...>::trivially_move_constructible, Types...> {
  using base = variadic_storage_move_constructor_base<variant_traits<Types...>::trivially_move_constructible, Types...>;
  using base::base;


  constexpr variadic_storage_copy_assignment_base(variadic_storage_copy_assignment_base const &other) = default;
  constexpr variadic_storage_copy_assignment_base(variadic_storage_copy_assignment_base &&other) = default;
  constexpr variadic_storage_copy_assignment_base& operator=(variadic_storage_copy_assignment_base const &other) = default;
  constexpr variadic_storage_copy_assignment_base& operator=(variadic_storage_copy_assignment_base &&other) = default;
};


/**
 * bases for move assignment
 */
template <bool is_trivial, typename ...Types>
struct variadic_storage_move_assignment_base :
    variadic_storage_copy_assignment_base<variant_traits<Types...>::trivially_copy_assignable, Types...> {
  using base = variadic_storage_copy_assignment_base<variant_traits<Types...>::trivially_copy_assignable, Types...>;
  using base::base;

  constexpr variadic_storage_move_assignment_base& operator=(variadic_storage_move_assignment_base &&other)
      noexcept(variant_traits<Types...>::nothrow_move_assignable) {
    variant<Types...> &&other_variant = variant_cast(std::forward<decltype(other)>(other));
    variant<Types...> &this_variant = variant_cast(*this);

    if (other_variant.valueless_by_exception()) {
      if (!this_variant.valueless_by_exception()) {
        this_variant.destroy();
      }
    } else {
      visit_index<void>([&this_variant, &other_variant](auto this_index, auto other_index) {
        if constexpr (this_index == other_index) {
          get<this_index>(this_variant) = std::move(get<other_index>(other_variant));
        } else {
          this_variant.emplace_variant(std::move(other_variant));
        }
      }, this_variant, std::move(other_variant));
    }
    this->index_val = other.index_val;

    return *this;
  }


  constexpr variadic_storage_move_assignment_base(variadic_storage_move_assignment_base const &other) = default;
  constexpr variadic_storage_move_assignment_base(variadic_storage_move_assignment_base &&other) = default;
  constexpr variadic_storage_move_assignment_base &operator=(variadic_storage_move_assignment_base const &other) = default;
};

template <typename ...Types>
struct variadic_storage_move_assignment_base<true, Types...> :
    variadic_storage_copy_assignment_base<variant_traits<Types...>::trivially_copy_assignable, Types...> {
  using base = variadic_storage_copy_assignment_base<variant_traits<Types...>::trivially_copy_assignable, Types...>;
  using base::base;


  constexpr variadic_storage_move_assignment_base(variadic_storage_move_assignment_base const &other) = default;
  constexpr variadic_storage_move_assignment_base(variadic_storage_move_assignment_base &&other) = default;
  constexpr variadic_storage_move_assignment_base &operator=(variadic_storage_move_assignment_base const &other) = default;
  constexpr variadic_storage_move_assignment_base &operator=(variadic_storage_move_assignment_base &&other) = default;
};


/**
 * main base
 */
template <typename ...Types>
struct variadic_storage : variadic_storage_move_assignment_base<variant_traits<Types...>::trivially_move_assignable, Types...> {
  using base = variadic_storage_move_assignment_base<variant_traits<Types...>::trivially_move_assignable, Types...>;
  using base::base;

  constexpr variadic_storage() noexcept(variant_traits<Types...>::nothrow_default_constructible)
    : base(in_place_index<0>) {};


  constexpr variadic_storage(variadic_storage const &other) = default;
  constexpr variadic_storage(variadic_storage &&other) = default;
  constexpr variadic_storage &operator=(variadic_storage const &other) = default;
  constexpr variadic_storage &operator=(variadic_storage &&other) = default;
};
