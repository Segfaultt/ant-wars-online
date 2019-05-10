#pragma once
#include <string.h>

class texture_wrapper {
    public:
      texture_wrapper();
      ~texture_wrapper();

      // setters
      void set_blending(SDL_BlendMode blending);
      void set_alpha(Uint8 alpha);
      void set_color(Uint8 red, Uint8 green, Uint8 blue);

      // getters
      int get_height();
      int get_width();

      // other functions
      void free();
      bool load_texture(std::string path, SDL_Renderer *renderer);
      bool load_text(std::string text, SDL_Color colour, std::string font,
                     int size, SDL_Renderer *renderer);
      // rendering
      void render(int x, int y, double angle, SDL_Renderer *renderer);
      void render(int x, int y, SDL_Renderer *renderer);
      void render_from_centre(int x, int y, SDL_Renderer *renderer);

    protected:
      int width, height;
      SDL_Texture *texture;
};
