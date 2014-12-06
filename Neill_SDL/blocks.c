#include <stdio.h>
#include <stdlib.h>
#include "neillsdl2.h"

#define RECTSIZE 20
#define MILLISECONDDELAY 50

int main(void)
{

   SDL_Simplewin sw;
   SDL_Rect rectangle;
   rectangle.w = RECTSIZE;
   rectangle.h = RECTSIZE;

   Neill_SDL_Init(&sw);

   do{

      // Sleep for a short time
      SDL_Delay(MILLISECONDDELAY);

      // Choose a random colour, a mixture of red, green and blue.
      Neill_SDL_SetDrawColour(&sw,
                             rand()%SDL_8BITCOLOUR, rand()%SDL_8BITCOLOUR,
                             rand()%SDL_8BITCOLOUR);

      // Filled Rectangle, fixed size, random position
      rectangle.x = rand()%(WWIDTH-RECTSIZE);
      rectangle.y = rand()%(WHEIGHT-RECTSIZE);
      SDL_RenderFillRect(sw.renderer, &rectangle);

      // Unfilled Rectangle, fixed size, random position
      rectangle.x = rand()%(WWIDTH-RECTSIZE);
      rectangle.y = rand()%(WHEIGHT-RECTSIZE);
      SDL_RenderDrawRect(sw.renderer, &rectangle);

      // Update window - no graphics appear on some devices until this is finished
      SDL_RenderPresent(sw.renderer);
      SDL_UpdateWindowSurface(sw.win); 
      
      // Has anyone pressed ESC or killed the SDL window ?
      // Must be called frequently - it's the only way of escaping 
      Neill_SDL_Events(&sw);

   }while(!sw.finished);


   // Clear up graphics subsystems
   atexit(SDL_Quit);

   return 0;

}
