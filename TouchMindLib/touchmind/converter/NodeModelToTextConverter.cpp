#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/converter/NodeModelToTextConverter.h"

touchmind::converter::NodeModelToTextConverter::NodeModelToTextConverter(void)
{
}

touchmind::converter::NodeModelToTextConverter::~NodeModelToTextConverter(void)
{
}

std::shared_ptr<touchmind::model::node::NodeModel> touchmind::converter::NodeModelToTextConverter::FromText( const std::wstring &text )
{
    std::shared_ptr<touchmind::model::node::NodeModel> root;

    std::vector<std::shared_ptr<touchmind::model::node::NodeModel>> parents;
    std::vector<std::wstring::size_type> parentsNumberOfSpaces;
    std::wistringstream stream(text);
    std::wstring line;
    while (getline(stream, line)) {
        std::wstring::size_type numberOfSpaces = touchmind::CountLeftSpaces(line);
        std::shared_ptr<touchmind::model::node::NodeModel> node = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
        std::wstring trimedLine;
        touchmind::Trim(line, trimedLine);
        node->SetText(trimedLine);
        if (parentsNumberOfSpaces.empty()) {
            parents.push_back(node);
            parentsNumberOfSpaces.push_back(numberOfSpaces);
        } else {
            if (numberOfSpaces <= parentsNumberOfSpaces[0]) {
                // if there are more than equal to 2 root nodes, stop to parse.
                break;
            }
            if (parentsNumberOfSpaces[parentsNumberOfSpaces.size() - 1] < numberOfSpaces) {
                parents[parents.size() - 1]->AddChild(node);
                parents.push_back(node);
                parentsNumberOfSpaces.push_back(numberOfSpaces);
            } else {
                do {
                    parents.pop_back();
                    parentsNumberOfSpaces.pop_back();
                } while (parentsNumberOfSpaces[parentsNumberOfSpaces.size() - 1] >= numberOfSpaces);
                parents[parents.size() - 1]->AddChild(node);
                parents.push_back(node);
                parentsNumberOfSpaces.push_back(numberOfSpaces);
            }
        }
    }

    if (parents.size() >= 1) {
        return parents[0];
    } else {
        return nullptr;
    }
}

class ConvertToTextTreeVisitor
{
private:
    std::wostringstream& m_stream;
public:
    ConvertToTextTreeVisitor(std::wostringstream& stream) :
        m_stream(stream) {}
    touchmind::VISITOR_RESULT operator()(std::shared_ptr<touchmind::model::node::NodeModel> node) {
        int level = node->GetLevel();
        for (int i = 0; i < level; ++i) {
            m_stream << L"    ";
        }
        m_stream << node->GetText() << std::endl;
        return touchmind::VISITOR_RESULT_CONTINUE;
    }
};

void touchmind::converter::NodeModelToTextConverter::ToText(
    const std::shared_ptr<touchmind::model::node::NodeModel> &node,
    std::wstring &text)
{
    std::wostringstream stream;
    ConvertToTextTreeVisitor visitor(stream);
    node->ApplyVisitor(visitor, true);
    text = stream.str();
}
