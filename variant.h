#pragma once

#include <algorithm>
#include <utility>
#include "variadic_storage.h"
#include <bits/enable_special_members.h>

template <typename ...Types>
struct variant : private var::variadic_storage<Types...>,
                 private std::_Enable_default_constructor<var::variant_traits<Types...>::default_constructible>,
                 private std::_Enable_copy_move<var::variant_traits<Types...>::copy_constructible, var::variant_traits<Types...>::copy_assignable,
                                                var::variant_traits<Types...>::move_constructible, var::variant_traits<Types...>::move_assignable> {

  using default_ctor_enabler = std::_Enable_default_constructor<var::variant_traits<Types...>::default_constructible>;
  using base = var::variadic_storage<Types...>;
  using base::base;

public:
  constexpr variant() = default;
  constexpr variant(variant const &other) = default;
  constexpr variant(variant &&other) = default;
  constexpr variant &operator=(variant const &) = default;
  constexpr variant &operator=(variant &&) = default;
  ~variant() = default;

  template <typename T, std::enable_if_t<var::variant_traits<Types...>::template converting_constructible<T>, int> = 0>
  constexpr variant(T &&t) noexcept(var::variant_traits<Types...>::template nothrow_converting_constructible<T>)
      : base(std::forward<T>(t)), default_ctor_enabler(std::_Enable_default_constructor_tag{}) {}

  template <typename T, typename ...Args, std::enable_if_t<var::variant_traits<Types...>::template in_place_type_constructible<T, Args...>, int> = 0>
  constexpr explicit variant(in_place_type_t<T>, Args&&... args)
      : base(in_place_type<T>, std::forward<Args>(args)...), default_ctor_enabler(std::_Enable_default_constructor_tag{}) {}

  template <size_t I, typename ...Args, std::enable_if_t<var::variant_traits<Types...>::template
      in_place_index_constructible_base<I>::template in_place_index_constructible<Args...>, int> = 0>
  constexpr explicit variant(in_place_index_t<I>, Args &&...args)
    : base(in_place_index<I>, std::forward<Args>(args)...), default_ctor_enabler(std::_Enable_default_constructor_tag{}) {}

  template <typename T, std::enable_if_t<var::variant_traits<Types...>::template converting_assignable<T>, int> = 0>
  constexpr variant &operator=(T &&t) noexcept(var::variant_traits<Types...>::template nothrow_converting_assignable<T>) {
    var::visit_index<void>([this, &t](auto _index) {
      constexpr auto new_index = var::index_chooser_v<T, variant<Types...>>;
      if constexpr (new_index == _index) {
        get(in_place_index<_index>) = std::forward<T>(t);
      } else {
        using new_type = variant_alternative_t<new_index, variant<Types...>>;
        if constexpr (std::is_nothrow_constructible_v<new_type , T> || !std::is_nothrow_move_constructible_v<T>) {
          this->emplace<new_index>(std::forward<T>(t));
        } else {
          operator=(variant(std::forward<T>(t)));
        }
      }
    }, *this);

    return *this;
  }

  constexpr size_t index() const noexcept {
    return this->index_val;
  }

  constexpr bool valueless_by_exception() const noexcept {
    return index() == variant_npos;
  }

  template <typename T, typename ...Args>
  T& emplace(Args &&...args) {
    return this->emplace<var::type_index<T, Types...>, Args...>(std::forward<Args>(args)...);
  }

  template <size_t I, typename ... Args>
  variant_alternative_t<I, variant<Types...>> &emplace(Args &&...args) {
    reset();
    this->construct(in_place_index<I>, std::forward<Args>(args)...);
    return get(in_place_index<I>);
  }

  void swap(variant &other) noexcept(((std::is_nothrow_move_constructible_v<Types> &&
      std::is_nothrow_swappable_v<Types>) && ...)) {
    if (other.valueless_by_exception() || this->valueless_by_exception()) {
      if (other.valueless_by_exception() && this->valueless_by_exception()) {
        return;
      }

      if (other.valueless_by_exception()) {
        other.emplace_variant(std::move(*this));
        this->reset();
        return;
      }

      if (this->valueless_by_exception()) {
        this->emplace_variant(std::move(other));
        other.reset();
        return;
      }
    } else if (other.index() == this->index()) {
      var::visit_index<void>([this, &other](auto this_index, auto other_index) {
          using std::swap;
          swap(this->get(in_place_index<this_index>), other.get(in_place_index<this_index>));
          swap(this->index_val, other.index_val);
      }, *this, other);
    } else {
      var::visit_index<void>([this, &other](auto this_index, auto other_index){
        auto tmp(std::move(::get<other_index>(other)));

        other.emplace_variant(std::move(::get<this_index>(*this)));
        this->reset();
        this->emplace_variant(std::move(tmp));
      }, *this, other);
    }
  }

  void reset() {
    if (!valueless_by_exception()) {
      this->destroy();
      this->index_val = variant_npos;
    }
  }

private:
  void emplace_variant(variant &&other) {
    var::visit_index<void>([this, &other](auto _index) {
      this->template emplace<_index>(::get<_index>(std::forward<decltype(other)>(other)));
    }, std::forward<variant>(other));
  }

  void emplace_variant(variant const &other) {
    var::visit_index<void>([this, &other](auto _index) {
      this->template emplace<_index>(::get<_index>(other));
    }, other);
  }

  void destroy() {
    var::visit_index<void>([this](auto _index){
      destroy(in_place_index<_index>);
    }, *this);
  }

  template <size_t I>
  void destroy(in_place_index_t<I>) {
    this->value.destroy(in_place_index<I>);
  }

  template <size_t I, typename ...Args>
  void construct(in_place_index_t<I>, Args &&...args) {
    this->value.template construct(in_place_index<I>, std::forward<Args>(args)...);
    this->index_val = I;
  }

  template <size_t I, typename ...Args>
  friend constexpr variant_alternative_t<I, variant<Args...>> &get(variant<Args...> &v);

  template <size_t I, typename ...Args>
  friend constexpr variant_alternative_t<I, variant<Args...>> const &get(variant<Args...> const &v);

  template <size_t I>
  constexpr variant_alternative_t<I, variant<Types...>> &get(in_place_index_t<I>) noexcept {
    return this->value.get(in_place_index<I>);
  }

  template <size_t I>
  constexpr variant_alternative_t<I, variant<Types...>> const &get(in_place_index_t<I>) const noexcept {
    return this->value.get(in_place_index<I>);
  }

  template <bool is_trivial, typename First, typename ...Rest>
  friend struct var::variadic_storage_destructor_base;
  template <bool is_trivial, typename ...F_Types>
  friend struct var::variadic_storage_copy_constructor_base;
  template <bool is_trivial, typename ...F_Types>
  friend struct var::variadic_storage_move_constructor_base;
  template <bool is_trivial, typename ...F_Types>
  friend struct var::variadic_storage_copy_assignment_base;
  template <bool is_trivial, typename ...F_Types>
  friend struct var::variadic_storage_move_assignment_base;
  template <typename ...F_Types>
  friend struct var::variadic_storage;
};

template <typename ...Types>
constexpr bool operator==(variant<Types...> const &v, variant<Types...> const &w) {
  return var::visit_index<bool>([&v, &w](auto index_v, auto index_w) {
    if constexpr (index_v != index_w) {
      return false;
    } else {
      if (v.valueless_by_exception()) {
        return true;
      } else {
        return get<index_v>(v) == get<index_w>(w);
      }
    }
  }, v, w);
}

template <typename ...Types>
constexpr bool operator!=(variant<Types...> const &v, variant<Types...> const &w) {
  return var::visit_index<bool>([&v, &w](auto index_v, auto index_w) {
    if constexpr (index_v != index_w) {
      return true;
    } else {
      if (v.valueless_by_exception()) {
        return false;
      } else {
        return get<index_v>(v) != get<index_w>(w);
      }
    }
  }, v, w);
}

template <typename ...Types>
constexpr bool operator<(variant<Types...> const &v, variant<Types...> const &w) {
  return var::visit_index<bool>([&v, &w](auto index_v, auto index_w) {
    if constexpr (index_w == variant_npos) {
      return false;
    } else if constexpr (index_v == variant_npos) {
      return true;
    } else if constexpr (index_v < index_w) {
      return true;
    } else if constexpr (index_v > index_w) {
      return false;
    } else {
      return get<index_v>(v) < get<index_w>(w);
    }
  }, v, w);
}

template <typename ...Types>
constexpr bool operator>(variant<Types...> const &v, variant<Types...> const &w) {
  return var::visit_index<bool>([&v, &w](auto index_v, auto index_w) {
    if constexpr (index_v == variant_npos) {
      return false;
    } else if constexpr (index_w == variant_npos) {
      return true;
    } else if constexpr (index_v > index_w) {
      return true;
    } else if constexpr (index_v < index_w) {
      return false;
    } else {
      return get<index_v>(v) > get<index_w>(w);
    }
  }, v, w);
}

template <typename ...Types>
constexpr bool operator<=(variant<Types...> const &v, variant<Types...> const &w) {
  return var::visit_index<bool>([&v, &w](auto index_v, auto index_w) {
    if constexpr (index_v == variant_npos) {
      return true;
    } else if constexpr (index_w == variant_npos) {
      return false;
    } else if constexpr (index_v < index_w) {
      return true;
    } else if constexpr (index_v > index_w) {
      return false;
    } else {
      return get<index_v>(v) <= get<index_w>(w);
    }
  }, v, w);
}

template <typename ...Types>
constexpr bool operator>=(variant<Types...> const &v, variant<Types...> const &w) {
  return var::visit_index<bool>([&v, &w](auto index_v, auto index_w) {
    if constexpr (index_w == variant_npos) {
      return true;
    } else if constexpr (index_v == variant_npos) {
      return false;
    } else if constexpr (index_v > index_w) {
      return true;
    } else if constexpr (index_v < index_w) {
      return false;
    } else {
      return get<index_v>(v) >= get<index_w>(w);
    }
  }, v, w);
}

template <typename ...Types>
void swap(variant<Types...> &lhs, variant<Types...> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}