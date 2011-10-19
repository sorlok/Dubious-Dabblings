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
DLLEXPORT void my_basic_update();
DLLEXPORT void demo_display();
DLLEXPORT void sfml_display();
DLLEXPORT void close_sfml();

//Post FX
DLLEXPORT int can_use_postfx();
DLLEXPORT sf::PostFX* new_postfx(const char* filename);
DLLEXPORT void demo_set_default_postfx(sf::PostFX* item);
DLLEXPORT void demo_update_postfx_color(sf::PostFX* item);

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

//Game functionality
DLLEXPORT int game_get_mouse_x();
DLLEXPORT int game_get_mouse_y();
DLLEXPORT float game_get_frame_time_s();
DLLEXPORT void game_set_poly_pos(int x, int y);
DLLEXPORT void game_draw_item(sf::Drawable* item);
DLLEXPORT void game_del_item(sf::Drawable* item);



#ifdef __cplusplus
}
#endif



//+1 global variables!
sf::RenderWindow myWindow;
float polyScale;
bool polyScaleDec;



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


//sf::Image img;
//sf::Sprite spr1;
//sf::Sprite spr2;
sf::String fps;
sf::Shape poly;

sf::View view;


RollingAverage framerate(1000);
sf::Clock frClock;

void flipPoly(int id1, int id2)
{
	sf::Color old = poly.GetPointColor(id2);
	poly.SetPointColor(id2, poly.GetPointColor(id1));
	poly.SetPointColor(id1, old);
}



int init_sfml(int width, int height, int depth)
{
	myWindow.Create(sf::VideoMode(width, height, depth), "Here is a test window.");

	//Init resources
	fps.SetPosition(10, 10);
	poly.AddPoint(0, -50,  sf::Color(0xFF, 0, 0));
	poly.AddPoint(50, 0,   sf::Color(0, 0xFF, 0));
	poly.AddPoint(50, 50,  sf::Color(0, 0, 0xFF));
	poly.AddPoint(0, 100,  sf::Color(0xFF, 0xFF, 0));
	poly.AddPoint(-50, 50, sf::Color(0, 0xFF, 0xFF));
	poly.AddPoint(-50, 0,  sf::Color(0xFF, 0, 0xFF));
	poly.SetPosition(800/2, 600/3);

	view.SetCenter(800/2, 600/2);
	view.SetHalfSize(800*75/100, 600*75/100);

	polyScale = 1.0;
	polyScaleDec = true;

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

void my_basic_update()
{
	//TODO
	//spr1.SetRotation(spr1.GetRotation()+50*myWindow.GetFrameTime());
	//spr2.SetRotation(spr2.GetRotation()-80*myWindow.GetFrameTime());

	//sf::Color newColor = spr1.GetColor();
	//newColor.b = (unsigned int) ((1.0-polyScale)*255);
	//spr1.SetColor(newColor);

	//newColor = spr2.GetColor();
	//newColor.a = (unsigned int) (polyScale*255);
	//spr2.SetColor(newColor);

	polyScale += (polyScaleDec?-1:1)*myWindow.GetFrameTime();
	if (polyScale<0.0) {
		polyScale = 0.0;
		polyScaleDec = false;
		flipPoly(1, 5);
		flipPoly(2, 4);
	} else if (polyScale>1.0) {
		polyScale = 1.0;
		polyScaleDec = true;
	}
	poly.SetScaleX(polyScale);


	//Grab a handle on the input struct
	const sf::Input& myInput = myWindow.GetInput();

	//Sample (erroneous) functionality to be replaced in Parrot
	poly.SetPosition(myInput.GetMouseY(), myInput.GetMouseX());
}

void demo_display()
{
	myWindow.Clear(sf::Color(0x33, 0x33, 0x33));

	//myWindow.SetView(view);

	//myWindow.Draw(spr1);
	//myWindow.Draw(spr2);
	myWindow.Draw(poly);

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



void game_del_item(sf::Drawable* item)
{
	delete item;
}


void demo_set_default_postfx(sf::PostFX* item)
{
	item->SetTexture("framebuffer", NULL); //"NULL" means use the screen.
	item->SetParameter("color", 1.f, 1.f, 1.f);
}


void demo_update_postfx_color(sf::PostFX* item)
{
	float framePerc = myWindow.GetFrameTime() - static_cast<int>(myWindow.GetFrameTime());
	float rPerc = polyScale;
	float gPerc = framePerc;
	float bPerc = 1.0 - framePerc;
	item->SetParameter("color", rPerc, gPerc, bPerc);
}



void game_draw_item(sf::Drawable* item)
{
	myWindow.Draw(*item);
}


int game_get_mouse_x()
{
	return myWindow.GetInput().GetMouseX();
}

int game_get_mouse_y()
{
	return myWindow.GetInput().GetMouseY();
}

void game_set_poly_pos(int x, int y)
{
	poly.SetPosition(x, y);
}

float game_get_frame_time_s()
{
	return myWindow.GetFrameTime();
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

