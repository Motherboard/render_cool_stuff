#include "app.h"
#include <iostream>
#include <chrono>

App::App(unsigned int width, unsigned int height): _width(width), _height(height) {
    _window.create(sf::VideoMode(width, height), "My window");
    _texture.create(width, height);
    _window.setVerticalSyncEnabled(true);
}


void App::run() {
    while (_window.isOpen())
    {
        auto start = std::chrono::high_resolution_clock::now();
        process_events();
        render_image();
        auto end = std::chrono::high_resolution_clock::now();
        _frame_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        _elapsed_frames++;
        if (_elapsed_frames % 60 == 0) {
            std::cout << "FPS: " << 60000.0l / _frame_time << std::endl;
            _frame_time = 0;
            _elapsed_frames = 0;
        }
    }
}

void App::set_render(std::unique_ptr<BaseRender> && render) {
    _render_p = std::move(render);
}

void App::render_image() {
    // clear the window with black color
    _window.clear(sf::Color::Black);
    // draw everything here...
    if (_render_p)
        _texture.update(reinterpret_cast<u_char *>(_render_p->get_image().data()));
    _sprite.setTexture(_texture, true);
    _window.draw(_sprite);
    // end the current frame
    _window.display();
}

void App::process_events() {
    sf::Event event;
    bool processed_move = false;
    while (_window.pollEvent(event))
    {
        // "close requested" event: we close the window
        if (event.type == sf::Event::Closed)
            _window.close();
        if (event.type == sf::Event::KeyPressed && !processed_move && _render_p) {
            processed_move = true;
            switch (event.key.code) {
            case sf::Keyboard::Up:
                _render_p->up();
                break;
            case sf::Keyboard::Down:
                _render_p->down();
                break;
            case sf::Keyboard::Left:
                _render_p->left();
                break;
            case sf::Keyboard::Right:
                _render_p->right();
                break;
            case sf::Keyboard::A:
                _render_p->foreward();
                break;
            case sf::Keyboard::Z:
                _render_p->backward();
                break;
            case sf::Keyboard::Equal:
                _render_p->more_iterations();
                break;
            case sf::Keyboard::Tilde:
                _render_p->less_iterations();
                std::cout << "less iters" << std::endl;
                break;
            default:
                std::cout << event.key.code << std::endl;
                break;
            }
        }
    }
}

