#ifndef TOUCHMIND_PROP_NODESHAPE_H_
#define TOUCHMIND_PROP_NODESHAPE_H_

namespace touchmind
{
#define NUMBER_OF_NODE_SHAPES 2
enum NODE_SHAPE {
    NODE_SHAPE_ROUNDED_RECTANGLE = 0,
    NODE_SHAPE_RECTANGLE = 1,
    NODE_SHAPE_UNSPECIFIED = -1,
};
std::wostream& operator<< (std::wostream& os, const NODE_SHAPE &nodeShape);

namespace prop
{

class NodeShape
{
private:
    NodeShape() {}
public:
    static NODE_SHAPE GetDefaultNodeShape() {
        return NODE_SHAPE_ROUNDED_RECTANGLE;
    }
    static const std::wstring ToString(NODE_SHAPE nodeShape) {
        switch (nodeShape) {
        case NODE_SHAPE_RECTANGLE:
            return std::wstring(L"RECTANGLE");
        }
        return std::wstring(L"ROUNDED_RECTANGLE");
    }
    static UINT32 ToIndex(NODE_SHAPE nodeShape) {
        switch (nodeShape) {
        case NODE_SHAPE_ROUNDED_RECTANGLE:
            return 0;
        case NODE_SHAPE_RECTANGLE:
            return 1;
        }
        return 0;
    }
    static NODE_SHAPE ToNodeShape(const std::wstring &s) {
        std::wstring _s(s);
        std::transform(_s.begin (), _s.end (), _s.begin (), std::toupper);
        if (s == L"RECTANGLE") {
            return NODE_SHAPE_RECTANGLE;
        }
        return NODE_SHAPE_ROUNDED_RECTANGLE;
    }
    static NODE_SHAPE IndexToNodeShape(UINT32 index) {
        const static NODE_SHAPE NODE_SHAPE_LIST[] = {NODE_SHAPE_ROUNDED_RECTANGLE, NODE_SHAPE_RECTANGLE};
        if (0 <= index && index < NUMBER_OF_NODE_SHAPES) {
            return NODE_SHAPE_LIST[index];
        }
        return NODE_SHAPE_ROUNDED_RECTANGLE;
    }
    static size_t GetNumberOfNodeShapes() {
        return NUMBER_OF_NODE_SHAPES;
    }
};

} // prop
} // touchmind

#endif // TOUCHMIND_PROP_NODESHAPE_H_