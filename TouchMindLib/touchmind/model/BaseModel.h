#ifndef TOUCHMIND_MODEL_BASEMODEL_H_
#define TOUCHMIND_MODEL_BASEMODEL_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"

namespace touchmind
{
namespace model
{

class BaseModel
{
private:
    REPAINT_COUNTER m_repaintRequiredCounter;
    SAVE_COUNTER m_saveRequiredCounter;

public:
    BaseModel() :
        m_repaintRequiredCounter(MODEL_INITIAL_REPAINT_COUNTER),
        m_saveRequiredCounter(MODEL_INITIAL_SAVE_COUNTER)
    {}
    BaseModel(const BaseModel &other) :
        m_repaintRequiredCounter(other.m_repaintRequiredCounter - 1),
        m_saveRequiredCounter(other.m_saveRequiredCounter - 1)
    {}
    virtual ~BaseModel() {}
    // repaint required
    REPAINT_COUNTER IncrementRepaintRequiredCounter() {
        return ++m_repaintRequiredCounter;
    }
    REPAINT_COUNTER GetRepaintRequiredCounter() const {
        return m_repaintRequiredCounter;
    }
    bool IsRepaintRequired(REPAINT_COUNTER repaintCounter) const {
        return repaintCounter != m_repaintRequiredCounter;
    }
    // save required
    SAVE_COUNTER IncrementSaveRequiredCounter() {
        return ++m_saveRequiredCounter;
    }
    SAVE_COUNTER GetSaveRequiredCounter() const {
        return m_saveRequiredCounter;
    }
    bool IsSaveRequired(SAVE_COUNTER saveCounter) const {
        return saveCounter != m_saveRequiredCounter;
    }
};

} // model
} // touchmind

#endif // TOUCHMIND_MODEL_BASEMODEL_H_