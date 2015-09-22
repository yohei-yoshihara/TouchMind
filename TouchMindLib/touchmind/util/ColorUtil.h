#ifndef TOUCHMIND_UTIL_COLORUTIL_H_
#define TOUCHMIND_UTIL_COLORUTIL_H_

namespace touchmind {
  namespace util {

    class ColorUtil {
    private:
      ColorUtil(void) {
      }
      ~ColorUtil(void) {
      }

    public:
      static bool Equal(const D2D1_COLOR_F &color1, const D2D1_COLOR_F &color2) {
        return color1.r == color2.r && color1.g == color2.g && color1.b == color2.b && color1.a == color2.a;
      }
      static void Copy(const D2D1_COLOR_F &src, D2D1_COLOR_F &dest) {
        dest.r = src.r;
        dest.g = src.g;
        dest.b = src.b;
        dest.a = src.a;
      }
      static D2D1_COLOR_F ToColorF(const COLORREF &colorref) {
        return D2D1::ColorF(GetRValue(colorref) / 255.0f, GetGValue(colorref) / 255.0f, GetBValue(colorref) / 255.0f,
                            1.0f);
      }
      static COLORREF ToColorref(const D2D1_COLOR_F &colorF) {
        return RGB(colorF.r * 255, colorF.g * 255, colorF.b * 255);
      }
      static void RgbToHsv(IN FLOAT r, IN FLOAT g, IN FLOAT b, OUT FLOAT &h, OUT FLOAT &s, OUT FLOAT &v) {
        FLOAT _max = v = std::max(std::max(r, g), b);
        FLOAT _min = std::min(std::min(r, g), b);
        s = _max != 0.0f ? (_max - _min) / _max : 0.0f; // if _max == 0.0f, s is undefined.
        if (_max == r) {
          h = (_max - _min != 0.0f) ? 60.0f * (g - b) / (_max - _min) : 0.0f; // if _max == _min, h is undefined.
        } else if (_max == g) {
          h = (_max - _min != 0.0f) ? 60.0f * (b - r) / (_max - _min) + 120.0f : 0.0f;
        } else {
          h = (_max - _min != 0.0f) ? 60.0f * (r - g) / (_max - _min) + 240.0f : 0.0f;
        }
        if (h < 0.0f) {
          h += 360.0f;
        }
      }
      static void HsvToRgb(IN FLOAT h, IN FLOAT s, IN FLOAT v, OUT FLOAT &r, OUT FLOAT &g, OUT FLOAT &b) {
        int hi = static_cast<int>(h / 60.0f) % 6;
        FLOAT f = (h / 60.0f) - hi;
        FLOAT p = v * (1.0f - s);
        FLOAT q = v * (1.0f - f * s);
        FLOAT t = v * (1.0f - (1.0f - f) * s);
        switch (hi) {
        case 0:
          r = v;
          g = t;
          b = p;
          break;
        case 1:
          r = q;
          g = v;
          b = p;
          break;
        case 2:
          r = p;
          g = v;
          b = t;
          break;
        case 3:
          r = p;
          g = q;
          b = v;
          break;
        case 4:
          r = t;
          g = p;
          b = v;
          break;
        case 5:
          r = v;
          g = p;
          b = q;
          break;
        }
      }
      static D2D1_COLOR_F HsvToRgb(FLOAT h, FLOAT s, FLOAT v, FLOAT a) {
        D2D1_COLOR_F c;
        HsvToRgb(h, s, v, c.r, c.g, c.b);
        c.a = a;
        return c;
      }
      static FLOAT Saturate(FLOAT value, FLOAT limit) {
        return value > limit ? limit : value;
      }
    };
  }
}

std::wostream &operator<<(std::wostream &os, const D2D1_COLOR_F &color);

#endif // TOUCHMIND_UTIL_COLORUTIL_H_
