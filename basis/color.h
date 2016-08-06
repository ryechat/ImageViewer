#pragma once
#ifndef GUID_7D79D07FF2F14AA7BF2BDD0A976FCC45
#define GUID_7D79D07FF2F14AA7BF2BDD0A976FCC45
/*!
    @file CColor class definition.
*/
#include "types.h"

#ifndef STDAFX_H
#include <algorithm>
#endif

#include "iunit_test.h"
UNIT_TEST(CColor)



/*!    Color expressed by A-RGB or A-HSV.
    Alpha express (1 - transparency).
*/
class CColor {
    double m_r, m_g, m_b, m_a;
public:
    CColor() : CColor(0, 0, 0) {}
    CColor(double r, double g, double b, double a = 255.0)
        : m_r(r), m_g(g), m_b(b), m_a(a) {}
    CColor(int r, int g, int b, int a = 255)
        : m_r(static_cast<double>(r)),
        m_g(static_cast<double>(g)),
        m_b(static_cast<double>(b)),
        m_a(static_cast<double>(a)) {}

    operator COLORREF() { return RGB(static_cast<DWORD>(m_r), static_cast<DWORD>(m_g), static_cast<DWORD>(m_b));
    }

    //! Red val.
    double red() { return m_r; }

    //! Green val.
    double green() { return m_g; }

    //! Blue val.
    double blue() { return m_b; }

    //! Red val.
    unsigned char r() {
        return static_cast<unsigned char>(m_r);
    }

    //! Green val.
    unsigned char g() {
        return static_cast<unsigned char>(m_g);
    }

    //! Blue val.
    unsigned char b() {
        return static_cast<unsigned char>(m_b);
    }

    //! Multiply each RGB members.
    CColor operator*(double d) {
        return { d * m_r, d * m_g, d * m_b , m_a };
    }

    /*! Get the color if rhs is underneath. 
    */
    CColor underLay(CColor rhs) {
        if (m_a > 255 - 1)
            return *this;

        CColor temp{ *this * (m_a / 255) };
        rhs = rhs * ((255 - m_a) / 255);
        temp.m_r += rhs.m_r;
        temp.m_g += rhs.m_g;
        temp.m_b += rhs.m_b;
        temp.m_a = m_a + (255 - m_a) * (rhs.m_a / 255);
        return temp;
    }

    //! Get hue, the direction of color. êFëä
    double hue()    // [0, 360)
    {
        auto mm = std::minmax({ m_r, m_g, m_b });
        auto &min = mm.first;
        auto &max = mm.second;

        double hue{ 60.0 };

        if (m_r == m_g && m_g == m_b) {
            return .0;
        }
        if (m_r > m_b && m_r > m_g) {
            hue *= (m_g - m_b) / (max - min);
        }
        else if (m_g > m_b) {
            hue *= (m_b - m_r) / (max - min);
            hue += 120;
        }
        else {
            hue *= (m_r - m_g) / (max - min);
            hue += 240;
        }
        if (hue < 0)
            hue += 360;
        if (hue >= 360)
            hue -= 360;
        return hue;
    }

    //! Get saturation, the color freshness. ç ìx
    double saturation() {
        auto mm{ std::minmax({m_r, m_g, m_b}) };
        return 255.0 * (mm.second - mm.first) / mm.second;
    }

    //! Get value, means briteness. ñæìx
    double value() {
        return (std::max)({ m_r, m_g, m_b });
    }

    //! Get hue int.
    unsigned int h() { static_cast<unsigned int>(hue()); }

    //! Get saturation in u-char.
    unsigned char s() { static_cast<unsigned char>(saturation()); }

    //! Get value, briteness in u-char.
    unsigned char v() { static_cast<unsigned char>(value()); }

    //! Create instance specified by HSV values.
    static CColor fromHSV(double h, double s, double v) {
        double min = v - ((s / 255) * v);    // v = max
        if (h < 60) {
            return{ v, h / 60 * (v - min) + min, min };
        }
        if (h < 120) {
            return{ (120 - h) / 60 * (v - min) + min, v, min };
        }
        if (h < 180) {
            return{ min, v, (h - 120) / 60 * (v - min) + min };
        }
        if (h < 240) {
            return{ min, (240 - h) / 60 * (v - min) + min, v };
        }
        if (h < 300) {
            return{ (h - 240) / 60 * (v - min) + min, min, v };
        }
        return{ v, min, (360 - h) / 60 * (v - min) + min };
    }
};



UNIT_TEST_FUNC(CColor) {

    double max_diff = 0.0;

    for (int i = 0; i < 255; i++) {
        unsigned char r, g, b;
        r = rand() % 255;
        g = rand() % 255;
        b = rand() % 255;

        CColor rgb;
        rgb = { r, g, b };

        double h, s, v;
        h = rgb.hue();
        s = rgb.saturation();
        v = rgb.value();

        CColor hsv;
        hsv = CColor::fromHSV(h, s, v);

        double diff;
        diff = rgb.red() + rgb.green() + rgb.blue();
        diff = abs(diff - hsv.red() - hsv.green() - hsv.blue());
        if (diff > max_diff)
            max_diff = diff;
        if (diff > 3)
            throw 0;
    }
    return true;
}


#endif