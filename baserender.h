#ifndef BASERENDER_H
#define BASERENDER_H

#include <vector>

namespace my {
    struct Color {
      unsigned char r, g, b, a;
    };

    Color rainbow(double x);
}




class BaseRender
{
public:
    virtual std::vector<my::Color> & get_image() = 0;
    virtual void foreward() = 0;
    virtual void backward() = 0;
    virtual void left() = 0;
    virtual void right() = 0;
    virtual void up() = 0;
    virtual void down() = 0;
    virtual void more_iterations() = 0;
    virtual void less_iterations() = 0;
};

#endif // BASERENDER_H
