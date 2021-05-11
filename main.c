#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "sdl.h"
#include <unistd.h>
#include <string.h>

typedef struct {
   SDL_Point *parts;  // pole souradnic aktivnich clanku hada
   int tail;          // index souradnice ocasu v poli parts
   int head;          // index souradnice hlavy v poli parts
   SDL_Point direction;                   // point direction
} Snake;

typedef struct Game
{
    int rows;
    int cols;
    int tileSize;
    Snake snake;
    SDL_Point food;         // yummy
} Game;



int incIndex(Game *game, int i)
{
    return (i+1) % (game->rows * game->cols);
}

void spawnFood(Game *game)
{
    game->food.x = rand() % ((game->cols - 3) - 2 + 1) + 2;
    game->food.y = rand() % ((game->rows - 3) - 2 + 1) + 2;
    // rand() % (max - min + 1) + min;
}

void renderSnake(SDL_Context *ctx, SDL_Texture *image, SDL_Rect *r, int x, int y)
{
    SDL_Rect r_image;
    r_image.w = r_image.h = 64;
    r_image.y = y * r_image.w;
    r_image.x = x * r_image.w;
    SDL_RenderCopy(ctx->renderer, image, &r_image, r);
}

int main(int argc, char **argv) {
  int width = 800;
  int height = 600;
  int score = 0;
  srand(time(NULL));

  Game game = {0}; // = memset(&game, 0, sizeof(Game));
  game.tileSize = 32;
  game.cols = width / game.tileSize;
  game.rows = height / game.tileSize;
  game.snake.parts = malloc(sizeof(SDL_Point) * game.rows * game.cols);

  game.snake.direction.x = 1;
  game.snake.direction.y = 0;
  game.snake.tail = 0;
  game.snake.parts[0].y = game.rows / 2;
  game.snake.parts[0].x = game.cols / 2;
  game.snake.head++;
  game.snake.parts[1].y = game.rows / 2;
  game.snake.parts[1].x = game.cols / 2 + 1;
  game.snake.head++;

  spawnFood(&game);

  if(argc >= 2) {
    if(sscanf(argv[1], "%dx%d", &width, &height) != 2) {
      fprintf(stderr, "Usage: %s WxH\n", argv[0]);
      exit(1);
    }
  }

  // initialize SDL and create window (for details see sdl.h/sdl.c)
  SDL_Context *ctx = sdl_context_new("game", width, height);
  SDL_SetWindowSize(ctx->window, game.tileSize*game.cols + 1, game.tileSize*game.rows+1);

  SDL_Texture *image = IMG_LoadTexture(ctx->renderer, "resources/snake-sprite.png");
  SDL_Texture *image2 = IMG_LoadTexture(ctx->renderer, "resources/snake.png");
  if (image == NULL) {
    fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
    return 1;
  }
  
  bool quit = false;
  Uint64 prevCounter = SDL_GetPerformanceCounter();
  double remaining = 0;
  while (!quit) {
    Uint64 now = SDL_GetPerformanceCounter();
    double elapsed_ms = (now - prevCounter) * 1000 / (double) SDL_GetPerformanceFrequency();
    prevCounter = now;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
        break;
      }
      else if (e.type == SDL_KEYDOWN) {
        // key is pressed DOWN
        switch (e.key.keysym.sym) {
          case SDLK_ESCAPE:
            quit = true;
            break;
          case SDLK_q:
            quit = true;
            break;
          case SDLK_UP:
            game.snake.direction.x = 0;
            game.snake.direction.y = -1;
            break;
          case SDLK_DOWN:
            game.snake.direction.x = 0;
            game.snake.direction.y = 1;
            break;
          case SDLK_RIGHT:
            game.snake.direction.x = 1;
            game.snake.direction.y = 0;
            break;
          case SDLK_LEFT:
            game.snake.direction.x = -1;
            game.snake.direction.y = 0;
            break;
        }
      } else if(e.type == SDL_MOUSEMOTION) {
        // mouse move
        //printf("mouse x=%d y=%d\n", e.motion.x, e.motion.y);
      } else if(e.type == SDL_MOUSEBUTTONUP) {
        // e.button.button: SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT
        //printf("mouse x=%d y=%d button=%d\n", e.button.x, e.button.y, e.button.button);
      }
    }
    


    // clear the buffer
    // SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_SetRenderDrawColor(ctx->renderer, 79,162,76, 255);
    SDL_RenderClear(ctx->renderer);
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 150);
    // int offsetX = game->cols;
    // int offsetY = game->rows;
    for (int i = 2; i < game.cols - 1; i++)
    {
        SDL_RenderDrawLine(ctx->renderer, i*game.tileSize, 2*game.tileSize, i*game.tileSize, 513);
    }
    
    for (int i = 2; i < game.rows - 1; i++)
    {
        SDL_RenderDrawLine(ctx->renderer, 2*game.tileSize, i*game.tileSize, (width - 2*game.tileSize), i*game.tileSize);
    }
    
    // had

    if (remaining < 0)
    {
      int idx = game.snake.head - 1;
      if (idx < 0)
      {
        idx = game.rows*game.cols - 1;
      }
      SDL_Point newHead = game.snake.parts[idx];
      newHead.x += game.snake.direction.x;
      newHead.y += game.snake.direction.y;
      game.snake.parts[game.snake.head] = newHead;
      //printf("hlava: %d  ocas: %d\n", game.snake.head, game.snake.tail);
      if (game.snake.parts[game.snake.head].x == game.food.x && game.snake.parts[game.snake.head].y == game.food.y)
      {
          printf("kolize");
          spawnFood(&game);
          score++;
      }
      else
      {
          game.snake.tail = incIndex(&game, game.snake.tail);
      }
      
      game.snake.head = incIndex(&game, game.snake.head);
      
      //zkontrolovat head s food - neinktrementovat
      
      
      
      remaining = 125;
    }
    else
    {
      remaining -= elapsed_ms;
    }

    for (int i = game.snake.tail; i < game.snake.head; i++)
    {
        if (game.snake.parts[i].x > (game.cols - 3) || game.snake.parts[i].x < 2)
        {
            quit = true;
        }
        else if (game.snake.parts[i].y > (game.rows - 3) || game.snake.parts[i].y < 2)
        {
            quit = true;
        }
        else
        {
          for (int x = i+1; x < game.snake.head; x++)
          {
              if (game.snake.parts[i].x == game.snake.parts[x].x && game.snake.parts[i].y == game.snake.parts[x].y)
              {
                quit = true;
              }
          }
        }
    }
    //printf("\nhlava x = %i      hlava y = %i\nfood x = %i      food y = %i", game.snake.parts[game.snake.tail].x, game.snake.parts[game.snake.tail].y, game.food.x, game.food.y);
    int i = game.snake.tail;
    while (i != game.snake.head)
    {      
      SDL_SetRenderDrawColor(ctx->renderer, 70,154,55, 0);
      SDL_Rect r;
      r.w = r.h = game.tileSize;
      r.x = game.snake.parts[i].x * game.tileSize;
      r.y = game.snake.parts[i].y * game.tileSize;
      SDL_RenderFillRect(ctx->renderer, &r);
      printf("\ni = %d\nhead = %d\ntail = %d", i, game.snake.head, game.snake.tail);
      
      // zde jsou podminky pro urceni textur hlavy Snake
      
      if (game.snake.direction.x == 0 && game.snake.direction.y == -1 && i == (game.snake.head - 1))
      {
          renderSnake(ctx, image, &r, 3, 0);
      }
      else if (game.snake.direction.x == 0 && game.snake.direction.y == 1 && i == (game.snake.head - 1))
      {
          renderSnake(ctx, image, &r, 4, 1);
      }
      else if (game.snake.direction.x == 1 && game.snake.direction.y == 0 && i == (game.snake.head - 1))
      {
          renderSnake(ctx, image, &r, 4, 0);
      }
      else if (game.snake.direction.x == -1 && game.snake.direction.y == 0 && i == (game.snake.head - 1))
      {
          renderSnake(ctx, image, &r, 3, 1);
      }
      
      // zde jsou podminky pro urceni textur ocasu Snake

      if (game.snake.parts[game.snake.tail].x == game.snake.parts[game.snake.tail + 1].x && game.snake.parts[game.snake.tail].y < game.snake.parts[game.snake.tail + 1].y && i == (game.snake.tail))
      {
          renderSnake(ctx, image, &r, 4, 3);
      }
      else if (game.snake.parts[game.snake.tail].x == game.snake.parts[game.snake.tail + 1].x && game.snake.parts[game.snake.tail].y > game.snake.parts[game.snake.tail + 1].y && i == (game.snake.tail))
      {
          renderSnake(ctx, image, &r, 3, 2);
      }
      else if (game.snake.parts[game.snake.tail].x < game.snake.parts[game.snake.tail + 1].x && game.snake.parts[game.snake.tail].y == game.snake.parts[game.snake.tail + 1].y && i == (game.snake.tail))
      {
          renderSnake(ctx, image, &r, 4, 2);
      }
      else if (game.snake.parts[game.snake.tail].x > game.snake.parts[game.snake.tail + 1].x && game.snake.parts[game.snake.tail].y == game.snake.parts[game.snake.tail + 1].y && i == (game.snake.tail))
      {
          renderSnake(ctx, image, &r, 3, 3);
      }

      // zde je vykresleni tela hada

      if (i != game.snake.tail && i != (game.snake.head - 1))
      {
          if (game.snake.parts[i].x == game.snake.parts[i+1].x && game.snake.parts[i].y < game.snake.parts[i+1].y && game.snake.parts[i-1].x < game.snake.parts[i].x && game.snake.parts[i-1].y == game.snake.parts[i].y)
          {
              renderSnake(ctx, image, &r, 2, 0);
          }
          else if (game.snake.parts[i].x > game.snake.parts[i+1].x && game.snake.parts[i].y == game.snake.parts[i+1].y && game.snake.parts[i-1].x == game.snake.parts[i].x && game.snake.parts[i-1].y > game.snake.parts[i].y)
          {
              // obracene
              renderSnake(ctx, image, &r, 2, 0);
          }
          else if (game.snake.parts[i].x < game.snake.parts[i+1].x && game.snake.parts[i].y == game.snake.parts[i+1].y && game.snake.parts[i-1].x == game.snake.parts[i].x && game.snake.parts[i-1].y > game.snake.parts[i].y)
          {
              renderSnake(ctx, image, &r, 0, 0);
          }
          else if (game.snake.parts[i].x == game.snake.parts[i+1].x && game.snake.parts[i].y < game.snake.parts[i+1].y && game.snake.parts[i-1].x > game.snake.parts[i].x && game.snake.parts[i-1].y == game.snake.parts[i].y)
          {
              // obracene
              renderSnake(ctx, image, &r, 0, 0);
          }
          else if (game.snake.parts[i].x < game.snake.parts[i+1].x && game.snake.parts[i].y == game.snake.parts[i+1].y && game.snake.parts[i-1].x == game.snake.parts[i].x && game.snake.parts[i-1].y < game.snake.parts[i].y)
          {
              renderSnake(ctx, image, &r, 0, 1);
          }
          else if (game.snake.parts[i].x == game.snake.parts[i+1].x && game.snake.parts[i].y > game.snake.parts[i+1].y && game.snake.parts[i-1].x > game.snake.parts[i].x && game.snake.parts[i-1].y == game.snake.parts[i].y)
          {
              // test
              renderSnake(ctx, image, &r, 0, 1);
          }
          else if (game.snake.parts[i].x > game.snake.parts[i+1].x && game.snake.parts[i].y == game.snake.parts[i+1].y && game.snake.parts[i-1].x == game.snake.parts[i].x && game.snake.parts[i-1].y < game.snake.parts[i].y)
          {
              renderSnake(ctx, image, &r, 2, 2);
          }
          else if (game.snake.parts[i].x == game.snake.parts[i+1].x && game.snake.parts[i].y > game.snake.parts[i+1].y && game.snake.parts[i-1].x < game.snake.parts[i].x && game.snake.parts[i-1].y == game.snake.parts[i].y)
          {
              // obracene
              renderSnake(ctx, image, &r, 2, 2);
          }
          else if (game.snake.parts[i].y == game.snake.parts[i+1].y && game.snake.parts[i-1].y == game.snake.parts[i].y)
          {
              renderSnake(ctx, image, &r, 1, 0);
          }
          else if (game.snake.parts[i].x == game.snake.parts[i+1].x && game.snake.parts[i-1].x == game.snake.parts[i].x)
          {
              renderSnake(ctx, image, &r, 2, 1);
          }
      }


      SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 0);
      SDL_Rect food;
      food.w = food.h = game.tileSize;
      food.x = game.food.x * game.tileSize;
      food.y = game.food.y * game.tileSize;
      // printf("%d %d\n", game.food.x, game.food.y);
      SDL_RenderFillRect(ctx->renderer, &food);

      SDL_Rect src;
      src.w = src.h = 64;
      src.y = 3 * src.w;
      src.x = 0;
      SDL_RenderCopy(ctx->renderer, image, &src, &food);

      i = incIndex(&game, i);

      if (i >= (game.cols * game.rows))
      {
        i = 0;
      }
    }
    
    // your code here
    // show the buffer

    // game end - displaying final text with score and GG

    if (quit == true)
    {
        // itoa(score, scoreNum, 16); - unsupported by GCC I suppose, cannot compile
        char i[128];
        // itoa(i, text1+score, 10);
        snprintf(i, 25, "%s %d", "Game over! Score:", score);
        SDL_RenderClear(ctx->renderer);
        TTF_Font* Sans = TTF_OpenFont("DejaVuSansMono.ttf", 75);
        SDL_Color White = {255, 255, 255, 255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, i, White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
        SDL_Texture* Message = SDL_CreateTextureFromSurface(ctx->renderer, surfaceMessage); //now you can convert it into a texture
        SDL_Rect Message_rect; //create a rect
        Message_rect.w = 525; // controls the width of the rect
        Message_rect.h = 50; // controls the height of the rect
        Message_rect.x = width / 2 - Message_rect.w / 2;  //controls the rect's x coordinate 
        Message_rect.y = height / 4 - Message_rect.h / 2; // controls the rect's y coordinte
        SDL_RenderCopy(ctx->renderer, Message, NULL, &Message_rect);
        SDL_Rect r;
        r.w = r.h = 200;
        r.x = 300;
        r.y = 265;
        SDL_Rect r_image;
        r_image.w = r_image.h = 512;
        r_image.y = 0;
        r_image.x = 0;
        SDL_RenderFillRect(ctx->renderer, &r);
        SDL_RenderCopy(ctx->renderer, image2, &r_image, &r);
        SDL_RenderPresent(ctx->renderer);
        usleep(3000000);

    }

    SDL_RenderPresent(ctx->renderer);
  }


  // cleanup all resources
  SDL_DestroyTexture(image);
  sdl_context_delete(ctx);
  return 0;
}
