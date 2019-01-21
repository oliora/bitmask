#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <bitmask/bitmask.hpp>


namespace intrusive
{
    enum class syntax_option_type
    {
        ECMAScript = 0,
        icase      = 1 << 0,
        nosubs     = 1 << 1,
        optimize   = 1 << 2,
        collate    = 1 << 3,
        basic      = 1 << 4,
        awk        = 1 << 5,

        _bitmask_max_element = awk
    };

    BITMASK_DEFINE(syntax_option_type)

    enum class open_mode
    {
        app     = 0x01,
        binary  = 0x02,
        ate     = 0x40,

        _bitmask_value_mask = 0x43
    };

    BITMASK_DEFINE(open_mode)
}

namespace nonintrusive
{
    enum class syntax_option_type
    {
        ECMAScript = 0,
        icase      = 1 << 0,
        nosubs     = 1 << 1,
        optimize   = 1 << 2,
        collate    = 1 << 3,
        basic      = 1 << 4,
        awk        = 1 << 5,
    };

    BITMASK_DEFINE_MAX_ELEMENT(syntax_option_type, awk)

    enum class open_mode
    {
        app     = 0x01,
        binary  = 0x02,
        ate     = 0x40,
    };

    BITMASK_DEFINE_VALUE_MASK(open_mode, 0x43)
}


#include <cstdint>
#include <unordered_map>
#include <map>


TEST_CASE( "zero_bitmask", "[]" )
{
    using intrusive::syntax_option_type;

    bitmask::bitmask<syntax_option_type> x;

    int i = x.bits();

    CHECK(i == 0);
    CHECK(!x);
    CHECK_FALSE(x);

    CHECK(!(x & syntax_option_type::icase));
    CHECK((x == 0));
    CHECK((0 == x));
    CHECK((x & syntax_option_type::icase) == 0);
    CHECK(0 == (x & syntax_option_type::icase));
    CHECK(x.bits() == 0);
    CHECK(0 == x.bits());

    CHECK(((x & syntax_option_type::ECMAScript) == syntax_option_type::ECMAScript));
    CHECK((x == syntax_option_type::ECMAScript));
    CHECK((syntax_option_type::ECMAScript == x));

    bitmask::bitmask<syntax_option_type> y = 0;

    CHECK(!y);
    CHECK_FALSE(y);
    CHECK(y.bits() == 0);

    using nonintrusive::open_mode;

    bitmask::bitmask<open_mode> z;

    CHECK(!z);
    CHECK_FALSE(z);
    CHECK(0 == z.bits());

    CHECK(!(z & open_mode::app));
    CHECK((z & open_mode::app) == 0);
    CHECK(z.bits() == 0);

    bitmask::bitmask<open_mode> z2 = nullptr;
    CHECK(!z2);
    CHECK_FALSE(z2);
    CHECK(0 == z2.bits());

    bitmask::bitmask<open_mode> z3{nullptr};
    CHECK(!z3);
    CHECK_FALSE(z3);
    CHECK(0 == z3.bits());

    bitmask::bitmask<open_mode> z4{0};
    CHECK(!z4);
    CHECK_FALSE(z4);
    CHECK(0 == z4.bits());

    bitmask::bitmask<open_mode> z5 = 0;
    CHECK(!z5);
    CHECK_FALSE(z5);
    CHECK(0 == z5.bits());
}

TEST_CASE( "bitmask_basics", "[]" )
{
    using intrusive::syntax_option_type;

    static_assert(std::is_same<bitmask::bitmask<syntax_option_type>::underlying_type, unsigned>::value, "");

    bitmask::bitmask<syntax_option_type> x{syntax_option_type::awk | syntax_option_type::icase};

    CHECK(x.bits() == (static_cast<int>(syntax_option_type::awk) | static_cast<int>(syntax_option_type::icase)));

    int i = x.bits();

    CHECK(i == (static_cast<int>(syntax_option_type::awk) | static_cast<int>(syntax_option_type::icase)));
    CHECK(i == x.bits());
    CHECK(i == x);
    CHECK(x == i);

    CHECK(x);

    CHECK((x & syntax_option_type::icase));
    if (x & syntax_option_type::icase)
        CHECK(true);
    else
        CHECK(false);
    CHECK((x & syntax_option_type::icase) != 0);
    CHECK(0 != (x & syntax_option_type::icase));
    CHECK((x & syntax_option_type::icase) == syntax_option_type::icase);
    CHECK(syntax_option_type::icase == (x & syntax_option_type::icase));
    CHECK((x & syntax_option_type::icase) != syntax_option_type::awk);
    CHECK(syntax_option_type::awk != (x & syntax_option_type::icase));

    CHECK((x & syntax_option_type::awk));
    CHECK((x & syntax_option_type::awk) != 0);
    CHECK(0 != (x & syntax_option_type::awk));

    CHECK((x & x).bits() == x.bits());

    CHECK(!(x & syntax_option_type::nosubs));
    CHECK((x & syntax_option_type::nosubs) == 0);
    CHECK(0 == (x & syntax_option_type::nosubs));
    if (x & syntax_option_type::nosubs)
        CHECK(false);
    else
        CHECK(true);

    CHECK(x == (syntax_option_type::awk | syntax_option_type::icase));
    CHECK((syntax_option_type::awk | syntax_option_type::icase) == x);
    CHECK((syntax_option_type::awk | syntax_option_type::basic) != x);

    CHECK((x | syntax_option_type::basic) != x);
    CHECK((x | syntax_option_type::icase) == x);
    CHECK((x | syntax_option_type::basic) == (syntax_option_type::basic | x));
    CHECK(((syntax_option_type::icase ^ x) == syntax_option_type::awk));
    CHECK(((x ^ syntax_option_type::icase) == syntax_option_type::awk));
    CHECK((syntax_option_type::awk == (x ^ syntax_option_type::icase)));
    CHECK((syntax_option_type::awk == (syntax_option_type::icase ^ x)));

    auto y = syntax_option_type::awk | syntax_option_type::icase | syntax_option_type::collate;

    CHECK(y);
    CHECK(y.bits());

    CHECK_FALSE((y ^ y));
    CHECK((y ^ y) == 0);
    CHECK((y ^ y) != 1);
    CHECK(1 != (y ^ y));

    CHECK_FALSE((syntax_option_type::icase ^ syntax_option_type::icase));
    CHECK((syntax_option_type::icase ^ syntax_option_type::icase) == 0);
    CHECK(0 == (syntax_option_type::icase ^ syntax_option_type::icase));

    CHECK((syntax_option_type::icase ^ syntax_option_type::basic) == (syntax_option_type::basic ^ syntax_option_type::icase));
    CHECK((syntax_option_type::basic == (syntax_option_type::icase ^ syntax_option_type::basic ^ syntax_option_type::icase)));

    CHECK((y & syntax_option_type::icase));
    CHECK((y & syntax_option_type::icase) != 0);

    CHECK((y & syntax_option_type::awk));
    CHECK(((y & syntax_option_type::awk) != 0));

    CHECK((y & syntax_option_type::collate));
    CHECK(((y & syntax_option_type::collate) != 0));

    CHECK(!(y & syntax_option_type::nosubs));
    CHECK((y & syntax_option_type::nosubs) == 0);

    CHECK(0 == (syntax_option_type::collate & syntax_option_type::awk));
    CHECK(static_cast<int>(syntax_option_type::collate) == (syntax_option_type::collate & syntax_option_type::collate));
    CHECK(0 != (syntax_option_type::collate & (syntax_option_type::collate & syntax_option_type::collate)));

    const auto y_copy = y;

    CHECK(y_copy == y);
    CHECK(y == y_copy);
}

namespace {
    enum class short_enum: short
    {
        v1 = 1,
        v2 = 2,

        _bitmask_max_element = v2
    };

    BITMASK_DEFINE(short_enum)

    enum class long_enum: long
    {
        v1 = 1,
        v2 = 2,

        _bitmask_max_element = v2
    };

    BITMASK_DEFINE(long_enum)

    enum class longest_enum: uint64_t
    {
        v1 = 1,
        v2 = 0x8000000000000000,

        _bitmask_max_element = v2
    };

    BITMASK_DEFINE(longest_enum)
}

TEST_CASE( "bitmask_underlying_type", "[]" )
{
    static_assert(sizeof(bitmask::bitmask<short_enum>::underlying_type) == sizeof(short),
                  "sizes are equal");
    static_assert(sizeof(bitmask::bitmask<short_enum>::underlying_type) == sizeof(short),
                  "sizes are equal");
    static_assert(sizeof(bitmask::bitmask<short_enum>::underlying_type) == sizeof(decltype(bitmask::bitmask<short_enum>{}.bits())),
                  "sizes are equal");
    static_assert(sizeof(bitmask::bitmask<long_enum>::underlying_type) == sizeof(decltype(bitmask::bitmask<long_enum>{}.bits())),
                  "sizes are equal");

    bitmask::bitmask<longest_enum> x = longest_enum::v1 | longest_enum::v2;
    CHECK(x.bits() == 0x8000000000000001);
    CHECK((longest_enum::v1 | longest_enum::v2).bits() == 0x8000000000000001);
}

TEST_CASE( "bitmask_compare_and_hash", "[]" )
{
    using intrusive::syntax_option_type;

    bitmask::bitmask<syntax_option_type> x1{syntax_option_type::collate ^ syntax_option_type::awk};
    bitmask::bitmask<syntax_option_type> x2{syntax_option_type::collate};
    bitmask::bitmask<syntax_option_type> x3 = x1;

    using hash = std::hash<bitmask::bitmask<syntax_option_type>>;

    CHECK(bitmask::bitmask<syntax_option_type>{} < x1);
    CHECK(x1 != x2);
    CHECK(x1 == x1);
    CHECK_FALSE(x1 < x2);
    CHECK(x2 < x1);
    CHECK_FALSE(x1 < x1);
    CHECK(hash{}(x1) == hash{}(x1));
    CHECK(hash{}(x1) != hash{}(x2));

    CHECK(x1 == x3);
    CHECK_FALSE(x1 < x3);
    CHECK_FALSE(x3 < x1);
    CHECK(hash{}(x1) == hash{}(x3));

    std::map<bitmask::bitmask<syntax_option_type>, int> map;
    map.emplace(syntax_option_type::collate, 2);
    map.emplace(bitmask::bitmask<syntax_option_type>{}, 3);
    CHECK(map.begin()->second == 3);
    CHECK((--map.end())->second == 2);
    CHECK(map[syntax_option_type::collate] == 2);
    CHECK(map[nullptr] == 3);

    std::unordered_map<bitmask::bitmask<syntax_option_type>, int> umap;
    map.emplace(nullptr, 2);
    map.emplace(syntax_option_type::collate, 3);
}

TEST_CASE( "bitmask_assign_op", "[]" )
{
    using intrusive::open_mode;

    bitmask::bitmask<open_mode> x = open_mode::app;
    CHECK((x & open_mode::app));
    CHECK(!(x & open_mode::ate));

    x |= open_mode::ate;

    CHECK((x & open_mode::app));
    CHECK((x & open_mode::ate));
    CHECK(!(x & open_mode::binary));

    x &= open_mode::binary;

    CHECK(!(x & open_mode::app));
    CHECK(!(x & open_mode::ate));
    CHECK(!(x & open_mode::binary));

    x = open_mode::ate;
    x |= open_mode::app;

    CHECK((x & open_mode::app));
    CHECK((x & open_mode::ate));

    x &= open_mode::ate;

    CHECK(!(x & open_mode::app));
    CHECK((x & open_mode::ate));

    x |= open_mode::app;

    x &= open_mode::app | open_mode::ate;

    CHECK((x & open_mode::app));
    CHECK((x & open_mode::ate));

    x &= open_mode::app | open_mode::binary;

    CHECK((x & open_mode::app));
    CHECK(!(x & open_mode::ate));
    CHECK(!(x & open_mode::binary));

    x ^= open_mode::app;

    CHECK(!(x & open_mode::app));
    CHECK(!(x & open_mode::ate));
    CHECK(!(x & open_mode::binary));

    x = open_mode::ate | open_mode::app;

    CHECK((x & open_mode::app));
    CHECK((x & open_mode::ate));

    x ^= open_mode::app | open_mode::binary;

    CHECK(!(x & open_mode::app));
    CHECK((x & open_mode::ate));
    CHECK((x & open_mode::binary));
}

TEST_CASE("bitmask_bits_func", "[]")
{
    CHECK(bits(intrusive::open_mode::app) == 0x01);
    CHECK(bits(nonintrusive::syntax_option_type::collate) == 0x08);

    CHECK(bits(intrusive::open_mode::app | intrusive::open_mode::ate) == 0x41);
    CHECK(bits(intrusive::open_mode::app | intrusive::open_mode::ate) == (intrusive::open_mode::app | intrusive::open_mode::ate).bits());

    using bm = bitmask::bitmask<intrusive::open_mode>;
    CHECK(bits(bm{}) == 0);
    CHECK(bits(bm{intrusive::open_mode::ate}) == 0x40);
    CHECK(bits(bm{intrusive::open_mode::ate}) == bm{intrusive::open_mode::ate}.bits());
}

TEST_CASE( "bitmask_intrusive_value_mask", "[]" )
{
    CHECK(bitmask::bitmask<intrusive::open_mode>::mask_value == 0x43);
}

TEST_CASE( "bitmask_intrusive_max_element", "[]" )
{
    CHECK(bitmask::bitmask<intrusive::syntax_option_type>::mask_value == 0x3F);
}

TEST_CASE( "bitmask_nonintrusive_value_mask", "[]" )
{
    CHECK(bitmask::bitmask<nonintrusive::open_mode>::mask_value == static_cast<int>(0x43));
}

TEST_CASE( "bitmask_nonintrusive_max_element", "[]" )
{
    CHECK(bitmask::bitmask<nonintrusive::syntax_option_type>::mask_value == 0x3F);
}


namespace {
    namespace tst1
    {
        enum unscoped_enum
        {
            flag1_1 = 0x01,
            flag1_2 = 0x02,
            flag1_3 = 0x10,

            _bitmask_value_mask = 0x13
        };

        BITMASK_DEFINE(unscoped_enum)
    }
    namespace tst2
    {
        enum unscoped_enum
        {
            flag2_1 = 0x01,
            flag2_2 = 0x02,
            flag2_3 = 0x10,
        };

        BITMASK_DEFINE_VALUE_MASK(unscoped_enum, 0x13)
    }
}

TEST_CASE( "bitmask_unscoped_enum", "[]" )
{
    using intrusive::open_mode;

    CHECK((bitmask::bitmask<tst1::unscoped_enum>::mask_value == 0x13));
    CHECK((tst1::flag1_3 & (tst1::flag1_1 | tst1::flag1_3)));

    if (tst1::flag1_3 & (tst1::flag1_1 | tst1::flag1_3))
        CHECK(true);

    if (tst1::flag1_3 & (tst1::flag1_1 | tst1::flag1_3))
        CHECK(true);

    CHECK((tst1::flag1_1 | tst1::flag1_3).bits() == 0x11);
    CHECK(bits(tst1::flag1_1 | tst1::flag1_3) == 0x11);

    CHECK((bitmask::bitmask<tst2::unscoped_enum>::mask_value == 0x13));
    CHECK((tst2::flag2_1 | tst2::flag2_3).bits() == 0x11);
}

TEST_CASE("bitmask_ones_complement", "[]")
{
    using intrusive::open_mode;

    CHECK((~open_mode::binary) == (open_mode::app | open_mode::ate));
    CHECK((~open_mode::binary).bits() == 0x41);
    CHECK(0 == (~open_mode::binary & open_mode::binary));
    CHECK((~bitmask::bitmask<open_mode>{}).bits() == bitmask::bitmask<open_mode>::mask_value);
    CHECK((~bitmask::bitmask<open_mode>{}).bits() == (open_mode::app | open_mode::ate | open_mode::binary));

    using intrusive::syntax_option_type;

    CHECK((~syntax_option_type::ECMAScript).bits() == 0x3F);
    CHECK((~syntax_option_type::ECMAScript).bits() == bitmask::bitmask<syntax_option_type>::mask_value);
}

namespace {
    enum class extreme_u8: uint8_t {
        min = 0x01,
        max = 0x80,

        _bitmask_max_element = max
    };

    BITMASK_DEFINE(extreme_u8)

    enum class extreme_max_8: int8_t {
        min = 1,
        max = 0x40,

        _bitmask_max_element = max
    };

    BITMASK_DEFINE(extreme_max_8)

    enum class extreme_mask_8: int8_t {
        min = 1,
        max = 0x40,

        _bitmask_value_mask = 0x7F
    };

    BITMASK_DEFINE(extreme_mask_8)

    enum class screwed_extreme_8: int8_t {
        max = int8_t(0x80),
        min = 0x40,

        _bitmask_value_mask = int8_t(0xC1)
    };

    BITMASK_DEFINE(screwed_extreme_8)
}

TEST_CASE("bitmask_limits", "[]")
{
    static_assert(std::is_same<bitmask::bitmask<extreme_u8>::underlying_type, uint8_t>::value, "");
    static_assert(bitmask::bitmask<extreme_u8>::mask_value == 0xFF, "");
    CHECK((extreme_u8::max | extreme_u8::min).bits() == 0x81);

    static_assert(std::is_same<bitmask::bitmask<extreme_max_8>::underlying_type, uint8_t>::value, "");
    static_assert(bitmask::bitmask<extreme_max_8>::mask_value == 0x7F, "");
    CHECK((extreme_max_8::max | extreme_max_8::min).bits() == 0x41);

    static_assert(std::is_same<bitmask::bitmask<extreme_mask_8>::underlying_type, uint8_t>::value, "");
    static_assert(bitmask::bitmask<extreme_mask_8>::mask_value == 0x7F, "");
    CHECK((extreme_mask_8::max | extreme_mask_8::min).bits() == 0x41);

    static_assert(std::is_same<bitmask::bitmask<screwed_extreme_8>::underlying_type, uint8_t>::value, "");
    static_assert(bitmask::bitmask<screwed_extreme_8>::mask_value == 0xC1, "");
    CHECK((screwed_extreme_8::max | screwed_extreme_8::min).bits() == 0xC0);
}

namespace {
    enum class flag_t : uint16_t {
        none = 0,
        execute = 1<<0,
        write = 1<<1,
        read = 1<<2,
        all = execute|write|read,
        max = 0xFFFF,
        _bitmask_value_mask = max
    };
    BITMASK_DEFINE(flag_t)

    class flag : public bitmask::bitmask<flag_t> {
    public:
        flag() {
            m_bits = 0;
        }

        flag(const flag_t& flag) {
            m_bits = static_cast<underlying_type>(flag);
        }

        std::string str() {
            std::ostringstream ss;
            ss<<(*this & flag_t::read ? "r" : "-");
            ss<<(*this & flag_t::write ? "w" : "-");
            ss<<(*this & flag_t::execute ? "x" : "-");
            return ss.str();
        }
    };
}

TEST_CASE("class_derive", "[]")
{
    flag f1;
    CHECK(f1.bits() == 0);
    flag f = flag_t::execute;
    CHECK(f.bits() == 1);
    f &= ~flag_t::execute|flag_t::execute;
    f |= flag_t::write;
    CHECK(f.bits() == 3);
    CHECK(f.str() == "-wx");
    f &= ~f;
    CHECK(f.bits() == 0);
}

#if !defined _MSC_VER
// MS Visual Studio 2015 (even Update 3) has weird support for expressions SFINAE so this test can't be compiled.

namespace {
    using bitmask::bitmask_detail::void_t;
    using bitmask::bitmask_detail::underlying_type_t;

    template<class T, T value, typename = void_t<>>
    struct is_bitmask_constructible : std::false_type {};

    template<class T, T value>
    struct is_bitmask_constructible<T, value, void_t<std::integral_constant<underlying_type_t<T>, bitmask::bitmask<T>{value}.bits()>>> : std::true_type {};
}

TEST_CASE("incorrect_mask", "[]")
{
    using intrusive::open_mode;

    static_assert(is_bitmask_constructible<open_mode, open_mode::app>::value, "");

    static_assert(!is_bitmask_constructible<open_mode, static_cast<open_mode>(0x20)>::value, "");
}

#endif
