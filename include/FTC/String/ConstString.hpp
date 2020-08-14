/**
 * @file ConstString.hpp
 * Compile time strings
 */

#pragma once

#include <cstddef>      // for size_t
#include <iterator>     // for std::reverse_iterator
#include <string_view>  // for std::string_view
#include <utility>      // for std::index_sequence, std::make_index_sequence

namespace ftc {

// Forward declaration
template <size_t Len> class ConstString;

/// A wrapper around string literal value
///
/// StringLiteral is intended for capturing the length of a literal value, so that the literal can
/// be easily converted into a ConstString in the future. This class is mainly used as a
/// optimization for warping literal values, as it does not need to copy the entire string like
/// ConstString does. However, as long as there is some calculation on the string literal, it will
/// be converted to a ConstString.
///
/// @tparam Len The length of string (without terminator)
template <size_t Len> class StringLiteral
{
public:
    typedef const char (&LiteralType)[Len + 1];

    /// @defgroup STL container type requirements
    /// @{
    using value_type             = const char;
    using difference_type        = std::ptrdiff_t;
    using size_type              = std::size_t;
    using reference              = const char &;
    using const_reference        = const char &;
    using pointer                = const char *;
    using const_pointer          = const char *;
    using iterator               = const char *;
    using const_iterator         = const char *;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    /// @}

    /// Constructor, initialize a string literal from a literal value
    /// @param literal A string literal value
    [[nodiscard]] constexpr StringLiteral(LiteralType literal) : literal(literal) {}

    /// Default copy constructor
    [[nodiscard]] constexpr StringLiteral(const StringLiteral &) = default;

    /// Conversion back to literal value
    [[nodiscard]] constexpr operator LiteralType() const { return literal; }

    /// Conversion to std::string_view
    [[nodiscard]] constexpr operator std::string_view() const { return {literal, Len}; }

    /// A STL like function to get a the style string (const char *)
    [[nodiscard]] constexpr const char *c_str() const { return literal; }

    /// Gets the length of the string literal
    [[nodiscard]] constexpr size_t Length() const { return Len; }

    /// Checks whether the string literal is empty
    [[nodiscard]] constexpr bool Empty() const { return Len == 0; }

    /// Gets the character in string literal at index
    ///
    /// This function will check the correctness of the given index using static_assert.
    ///
    /// @tparam Index Index of the character, must be in range [0,Length()]
    /// @note Index equals to Length() will get a return value of 0 (terminator).
    template <size_t Index>[[nodiscard]] constexpr const_reference At() const
    {
        static_assert(0 <= Index && Index <= Len, "String literal index out of range");
        return literal[Index];
    }

    /// Gets the character in string literal at index
    /// @param i Index of the character, must be in range [0,Length()]
    /// @note Index equals to Length() will get a return value of 0 (terminator).
    [[nodiscard]] constexpr const_reference operator[](size_t i) const { return literal[i]; }

    /// Gets the front character of the string literal
    [[nodiscard]] constexpr const_reference Front() const { return At<0>(); }

    /// Gets the back character of the string literal
    [[nodiscard]] constexpr const_reference Back() const { return At<Len - 1>(); }

    /// Gets an iterator at the beginning
    [[nodiscard]] constexpr iterator Begin() const { return &literal[0]; }

    /// Gets an iterator at the end
    [[nodiscard]] constexpr iterator End() const { return &literal[Len]; }

    // Range based for loop support, do not use directly
    [[nodiscard]] constexpr iterator begin() const { return Begin(); }
    [[nodiscard]] constexpr iterator end() const { return End(); }

    // StringLiteral can not be assigned
    constexpr StringLiteral &operator=(const StringLiteral &) = delete;

private:
    LiteralType literal;
};

/// Deduction guide for StringLiteral
template <size_t LenWithTerminator>
StringLiteral(const char (&)[LenWithTerminator]) -> StringLiteral<LenWithTerminator - 1>;

template <size_t Len> StringLiteral(ConstString<Len>) -> StringLiteral<Len>;

/// @defgroup Concatenates two string literals, string literal values, char literal values
/// @{
template <size_t L1, size_t L2>
[[nodiscard]] constexpr ConstString<L1 + L2> operator+(const StringLiteral<L1> &lhs,
                                                       const StringLiteral<L2> &rhs);

template <size_t L1, size_t L2>
[[nodiscard]] constexpr ConstString<L1 + L2 - 1> operator+(const StringLiteral<L1> &lhs,
                                                           const char (&rhs)[L2]);

template <size_t L1, size_t L2>
[[nodiscard]] constexpr ConstString<L1 + L2 - 1> operator+(const char (&lhs)[L1],
                                                           const StringLiteral<L2> &rhs);

template <size_t L>
[[nodiscard]] constexpr ConstString<L + 1> operator+(const StringLiteral<L> &lhs, char rhs);

template <size_t L>
[[nodiscard]] constexpr ConstString<L + 1> operator+(char lhs, const StringLiteral<L> &rhs);
/// @}

/// Compile time string
///
/// ConstString is mainly for compile time string storage, it doesn't support many calculations,
/// such as find / find_first_of, etc. If string calculation is needed, it should be first converted
/// to a std::string_view, the use the string_view to calcuate a constexpr result.
///
/// @tparam Len The length of string (without terminator)
template <size_t Len> class ConstString
{
public:
    using StringLiteralType = StringLiteral<Len>;
    using LiteralType       = typename StringLiteralType::LiteralType;

    /// @defgroup STL container type requirements
    /// @{
    using value_type             = const char;
    using difference_type        = std::ptrdiff_t;
    using size_type              = std::size_t;
    using reference              = const char &;
    using const_reference        = const char &;
    using pointer                = const char *;
    using const_pointer          = const char *;
    using iterator               = const char *;
    using const_iterator         = const char *;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    /// @}

    /// Constructor, initialize a const string from a StringLiteral
    [[nodiscard]] constexpr ConstString(StringLiteralType str);

    /// Constructor, initialize a const string from a string literal value
    [[nodiscard]] constexpr ConstString(LiteralType literal);

    /// Constructor, initialize a const string from duplicating char values
    [[nodiscard]] constexpr ConstString(char ch);

    /// Constructor, initialize a const string from a std::string_view
    [[nodiscard]] constexpr ConstString(std::string_view sv);

    /// Constructor, initialize from concatenating two StringLiteral
    /// @note The total length of two strings must be equal to the length of initialized string.
    template <size_t L1, typename = std::enable_if_t<L1 <= Len>>
    [[nodiscard]] constexpr ConstString(const StringLiteral<L1> &      str1,
                                        const StringLiteral<Len - L1> &str2);

    /// Default copy constructor
    [[nodiscard]] constexpr ConstString(const ConstString &) = default;

    /// Get the string literal value from a const string
    [[nodiscard]] constexpr LiteralType GetLiteral() const { return array; }

    /// Conversion to string literal value
    [[nodiscard]] constexpr operator LiteralType() const { return array; }

    /// Get the StringLiteral from a const string
    [[nodiscard]] constexpr StringLiteralType GetStringLiteral() const { return array; }

    /// Explicit conversion to StringLiteral
    [[nodiscard]] constexpr operator StringLiteralType() const { return array; }

    /// Conversion to std::string_view
    [[nodiscard]] constexpr operator std::string_view() const { return {array, Len}; }

    /// A STL like function to get the c style string (const char *)
    [[nodiscard]] constexpr const char *c_str() const { return array; }

    /// Gets the length of the const string
    [[nodiscard]] constexpr size_t Length() const { return Len; }

    /// Checks whether the const string is empty
    [[nodiscard]] constexpr bool Empty() const { return Len == 0; }

    /// Gets the character in string literal at index
    ///
    /// This function will check the correctness of the given index using static_assert.
    ///
    /// @tparam Index Index of the character, must be in range [0,Length()]
    /// @note Index equals to Length() will get a return value of 0 (terminator).
    template <size_t Index>[[nodiscard]] constexpr const_reference At() const
    {
        static_assert(0 <= Index && Index <= Len, "Const string index out of range");
        return array[Index];
    }

    /// Gets the character in const string at index
    /// @param i Index of the character, must be in range [0,Length()]
    /// @note Index equals to Length() will get a return value of 0 (terminator).
    [[nodiscard]] constexpr const_reference operator[](size_t i) const { return array[i]; }

    /// Gets the front character of the const string
    [[nodiscard]] constexpr const_reference Front() const { return At<0>(); }

    /// Gets the back character of the const string
    [[nodiscard]] constexpr const_reference Back() const { return At<Len - 1>(); }

    /// Gets an iterator at the beginning
    [[nodiscard]] constexpr iterator Begin() const { return &array[0]; }

    /// Gets an iterator at the end
    [[nodiscard]] constexpr iterator End() const { return &array[Len]; }

    /// Gets a substring of range [Pos, Pos + Count)
    /// @tparam Pos Position of the first character
    /// @tparam Count Substring length
    template <size_t Pos = 0, size_t Count = Len>[[nodiscard]] constexpr auto SubStr() const;

    // Range based for loop support, do not use directly
    [[nodiscard]] constexpr iterator begin() const { return Begin(); }
    [[nodiscard]] constexpr iterator end() const { return End(); }

    // ConstString can not be assigned
    constexpr ConstString &operator=(const ConstString &other) = delete;

private:
    template <size_t L> friend class ConstString;

    template <typename IndexableString,
              size_t... I,
              typename = decltype(void(std::declval<IndexableString>()[0]))>
    constexpr ConstString(IndexableString str, std::index_sequence<I...>);

    template <size_t... I> constexpr ConstString(char ch, std::index_sequence<I...>);

    template <size_t L, size_t... I1, size_t... I2>
    constexpr ConstString(const StringLiteral<L> &      str1,
                          const StringLiteral<Len - L> &str2,
                          std::index_sequence<I1...>,
                          std::index_sequence<I2...>);

    const char array[Len + 1];
};

/// Deduction guide for ConstString
template <size_t LenWithTerminator>
ConstString(const char (&)[LenWithTerminator]) -> ConstString<LenWithTerminator - 1>;

/// @defgroup Concatenates two const string or string literal
/// @{
template <size_t L1, size_t L2>
[[nodiscard]] constexpr ConstString<L1 + L2> operator+(const ConstString<L1> &lhs,
                                                       const ConstString<L2> &rhs);

template <size_t L1, size_t L2>
[[nodiscard]] constexpr ConstString<L1 + L2> operator+(const ConstString<L1> &  lhs,
                                                       const StringLiteral<L2> &rhs);

template <size_t L1, size_t L2>
[[nodiscard]] constexpr ConstString<L1 + L2> operator+(const StringLiteral<L1> &lhs,
                                                       const ConstString<L2> &  rhs);

template <size_t L1, size_t L2>
[[nodiscard]] constexpr ConstString<L1 + L2 - 1> operator+(const ConstString<L1> &lhs,
                                                           const char (&rhs)[L2]);

template <size_t L1, size_t L2>
[[nodiscard]] constexpr ConstString<L1 + L2 - 1> operator+(const char (&lhs)[L1],
                                                           const ConstString<L2> &rhs);

template <size_t L>
[[nodiscard]] constexpr ConstString<L + 1> operator+(const ConstString<L> &lhs, char rhs);

template <size_t L>
[[nodiscard]] constexpr ConstString<L + 1> operator+(char lhs, const ConstString<L> &rhs);
/// @}

}  // namespace ftc

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

namespace ftc {

namespace detail {

    template <size_t N> struct RepeatNTimes
    {
        template <typename T> static constexpr auto Identity(T v) { return v; };
    };

}  // namespace detail

template <size_t L1, size_t L2>
inline constexpr ConstString<L1 + L2> operator+(const StringLiteral<L1> &lhs,
                                                const StringLiteral<L2> &rhs)
{
    return ConstString<L1 + L2>(lhs, rhs);
}

template <size_t L1, size_t L2>
inline constexpr ConstString<L1 + L2 - 1> operator+(const StringLiteral<L1> &lhs,
                                                    const char (&rhs)[L2])
{
    return ConstString<L1 + L2 - 1>(lhs, StringLiteral<L2 - 1> {rhs});
}

template <size_t L1, size_t L2>
inline constexpr ConstString<L1 + L2 - 1> operator+(const char (&lhs)[L1],
                                                    const StringLiteral<L2> &rhs)
{
    return ConstString<L1 + L2 - 1>(StringLiteral<L1 - 1> {lhs}, rhs);
}

template <size_t L>
inline constexpr ConstString<L + 1> operator+(const StringLiteral<L> &lhs, char rhs)
{
    return ConstString<L>(lhs) + rhs;
}

template <size_t L>
inline constexpr ConstString<L + 1> operator+(char lhs, const StringLiteral<L> &rhs)
{
    return lhs + ConstString<L>(rhs);
}

template <size_t Len>
template <typename IndexableString, size_t... I, typename>
inline constexpr ConstString<Len>::ConstString(IndexableString str, std::index_sequence<I...>)
    : array {str[I]..., '\0'}
{}

template <size_t Len>
template <size_t... I>
inline constexpr ConstString<Len>::ConstString(char ch, std::index_sequence<I...>)
    : array {detail::RepeatNTimes<I>::Identity(ch)..., '\0'}
{}

template <size_t Len>
template <size_t L, size_t... I1, size_t... I2>
inline constexpr ConstString<Len>::ConstString(const StringLiteral<L> &      str1,
                                               const StringLiteral<Len - L> &str2,
                                               std::index_sequence<I1...>,
                                               std::index_sequence<I2...>)
    : array {str1[I1]..., str2[I2]..., '\0'}
{}

template <size_t Len>
inline constexpr ConstString<Len>::ConstString(StringLiteralType str)
    : ConstString(str, std::make_index_sequence<Len> {})
{}

template <size_t Len>
inline constexpr ConstString<Len>::ConstString(LiteralType literal)
    : ConstString(literal, std::make_index_sequence<Len> {})
{}

template <size_t Len>
inline constexpr ConstString<Len>::ConstString(char ch)
    : ConstString(ch, std::make_index_sequence<Len> {})
{}

template <size_t Len>
inline constexpr ConstString<Len>::ConstString(std::string_view sv)
    : ConstString((void(/* TODO: Assert sv.size() == Len */), sv), std::make_index_sequence<Len> {})
{}

template <size_t Len>
template <size_t L1, typename>
inline constexpr ConstString<Len>::ConstString(const StringLiteral<L1> &      str1,
                                               const StringLiteral<Len - L1> &str2)
    : ConstString(str1,
                  str2,
                  std::make_index_sequence<L1> {},
                  std::make_index_sequence<Len - L1> {})
{}

template <size_t Len>
template <size_t Pos, size_t Count>
inline constexpr auto ConstString<Len>::SubStr() const
{
    static_assert(Pos < Len, "Substring position must be in range [0, Len)");
    constexpr size_t MaxLength = Len - Pos;
    constexpr size_t SubLength = Count < MaxLength ? Count : MaxLength;

    return ConstString<SubLength>(array + Pos, std::make_index_sequence<SubLength>());
}

template <size_t L1, size_t L2>
inline constexpr ConstString<L1 + L2> operator+(const ConstString<L1> &lhs,
                                                const ConstString<L2> &rhs)
{
    return ConstString<L1 + L2>(lhs.GetStringLiteral(), rhs.GetStringLiteral());
}

template <size_t L1, size_t L2>
inline constexpr ConstString<L1 + L2> operator+(const ConstString<L1> &  lhs,
                                                const StringLiteral<L2> &rhs)
{
    return ConstString<L1 + L2>(lhs.GetStringLiteral(), rhs);
}

template <size_t L1, size_t L2>
inline constexpr ConstString<L1 + L2> operator+(const StringLiteral<L1> &lhs,
                                                const ConstString<L2> &  rhs)
{
    return ConstString<L1 + L2>(lhs, rhs.GetStringLiteral());
}

template <size_t L1, size_t L2>
inline constexpr ConstString<L1 + L2 - 1> operator+(const ConstString<L1> &lhs,
                                                    const char (&rhs)[L2])
{
    return ConstString<L1 + L2 - 1>(lhs.GetStringLiteral(), StringLiteral<L2 - 1> {rhs});
}

template <size_t L1, size_t L2>
inline constexpr ConstString<L1 + L2 - 1> operator+(const char (&lhs)[L1],
                                                    const ConstString<L2> &rhs)
{
    return ConstString<L1 + L2 - 1>(StringLiteral<L1 - 1> {lhs}, rhs.GetStringLiteral());
}

template <size_t L>
inline constexpr ConstString<L + 1> operator+(const ConstString<L> &lhs, char rhs)
{
    return lhs + ConstString<1>(rhs);
}

template <size_t L>
inline constexpr ConstString<L + 1> operator+(char lhs, const ConstString<L> &rhs)
{
    return ConstString<1>(lhs) + rhs;
}

}  // namespace ftc
