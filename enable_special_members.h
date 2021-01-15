#pragma once

struct Enable_default_constructor_tag
{
  explicit constexpr Enable_default_constructor_tag() = default;
};

/**
  * @brief A mixin helper to conditionally enable or disable the default
  * constructor.
  * @sa Enable_special_members
  */
template<bool Switch, typename Tag = void>
struct Enable_default_constructor
{
  constexpr Enable_default_constructor() noexcept = default;
  constexpr Enable_default_constructor(Enable_default_constructor const&)
  noexcept  = default;
  constexpr Enable_default_constructor(Enable_default_constructor&&)
  noexcept = default;
  Enable_default_constructor&
  operator=(Enable_default_constructor const&) noexcept = default;
  Enable_default_constructor&
  operator=(Enable_default_constructor&&) noexcept = default;

  // Can be used in other ctors.
  constexpr explicit
  Enable_default_constructor(Enable_default_constructor_tag) { }
};


/**
  * @brief A mixin helper to conditionally enable or disable the default
  * destructor.
  * @sa Enable_special_members
  */
template<bool Switch, typename Tag = void>
struct Enable_destructor { };

/**
  * @brief A mixin helper to conditionally enable or disable the copy/move
  * special members.
  * @sa Enable_special_members
  */
template<bool Copy, bool CopyAssignment,
    bool Move, bool MoveAssignment,
    typename Tag = void>
struct Enable_copy_move { };

/**
  * @brief A mixin helper to conditionally enable or disable the special
  * members.
  *
  * The @c Tag type parameter is to make mixin bases unique and thus avoid
  * ambiguities.
  */
template<bool Default, bool Destructor,
    bool Copy, bool CopyAssignment,
    bool Move, bool MoveAssignment,
    typename Tag = void>
struct Enable_special_members
    : private Enable_default_constructor<Default, Tag>,
      private Enable_destructor<Destructor, Tag>,
      private Enable_copy_move<Copy, CopyAssignment,
                                Move, MoveAssignment,
                                Tag>
{ };

// Boilerplate follows.

template<typename Tag>
struct Enable_default_constructor<false, Tag>
{
  constexpr Enable_default_constructor() noexcept = delete;
  constexpr Enable_default_constructor(Enable_default_constructor const&)
  noexcept  = default;
  constexpr Enable_default_constructor(Enable_default_constructor&&)
  noexcept = default;
  Enable_default_constructor&
  operator=(Enable_default_constructor const&) noexcept = default;
  Enable_default_constructor&
  operator=(Enable_default_constructor&&) noexcept = default;

  // Can be used in other ctors.
  constexpr explicit
  Enable_default_constructor(Enable_default_constructor_tag) { }
};

template<typename Tag>
struct Enable_destructor<false, Tag>
{ ~Enable_destructor() noexcept = delete; };

template<typename Tag>
struct Enable_copy_move<false, true, true, true, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = delete;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = default;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = default;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = default;
};

template<typename Tag>
struct Enable_copy_move<true, false, true, true, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = default;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = default;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = delete;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = default;
};

template<typename Tag>
struct Enable_copy_move<false, false, true, true, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = delete;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = default;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = delete;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = default;
};

template<typename Tag>
struct Enable_copy_move<true, true, false, true, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = default;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = delete;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = default;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = default;
};

template<typename Tag>
struct Enable_copy_move<false, true, false, true, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = delete;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = delete;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = default;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = default;
};

template<typename Tag>
struct Enable_copy_move<true, false, false, true, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = default;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = delete;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = delete;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = default;
};

template<typename Tag>
struct Enable_copy_move<false, false, false, true, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = delete;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = delete;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = delete;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = default;
};

template<typename Tag>
struct Enable_copy_move<true, true, true, false, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = default;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = default;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = default;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = delete;
};

template<typename Tag>
struct Enable_copy_move<false, true, true, false, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = delete;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = default;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = default;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = delete;
};

template<typename Tag>
struct Enable_copy_move<true, false, true, false, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = default;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = default;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = delete;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = delete;
};

template<typename Tag>
struct Enable_copy_move<false, false, true, false, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = delete;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = default;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = delete;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = delete;
};

template<typename Tag>
struct Enable_copy_move<true, true, false, false, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = default;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = delete;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = default;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = delete;
};

template<typename Tag>
struct Enable_copy_move<false, true, false, false, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = delete;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = delete;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = default;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = delete;
};

template<typename Tag>
struct Enable_copy_move<true, false, false, false, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = default;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = delete;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = delete;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = delete;
};

template<typename Tag>
struct Enable_copy_move<false, false, false, false, Tag>
{
  constexpr Enable_copy_move() noexcept                          = default;
  constexpr Enable_copy_move(Enable_copy_move const&) noexcept  = delete;
  constexpr Enable_copy_move(Enable_copy_move&&) noexcept       = delete;
  Enable_copy_move&
  operator=(Enable_copy_move const&) noexcept                    = delete;
  Enable_copy_move&
  operator=(Enable_copy_move&&) noexcept                         = delete;
};
