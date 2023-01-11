
#ifndef COLOR_HPP
#define COLOR_HPP


class Color {
    public:
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        Color(uint8_t r, uint8_t b, uint8_t g, uint8_t a) : r(r), g(g), b(b), a(a) {};
        Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}; 

        Color() : r(0), g(0), b(0), a(0) {};
};

const Color WHITE = Color(1, 1, 1, 1);
const Color BLACK = Color(0, 0, 0, 1);

#endif