#ifndef TOUCHMIND_TSF_MESSAGEQUEUE_H_
#define TOUCHMIND_TSF_MESSAGEQUEUE_H_

namespace touchmind {
  namespace tsf {

    class Message {
    public:
      Message()
          : hWnd(0)
          , message(0)
          , wParam(0)
          , lParam(0){};
      Message(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
          : hWnd(_hWnd)
          , message(_message)
          , wParam(_wParam)
          , lParam(_lParam) {
      }
      HWND hWnd;
      UINT message;
      WPARAM wParam;
      LPARAM lParam;
    };

    class MessageQueue {
    private:
      std::list<std::shared_ptr<Message>> m_messageQueue;

    public:
      MessageQueue(void);
      ~MessageQueue(void);
      void Push(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
      void Pop(Message *pMessage);
      bool IsEmpty(void);
    };

  } // tsf
} // touchmind

#endif // TOUCHMIND_TSF_MESSAGEQUEUE_H_