#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


#include <list>
#include <iostream>
#include <sstream>

using namespace std;


//Prototype shared library functions
#ifdef __cplusplus
extern "C" {
#endif


//Just run the main code in its own loop. Will create a window, etc.
void run_main_loop();



#ifdef __cplusplus
}
#endif




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



void handleEvent(const sf::Event& event, bool& keepAppRunning)
{
	switch (event.Type) {
		case sf::Event::Closed:
			keepAppRunning = false;
			break;


		//Avoid warnings
		default:
			break;

	}


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



void run_main_loop()
{
	sf::RenderWindow myWindow;
	myWindow.Create(sf::VideoMode(800, 600, 32), "Here is a test window.");
	//myWindow.UseVerticalSync(true); //Restrict to 50fps

	//Check
	if (!sf::PostFX::CanUsePostFX()) {
		std::cout <<"Your graphics card doesn't support post-effects.\n";
		return;
	}

	//Init resources
	if (!img.LoadFromFile("person.png")) {
		std::cout <<"Error loading image!\n";
		return;
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
	    return;
	}

	pEffect.SetTexture("framebuffer", NULL); //"NULL" means use the screen.
	pEffect.SetParameter("color", 1.f, 1.f, 1.f);

	//Bind to input
	const sf::Input& myInput = myWindow.GetInput();

	float polyScale = 1.0;
	bool polyScaleDec = true;

	bool appRunning = true;
	while(appRunning) {
		sf::Event event;
		while (myWindow.GetEvent(event)) {
			handleEvent(event, appRunning);
		}
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


		//Sample (erroneous) functionality to be replaced in Parrot
		poly.SetPosition(myInput.GetMouseY(), myInput.GetMouseX());


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

	myWindow.Close(); //Not strictly needed; myWindow will go out of scope.
}


#ifndef IS_BUILDING_LIBRARY

int main(int argc, char** argv)
{
	run_main_loop();
	return 0;
}

#endif //IS_BUILDING_LIBRARY


