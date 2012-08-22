#ifndef TOUCHMIND_OPERATION_IOPERATION_H_
#define TOUCHMIND_OPERATION_IOPERATION_H_

namespace touchmind
{
namespace operation
{
class IOperation
{
public:
    virtual void Destroy() = 0;
};
}
}

#endif // TOUCHMIND_OPERATION_IOPERATION_H_