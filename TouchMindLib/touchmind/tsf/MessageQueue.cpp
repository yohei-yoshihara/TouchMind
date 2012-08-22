#include "StdAfx.h"
#include "MessageQueue.h"

touchmind::tsf::MessageQueue::MessageQueue(void)
{
}

touchmind::tsf::MessageQueue::~MessageQueue(void)
{
}

void touchmind::tsf::MessageQueue::Push(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::shared_ptr<Message> msg(new Message(hWnd, message, wParam, lParam));
    m_messageQueue.push_front(msg);
}

void touchmind::tsf::MessageQueue::Pop(Message* pMessage)
{
    std::shared_ptr<tsf::Message> msg = m_messageQueue.back();
    m_messageQueue.pop_back();
    pMessage->message = msg->message;
    pMessage->wParam = msg->wParam;
    pMessage->lParam = msg->lParam;
}

bool touchmind::tsf::MessageQueue::IsEmpty(void)
{
    return m_messageQueue.empty();
}

