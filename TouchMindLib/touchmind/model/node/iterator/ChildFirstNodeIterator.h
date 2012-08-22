#ifndef TOUCHMIND_MODEL_NODE_CHILDFIRSTNODEITERATOR_H_
#define TOUCHMIND_MODEL_NODE_CHILDFIRSTNODEITERATOR_H_

#include "touchmind/model/node/NodeModel.h"

namespace touchmind
{
namespace model
{
namespace node
{
namespace iterator
{

class ChildFirstNodeIterator :
    public std::iterator<std::forward_iterator_tag, std::shared_ptr<touchmind::model::node::NodeModel>, void>
{
private:
    std::shared_ptr<touchmind::model::node::NodeModel> m_root;
    std::stack<std::shared_ptr<touchmind::model::node::NodeModel>> m_stack;

public:
    typedef NodeModel::value_type value_type;
    typedef NodeModel::difference_type difference_type;
    typedef NodeModel::reference reference;

    ChildFirstNodeIterator(
        value_type root,
        bool endIterator) :
        m_root(root),
        m_stack() {
        if (!endIterator) {
            m_stack.push(root);
            while (true) {
                auto child = m_stack.top()->GetFirstChild();
                if (child != nullptr) {
                    m_stack.push(child);
                } else {
                    break;
                }
            }
        }
    }
    ChildFirstNodeIterator(const ChildFirstNodeIterator& other) :
        m_root(other.m_root),
        m_stack(other.m_stack)
    {}
    ChildFirstNodeIterator& operator=(const ChildFirstNodeIterator& other) {
        if (this != &other) {
            m_root = other.m_root;
            m_stack = other.m_stack;
        }
        return *this;
    }
    ~ChildFirstNodeIterator() {}
    reference operator*() {
        if (m_stack.size() > 0) {
            return m_stack.top();
        } else {
            return m_root;
        }
    }
    reference operator->() {
        return operator*();
    }
    ChildFirstNodeIterator& operator++() {
        if (!m_stack.empty()) {
            auto next = m_stack.top()->GetRightSibling();
            if (next != nullptr) {
                m_stack.pop();
                m_stack.push(next);
                while (true) {
                    auto child = m_stack.top()->GetFirstChild();
                    if (child != nullptr) {
                        m_stack.push(child);
                    } else {
                        break;
                    }
                }
            } else {
                m_stack.pop();
            }
        }
        return *this;
    }
    ChildFirstNodeIterator operator++(int) {
        ChildFirstNodeIterator copy_it(*this);
        operator++();
        return copy_it;
    }
    bool operator==(const ChildFirstNodeIterator &other) const {
        return m_root == other.m_root &&
               m_stack.size() == other.m_stack.size() &&
               ((m_stack.empty() && other.m_stack.empty()) || (m_stack.top() == other.m_stack.top()));
    }
    bool operator!=(const ChildFirstNodeIterator &other) const {
        return !(*this == other);
    }
};

} // iterator
} // node
} // model
} // touchmind

#endif // TOUCHMIND_MODEL_NODE_CHILDFIRSTNODEITERATOR_H_