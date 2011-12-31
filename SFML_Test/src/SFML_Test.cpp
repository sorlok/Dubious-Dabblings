#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


#include <list>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

//Windows export stuff
#ifdef __WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif


//Prototype shared library functions
#ifdef __cplusplus
extern "C" {
#endif

//Core functionality
DLLEXPORT int init_sfml(int width, int height, int depth);
DLLEXPORT int sfml_handle_events();
DLLEXPORT void demo_display();
DLLEXPORT void sfml_display();
DLLEXPORT void close_sfml();

//Post FX
DLLEXPORT int can_use_postfx();
DLLEXPORT sf::PostFX* new_postfx(const char* filename);
DLLEXPORT void demo_set_default_postfx(sf::PostFX* item);
DLLEXPORT void demo_update_postfx_color(sf::PostFX* item);

//Demo
DLLEXPORT sf::Shape* demo_init_poly();

//Image
DLLEXPORT sf::Image* new_image(const char* filename);
DLLEXPORT int image_get_width(const sf::Image* item);
DLLEXPORT int image_get_height(const sf::Image* item);

//Sprite
DLLEXPORT sf::Sprite* new_sprite();
DLLEXPORT void sprite_set_image(sf::Sprite* item, const sf::Image* img);
DLLEXPORT void sprite_set_position(sf::Sprite* item, int x, int y);
DLLEXPORT void sprite_set_center(sf::Sprite* item, int x, int y);
DLLEXPORT float sprite_get_rotation(const sf::Sprite* item);
DLLEXPORT void sprite_set_rotation(sf::Sprite* item, float angle);
DLLEXPORT sf::Color* sprite_get_color(const sf::Sprite* item);
DLLEXPORT void sprite_set_color(sf::Sprite* item, sf::Color* color);

//Polygon
DLLEXPORT sf::Color* poly_get_point_color(const sf::Shape* item, int pointID);
DLLEXPORT void poly_set_point_color(sf::Shape* item, int pointID, const sf::Color* color);
DLLEXPORT void poly_set_scale_x(sf::Shape* item, float scale);
DLLEXPORT void poly_set_scale_y(sf::Shape* item, float scale);
DLLEXPORT void poly_set_pos_x(sf::Shape* item, int pos);
DLLEXPORT void poly_set_pos_y(sf::Shape* item, int pos);

//Color
DLLEXPORT void color_set_red(sf::Color* item, int red);
DLLEXPORT void color_set_green(sf::Color* item, int green);
DLLEXPORT void color_set_blue(sf::Color* item, int blue);
DLLEXPORT void color_set_alpha(sf::Color* item, int alpha);

//Game functionality
DLLEXPORT const sf::Input* game_get_input();
DLLEXPORT int game_get_mouse_x();
DLLEXPORT int game_get_mouse_y();
DLLEXPORT float game_get_frame_time_s();
DLLEXPORT void game_draw_item(sf::Drawable* item);

//Cleanup
DLLEXPORT void game_del_item(sf::Drawable* item);
DLLEXPORT void game_del_color(sf::Color* color);
DLLEXPORT void game_del_image(sf::Image* item);



#ifdef __cplusplus
}
#endif



//+1 global variables!
sf::RenderWindow myWindow;



class RollingAverage {
public:
	RollingAverage(unsigned int sampleSize) : sampleSize(sampleSize), runningTotal(0) {}

	void addSample(float val) {
		runningTotal += val;

		sample.push_back(val);
		if (sample.size()>sampleSize) {
			runningTotal -= sample.front();
			sample.pop_front();
		}
	}

	float getAverage() {
		return runningTotal/sample.size();
	}

private:
	std::list<float> sample;
	unsigned int sampleSize;
	float runningTotal;
};



//Return true if the app should exit reasonably soon after this event.
bool handleEvent(const sf::Event& event)
{
	switch (event.Type) {
		case sf::Event::Closed:
			return true;
			break;


		//Avoid warnings
		default:
			break;

	}


	return false;
}


//We could even remove this too, but I don't see the need right now.
sf::String fps;

//This should stay in the engine.
RollingAverage framerate(1000);
sf::Clock frClock;



int init_sfml(int width, int height, int depth)
{
	myWindow.Create(sf::VideoMode(width, height, depth), "Here is a test window.");

	//Init resources
	fps.SetPosition(10, 10);

	return 1;
}


int sfml_handle_events()
{
	bool doExit = false;
	sf::Event event;
	while (myWindow.GetEvent(event)) {
		doExit = handleEvent(event) || doExit;
	}
	return doExit ? 1 : 0;
}



void demo_display()
{
	myWindow.Clear(sf::Color(0x33, 0x33, 0x33));

	if (myWindow.GetFrameTime()>0.0) {
		framerate.addSample(1.0/myWindow.GetFrameTime());
	}

	if (frClock.GetElapsedTime()>0.5) {
		frClock.Reset();
		std::stringstream fpsStr;
		fpsStr <<framerate.getAverage() <<" fps";
		fps.SetText(fpsStr.str());
	}

	myWindow.Draw(fps);
}


void sfml_display()
{
	myWindow.Display();
}


void close_sfml()
{
	myWindow.Close();

}


int can_use_postfx()
{
	return sf::PostFX::CanUsePostFX()?1:0;
}


sf::PostFX* new_postfx(const char* filename)
{
	sf::PostFX* item = new sf::PostFX();
	if (item->LoadFromFile(filename)) {
		return item;
	}

	//Else, cleanup and return null;
	delete item;
	return NULL;
}



sf::Image* new_image(const char* filename)
{
	sf::Image* item = new sf::Image();
	if (item->LoadFromFile(filename)) {
		return item;
	}

	//Else, cleanup and return null;
	delete item;
	return NULL;
}

int image_get_width(const sf::Image* item)
{
	return item->GetWidth();
}


int image_get_height(const sf::Image* item)
{
	return item->GetHeight();
}


sf::Sprite* new_sprite()
{
	sf::Sprite* item = new sf::Sprite();
	return item;
}

void sprite_set_image(sf::Sprite* item, const sf::Image* img)
{
	item->SetImage(*img);
}

void sprite_set_position(sf::Sprite* item, int x, int y)
{
	item->SetPosition(x, y);
}

void sprite_set_center(sf::Sprite* item, int x, int y)
{
	item->SetCenter(x, y);
}

float sprite_get_rotation(const sf::Sprite* item)
{
	return item->GetRotation();
}

void sprite_set_rotation(sf::Sprite* item, float angle)
{
	item->SetRotation(angle);
}

sf::Color* sprite_get_color(const sf::Sprite* item)
{
	sf::Color* res = new sf::Color(item->GetColor());
	return res;
}

void sprite_set_color(sf::Sprite* item, sf::Color* color)
{
	item->SetColor(*color);
}

sf::Color* poly_get_point_color(const sf::Shape* item, int pointID)
{
	sf::Color* res = new sf::Color(item->GetPointColor(pointID));
	return res;
}

void poly_set_point_color(sf::Shape* item, int pointID, const sf::Color* color)
{
	item->SetPointColor(pointID, *color);
}

void poly_set_scale_x(sf::Shape* item, float scale)
{
	item->SetScaleX(scale);
}

void poly_set_scale_y(sf::Shape* item, float scale)
{
	item->SetScaleY(scale);
}

void poly_set_pos_x(sf::Shape* item, int pos)
{
	item->SetX(pos);
}

void poly_set_pos_y(sf::Shape* item, int pos)
{
	item->SetY(pos);
}

void color_set_red(sf::Color* item, int red)
{
	item->r = (red&0xFF);
}

void color_set_green(sf::Color* item, int green)
{
	item->g = (green&0xFF);
}

void color_set_blue(sf::Color* item, int blue)
{
	item->b = (blue&0xFF);
}

void color_set_alpha(sf::Color* item, int alpha)
{
	item->a = (alpha&0xFF);
}


void game_del_item(sf::Drawable* item)
{
	delete item;
}

void game_del_color(sf::Color* color)
{
	delete color;
}

void game_del_image(sf::Image* img)
{
	delete img;
}


void demo_set_default_postfx(sf::PostFX* item)
{
	item->SetParameter("color", 1.f, 1.f, 1.f);
	item->SetTexture("framebuffer", NULL); //"NULL" means use the screen.
}


void demo_update_postfx_color(sf::PostFX* item)
{
	float framePerc = myWindow.GetFrameTime() - static_cast<int>(myWindow.GetFrameTime());
	float rPerc = framePerc;//polyScale;
	float gPerc = framePerc;
	float bPerc = 1.0 - framePerc;
	item->SetParameter("color", rPerc, gPerc, bPerc);
}



void game_draw_item(sf::Drawable* item)
{
	myWindow.Draw(*item);
}

const sf::Input* game_get_input()
{
	return &myWindow.GetInput();
}

int game_get_mouse_x()
{
	return myWindow.GetInput().GetMouseX();
}

int game_get_mouse_y()
{
	return myWindow.GetInput().GetMouseY();
}

sf::Shape* demo_init_poly()
{
	sf::Shape* poly = new sf::Shape();
	poly->AddPoint(0, -50,  sf::Color(0xFF, 0, 0));
	poly->AddPoint(50, 0,   sf::Color(0, 0xFF, 0));
	poly->AddPoint(50, 50,  sf::Color(0, 0, 0xFF));
	poly->AddPoint(0, 100,  sf::Color(0xFF, 0xFF, 0));
	poly->AddPoint(-50, 50, sf::Color(0, 0xFF, 0xFF));
	poly->AddPoint(-50, 0,  sf::Color(0xFF, 0, 0xFF));
	poly->SetPosition(800/2, 600/3);
	return poly;
}

float game_get_frame_time_s()
{
	return myWindow.GetFrameTime();
}



void my_basic_update()
{

}



void main_loop_hack()
{
	if (init_sfml(800, 600, 32)==0) {
		return;
	}

	for(;;) {
		bool doExit = sfml_handle_events();

		my_basic_update();

		sfml_display();

		if (doExit) {
			break;
		}
	}

	close_sfml();

	return;
}


#ifndef IS_BUILDING_LIBRARY

int main(int argc, char** argv)
{
	main_loop_hack();
	return 0;
}

#endif //IS_BUILDING_LIBRARY

