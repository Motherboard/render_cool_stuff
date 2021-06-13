#include "mandelbrotrender.h"
#include <algorithm>
#include <execution>
#include <iostream>
#include <chrono>


MandelbrotRender::MandelbrotRender(unsigned int width, unsigned int height, double initial_zoom, std::complex<long double> initial_center)
    : _width(width), _height(height), _zoom(initial_zoom), _center(initial_center)
{
    _img.resize(width * height);
    _iterations.resize(width * height);
    _indices_to_revisit.reserve(width * height);
    _max_iter = 1000;
    compute(0, 0, width, height);
    render(0, 0, width, height);
}

std::vector<my::Color> & MandelbrotRender::get_image() {
    return _img;
}



int converge_count(long double c_r, long double c_i, unsigned int max_iter) {
    auto z_r(c_r), z_i(c_i);
    auto z_r_sq = z_r * z_r, z_i_sq = z_i * z_i;
    unsigned int count = 1;
    for (; (count < max_iter) && (z_i_sq + z_i_sq < 4); count++) {
        z_i = z_r * z_i;
        z_i += z_i + c_i;
        z_r = z_r_sq - z_i_sq + c_r;
        z_r_sq = z_r * z_r; z_i_sq = z_i * z_i;
    }
    return count == max_iter ? -1 : count;
}

int converge_count(std::complex<long double> &&c, unsigned int max_iter) {
    std::complex<long double> z(c);
    for (unsigned int count = 1; count < max_iter; count++) {
        z = z*z + c;
        if (std::norm(z) > 4) {
            return count;
        }
    }
    return -1;
}



void MandelbrotRender::render(unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned y_max) {
    double max_log = std::log(_max_iter);
    #pragma omp parallel for collapse(2)
    for (int y = y_min; y < y_max; y++) {
        for (int x = x_min; x < x_max; x++) {
            if (_iterations[x + y * _width] > 0)
                _img[x + y * _width] = my::rainbow(std::log(static_cast<double>(_iterations[x + y * _width]))/max_log);
            else
                _img[x + y * _width] = {0, 0, 0, 0};
        }

    }
}



void MandelbrotRender::compute(unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned y_max) {
#pragma omp parallel for collapse(2)
    for (int y = y_min; y < y_max; y++) {
        for (int x = x_min; x < x_max; x++) {
            _iterations[x + y * _width] = converge_count(
                        _center + std::complex<long double>(
                            (static_cast<long double>(x) - _width/2) / _zoom,
                            (static_cast<long double>(y) - _height/2) / _zoom) , _max_iter);
        }
    }
}



void MandelbrotRender::foreward() {
    _zoom *= 1.1;
    auto start = std::chrono::high_resolution_clock::now();
    compute(0, 0, _width, _height);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    render(0, 0, _width, _height);
}

void MandelbrotRender::backward() {
    _zoom *= 0.9;
    compute(0, 0, _width, _height);
    render(0, 0, _width, _height);
}

void MandelbrotRender::up() {
    _center -= std::complex<long double>(0, 10 / _zoom);
    std::shift_right(_img.begin(), _img.end(), _width * 10);
    compute(0, 0, _width, 10);
    render(0, 0, _width, 10);
}

void MandelbrotRender::down() {
    _center += std::complex<long double>(0, 10 / _zoom);
    std::shift_left(_img.begin(), _img.end(), _width * 10);
    compute(0, _height - 10, _width, _height);
    render(0, _height - 10, _width, _height);
}

void MandelbrotRender::left() {
    _center -= std::complex<long double>(10 / _zoom, 0);
    for (int y = 0; y < _height; y++) {
        std::shift_right(_img.begin() + y * _width, _img.begin() + (y + 1) * _width - 1, 10);
    }
    compute(0, 0, 10, _height);
    render(0, 0, 10, _height);
}

void MandelbrotRender::right() {
    _center += std::complex<long double>(10 / _zoom, 0);
    for (int y = 0; y < _height; y++) {
        std::shift_left(_img.begin() + y * _width, _img.begin() + (y + 1) * _width - 1, 10);
    }
    compute(_width - 11, 0, _width, _height);
    render(_width - 11, 0, _width, _height);
}

void MandelbrotRender::more_iterations() {
    _max_iter += 500;
    recompute_convergant(_max_iter - 500);
    render(0, 0, _width, _height);
}

void MandelbrotRender::less_iterations() {
    if (_max_iter < 1000)
        return;
    _max_iter -= 500;
    std::cout << "max iterations: "  << _max_iter << std::endl;
}


void MandelbrotRender::recompute_convergant(int recalc_threshold) {
    std::vector<int> mask(_iterations.size());
    std::copy(_iterations.cbegin(), _iterations.cend(), mask.begin());
#pragma omp parallel for collapse(2)
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            if (mask[x + y * _width] < 0)
                _iterations[x + y * _width] = converge_count(
                            _center + std::complex<long double>(
                                (static_cast<long double>(x) - _width/2) / _zoom,
                                (static_cast<long double>(y) - _height/2) / _zoom) , _max_iter);
        }
    }
}
