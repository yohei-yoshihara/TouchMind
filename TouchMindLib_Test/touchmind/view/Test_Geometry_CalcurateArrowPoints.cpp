#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/view/GeometryBuilder.h"

using namespace touchmind;
using namespace touchmind::view;

TEST(touchmind_view_GeometryBuilder_CalculateArrowPoints, test000) {
  std::vector<D2D1_POINT_2F> points;
  GeometryBuilder::CalculateArrowPoints(D2D1::Point2F(), 2.0f, 4.0f, ToRadian(0.0f), NODE_SIDE_RIGHT, points);

  ASSERT_FLOAT_EQ(4.0f, points[1].x);
  ASSERT_FLOAT_EQ(-1.0f, points[1].y);
  ASSERT_FLOAT_EQ(4.0f, points[2].x);
  ASSERT_FLOAT_EQ(1.0f, points[2].y);
}

TEST(touchmind_view_GeometryBuilder_CalculateArrowPoints, test001) {
  std::vector<D2D1_POINT_2F> points;
  GeometryBuilder::CalculateArrowPoints(D2D1::Point2F(), 2.0f, 4.0f, ToRadian(90.0f), NODE_SIDE_RIGHT, points);

  ASSERT_FLOAT_EQ(1.0f, points[1].x);
  ASSERT_FLOAT_EQ(4.0f, points[1].y);
  ASSERT_FLOAT_EQ(-1.0f, points[2].x);
  ASSERT_FLOAT_EQ(4.0f, points[2].y);
}

TEST(touchmind_view_GeometryBuilder_CalculateArrowPoints, test002) {
  std::vector<D2D1_POINT_2F> points;
  GeometryBuilder::CalculateArrowPoints(D2D1::Point2F(), 2.0f, 4.0f, ToRadian(-90.0f), NODE_SIDE_RIGHT, points);

  ASSERT_FLOAT_EQ(-1.0f, points[1].x);
  ASSERT_FLOAT_EQ(-4.0f, points[1].y);
  ASSERT_FLOAT_EQ(1.0f, points[2].x);
  ASSERT_FLOAT_EQ(-4.0f, points[2].y);
}

TEST(touchmind_view_GeometryBuilder_CalculateArrowPoints, test003) {
  std::vector<D2D1_POINT_2F> points;
  GeometryBuilder::CalculateArrowPoints(D2D1::Point2F(), 2.0f, 4.0f, ToRadian(0.0f), NODE_SIDE_LEFT, points);

  ASSERT_FLOAT_EQ(-4.0f, points[1].x);
  ASSERT_FLOAT_EQ(1.0f, points[1].y);
  ASSERT_FLOAT_EQ(-4.0f, points[2].x);
  ASSERT_FLOAT_EQ(-1.0f, points[2].y);
}

TEST(touchmind_view_GeometryBuilder_CalculateArrowPoints, test004) {
  std::vector<D2D1_POINT_2F> points;
  GeometryBuilder::CalculateArrowPoints(D2D1::Point2F(), 2.0f, 4.0f, ToRadian(90.0f), NODE_SIDE_LEFT, points);

  ASSERT_FLOAT_EQ(1.0f, points[1].x);
  ASSERT_FLOAT_EQ(4.0f, points[1].y);
  ASSERT_FLOAT_EQ(-1.0f, points[2].x);
  ASSERT_FLOAT_EQ(4.0f, points[2].y);
}

TEST(touchmind_view_GeometryBuilder_CalculateArrowPoints, test005) {
  std::vector<D2D1_POINT_2F> points;
  GeometryBuilder::CalculateArrowPoints(D2D1::Point2F(), 2.0f, 4.0f, ToRadian(-90.0f), NODE_SIDE_LEFT, points);

  ASSERT_FLOAT_EQ(-1.0f, points[1].x);
  ASSERT_FLOAT_EQ(-4.0f, points[1].y);
  ASSERT_FLOAT_EQ(1.0f, points[2].x);
  ASSERT_FLOAT_EQ(-4.0f, points[2].y);
}
