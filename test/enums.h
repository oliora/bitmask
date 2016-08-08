#pragma once

#include <boost/bitmask.hpp>


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

    BOOST_BITMASK(syntax_option_type)

    enum class open_mode
    {
        app     = 0x01,
        binary  = 0x02,
        ate     = 0x40,

        _bitmask_value_mask = 0x43
    };

    BOOST_BITMASK(open_mode)
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

    BOOST_BITMASK_MAX_ELEMENT(syntax_option_type, awk)

    enum class open_mode
    {
        app     = 0x01,
        binary  = 0x02,
        ate     = 0x40,
    };

    BOOST_BITMASK_VALUE_MASK(open_mode, 0x43)
}
