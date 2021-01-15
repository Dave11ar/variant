#pragma once

namespace var {

struct Enable_default_constructor_tag
{
  explicit constexpr Enable_default_constructor_tag() = default;
};

template<bool Default, typename Tag = void>
struct Enable_default_constructor
{
  constexpr Enable_default_constructor() noexcept = default;
  constexpr Enable_default_constructor(Enable_default_constructor const&) noexcept  = default;
  constexpr Enable_default_constructor(Enable_default_constructor&&) noexcept = default;
  constexpr Enable_default_constructor &operator=(Enable_default_constructor const&) noexcept = default;
  constexpr Enable_default_constructor &operator=(Enable_default_constructor&&) noexcept = default;

  // Can be used in other ctors.
  constexpr explicit Enable_default_constructor(Enable_default_constructor_tag) { }
};
template<typename Tag>
struct Enable_default_constructor<false, Tag> {
  constexpr Enable_default_constructor() noexcept = delete;
  constexpr Enable_default_constructor(Enable_default_constructor const&) noexcept  = default;
  constexpr Enable_default_constructor(Enable_default_constructor&&) noexcept = default;
  constexpr Enable_default_constructor &operator=(Enable_default_constructor const&) noexcept = default;
  constexpr Enable_default_constructor &operator=(Enable_default_constructor&&) noexcept = default;

  // Can be used in other ctors.
  constexpr explicit Enable_default_constructor(Enable_default_constructor_tag) { }
};


template <bool Copy, typename Tag = void>
struct Enable_copy_constructor {
  constexpr Enable_copy_constructor() noexcept = default;
  constexpr Enable_copy_constructor(Enable_copy_constructor const&) noexcept  = default;
  constexpr Enable_copy_constructor(Enable_copy_constructor&&) noexcept = default;
  constexpr Enable_copy_constructor &operator=(Enable_copy_constructor const&) noexcept = default;
  constexpr Enable_copy_constructor &operator=(Enable_copy_constructor&&) noexcept = default;
};
template <typename Tag>
struct Enable_copy_constructor<false, Tag> {
  constexpr Enable_copy_constructor() noexcept = default;
  constexpr Enable_copy_constructor(Enable_copy_constructor const&) noexcept  = delete;
  constexpr Enable_copy_constructor(Enable_copy_constructor&&) noexcept = default;
  constexpr Enable_copy_constructor &operator=(Enable_copy_constructor const&) noexcept = default;
  constexpr Enable_copy_constructor &operator=(Enable_copy_constructor&&) noexcept = default;
};

template <bool Move, typename Tag = void>
struct Enable_move_constructor {
  constexpr Enable_move_constructor() noexcept = default;
  constexpr Enable_move_constructor(Enable_move_constructor const&) noexcept  = default;
  constexpr Enable_move_constructor(Enable_move_constructor&&) noexcept = default;
  constexpr Enable_move_constructor &operator=(Enable_move_constructor const&) noexcept = default;
  constexpr Enable_move_constructor &operator=(Enable_move_constructor&&) noexcept = default;
};
template <typename Tag>
struct Enable_move_constructor<false, Tag> {
  constexpr Enable_move_constructor() noexcept = default;
  constexpr Enable_move_constructor(Enable_move_constructor const&) noexcept  = default;
  constexpr Enable_move_constructor(Enable_move_constructor&&) noexcept = delete;
  constexpr Enable_move_constructor &operator=(Enable_move_constructor const&) noexcept = default;
  constexpr Enable_move_constructor &operator=(Enable_move_constructor&&) noexcept = default;
};

template <bool CopyAssignment, typename Tag = void>
struct Enable_copy_assignment {
  constexpr Enable_copy_assignment() noexcept = default;
  constexpr Enable_copy_assignment(Enable_copy_assignment const&) noexcept  = default;
  constexpr Enable_copy_assignment(Enable_copy_assignment&&) noexcept = default;
  constexpr Enable_copy_assignment &operator=(Enable_copy_assignment const&) noexcept = default;
  constexpr Enable_copy_assignment &operator=(Enable_copy_assignment&&) noexcept = default;
};
template <typename Tag>
struct Enable_copy_assignment<false, Tag> {
  constexpr Enable_copy_assignment() noexcept = default;
  constexpr Enable_copy_assignment(Enable_copy_assignment const&) noexcept  = default;
  constexpr Enable_copy_assignment(Enable_copy_assignment&&) noexcept = default;
  constexpr Enable_copy_assignment &operator=(Enable_copy_assignment const&) noexcept = delete;
  constexpr Enable_copy_assignment &operator=(Enable_copy_assignment&&) noexcept = default;
};

template <bool MoveAssignment, typename Tag = void>
struct Enable_move_assignment {
  constexpr Enable_move_assignment() noexcept = default;
  constexpr Enable_move_assignment(Enable_move_assignment const&) noexcept  = default;
  constexpr Enable_move_assignment(Enable_move_assignment&&) noexcept = default;
  constexpr Enable_move_assignment &operator=(Enable_move_assignment const&) noexcept = default;
  constexpr Enable_move_assignment &operator=(Enable_move_assignment&&) noexcept = default;
};
template <typename Tag>
struct Enable_move_assignment<false, Tag> {
  constexpr Enable_move_assignment() noexcept = default;
  constexpr Enable_move_assignment(Enable_move_assignment const&) noexcept  = default;
  constexpr Enable_move_assignment(Enable_move_assignment&&) noexcept = default;
  constexpr Enable_move_assignment &operator=(Enable_move_assignment const&) noexcept = default;
  constexpr Enable_move_assignment &operator=(Enable_move_assignment&&) noexcept = delete;
};
} //end of var namespace
