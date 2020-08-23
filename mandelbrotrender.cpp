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
    compute_and_set_max_iterations(0, 0, width, height);
    render(0, 0, width, height);
}

std::vector<my::Color> & MandelbrotRender::get_image() {
    return _img;
}

int converge_count(std::complex<long double> c, unsigned int max_iter) {
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



void MandelbrotRender::compute_and_set_max_iterations(unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned y_max) {
    #pragma omp parallel for collapse(2)
    for (int y = y_min; y < y_max; y++) {
        for (int x = x_min; x < x_max; x++) {
            int num_iterations = converge_count(
                        _center + std::complex<long double>(
                            (static_cast<long double>(x) - _width/2) / _zoom,
                            (static_cast<long double>(y) - _height/2) / _zoom) , _max_iter);
            _iterations[x + y * _width] = num_iterations;
        }
    }
}



void MandelbrotRender::foreward() {
    _zoom *= 1.1;
    auto start = std::chrono::high_resolution_clock::now();
    compute_and_set_max_iterations(0, 0, _width, _height);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    render(0, 0, _width, _height);
}

void MandelbrotRender::backward() {
    _zoom *= 0.9;
    compute_and_set_max_iterations(0, 0, _width, _height);
    render(0, 0, _width, _height);
}

void MandelbrotRender::up() {
    _center -= std::complex<long double>(0, 10 / _zoom);
    std::shift_right(_img.begin(), _img.end(), _width * 10);
    compute_and_set_max_iterations(0, 0, _width, 10);
    render(0, 0, _width, 10);
}

void MandelbrotRender::down() {
    _center += std::complex<long double>(0, 10 / _zoom);
    std::shift_left(_img.begin(), _img.end(), _width * 10);
    compute_and_set_max_iterations(0, _height - 10, _width, _height);
    render(0, _height - 10, _width, _height);
}

void MandelbrotRender::left() {
    _center -= std::complex<long double>(10 / _zoom, 0);
    for (int y = 0; y < _height; y++) {
        std::shift_right(_img.begin() + y * _width, _img.begin() + (y + 1) * _width - 1, 10);
    }
    compute_and_set_max_iterations(0, 0, 10, _height);
    render(0, 0, 10, _height);
}

void MandelbrotRender::right() {
    _center += std::complex<long double>(10 / _zoom, 0);
    for (int y = 0; y < _height; y++) {
        std::shift_left(_img.begin() + y * _width, _img.begin() + (y + 1) * _width - 1, 10);
    }
    compute_and_set_max_iterations(_width - 11, 0, _width, _height);
    render(_width - 11, 0, _width, _height);
}
