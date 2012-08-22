#ifndef FORWARDDECL_H_
#define FORWARDDECL_H_

// touchmind
namespace touchmind
{
class Configuration;
class D3DStates;
class Context;
class IRenderEventListener;
class Insets;
class MUI;
}

// touchmind::animation
namespace touchmind
{
namespace animation
{
class AnimationManagerEventHandler;
class IAnimationStatusChangedListener;
}
}

// touchmind::control
namespace touchmind
{
namespace control
{
class CompositionAttribute;
class DWriteEditControl;
class DisplayAttributeProperties;
class DWriteEditControlDisplayAttribute;
class IDWLightEditControlChangeListener;
class IDWLightEditControlSelectionChangeListener;
class DWriteEditControlManager;
class DWriteEditControlTextEditSink;
class DWriteEditControlTextStoreACP;
class DWriteTextSizeProvider;
class StatusBar;
}
}

// touchmind::converter
namespace touchmind
{
namespace converter
{
class NodeModelToTextConverter;
class NodeModelXMLDecoder;
class NodeModelXMLEncoder;
}
}

// touchmind::filter
namespace touchmind
{
namespace filter
{
class GaussFilter;
}
}

// touchmind::layout
namespace touchmind
{
namespace layout
{
class ILayoutManager;
class ITextSizeProvider;
class LayoutWorkData;
class LayoutManager;
class TreeRect;
class TreeRectVisitor;
}
}

// touchmind::model
namespace touchmind
{
namespace model
{
class BaseModel;
class CurvePoints;
class IMapModelChangeListener;
class MapModel;
}
}

// touchmind::model::link
namespace touchmind
{
namespace model
{
namespace link
{
class LinkModel;
}
}
}

// touchmind::model::linkedge
namespace touchmind
{
namespace model
{
namespace linkedge
{
class LinkEdgeModel;
}
}
}

// touchmind::model::node
namespace touchmind
{
namespace model
{
namespace node
{
class NodeModel;
}
}
}

// touchmind::model::node::iterator
namespace touchmind
{
namespace model
{
namespace node
{
namespace iterator
{
class ChildFirstNodeIterator;
class ParentFirstNodeIterator;
}
}
}
}

// touchmind::model::path
namespace touchmind
{
namespace model
{
namespace path
{
class PathModel;
}
}
}

// touchmind::operation
namespace touchmind
{
namespace operation
{
class CreateLinkOperation;
class FileOperation;
class NodeMovingOperation;
class NodeResizingOperation;
}
}

// touchmind::print
namespace touchmind
{
namespace print
{
class PrintTicketHelper;
class XpsDWriteTextRenderer;
class XPSGeometryBuilder;
class XPSPrint;
class XPSPrintSetupDialog;
}
}

// touchmind::prop
namespace touchmind
{
namespace prop
{
class LineEdgeStyle;
class LineStyle;
class LineWidth;
class NodeShape;
}
}

// touchmind::ribbon
namespace touchmind
{
namespace ribbon
{
class PropertySet;
class RibbonCommandHandler;
class RibbonFramework;
}
}

// touchmind::ribbon::dispatch
namespace touchmind
{
namespace ribbon
{
namespace dispatch
{
class ILineProperty;
class INodeProperty;
class RibbonRequestDispatcher;
}
}
}

// touchmind::ribbon::handler
namespace touchmind
{
namespace ribbon
{
namespace handler
{
class FontControlCommandHandler;
class LineColorCommandHandler;
class LineEdgeStyleCommandHandler;
class LineStyleCommandHandler;
class LineWidthCommandHandler;
class NodeBackgroundColorCommandHandler;
class NodeShapeCommandHandler;
}
}
}

// touchmind::selection
namespace touchmind
{
namespace selection
{
class DefaultPropertyModel;
class SelectableSupport;
class SelectionManager;
}
}

// touchmind::shell
namespace touchmind
{
namespace shell
{
class Clipboard;
class FileAssociation;
class MRUManager;
}
}

// touchmind::text
namespace touchmind
{
namespace text
{
class FontAttribute;
class FontAttributeCommand;
}
}

// touchmind::touch
namespace touchmind
{
namespace touch
{
class ManipulationResult;
class AbstractManipulationHelper;
class ManipulationHelper;
class TouchUtil;
}
}

// touchmind::tsf
namespace touchmind
{
namespace tsf
{
class Message;
class MessageQueue;
class TSFManager;
}
}

// touchmind::util
namespace touchmind
{
namespace util
{
class BitmapHelper;
class ColorUtil;
class CoordinateConversion;
class OSVersionChecker;
class ScrollBarHelper;
class TimeUtil;
class LastErrorArgs;
}
}

// touchmind::view
namespace touchmind
{
namespace view
{
class BaseView;
class GeometryBuilder;
}
}

// touchmind::view::link
namespace touchmind
{
namespace view
{
namespace link
{
class BaseLinkView;
class LinkViewFactory;
class LinkViewManager;
}
}
}

// touchmind::view::link::impl
namespace touchmind
{
namespace view
{
namespace link
{
namespace impl
{
class DefaultLinkView;
}
}
}
}

// touchmind::view::linkedge
namespace touchmind
{
namespace view
{
namespace linkedge
{
class BaseLinkEdgeView;
class LinkEdgeViewFactory;
}
}
}

// touchmind::view::linkedge::impl
namespace touchmind
{
namespace view
{
namespace linkedge
{
namespace impl
{
class ArrowLinkEdgeView;
class CircleLinkEdgeView;
class NormalLinkEdgeView;
}
}
}
}

// touchmind::view::node
namespace touchmind
{
namespace view
{
namespace node
{
class BaseNodeView;
class NodeFigureHelper;
class NodeViewFactory;
class NodeViewManager;
}
}
}

// touchmind::view::node::impl
namespace touchmind
{
namespace view
{
namespace node
{
namespace impl
{
class RectangleNodeView;
class RoundedRectNodeView;
}
}
}
}

// touchmind::view::path
namespace touchmind
{
namespace view
{
namespace path
{
class BasePathView;
class PathViewFactory;
}
}
}

// touchmind::view::path::impl
namespace touchmind
{
namespace view
{
namespace path
{
namespace impl
{
class DefaultPathView;
}
}
}
}

// touchmind::visitor
namespace touchmind
{
namespace visitor
{
class NodeDrawVisitor;
class NodeHitTestVisitor;
}
}

// touchmind::win
namespace touchmind
{
namespace win
{
class CanvasPanel;
class TouchMindApp;
}
}

#endif // FORWARDDECL_H_
