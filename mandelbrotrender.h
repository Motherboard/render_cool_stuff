#ifndef MANDELBROTRENDER_H
#define MANDELBROTRENDER_H

#include "baserender.h"
#include <complex>


struct IndexAndComplexPoint {
    unsigned int idx;
    std::complex<long double> point;
};



class MandelbrotRender : public BaseRender
{
public:
    MandelbrotRender(unsigned int width, unsigned int height, double initial_zoom, std::complex<long double> initial_center);
    std::vector<my::Color> & get_image() final;
    void foreward() final;
    void backward() final;
    void left() final;
    void right() final;
    void up() final;
    void down() final;
    void more_iterations() final;
    void less_iterations() final;
private:
    unsigned int _width, _height, _max_iter;
    long double _zoom, _last_computed_zoom;
    std::complex<long double> _center;
    std::vector<my::Color> _img;
    std::vector<int> _iterations;
    std::vector<IndexAndComplexPoint> _indices_to_revisit;
    void render(unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned y_max);
    void compute(unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned y_max);
    void recompute_convergant(int recalc_threshold);
};

#endif // MANDELBROTRENDER_H
