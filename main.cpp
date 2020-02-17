#include <iostream>
#include <cinttypes>
#include <random>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>


class Field
{
public:
    constexpr static int16_t mMax = 255;    // the maximum value a cell can have
    constexpr static int mDim = 600;        // The dimension of the playing field
    constexpr static int mFactor = 1;       // the display factor (mFactor*mDim is the resolution of the window=)
    constexpr static int16_t r = 3;         // effective radius which influences the current cell
    constexpr static double mFac = 0.5;     // factor which influences how much a field follows the average around it
    int16_t mField[mDim][mDim];             // the playing field

    // calculates the average number around the current cell
    int16_t av(const uint32_t x, const uint16_t y)  
    {
        int16_t n = 0;      // number of cells counted
        int16_t s = 0;      // sum of cell values
        for (int i = -r; i <= r; i++)
        {
            int16_t xP = x+i;
            if (xP >= 0 && xP < mDim)
            {
                int16_t dx = sqrt(r * r - i * i);
                for (int j = -dx; j <= dx; j++)
                {
                    int16_t yP = y+j;
                    if (yP >= 0 && yP < mDim && ((!(i == 0) != !(j == 0)) || (i != 0) || (j != 0)))
                    {
                        int16_t curr = abs((i!=0?i:1)*(j!=0?j:1));
                        n += 1/curr;
                        s += mField[xP][yP]/curr;
                    }
                }
            }
        }
        return s/n;
    }

    // the sum of values around the current cell
    int16_t sum(const uint32_t x, const uint16_t y)
    {
        int16_t s = 0;
        for (int i = -r; i <= r; i++)
        {
            int16_t xP = x+i;
            if (xP >= 0 && xP < mDim)
            {
                int16_t dx = sqrt(r * r - i * i);
                for (int j = -dx; j <= dx; j++)
                {
                    int16_t yP = y+j;
                    if (yP >= 0 && yP < mDim && ((!(i == 0) != !(j == 0)) || (i != 0) || (j != 0)))
                    {
                        s += mField[xP][yP]/abs((i!=0?i:1)*(j!=0?j:1));
                    }
                }
            }
        }
        return s;
    }

    // returns the value of the current cell
    int16_t value(const uint16_t x, const uint16_t y)
    {
        return mField[x][y];
    }

public:
    Field()
    {
        init();
    }

    // calculates the next value for the current cell.
    // the cells value follows the average around it with a factor
    void calculateOne(const uint16_t x, const uint16_t y)
    {
        int16_t v = value(x,y);

        v  += int16_t(double(av(x,y) - value(x,y)) * mFac);

        if (v > mMax)
        {
            v = mMax;
        }
        if (v < -mMax)
        {
            v = -mMax;
        }

        mField[x][y] = v;
    }
    
    void calculateAll()
    {
        for (int i = 0; i < mDim; i++)
        {
            for (int j = 0; j < mDim; j++)
            {
                calculateOne(i,j);
            }
        }
    }

    // initialises the field, at this time that's random, but that choice is arbitrary
    void init()
    {
        srand(time(NULL));
        for (int i = 0; i < mDim; i++)
        {
            for (int j = 0; j < mDim; j++)
            {
                int16_t in = rand()%255;
                int16_t p = rand()%2;
                if (p%2 == 0)
                {
                    in *= -1;
                }
                mField[i][j] = in;
            }
        }
    }

    // Draws one cell onto the window
    void draw(SDL_Renderer *renderer,  uint16_t x, uint16_t y)
    {
        int16_t v =  value(x,y);
        uint16_t r = 0;
        uint16_t g = 0;
        uint16_t b = 0;
        if (v < 0)
        {
            r = -v;
        }
        else
        {
            b = v;
        }

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        if (mFactor == 1)
        {
            SDL_RenderDrawPoint(renderer, x, y);
        }
        else
        {
            SDL_Rect pos;
            pos.x = x*mFactor;
            pos.y = y*mFactor;
            pos.w = mFactor;
            pos.h = mFactor;
            SDL_RenderFillRect(renderer, &pos);
        }
    }

    // Draws the entire field
    void draw(SDL_Renderer *renderer)
    {
        for (int i = 0; i < mDim; i++)
        {
            for (int j = 0; j < mDim; j++)
            {
                draw(renderer, i, j);
            }
        }
    }

    // prints the numerical values of the field
    void print()
    {
        for (int i = 0; i < mDim; i++)
        {
            for (int j = 0; j < mDim; j++)
            {
                if (abs(mField[i][j]) < 10)
                {
                    std::cout<<" ";
                }
                if (mField[i][j] >= 0)
                {
                    std::cout<<" ";
                }
                std::cout<<std::to_string(mField[i][j])<<" ";
            }
            std::cout<<"\n";
        }
        std::cout<<"\n";
    }
};

int main()
{
    // +++ initialise playing field
    Field f;
    int w	= f.mDim*f.mFactor;
    int h	= f.mDim*f.mFactor;
    // --- initialise playing field

    // +++ initialise SDL2 Window
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    // --- initialise SDL2 Window


    int ms			= 1000; // duration to wait in each loop.
    int quit		= -1;   // set to a positive number in order to quit

	while (true)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        f.draw(renderer);
        f.calculateAll();

        SDL_RenderPresent(renderer);
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_q)
                {
                    quit = 1;
                }
                else if (event.key.keysym.sym == SDLK_UP)
                {
                    ms += 50;
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {
                    ms -= 50;
                }
                break;
            case SDL_QUIT:
                quit = 1;
                break;
            default:
                break;
            }
        }
        if (quit >= 0)
        {
            break;
        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
//        The delay is disabled since my computer is a bit slow, so that's not necessary.
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
    return 0;
}


