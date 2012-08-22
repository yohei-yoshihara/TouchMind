#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
/*
using namespace touchmind;
using namespace touchmind::model;
using namespace touchmind::model::node;
using namespace touchmind::model::link;
using namespace touchmind::model::linkedge;

// CalculateAngleAndLength

TEST( touchmind_model_LinkEdgeModel, CalculateAngleAndLength_001 )
{
    LinkEdgeModel edge;
    edge.CalculateAngleAndLength(D2D1::Point2F(10.0f, 10.0f), D2D1::Point2F(20.0f, 10.0f));
    ASSERT_FLOAT_EQ(0.0f, edge.GetAngle());
    ASSERT_FLOAT_EQ(10.0f, edge.GetLength());
}

TEST( touchmind_model_LinkEdgeModel, CalculateAngleAndLength_002 )
{
    LinkEdgeModel edge;
    edge.CalculateAngleAndLength(D2D1::Point2F(10.0f, 10.0f), D2D1::Point2F(5.0f, 10.0f));
    ASSERT_FLOAT_EQ(0.0f, edge.GetAngle());
    ASSERT_FLOAT_EQ(5.0f, edge.GetLength());
}

TEST( touchmind_model_LinkEdgeModel, CalculateAngleAndLength_003 )
{
    LinkEdgeModel edge;
    edge.CalculateAngleAndLength(D2D1::Point2F(10.0f, 10.0f), D2D1::Point2F(10.0f, 0.0f));
    ASSERT_FLOAT_EQ(touchmind::PI_2, edge.GetAngle());
    ASSERT_FLOAT_EQ(10.0f, edge.GetLength());
}

TEST( touchmind_model_LinkEdgeModel, CalculateAngleAndLength_004 )
{
    LinkEdgeModel edge;
    edge.CalculateAngleAndLength(D2D1::Point2F(10.0f, 10.0f), D2D1::Point2F(10.0f, 17.0f));
    ASSERT_FLOAT_EQ(-touchmind::PI_2, edge.GetAngle());
    ASSERT_FLOAT_EQ(7.0f, edge.GetLength());
}

TEST( touchmind_model_LinkEdgeModel, CalculateAngleAndLength_005 )
{
    LinkEdgeModel edge;
    edge.CalculateAngleAndLength(D2D1::Point2F(10.0f, 10.0f), D2D1::Point2F(20.0f, 0.0f));
    ASSERT_FLOAT_EQ(touchmind::PI_4, edge.GetAngle());
    ASSERT_FLOAT_EQ(10.0f * touchmind::SQRT_2, edge.GetLength());
}

TEST( touchmind_model_LinkEdgeModel, CalculateAngleAndLength_006 )
{
    LinkEdgeModel edge;
    edge.CalculateAngleAndLength(D2D1::Point2F(10.0f, 10.0f), D2D1::Point2F(0.0f, 0.0f));
    ASSERT_FLOAT_EQ(touchmind::PI_4, edge.GetAngle());
    ASSERT_FLOAT_EQ(10.0f * touchmind::SQRT_2, edge.GetLength());
}

TEST( touchmind_model_LinkEdgeModel, CalculateAngleAndLength_007 )
{
    LinkEdgeModel edge;
    edge.CalculateAngleAndLength(D2D1::Point2F(10.0f, 10.0f), D2D1::Point2F(20.0f, 20.0f));
    ASSERT_FLOAT_EQ(-touchmind::PI_4, edge.GetAngle());
    ASSERT_FLOAT_EQ(10.0f * touchmind::SQRT_2, edge.GetLength());
}

TEST( touchmind_model_LinkEdgeModel, CalculateAngleAndLength_008 )
{
    LinkEdgeModel edge;
    edge.CalculateAngleAndLength(D2D1::Point2F(10.0f, 10.0f), D2D1::Point2F(0.0f, 20.0f));
    ASSERT_FLOAT_EQ(-touchmind::PI_4, edge.GetAngle());
    ASSERT_FLOAT_EQ(10.0f * touchmind::SQRT_2, edge.GetLength());
}

// CalculateHandlePosition

TEST( touchmind_model_LinkEdgeModel, CalculateHandlePosition_001 )
{
    LinkEdgeModel edge;
    edge.SetAngle(0.0f);
    edge.SetLength(10.0f);
    D2D1_POINT_2F p = edge.CalculateHandlePosition(D2D1::Point2F(10.0f, 10.0f), NODE_SIDE_RIGHT);
    ASSERT_FLOAT_EQ(20.0f, p.x);
    ASSERT_FLOAT_EQ(10.0f, p.y);
}

TEST( touchmind_model_LinkEdgeModel, CalculateHandlePosition_002 )
{
    LinkEdgeModel edge;
    edge.SetAngle(0.0f);
    edge.SetLength(5.0f);
    D2D1_POINT_2F p = edge.CalculateHandlePosition(D2D1::Point2F(10.0f, 10.0f), NODE_SIDE_LEFT);
    ASSERT_FLOAT_EQ(5.0f, p.x);
    ASSERT_FLOAT_EQ(10.0f, p.y);
}

TEST( touchmind_model_LinkEdgeModel, CalculateHandlePosition_003 )
{
    LinkEdgeModel edge;
    edge.SetAngle(touchmind::PI_2);
    edge.SetLength(10.0f);
    D2D1_POINT_2F p = edge.CalculateHandlePosition(D2D1::Point2F(10.0f, 10.0f), NODE_SIDE_LEFT);
    ASSERT_FLOAT_EQ(10.0f, p.x);
    ASSERT_FLOAT_EQ(0.0f, p.y);
}

TEST( touchmind_model_LinkEdgeModel, CalculateHandlePosition_004 )
{
    LinkEdgeModel edge;
    edge.SetAngle(-touchmind::PI_2);
    edge.SetLength(7.0f);
    D2D1_POINT_2F p = edge.CalculateHandlePosition(D2D1::Point2F(10.0f, 10.0f), NODE_SIDE_RIGHT);
    ASSERT_FLOAT_EQ(10.0f, p.x);
    ASSERT_FLOAT_EQ(17.0f, p.y);
}

TEST( touchmind_model_LinkEdgeModel, CalculateHandlePosition_005 )
{
    LinkEdgeModel edge;
    edge.SetAngle(touchmind::PI_4);
    edge.SetLength(10.0f * touchmind::SQRT_2);
    D2D1_POINT_2F p = edge.CalculateHandlePosition(D2D1::Point2F(10.0f, 10.0f), NODE_SIDE_RIGHT);
    ASSERT_FLOAT_EQ(20.0f, p.x);
    ASSERT_FLOAT_EQ(0.0f, p.y);
}

TEST( touchmind_model_LinkEdgeModel, CalculateHandlePosition_006 )
{
    LinkEdgeModel edge;
    edge.SetAngle(touchmind::PI_4);
    edge.SetLength(10.0f * touchmind::SQRT_2);
    D2D1_POINT_2F p = edge.CalculateHandlePosition(D2D1::Point2F(10.0f, 10.0f), NODE_SIDE_LEFT);
    ASSERT_FLOAT_EQ(0.0f, p.x);
    ASSERT_FLOAT_EQ(0.0f, p.y);
}

TEST( touchmind_model_LinkEdgeModel, CalculateHandlePosition_007 )
{
    LinkEdgeModel edge;
    edge.SetAngle(-touchmind::PI_4);
    edge.SetLength(10.0f * touchmind::SQRT_2);
    D2D1_POINT_2F p = edge.CalculateHandlePosition(D2D1::Point2F(10.0f, 10.0f), NODE_SIDE_RIGHT);
    ASSERT_FLOAT_EQ(20.0f, p.x);
    ASSERT_FLOAT_EQ(20.0f, p.y);
}

TEST( touchmind_model_LinkEdgeModel, CalculateHandlePosition_008 )
{
    LinkEdgeModel edge;
    edge.SetAngle(-touchmind::PI_4);
    edge.SetLength(10.0f * touchmind::SQRT_2);
    D2D1_POINT_2F p = edge.CalculateHandlePosition(D2D1::Point2F(10.0f, 10.0f), NODE_SIDE_LEFT);
    ASSERT_FLOAT_EQ(0.0f, p.x);
    ASSERT_FLOAT_EQ(20.0f, p.y);
}

*/