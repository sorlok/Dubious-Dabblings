#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


#include <list>
#include <iostream>
#include <sstream>

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
DLLEXPORT int init_sfml();
DLLEXPORT int sfml_handle_events();
DLLEXPORT void my_basic_update();
DLLEXPORT void sfml_display();
DLLEXPORT void close_sfml();

//Game functionality
DLLEXPORT int game_get_mouse_x();
DLLEXPORT int game_get_mouse_y();
DLLEXPORT void game_set_poly_pos(int x, int y);


//Temp
DLLEXPORT void main_loop_hack();


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


sf::Image img;
sf::Sprite spr1;
sf::Sprite spr2;
sf::String fps;
sf::Shape poly;

sf::View view;

sf::PostFX pEffect;

RollingAverage framerate(1000);
sf::Clock frClock;

void flipPoly(int id1, int id2)
{
	sf::Color old = poly.GetPointColor(id2);
	poly.SetPointColor(id2, poly.GetPointColor(id1));
	poly.SetPointColor(id1, old);
}



int init_sfml()
{
	myWindow.Create(sf::VideoMode(800, 600, 32), "Here is a test window.");
	//myWindow.UseVerticalSync(true); //Restrict to 50fps

	//Check
	if (!sf::PostFX::CanUsePostFX()) {
		std::cout <<"Your graphics card doesn't support post-effects.\n";
		return 0;
	}

	//Init resources
	if (!img.LoadFromFile("person.png")) {
		std::cout <<"Error loading image!\n";
		return 0;
	}
	spr1.SetImage(img);
	spr1.SetPosition(800/3, 600/2);
	spr1.SetCenter(img.GetWidth()/2, img.GetHeight()/2);
	spr2.SetImage(img);
	spr2.SetPosition(2*800/3, 600/2);
	spr2.SetCenter(img.GetWidth()/2, img.GetHeight()/2);
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

	if (!pEffect.LoadFromFile("colorize.sfx")) {
	    std::cout <<"Couldn't find post-effect file.\n";
	    return 0;
	}

	pEffect.SetTexture("framebuffer", NULL); //"NULL" means use the screen.
	pEffect.SetParameter("color", 1.f, 1.f, 1.f);

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
	spr1.SetRotation(spr1.GetRotation()+50*myWindow.GetFrameTime());
	spr2.SetRotation(spr2.GetRotation()-80*myWindow.GetFrameTime());

	sf::Color newColor = spr1.GetColor();
	newColor.b = (unsigned int) ((1.0-polyScale)*255);
	spr1.SetColor(newColor);

	newColor = spr2.GetColor();
	newColor.a = (unsigned int) (polyScale*255);
	spr2.SetColor(newColor);

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


	float rPerc = polyScale;
	float gPerc = spr1.GetRotation()/360.0;
	float bPerc = spr2.GetRotation()/360.0;
	pEffect.SetParameter("color", rPerc, gPerc, bPerc);

	//Grab a handle on the input struct
	const sf::Input& myInput = myWindow.GetInput();

	//Sample (erroneous) functionality to be replaced in Parrot
	poly.SetPosition(myInput.GetMouseY(), myInput.GetMouseX());
}

void sfml_display()
{
	myWindow.Clear(sf::Color(0x33, 0x33, 0x33));

	//myWindow.SetView(view);

	myWindow.Draw(spr1);
	myWindow.Draw(spr2);
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

	myWindow.Draw(pEffect);

	myWindow.Draw(fps);

	myWindow.Display();
}


void close_sfml()
{
	myWindow.Close();

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


void main_loop_hack()
{
	if (init_sfml()==0) {
		return /*1*/;
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

	return /*0*/;
}


#ifndef IS_BUILDING_LIBRARY

int main(int argc, char** argv)
{
	main_loop_hack();
	return 0;
}

#endif //IS_BUILDING_LIBRARY


