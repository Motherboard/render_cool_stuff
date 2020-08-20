#include <iostream>
#include <gmpxx.h>
#include <SFML/Graphics.hpp>
#include <array>
#include <math.h>
#include <chrono>
#include <limits>

constexpr unsigned int WIDTH = 640, HEIGHT = 480, MAX_ITERATIONS=1000;
constexpr int STEP=-5;

void color_faster(unsigned char data[], uint_fast32_t r, uint_fast32_t g, uint_fast32_t b, uint_fast32_t a=255) {
    *(reinterpret_cast<uint_fast32_t *>(data)) = r | (g << 8) | (b << 16) | (a << 24);
}

void color(unsigned char data[], unsigned char r, unsigned char g, unsigned char b, unsigned char a=255) {
    data[0] = r; data[1] = g; data[2] = b; data[3] = a;
}


struct Color {
  u_char r, g, b, a;
};

Color rainbow(double x) {
    static constexpr double step = 1.0/6;
    x -= std::floor(x);
    if (x < 1 * step)
        return {255, 0, static_cast<u_char>(255*x/step), 255};
    if (x < 2 * step)
        return {static_cast<u_char>(255 - 255.0/step*(x - (1 * step))), 0, 255, 255};
    if (x < 3 * step)
        return {0, static_cast<u_char>(255.0/step*(x - (2 * step))), 255, 255};
    if (x < 4 * step)
        return {0, 255, static_cast<u_char>(255 - 255.0/step*(x - (3 * step))), 255};
    if (x < 5 * step)
        return {static_cast<u_char>(255.0/step*(x - (4 * step))), 255, 0, 255};
    return {255, static_cast<u_char>(255-255.0/step*(x - (5 * step))), 0, 255};
}


void rainbow(unsigned char *data, double x) {
    constexpr double step = 1.0/6.0;
    x -= std::floor(x);
    if (x < 1 * step)
        color(data, 255, 0, 255*x/step);
    else if (x < 2 * step)
        color(data, 255 - 255.0*(x - (1 * step))/step, 0, 255);
    else if (x < 3 * step)
        color(data, 0, 255.0*(x - (2 * step))/step, 255);
    else if (x < 4 * step)
        color(data, 0, 255, 255.0 - 255.0*(x - (3 * step))/step);
    else if (x < 5 * step)
        color(data, 255.0*(x - (4 * step))/step, 255, 0);
    else
        color(data, 255, 255.0-255.0*(x - (5 * step))/step, 0);
}


std::array<sf::Uint8, WIDTH * HEIGHT * 4> circles(int offset) {
    constexpr double min_r = static_cast<double>(std::min(WIDTH, HEIGHT))/2;
    std::array<sf::Uint8, WIDTH * HEIGHT * 4> data;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double radius_coeff = (std::sqrt(std::pow(static_cast<double>(x) - (WIDTH / 2), 2) + std::pow(static_cast<double>(y) - (HEIGHT / 2), 2)) + offset) / min_r;
            rainbow(data.data() + x * 4 + y * WIDTH * 4, radius_coeff);
        }
    }
    return data;
}

template <typename T>
double converge_count(T c_r, T c_i) {
    T z_r(c_r), z_i(c_i);
    for (unsigned int count = 1; count < MAX_ITERATIONS; count++) {
        T new_z_r = (z_r * z_r) - (z_i * z_i) + c_r;
        z_i = 2 * (z_r * z_i) + c_i;
        z_r = std::move(new_z_r);
        if ((z_r * z_r) + (z_i * z_i) > 4) {
            return static_cast<double>(count) / MAX_ITERATIONS;
        }
    }
    return -1;
}


/*std::array<sf::Uint8, WIDTH * HEIGHT * 4> mandelbrot(mpq_class center_r, mpq_class center_i, mpz_class zoom) {
    std::array<sf::Uint8, WIDTH * HEIGHT * 4> data;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double count = converge_count(center_r - mpq_class(x - WIDTH/2, zoom), center_i - mpq_class(y - HEIGHT/2, zoom));
            rainbow(data.data() + x * 4 + y * WIDTH * 4, count);
        }
    }
    return data;
}*/


std::array<Color, WIDTH * HEIGHT> mandelbrot(long double center_r, long double center_i, long double zoom) {
    std::array<Color, WIDTH * HEIGHT> data;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double count = converge_count(center_r + (static_cast<long double>(x) - WIDTH/2) / zoom, center_i + (static_cast<long double>(y) - HEIGHT/2) / zoom);
            if (count > 0)
                data[x + y * WIDTH] = rainbow(count);
            else
                data[x + y * WIDTH] = {0, 0, 0, 0};
        }
    }
    return data;
}

std::array<sf::Uint8, WIDTH * HEIGHT * 4> mandelbrot(mpf_class center_r, mpf_class center_i, mpf_class zoom) {
    std::array<sf::Uint8, WIDTH * HEIGHT * 4> data;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double count = converge_count<mpf_class>(center_r + (mpf_class(x) - WIDTH/2) / zoom, center_i + (mpf_class(y) - HEIGHT/2) / zoom);
            if (count > 0)
                rainbow(data.data() + x * 4 + y * WIDTH * 4, count);
            else
                color(data.data() + x * 4 + y * WIDTH * 4, 0, 0, 0);
        }
    }
    return data;
}



class App {
public:
    App() {
        window.create(sf::VideoMode(WIDTH, HEIGHT), "My window");
        texture.create(WIDTH, HEIGHT);
        sprite.setTexture(texture, true);
        window.setVerticalSyncEnabled(true);
    }
    void run() {
        while (window.isOpen())
        {
            process_events();
            frame_time += render_image_get_processing_time();
            elapsed_frames++;
            if (elapsed_frames % 60 == 0) {
                std::cout << "FPS: " << 60000.0l / frame_time << std::endl;
                frame_time = 0;
                elapsed_frames = 0;
            }
        }
    }
private:
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    unsigned int elapsed_frames = 0;
    unsigned long long frame_time = 0;
    long double center_r{-std::exp(1l)/7}, center_i{-std::exp(1l)/20}, zoom{1e2};
    unsigned long render_image_get_processing_time() {
        // clear the window with black color
        window.clear(sf::Color::Black);
        // draw everything here...
        auto start = std::chrono::high_resolution_clock::now();
        texture.update(reinterpret_cast<u_char *>(mandelbrot(center_r, center_i, zoom).data()));
        sprite.setTexture(texture);
        window.draw(sprite);
        auto end = std::chrono::high_resolution_clock::now();
        // end the current frame
        window.display();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
    void process_events() {
        sf::Event event;
        bool processed_move = false;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && !processed_move) {
                processed_move = true;
                switch (event.key.code) {
                case sf::Keyboard::Up:
                    center_i -= 10 / zoom;
                    break;
                case sf::Keyboard::Down:
                    center_i += 10 / zoom;
                    break;
                case sf::Keyboard::Left:
                    center_r -= 10 / zoom;
                    break;
                case sf::Keyboard::Right:
                    center_r += 10 / zoom;
                    break;
                case sf::Keyboard::A:
                    zoom *= 1.1;
                    break;
                case sf::Keyboard::Z:
                    zoom *= 0.9;
                    break;
                default:
                    break;
                }
            }
        }
    }
};



int main()
{
    App app;
    app.run();
    return 0;
}
