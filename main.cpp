#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <windows.h>  
#include <tchar.h> 
#include <cmath>
#include <iostream>
#include <vector>

unsigned int MAX_ITER = 120; // maximum number of iterations for the complex number generator 
int resolution = 1;
constexpr int IMG_SIZE_X = 1920;
constexpr int IMG_SIZE_Y = 1080;
double zoomDepth = 0.004; // allow zoom
double offsetY = 0.0; // allow pan y
double offsetX = 0.0; // allow pan x
std::vector<sf::Color> palette = 
{
	sf::Color(0,   7,   100),
	sf::Color(32,  107, 203),
	sf::Color(237, 255, 255),
	sf::Color(255, 170, 0),
	sf::Color(0,   2,   0)
};

sf::Color colorGradient(double);
//sf::Color linearInterpolate(sf::Color, sf::Color, int);

// This program only works on windows. For a cross-platform version, see the crossplatform implementation. 
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	// Setup SFML renderer
	sf::RenderWindow newWindow(sf::VideoMode(IMG_SIZE_X, IMG_SIZE_Y), "Mandelbrot Generator");
	sf::Image img;
	img.create(IMG_SIZE_X, IMG_SIZE_Y, sf::Color(0, 0, 0));
	sf::Texture texture;
	sf::Sprite sprite;
	bool render = true;
	while (newWindow.isOpen()) {
		sf::Event event;
		while (newWindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				newWindow.close();
				return 0;
			}
			else if (event.type == sf::Event::KeyPressed) {
				render = true; // image needs to be recreated when the user changes zoom or offset
				switch (event.key.code) {
				case sf::Keyboard::Equal:
					zoomDepth *= 0.9;
					break;
				case sf::Keyboard::Escape:
					newWindow.close();
					return 0;
				case sf::Keyboard::Dash:
					zoomDepth /= 0.9;
					break;
				case sf::Keyboard::W:
					offsetY -= 40 * zoomDepth;
					break;
				case sf::Keyboard::A:
					offsetX -= 40 * zoomDepth;
					break;
				case sf::Keyboard::S:
					offsetY += 40 * zoomDepth;
					break;
				case sf::Keyboard::D:
					offsetX += 40 * zoomDepth;
					break;
				case sf::Keyboard::Right:
					MAX_ITER+=10;
					break;
				case sf::Keyboard::Left:
					MAX_ITER-=10;
					break;
				case sf::Keyboard::R:
					offsetX = 0;
					offsetY = 0;
					MAX_ITER = 120;
					resolution = 1;
					zoomDepth = 0.004;
					break;
				default:
					render = false;
					break;
				}
			}
		}
		if (render) {
			// the mandelbrot set is the set of points that do not diverge when iterated from z = 0 (the black-colored points)
			// in other words, it is the set of points where f1 does not go to infinity
			// when starting at z = 0 and going to a maximum iteration point, in this case, MAX_ITER
			// https://en.wikipedia.org/wiki/Mandelbrot_set#Formal_definition
			// based on the equation f1(z) := z^2 + c
			// in this implementation, we are simulating imaginary numbers, so the equation is actually
			// f2(x, y) := x*x - y*y + c1, 2*x*y + c2 
			// for real numbers x, y c1 and c2
			// c1 is the "real part" and c2 is the "imaginary part"
				// since z = (x+yi) and c = a+bi 
				// so f1 = (x+yi)*(x+yi) + a+bi
					  // = x*x + x*yi + x*yi + yi*yi + a+bi
					  // = x*x + 2x*yi + yi*yi + a + bi
					  // = x*x - y*y + i(2x*y + b) + a
					  // = (x^2 - y^2 + a) + i(2x*y+b)
			// both functions return a coordinate on a plane, f1 is simply on the gaussian plane and f2 is on the cartesian plane
			for (int horizontal = 0; horizontal < IMG_SIZE_X; horizontal += resolution) {
				for (int vertical = 0; vertical < IMG_SIZE_Y; vertical += resolution) {
					// simulates the "imaginary" Y axis by converting from the gaussian plane to the cartesian plane
					// doesn't matter if these numbers are negative
					double quote_REAL_endquote = (horizontal - IMG_SIZE_X / 2.0) * zoomDepth + offsetX;
					double quote_IMAG_endquote = (vertical - IMG_SIZE_Y / 2.0) * zoomDepth + offsetY;
					double x = 0, y = 0;
					int iteration = 0;
					// http://xahlee.info/cmaci/fractal/mandelbrot.html
					// this is literally pythagorean's theorem below
					while (x*x + y*y <= 4 /* which is 2^2 */ && iteration < MAX_ITER) {
						// grab the next x value. At pixel 0,0, nextX = 960
						double nextX = x*x - y*y + quote_REAL_endquote;
						// at pixel 0,0, y = 540
						y = 2 * x*y + quote_IMAG_endquote;
						x = nextX;
						// thus, pixel 0,0 is not colored black (it diverges), because the equation will yield larger and larger numbers
							// as _iteration_ increases. For the very center of the image, the point does not diverge.
						iteration++;
					}
					if (iteration < MAX_ITER) {
						double logBase = 1.0 / log(2.0);
						double logHalfBase = log(0.5)*logBase;
						iteration = 0 + iteration - logHalfBase - log(log(sqrt(x*x + y*y)))*logBase;
					}
					img.setPixel(horizontal, vertical, colorGradient(iteration));
				}
			}
			texture.loadFromImage(img);
			sprite.setTexture(texture);
		}
		render = false;
		newWindow.clear();
		newWindow.draw(sprite);
		newWindow.display();
	}
	return 0;
}

sf::Color colorGradient(double iterations) {
	if ((int)iterations == MAX_ITER) {
		return sf::Color(0, 0, 0);
	}
	else {
		int r = 0, g = 0, b = 0;
		if (iterations < 16) {
			r = 16 * (16 - iterations);
			g = 0;
			b = 16 * iterations - 1;
		}
		else if (iterations < 32) {
			r = 0;
			g = 16 * (iterations - 16);
			b = 16 * (32 - iterations) - 1;
		}
		else if (iterations < 64) {
			r = 8 * (iterations - 32);
			g = 8 * (64 - iterations) - 1;
			b = 0;
		}
		else { // range is 64 - 127
			r = 255 - (iterations - 64) * 4;
			g = 0;
			b = 0;
		}
		return sf::Color(r, g, b);
		//return sf::Color((int)(iterations) % 256, (int)(iterations) % 256, (int)(iterations) % 256, 255);
	}
}
//sf::Color linearInterpolate(sf::Color a, sf::Color b, int iterations) {
//	sf::Color final;
//	final.r = (1 - iterations) * a.r + iterations * b.r;
//	final.g = (1 - iterations) * a.g + iterations * b.g;
//	final.b = (1 - iterations) * a.b + iterations * b.b;
//	return final;
//}