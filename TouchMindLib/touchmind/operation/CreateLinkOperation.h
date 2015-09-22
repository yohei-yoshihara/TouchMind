#ifndef TOUCHMIND_WIN_OPERATION_CREATELINKOPERATION_H_
#define TOUCHMIND_WIN_OPERATION_CREATELINKOPERATION_H_

#include "forwarddecl.h"
#include "touchmind/operation/IOperation.h"

namespace touchmind {
  namespace operation {

    class CreateLinkOperation : public IOperation {
    private:
      static const FLOAT MESSAGE_BOX_WIDTH;
      static const FLOAT MESSAGE_BOX_HEIGHT;
      touchmind::win::CanvasPanel *m_pCanvasPanel;
      CComPtr<ID2D1SolidColorBrush> m_foregroundBrush;

      // start node message
      std::wstring m_startNodeMessasgeText;
      CComPtr<IDWriteTextFormat> m_startNodeMessageTextFormat;
      CComPtr<IDWriteTextLayout> m_startNodeMessageTextLayout;
      CComPtr<ID2D1SolidColorBrush> m_startNodeBodyBrush;

      // end node message
      std::wstring m_endNodeMessasgeText;
      CComPtr<IDWriteTextFormat> m_endNodeMessageTextFormat;
      CComPtr<IDWriteTextLayout> m_endNodeMessageTextLayout;
      CComPtr<ID2D1SolidColorBrush> m_endNodeBodyBrush;

      // start node number
      std::wstring m_startNodeNumberText;
      CComPtr<ID2D1SolidColorBrush> m_startNodeNumberMarkBrush;
      CComPtr<IDWriteTextFormat> m_startNodeNumberTextFormat;
      CComPtr<IDWriteTextLayout> m_startNodeNumberTextLayout;

      // message text animation
      CComPtr<IUIAnimationStoryboard> m_messageTextStoryboard;
      CComPtr<IUIAnimationVariable> m_messageTextVar;
      CComPtr<IUIAnimationTransition> m_messageTextTransition1;
      CComPtr<IUIAnimationTransition> m_messageTextTransition2;

      // start node animation
      CComPtr<IUIAnimationStoryboard> m_startNodeStoryboard;
      CComPtr<IUIAnimationVariable> m_startNodeRadius;
      CComPtr<IUIAnimationTransition> m_startNodeTransition;
      D2D1_POINT_2F m_startNodeCenterPoint;

      // end node animation
      CComPtr<IUIAnimationStoryboard> m_endNodeStoryboard;
      CComPtr<IUIAnimationVariable> m_endNodeRadius;
      CComPtr<IUIAnimationTransition> m_endNodeTransition;
      D2D1_POINT_2F m_endNodeCenterPoint;

      std::shared_ptr<touchmind::model::node::NodeModel> m_startNode;
      std::shared_ptr<touchmind::model::node::NodeModel> m_endNode;

    protected:
      void _CreateLink();
      void _StartStartNodeAnimation(touchmind::Context *pContext);
      void _StopStartNodeAnimation();
      void _StartEndNodeAnimation(touchmind::Context *pContext);
      void _StopEndNodeAnimation();

    public:
      CreateLinkOperation();
      virtual ~CreateLinkOperation();
      void SetCanvasPanel(touchmind::win::CanvasPanel *pCanvasPanel) {
        m_pCanvasPanel = pCanvasPanel;
      }
      void SetSelectedNode(touchmind::Context *pContext,
                           const std::shared_ptr<touchmind::model::node::NodeModel> &node);
      void CreateDeviceDependentResources(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget);
      void StartSelectFirstNode(touchmind::Context *pContext);
      void Draw(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget);
      void EndOperation();
      void DiscardDeviceDependentResources();
      virtual void Destroy() override;
    };

  } // operation
} // touchmind

#endif // TOUCHMIND_WIN_OPERATION_CREATELINKOPERATION_H_