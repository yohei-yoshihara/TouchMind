#ifndef TOUCHMIND_INSETS_H_
#define TOUCHMIND_INSETS_H_

#include "forwarddecl.h"

namespace touchmind
{

class Insets
{
public:
    FLOAT left;
    FLOAT top;
    FLOAT right;
    FLOAT bottom;
    Insets(FLOAT _left, FLOAT _top, FLOAT _right, FLOAT _bottom) :
        left(_left),
        top(_top),
        right(_right),
        bottom(_bottom) {}
};

inline std::wostream& operator<< (std::wostream& strm, Insets const& val)
{
    strm << L"Insets[left=" << val.left << L",top=" << val.top << L",right=" << val.right << L",bottom=" << val.bottom << L"]";
    return strm;
}

} // touchmind

#endif // TOUCHMIND_INSETS_H_