#pragma once

/*************************************************************************

    A generic implementation of the BitmaskType C++ concept
    http://en.cppreference.com/w/cpp/concept/BitmaskType
    by Andrey Upadyshev (oliora@gmail.com)

    Some documentation may be available at
    https://github.com/oliora/bitmask
 
    Version: 1.0
 
    **********************************************************************

    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <http://unlicense.org/>

 *************************************************************************/

#include <type_traits>
#include <functional>  // for std::hash


namespace boost {

    namespace bitmask_detail {
        // Let's use std::void_t. It's introduced in C++17 but we can easily add it by ourself
        // See more at http://en.cppreference.com/w/cpp/types/void_t
        template<typename... Ts> struct make_void { typedef void type;};
        template<typename... Ts> using void_t = typename make_void<Ts...>::type;

        template<class T>
        struct underlying_type
        {
            static_assert(std::is_enum<T>::value, "T must be a enum type");

            using type = typename std::underlying_type<T>::type;
        };

        template<class T>
        using underlying_type_t = typename underlying_type<T>::type;

        template<class T, T MaxElement = T::_bitmask_max_element>
        struct mask_from_max_element
        {
            static constexpr underlying_type_t<T> max_element_value_ =
                static_cast<underlying_type_t<T>>(MaxElement);

            static_assert(max_element_value_ >= 0, "Max element can not be negative");

            // `(value - 1 << 1) + 1` is used rather that simpler `(value << 1) - 1`
            // because latter can overflow.
            static constexpr underlying_type_t<T> value =
                max_element_value_ ? (max_element_value_ - 1 << 1) + 1 : 0;
        };

        template<class, class = void_t<>>
        struct has_max_element : std::false_type {};

        template<class T>
        struct has_max_element<T, void_t<decltype(T::_bitmask_max_element)>> : std::true_type {};

        template<class, class = void_t<>>
        struct has_value_mask : std::false_type {};

        template<class T>
        struct has_value_mask<T, void_t<decltype(T::_bitmask_value_mask)>> : std::true_type {};

        template<class T>
        struct is_valid_enum_definition : std::integral_constant<bool,
            !has_value_mask<T>::value || !has_max_element<T>::value> {};

        template<class T>
        inline constexpr typename std::enable_if<has_value_mask<T>::value, underlying_type_t<T>>::type
        get_enum_mask(T) noexcept
        {
            return static_cast<underlying_type_t<T>>(T::_bitmask_value_mask);
        }

        template<class T>
        inline constexpr typename std::enable_if<has_max_element<T>::value, underlying_type_t<T>>::type
        get_enum_mask(T) noexcept
        {
            return mask_from_max_element<T>::value;
        }

        template<class T>
        static constexpr underlying_type_t<T> disable_unused_function_warnings() noexcept
        {
            return (static_cast<T>(0) & static_cast<T>(0)).bits()
                & (static_cast<T>(0) | static_cast<T>(0)).bits()
                & (static_cast<T>(0) ^ static_cast<T>(0)).bits()
                & (~static_cast<T>(0)).bits()
                & bits(static_cast<T>(0));
        }
    }

    template<class T>
    inline constexpr bitmask_detail::underlying_type_t<T> get_enum_mask(T) noexcept
    {
        static_assert(bitmask_detail::is_valid_enum_definition<T>::value,
                      "Only one of _bitmask_max_element and _bitmask_value_mask can be specified");
        return bitmask_detail::get_enum_mask(T{});
    }


    template<class T>
    class bitmask
    {
    public:
        using value_type = T;
        using underlying_type = bitmask_detail::underlying_type_t<T>;
        static constexpr underlying_type mask_value = get_enum_mask(T{});

        constexpr bitmask() noexcept = default;
        constexpr bitmask(std::nullptr_t) noexcept: m_bits(0) {}
        constexpr bitmask(value_type value) noexcept: m_bits(static_cast<underlying_type>(value)) {}

        constexpr underlying_type bits() const noexcept { return m_bits; }

        constexpr explicit operator bool() const noexcept { return m_bits ? true : false; }

        constexpr bitmask operator ~ () const noexcept
        {
            return bitmask{~m_bits & mask_value};
        }

        constexpr bitmask operator & (const bitmask& r) const noexcept
        {
            return bitmask{m_bits & r.m_bits};
        }

        constexpr bitmask operator | (const bitmask& r) const noexcept
        {
            return bitmask{m_bits | r.m_bits};
        }

        constexpr bitmask operator ^ (const bitmask& r) const noexcept
        {
            return bitmask{m_bits ^ r.m_bits};
        }

        bitmask& operator |= (const bitmask& r) noexcept
        {
            m_bits |= r.m_bits;
            return *this;
        }

        bitmask& operator &= (const bitmask& r) noexcept
        {
            m_bits &= r.m_bits;
            return *this;
        }

        bitmask& operator ^= (const bitmask& r) noexcept
        {
            m_bits ^= r.m_bits;
            return *this;
        }

    private:
        explicit bitmask(const underlying_type& bits) noexcept : m_bits(bits) {}

        underlying_type m_bits = 0;
    };

    template<class T>
    inline constexpr bitmask<T> operator & (T l, const bitmask<T>& r) noexcept { return r & l; }

    template<class T>
    inline constexpr bitmask<T> operator | (T l, const bitmask<T>& r) noexcept { return r | l; }

    template<class T>
    inline constexpr bitmask<T> operator ^ (T l, const bitmask<T>& r) noexcept { return r ^ l; }

    template<class T>
    inline constexpr bool operator != (const bitmask<T>& l, const bitmask<T>& r) noexcept { return l.bits() != r.bits(); }

    template<class T>
    inline constexpr bool operator == (const bitmask<T>& l, const bitmask<T>& r) noexcept { return ! operator != (l, r); }

    template<class T>
    inline constexpr bool operator != (T l, const bitmask<T>& r) noexcept { return static_cast<bitmask_detail::underlying_type_t<T>>(l) != r.bits(); }

    template<class T>
    inline constexpr bool operator == (T l, const bitmask<T>& r) noexcept { return ! operator != (l, r); }

    template<class T>
    inline constexpr bool operator != (const bitmask<T>& l, T r) noexcept { return l.bits() != static_cast<bitmask_detail::underlying_type_t<T>>(r); }

    template<class T>
    inline constexpr bool operator == (const bitmask<T>& l, T r) noexcept { return ! operator != (l, r); }

    template<class Integral, class T>
    inline constexpr typename std::enable_if<std::is_integral<Integral>::value, bool>::type
    operator != (const Integral& l, const bitmask<T>& r) noexcept { return l != r.bits(); }

    template<class Integral, class T>
    inline constexpr typename std::enable_if<std::is_integral<Integral>::value, bool>::type
    operator == (const Integral& l, const bitmask<T>& r) noexcept { return ! operator != (l, r); }

    template<class Integral, class T>
    inline constexpr typename std::enable_if<std::is_integral<Integral>::value, bool>::type
    operator != (const bitmask<T>& l, const Integral& r) noexcept { return l.bits() != r; }

    template<class Integral, class T>
    inline constexpr typename std::enable_if<std::is_integral<Integral>::value, bool>::type
    operator == (const bitmask<T>& l, const Integral& r) noexcept { return ! operator != (l, r); }

    // Allow `bitmask` to be be used as a map key
    template<class T>
    inline constexpr bool operator < (const bitmask<T>& l, const bitmask<T>& r) noexcept { return l.bits() < r.bits(); }

    template<class T>
    inline constexpr typename bitmask<T>::underlying_type bits(const bitmask<T>& bm) noexcept
    {
        return bm.bits();
    }
}


namespace std
{
    template<class T>
    struct hash<boost::bitmask<T>>
    {
        constexpr std::size_t operator() (const boost::bitmask<T>& op) const noexcept
        {
            using ut = typename boost::bitmask<T>::underlying_type;
            return std::hash<ut>{}(op.bits());
        }
    };
}

// Implementation detail macros

#define BOOST_BITMASK_DETAIL_DEFINE_OPS(value_type) \
    inline constexpr boost::bitmask<value_type> operator & (value_type l, value_type r) noexcept { return boost::bitmask<value_type>{l} & r; } \
    inline constexpr boost::bitmask<value_type> operator | (value_type l, value_type r) noexcept { return boost::bitmask<value_type>{l} | r; } \
    inline constexpr boost::bitmask<value_type> operator ^ (value_type l, value_type r) noexcept { return boost::bitmask<value_type>{l} ^ r; } \
    inline constexpr boost::bitmask<value_type> operator ~ (value_type op) noexcept { return ~boost::bitmask<value_type>{op}; }                \
    inline constexpr boost::bitmask<value_type>::underlying_type bits(value_type op) noexcept { return boost::bitmask<value_type>{op}.bits(); }\
    using unused_bitmask_ ## value_type ## _t_ = decltype(boost::bitmask_detail::disable_unused_function_warnings<value_type>());

#define BOOST_BITMASK_DETAIL_DEFINE_VALUE_MASK(value_type, value_mask) \
    inline constexpr boost::bitmask_detail::underlying_type_t<value_type> get_enum_mask(value_type) noexcept { return value_mask; }

#define BOOST_BITMASK_DETAIL_DEFINE_MAX_ELEMENT(value_type, max_element) \
    inline constexpr boost::bitmask_detail::underlying_type_t<value_type> get_enum_mask(value_type) noexcept { \
        return boost::bitmask_detail::mask_from_max_element<value_type, value_type::max_element>::value;       \
    }


// Public macros

// Defines missing operations for a bit-mask elements enum 'value_type'
// Value mask is taken from 'value_type' definition. One should has either
// '_bitmask_value_mask' or '_bitmask_max_element' element defined.
#define BOOST_BITMASK(value_type) \
    BOOST_BITMASK_DETAIL_DEFINE_OPS(value_type)

// Defines missing operations and a value mask for
// a bit-mask elements enum 'value_type'
#define BOOST_BITMASK_VALUE_MASK(value_type, value_mask) \
    BOOST_BITMASK_DETAIL_DEFINE_VALUE_MASK(value_type, value_mask) \
    BOOST_BITMASK_DETAIL_DEFINE_OPS(value_type)

// Defines missing operations and a value mask based on max element for
// a bit-mask elements enum 'value_type'
#define BOOST_BITMASK_MAX_ELEMENT(value_type, max_element) \
    BOOST_BITMASK_DETAIL_DEFINE_MAX_ELEMENT(value_type, max_element) \
    BOOST_BITMASK_DETAIL_DEFINE_OPS(value_type)
