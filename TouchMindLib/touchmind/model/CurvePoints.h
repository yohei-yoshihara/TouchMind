#ifndef TOUCHMIND_MODEL_CURVEPOINTS_H_
#define TOUCHMIND_MODEL_CURVEPOINTS_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace model {
    // curve points for a node connection
    class CurvePoints {
    private:
      static const size_t NUMBER_OF_POINTS = 4;
      std::array<FLOAT, NUMBER_OF_POINTS> x;
      std::array<FLOAT, NUMBER_OF_POINTS> y;

    public:
      CurvePoints() {
        for (size_t i = 0; i < NUMBER_OF_POINTS; ++i) {
          x[i] = 0.0f;
          y[i] = 0.0f;
        }
      }
      size_t GetNumerOfPoints() const {
        return NUMBER_OF_POINTS;
      }
      FLOAT GetX(size_t index) const {
        return x[index];
      }
      void SetX(size_t index, FLOAT _x) {
        x[index] = _x;
      }
      FLOAT GetY(size_t index) const {
        return y[index];
      }
      void SetY(size_t index, FLOAT _y) {
        y[index] = _y;
      }
      void GetBounds(D2D1_RECT_F &bounds) {
        bounds.left = std::min(x[0], x[3]);
        bounds.right = std::max(x[0], x[3]);
        bounds.top = std::min(y[0], y[3]);
        bounds.bottom = std::max(y[0], y[3]);
      }
    };
    std::wostream &operator<<(std::wostream &os, const CurvePoints &curvePoints);

  } // model
} // touchmind

#endif // TOUCHMIND_MODEL_CURVEPOINTS_H_