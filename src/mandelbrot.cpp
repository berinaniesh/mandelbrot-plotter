//1. Create Game Window .. OK
//2. Draw Mandelbrot .. OK
//3. Zoom .. OK
//4. Move .. OK
//5. Iteration level .. OK
//6. Display text information .. OK
//7. openMP for better performance .. OK
//8. coloring .. OK
//9. another color .. OK
//10. change resolution

#include <stdio.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

const int W = 1920;
const int H = 1080;

int max_iter = 128;
double zoom = 1.0;
double min_re = -2.5, max_re = 1;
double min_im = -1, max_im = 1;

// computes v + t(u - v)
// t should be a value between 0 and 1
Color linear_interpolation(const Color& v, const Color& u, double a)
{
	auto const b = 1 - a;
	return Color(b*v.r + a * u.r, b*v.g + a * u.g, b*v.b + a * u.b);
}

int main(void)
{
	RenderWindow window(VideoMode(W, H), "Mandelbrot");
	Image image; image.create(W, H);
	Texture texture;
	Sprite sprite;
	//Font font; font.loadFromFile("arial.ttf");
	//Text text;
	//text.setFont(font);
	//text.setCharacterSize(24);
	//text.setFillColor(Color::White);

	while (window.isOpen())
	{
		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed) window.close();
			if (e.type == Event::KeyPressed)
			{
				//move delta
				double w = (max_re - min_re)*0.3;
				double h = (max_im - min_im)*0.3;

				if (e.key.code == Keyboard::Left) { min_re -= w, max_re -= w; }
				if (e.key.code == Keyboard::Right) { min_re += w, max_re += w; }
				if (e.key.code == Keyboard::Up) { min_im -= h, max_im -= h; }
				if (e.key.code == Keyboard::Down) { min_im += h, max_im += h; }

			}
			//Set Iteration level by mouse wheel
			//the more iteration level the better image result
			if (e.type == Event::MouseWheelScrolled)
			{
				if (e.MouseWheelScrolled)
				{
					if (e.mouseWheelScroll.wheel == Mouse::VerticalWheel)
					{
						if (e.mouseWheelScroll.delta > 0)max_iter *= 2;
						else max_iter /= 2;
						if (max_iter < 1)max_iter = 1;
					}
				}
			}
			if (e.type == Event::MouseButtonPressed)
			{
				auto zoom_x = [&](double z)
				{
					//mouse point will be new center point
					double cr = min_re + (max_re - min_re)*e.mouseButton.x / W;
					double ci = min_im + (max_im - min_im)*e.mouseButton.y / H;

					//zoom
					double tminr = cr - (max_re - min_re) / 2 / z;
					max_re = cr + (max_re - min_re) / 2 / z;
					min_re = tminr;

					double tmini = ci - (max_im - min_im) / 2 / z;
					max_im = ci + (max_im - min_im) / 2 / z;
					min_im = tmini;
				};
				//Left Click to ZoomIn
				if (e.mouseButton.button == Mouse::Left)
				{
					zoom_x(5);
					zoom *= 5;
				}
				//Right Click to ZoomOut
				if (e.mouseButton.button == Mouse::Right)
				{
					zoom_x(1.0 / 5);
					zoom /= 5;
				}
			}
		}
		window.clear();
#pragma omp parallel for
		for (int y = 0; y < H; y++)for (int x = 0; x < W; x++)
		{
			double cr = min_re + (max_re - min_re)*x / W;
			double ci = min_im + (max_im - min_im)*y / H;
			double re = 0, im = 0;
			int iter;
			for (iter = 0; iter < max_iter; iter++)
			{
				double tr = re * re - im * im + cr;
				im = 2 * re*im + ci;
				re = tr;
				if (re*re + im * im > 2 * 2) break;
			}
			int r = 1.0*(max_iter - iter) / max_iter * 0xff;
			int g = r, b = r;

			/*
			//color pallet similar to Ultra Fractal and Wikipedia
			static const vector<Color> colors{
				{0,7,100},
				{32,107,203},
				{237,255,255},
				{255,170,0},
				{0,2,0},
			};
			*/
			static const vector<Color> colors{
				{0,0,0},
				{213,67,31},
				{251,255,121},
				{62,223,89},
				{43,30,218},
				{0,255,247}
			};

			static const auto max_color = colors.size() - 1;
			if (iter == max_iter)iter = 0;
			double mu = 1.0*iter / max_iter;
			//scale mu to be in the range of colors
			mu *= max_color;
			auto i_mu = static_cast<size_t>(mu);
			auto color1 = colors[i_mu];
			auto color2 = colors[min(i_mu + 1, max_color)];
			Color c = linear_interpolation(color1, color2, mu - i_mu);


			image.setPixel(x, y, Color(c));
		}
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		window.draw(sprite);

		//char str[100];
		//sprintf(str, "max iter:%d\nzoom:x%2.2lf", max_iter, zoom);
		//text.setString(str);
		//window.draw(text);



		window.display();
	}
	return 0;
}
