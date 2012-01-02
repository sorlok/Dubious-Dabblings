#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "gamedata/GameMap.hpp"

#include <list>
#include <iostream>
#include <sstream>
#include <string>

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
DLLEXPORT void clear_display();
DLLEXPORT void demo_display();
DLLEXPORT void sfml_display();
DLLEXPORT void close_sfml();

//Post FX (todo: update to shaders)
DLLEXPORT int can_use_postfx();
//DLLEXPORT sf::PostFX* new_postfx(const char* filename);
//DLLEXPORT void demo_set_default_postfx(sf::PostFX* item);
//DLLEXPORT void demo_update_postfx_color(sf::PostFX* item);

//Demo
DLLEXPORT sf::Shape* demo_init_poly();

//Texture
DLLEXPORT sf::Texture* new_texture(const char* filename);
DLLEXPORT sf::RenderTexture* new_render_texture(int width, int height, sf::Color* bkgrdColor);
DLLEXPORT int texture_get_width(const sf::Texture* item);
DLLEXPORT int texture_get_height(const sf::Texture* item);
DLLEXPORT void texture_set_smooth(sf::Texture* item, int smooth);
//DLLEXPORT sf::Color* image_get_pixel(const sf::Texture* item, int x, int y);
//DLLEXPORT void image_set_pixel(sf::Texture* item, int x, int y, sf::Color* color);

//Sprite
DLLEXPORT sf::Sprite* new_sprite();
DLLEXPORT void sprite_set_image(sf::Sprite* item, const sf::Texture* img);
DLLEXPORT void sprite_set_blend_off(sf::Sprite* item);
DLLEXPORT void sprite_set_sub_rect(sf::Sprite* item, int x, int y, int width, int height);
DLLEXPORT void sprite_set_position(sf::Sprite* item, int x, int y);
DLLEXPORT void sprite_set_center(sf::Sprite* item, int x, int y);
DLLEXPORT float sprite_get_rotation(const sf::Sprite* item);
DLLEXPORT void sprite_set_rotation(sf::Sprite* item, float angle);
DLLEXPORT sf::Color* sprite_get_color(const sf::Sprite* item);
DLLEXPORT void sprite_set_color(sf::Sprite* item, sf::Color* color);
DLLEXPORT int sprite_get_sub_rect_x(const sf::Sprite* item);
DLLEXPORT int sprite_get_sub_rect_y(const sf::Sprite* item);

//Polygon
DLLEXPORT sf::Color* poly_get_point_color(const sf::Shape* item, int pointID);
DLLEXPORT void poly_set_point_color(sf::Shape* item, int pointID, const sf::Color* color);
DLLEXPORT void poly_set_scale_x(sf::Shape* item, float scale);
DLLEXPORT void poly_set_scale_y(sf::Shape* item, float scale);
DLLEXPORT void poly_set_pos_x(sf::Shape* item, int pos);
DLLEXPORT void poly_set_pos_y(sf::Shape* item, int pos);

//Color
DLLEXPORT sf::Color* new_color(int red, int green, int blue);
DLLEXPORT void color_set_red(sf::Color* item, int red);
DLLEXPORT void color_set_green(sf::Color* item, int green);
DLLEXPORT void color_set_blue(sf::Color* item, int blue);
DLLEXPORT void color_set_alpha(sf::Color* item, int alpha);

//GameMap
DLLEXPORT GameMap* new_tmx_map(const char* filename);
DLLEXPORT int gamemap_get_tile_size(GameMap* item);
DLLEXPORT int gamemap_get_map_width(GameMap* item);
DLLEXPORT int gamemap_get_map_height(GameMap* item);
DLLEXPORT const char* gamemap_get_tile_palette_path(GameMap* item);
DLLEXPORT int gamemap_get_tile_at(GameMap* item, int x, int y);

//Game functionality
//DLLEXPORT const sf::Mouse* game_get_input();
DLLEXPORT int game_get_mouse_x();
DLLEXPORT int game_get_mouse_y();
DLLEXPORT int game_get_width();
DLLEXPORT int game_get_height();
DLLEXPORT float game_get_frame_time_s();
DLLEXPORT sf::RenderWindow* game_get_window();
DLLEXPORT void render_target_draw_item(sf::RenderTarget* canvas, sf::Drawable* item);
DLLEXPORT void render_texture_flush(sf::RenderTexture* canvas);
DLLEXPORT sf::Texture* render_texture_get_texture(sf::RenderTexture* canvas);

//Cleanup
DLLEXPORT void game_del_rend_texture(sf::RenderTexture* item);
DLLEXPORT void game_del_item(sf::Drawable* item);
DLLEXPORT void game_del_color(sf::Color* color);
DLLEXPORT void game_del_texture(sf::Texture* item);
DLLEXPORT void game_del_gamemap(GameMap* item);



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


