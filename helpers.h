#pragma once
#include <cstddef>
#include <cassert>

template <typename ...Types>
struct variant;

namespace var{
template <typename ...Types>
struct variant_traits;

template <bool, typename ...Types>
union variadic_union;

template <typename ...Types>
struct variadic_storage;

template <bool is_trivial, typename First, typename ...Rest>
struct variadic_storage_destructor_base;
} // end of var namespace
template <typename T>
struct in_place_type_t {};
template <typename T>
inline constexpr in_place_type_t<T> in_place_type {};

template <size_t index>
struct in_place_index_t {};
template <size_t index>
inline constexpr in_place_index_t<index> in_place_index {};

inline constexpr size_t variant_npos = -1;

namespace var {
template <typename T, typename ...Types>
inline constexpr size_t type_index =  var::variadic_union<false, Types...>::template get_type_index<0>(in_place_type<T>);
} // end of var namespace

template <size_t I, typename Variant>
struct variant_alternative;

template <size_t I, typename First, typename ...Rest>
struct variant_alternative<I, variant<First, Rest...>>
    : variant_alternative<I - 1, variant<Rest...>> {};

template <typename First, typename ...Rest>
struct variant_alternative<0, variant<First, Rest...>> {
  using type = First;
};

template <size_t I, typename Variant>
using variant_alternative_t = typename variant_alternative<I, Variant>::type;


template <size_t I, typename Variant>
struct variant_alternative<I, const Variant> {
  using type = std::add_const_t<variant_alternative_t<I, Variant>>;
};

template <size_t I, typename Variant>
struct variant_alternative<I, volatile Variant> {
  using type = std::add_volatile_t<variant_alternative_t<I, Variant>>;
};

template <size_t I, typename Variant>
struct variant_alternative<I, const volatile Variant> {
  using type = std::add_cv_t<variant_alternative_t<I, Variant>>;
};


template <typename Variant>
struct variant_size;

template <typename ...Types>
struct variant_size<variant<Types...>>
    : std::integral_constant<size_t, sizeof...(Types)> {};

template <typename Variant>
struct variant_size<const Variant>
    : variant_size<Variant> {};

template <typename Variant>
struct variant_size<volatile Variant>
    : variant_size<Variant> {};

template <typename Variant>
struct variant_size<const volatile Variant>
    : variant_size<Variant> {};

template <typename Variant>
inline constexpr std::size_t variant_size_v = variant_size<Variant>::value;

struct bad_variant_access final : public std::exception {
  constexpr const char* what() const noexcept override {
    return "bad_variant_access";
  }
};

template <size_t I, typename ...Types>
constexpr variant_alternative_t<I, variant<Types...>> &get(variant<Types...> &v) {
  if (v.valueless_by_exception() || (I >= sizeof...(Types)) || I != v.index()) {
    throw bad_variant_access();
  }
  return v.template get(in_place_index<I>);
}

template <size_t I, typename ...Types>
constexpr variant_alternative_t<I, variant<Types...>> &&get(variant<Types...> &&v) {
  return std::move(get<I>(v));
}

template <size_t I, typename ...Types>
constexpr variant_alternative_t<I, variant<Types...>> const &get(variant<Types...> const &v) {
  if (v.valueless_by_exception() || (I >= sizeof...(Types)) || I != v.index()) {
    throw bad_variant_access();
  }
  return v.get(in_place_index<I>);
}

template <size_t I, typename ...Types>
constexpr variant_alternative_t<I, variant<Types...>> const &&get(variant<Types...> const &&v) {
  return std::move(get<I>(v));
}

template <typename T, typename ...Types>
constexpr T &get(variant<Types...> &v) {
  return get<var::type_index<T, Types...>>(v);
}

template <typename T, typename ...Types>
constexpr T &&get(variant<Types...> &&v) {
  return std::move(get<var::type_index<T>, Types...>(std::move(v)));
}

template <typename T, typename ...Types>
constexpr T const &get(variant<Types...> const &v) {
  return get<var::type_index<T, Types...>>(v);
}

template <typename T, typename ...Types>
constexpr T const &&get(variant<Types...> const &&v) {
  return std::forward<T const &&>(get<var::type_index<T>, Types...>(std::forward<variant<Types...> const &&>(v)));
}



namespace var {
template<size_t I>
struct index_wrapper : std::integral_constant<size_t, I> {};

template <bool IsValid, bool IsValue, typename R>
struct dispatcher;

template <bool IsValue, typename R>
struct dispatcher<false, IsValue, R> {
  template<size_t I, size_t ...Is, typename F, typename V, typename ...Vs>
  static R case_(F &&f, V &&v, Vs &&...vs) {
    throw bad_variant_access();
  }

  template<size_t B, size_t ...Is, typename F, typename V, typename ...Vs>
  static R switch_(F &&f, V &&v, Vs &&...vs) {
    throw bad_variant_access();
  }
};

template <bool IsValue, typename R>
struct dispatcher<true, IsValue, R> {
  template<size_t I, size_t ...Is, typename F, typename V, typename ...Vs>
  static constexpr R case_(F &&f, V &&v, Vs &&...vs) {
    if constexpr (sizeof...(Is) == sizeof...(Vs)) {
      if constexpr (IsValue) {
        return std::forward<F>(f)(get<Is>(std::forward<Vs>(vs))..., get<I>(std::forward<V>(v)));
      } else {
        return std::forward<F>(f)(index_wrapper<Is>{}..., index_wrapper<I>{});
      }
    } else {
      return switch_<0, Is..., I>(std::forward<F>(f), std::forward<Vs>(vs)..., std::forward<V>(v));
    }
  }

  template<size_t B, size_t ...Is, typename F, typename V, typename ...Vs>
  static constexpr R switch_(F &&f, V &&v, Vs &&...vs) {
    constexpr size_t size = variant_size_v<std::__remove_cvref_t<V>>;
    switch (v.index()) {
      case B + 0: {
        return dispatcher<B + 0 < size, IsValue, R>::template case_<B + 0, Is...>(
            std::forward<F>(f), std::forward<V>(v), std::forward<Vs>(vs)...);
      }
      case B + 1: {
        return dispatcher<B + 1 < size, IsValue, R>::template case_<B + 1, Is...>(
            std::forward<F>(f), std::forward<V>(v), std::forward<Vs>(vs)...);
      }
      case B + 2: {
        return dispatcher<B + 2 < size, IsValue, R>::template case_<B + 2, Is...>(
            std::forward<F>(f), std::forward<V>(v), std::forward<Vs>(vs)...);
      }
      case B + 3: {
        return dispatcher<B + 3 < size, IsValue, R>::template case_<B + 3, Is...>(
            std::forward<F>(f), std::forward<V>(v), std::forward<Vs>(vs)...);
      }
      default: {
        return dispatcher<B + 4 < size, IsValue, R>::template switch_<B + 4, Is...>(
            std::forward<F>(f), std::forward<V>(v), std::forward<Vs>(vs)...);
      }
    }
  }
};

template <typename F, typename ...V>
constexpr decltype(auto) visit_index(F &&f, V &&...v) {
  using R = decltype(std::invoke(std::forward<F>(f), get<0>(std::forward<V>(v))...));
  return visit_index<R>(std::forward<F>(f), std::forward<V>(v)...);
}

template <typename R, typename Visitor, typename ...Variants>
constexpr R visit_index(Visitor &&vis, Variants &&...vars) {
  return dispatcher<true, false, R>::template switch_<0>(std::forward<Visitor>(vis), std::forward<Variants>(vars)...);
}
} // end of var namespace

template <typename F, typename ...V>
constexpr decltype(auto) visit(F &&f, V &&...v) {
  if ((v.valueless_by_exception() || ...)) {
    throw bad_variant_access();
  }

  using R = decltype(std::invoke(std::forward<F>(f), get<0>(std::forward<V>(v))...));
  return var::dispatcher<true, true, R>::template switch_<0>(std::forward<F>(f), std::forward<V>(v)...);
}

template <typename R, typename Visitor, typename ...Variants>
constexpr R visit(Visitor &&vis, Variants &&...vars) {
  if ((vars.valueless_by_exception() || ...)) {
    throw bad_variant_access();
  }

  return var::dispatcher<true, true, R>::template switch_<0>(std::forward<Visitor>(vis), std::forward<Variants>(vars)...);
}


template<size_t I, typename ...Types>
constexpr std::add_pointer_t<variant_alternative_t<I, variant<Types...>>> get_if(variant<Types...> *pv) noexcept {
  if (pv->index() == I) {
    return &get<I>(*pv);
  } else {
    return nullptr;
  }
}

template<size_t I, typename ...Types>
constexpr std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>> get_if(variant<Types...> const*pv) noexcept {
  if (pv->index() == I) {
    return &get<I>(*pv);
  } else {
    return nullptr;
  }
}

template<typename T, typename ...Types>
constexpr std::add_pointer_t<T> get_if(variant<Types...> *pv) noexcept {
  return get_if<var::type_index<T, Types...>>(pv);
}

template<typename T, typename ...Types>
constexpr std::add_pointer_t<const T> get_if(variant<Types...> const *pv) noexcept {
  return get_if<var::type_index<T, Types...>>(pv);
}

template<typename T, typename ...Types>
constexpr bool holds_alternative(variant<Types...> const &v) noexcept {
  return v.index() == var::type_index<T, Types...>;
}

namespace var {
template<typename First, typename T, typename = void>
struct check_arr {
  First arr[1];
};

template<typename T>
struct check_arr<bool, T, std::enable_if_t<std::is_same_v<bool, std::remove_cv_t<T>>>> {
  bool arr[1];
};

template<typename T>
struct check_arr<bool, T, std::enable_if_t<!std::is_same_v<bool, std::remove_cv_t<T>>>> {};

template <typename T, typename Variant, typename = void>
struct checker {
  static constexpr void fun() {}
};

template <typename T, typename First, typename ...Rest>
struct checker<T, variant<First, Rest...>, std::void_t<decltype(check_arr<First, T>{{std::declval<T>()}})>>
    : checker<T, variant<Rest...>> {
  using checker<T, variant<Rest...>>::fun;
  static constexpr std::integral_constant<size_t , sizeof...(Rest)> fun(First);
};

template <typename T, typename Variant>
struct chooser {
  static constexpr void fun();
};

template <typename T, typename First, typename ...Rest>
struct chooser<T, variant<First, Rest...>> :
    checker<T, variant<First, Rest...>>,
    chooser<T, variant<Rest...>>
{
  using chooser<T, variant<Rest...>>::fun;
  using checker<T, variant<First, Rest...>>::fun;
};

template <typename T, typename Variant, typename V = void>
struct index_chooser {
  static constexpr size_t value = variant_npos;
};

template <typename T, typename ...Types>
struct index_chooser<T, variant<Types...>, std::void_t<decltype(chooser<T, variant<Types...>>::fun(std::declval<T>()))>> {
  static constexpr size_t value = sizeof...(Types) - 1 - decltype(chooser<T, variant<Types...>>::fun(std::declval<T>()))::value;
};

template <typename T, typename Variant>
static constexpr size_t index_chooser_v = index_chooser<T, Variant>::value;

template <bool is_trivial, typename ...Types>
constexpr variant<Types...> &variant_cast(variadic_storage_destructor_base<is_trivial, Types...> &base) {
  return static_cast<variant<Types...> &>(base);
}

template <bool is_trivial, typename ...Types>
constexpr variant<Types...> &&variant_cast(variadic_storage_destructor_base<is_trivial, Types...> &&base) {
  return static_cast<variant<Types...> &&>(base);
}

template <bool is_trivial, typename ...Types>
constexpr variant<Types...> const &variant_cast(variadic_storage_destructor_base<is_trivial, Types...> const &base) {
  return static_cast<variant<Types...> const &>(base);
}
} // end of var namespace
