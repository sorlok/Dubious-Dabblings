#include "SFML_Engine.hpp"

//+1 global variables!
sf::RenderWindow myWindow;

//We could even remove this too, but I don't see the need right now.
sf::Text fps;

//This should stay in the engine.
RollingAverage framerate(1000);
sf::Clock frClock;




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
	while (myWindow.PollEvent(event)) {
		doExit = handleEvent(event) || doExit;
	}
	return doExit ? 1 : 0;
}


void clear_display()
{
	myWindow.Clear(sf::Color(0x33, 0x33, 0x33));
}


void demo_display()
{
	if (myWindow.GetFrameTime()>0.0) {
		framerate.addSample(1.0/myWindow.GetFrameTime());
	}

	if (frClock.GetElapsedTime()>0.5) {
		frClock.Reset();
		std::stringstream fpsStr;
		fpsStr <<framerate.getAverage() <<" fps";
		fps.SetString(fpsStr.str());
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
	return sf::Shader::IsAvailable()?1:0;
	//return sf::PostFX::CanUsePostFX()?1:0;
}


/*sf::PostFX* new_postfx(const char* filename)
{
	sf::PostFX* item = new sf::PostFX();
	if (item->LoadFromFile(filename)) {
		return item;
	}

	//Else, cleanup and return null;
	delete item;
	return NULL;
}*/



sf::Texture* new_texture(const char* filename)
{
	sf::Texture* item = new sf::Texture();
	if (item->LoadFromFile(filename)) {
		return item;
	}

	//Else, cleanup and return null;
	delete item;
	return NULL;
}

int image_get_width(const sf::Texture* item)
{
	return item->GetWidth();
}


int image_get_height(const sf::Texture* item)
{
	return item->GetHeight();
}

/*sf::Color* image_get_pixel(const sf::Texture* item, int x, int y)
{
	sf::Color* res = new sf::Color(item->GetPixel(x, y));
	return res;
}

void image_set_pixel(sf::Texture* item, int x, int y, sf::Color* color)
{
	item->SetPixel(x, y, *color);
}*/


void image_set_smooth(sf::Texture* item, int smooth)
{
	item->SetSmooth(smooth==0?false:true);
}


sf::Sprite* new_sprite()
{
	sf::Sprite* item = new sf::Sprite();
	return item;
}

sf::Color* new_color(int red, int green, int blue)
{
	sf::Color* res = new sf::Color(red&0xFF, green&0xFF, blue&0xFF);
	return res;
}

sf::RenderTexture* new_render_texture(int width, int height, sf::Color* bkgrdColor)
{
	sf::RenderTexture* res = new sf::RenderTexture();
	res->Create(width, height);
	res->Clear(*bkgrdColor);
	return res;
}

void sprite_set_image(sf::Sprite* item, const sf::Texture* img)
{
	item->SetTexture(*img);
}

void sprite_set_blend_off(sf::Sprite* item)
{
	//TODO: Re-enable
	//NOTE: This is stored in the RenderStates class, which also applies shaders (which might be
	//      used to re-enable our polygon vertex shading). So it's going to get slightly more complex.
	//item->SetBlendMode(sf::Blend::None);
}

void sprite_set_sub_rect(sf::Sprite* item, int x, int y, int width, int height)
{
	sf::IntRect rect(x, y, x+width, y+height);
	item->SetTextureRect(rect);
}

void sprite_set_position(sf::Sprite* item, int x, int y)
{
	item->SetPosition(x, y);
}

void sprite_set_center(sf::Sprite* item, int x, int y)
{
	item->SetOrigin((float)x, (float)y);
}

int sprite_get_sub_rect_x(const sf::Sprite* item)
{
	return item->GetTextureRect().Left;
}

int sprite_get_sub_rect_y(const sf::Sprite* item)
{
	return item->GetTextureRect().Top;
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
	//TODO: re-enable later
	//sf::Color* res = new sf::Color(item->GetPointColor(pointID));
	sf::Color* res = new sf::Color(0x00, 0x00, 0x00);
	return res;
}

void poly_set_point_color(sf::Shape* item, int pointID, const sf::Color* color)
{
	//TODO: Re-enable later
	//item->SetPointColor(pointID, *color);
}

void poly_set_scale_x(sf::Shape* item, float scale)
{
	item->SetScale(scale, item->GetScale().y);
}

void poly_set_scale_y(sf::Shape* item, float scale)
{
	item->SetScale(item->GetScale().x, scale);
}

void poly_set_pos_x(sf::Shape* item, int pos)
{
	item->SetPosition(pos, item->GetPosition().y);
}

void poly_set_pos_y(sf::Shape* item, int pos)
{
	item->SetPosition(item->GetPosition().x, pos);
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

void game_del_rend_texture(sf::RenderTexture* item)
{
	delete item;
}

void game_del_item(sf::Drawable* item)
{
	delete item;
}

void game_del_color(sf::Color* color)
{
	delete color;
}

void game_del_image(sf::Texture* img)
{
	delete img;
}

//TODO: Perhaps our game items should all have a common parent class to
//      make dispatch easier?
void game_del_gamemap(GameMap* item)
{
	delete item;
}


/*void demo_set_default_postfx(sf::PostFX* item)
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
}*/


GameMap* new_tmx_map(const char* filename)
{
	GameMap* res = new GameMap();
	try {
		GameMap::InitTMXMap(*res, filename);
	} catch (std::exception& ex) {
		std::cout <<"Error loading map: " <<ex.what() <<std::endl;
		delete res;
		return NULL;
	}

	return res;
}

int gamemap_get_tile_size(GameMap* item)
{
	return item->tileSize;
}

int gamemap_get_map_width(GameMap* item)
{
	return item->mapSizeInTiles.first;
}

int gamemap_get_map_height(GameMap* item)
{
	return item->mapSizeInTiles.second;
}

//NOTE: The pointer returned by this is only valid for the lifetime of the
//      GameMap (so wrap it early!). It should NOT be freed.
const char* gamemap_get_tile_palette_path(GameMap* item)
{
	return item->tilePalettePath.c_str();
}

int gamemap_get_tile_at(GameMap* item, int x, int y)
{
	if (x<0 || y<0 || y>=item->maplayer.size() || x>=item->maplayer[0].size()) {
		return -1;
	}

	return item->maplayer[y][x];
}


unsigned int tileSize;
std::string tilePalettePath;

std::vector< std::vector<int> > maplayer;
std::pair<unsigned int, unsigned int> mapSizeInTiles;



//If canvas is null, use the main window.
void render_target_draw_item(sf::RenderTarget* canvas, sf::Drawable* item)
{
	if (canvas) {
		canvas->Draw(*item);
	} else {
		myWindow.Draw(*item);
	}
}

/*const sf::Mouse* game_get_input()
{
	return NULL;
}*/

int game_get_mouse_x()
{
	return sf::Mouse::GetPosition().x;
}

int game_get_mouse_y()
{
	return sf::Mouse::GetPosition().y;
}

int game_get_width()
{
	return myWindow.GetWidth();
}

int game_get_height()
{
	return myWindow.GetHeight();
}


sf::Shape* demo_init_poly()
{
	sf::ConvexShape* poly = new sf::ConvexShape(6);
	poly->SetPoint(0, sf::Vector2f(0, -50));//,  sf::Color(0xFF, 0, 0));
	poly->SetPoint(1, sf::Vector2f(50, 0));//,   sf::Color(0, 0xFF, 0));
	poly->SetPoint(2, sf::Vector2f(50, 50));//,  sf::Color(0, 0, 0xFF));
	poly->SetPoint(3, sf::Vector2f(0, 100));//,  sf::Color(0xFF, 0xFF, 0));
	poly->SetPoint(4, sf::Vector2f(-50, 50));//, sf::Color(0, 0xFF, 0xFF));
	poly->SetPoint(5, sf::Vector2f(-50, 0));//,  sf::Color(0xFF, 0, 0xFF));
	poly->SetPosition(800/2, 600/3);
	poly->SetOutlineColor(sf::Color(0xFF, 0x00, 0x00));  //TEMP: Shade later
	poly->SetFillColor(sf::Color(0xFF,0x00,0xFF));       //TEMP: Shade later
	poly->SetOutlineThickness(1.0F);                     //TEMP: Shade later
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
