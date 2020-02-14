#include <iostream>
#include <cinttypes>
#include <random>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>

constexpr int16_t n(const int16_t &r)
{
    int16_t n = 0;
    for (int i = -r; i <= r; i++)
    {
        int16_t dx = sqrt(r * r - i * i);
        for (int j = -dx; j <= dx; j++)
        {
            if ((!(i == 0) != !(j == 0)) || (i != 0) || (j != 0))
            {
                n += 1/abs((i!=0?i:1)*(j!=0?j:1));
            }
        }
    }
    return n;
}

class Field
{
public:
    constexpr static int16_t mMax = 255;
    constexpr static int mDim = 600;
    constexpr static int mFactor = 1;
    constexpr static int16_t r = 3;
    constexpr static int16_t ar = n(r);
    int16_t mField[mDim][mDim];

    int16_t av(const uint32_t x, const uint16_t y)
    {
        int16_t n = 0;
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
                        int16_t curr = abs((i!=0?i:1)*(j!=0?j:1));
                        n += 1/curr;
                        s += mField[xP][yP]/curr;
                    }
                }
            }
        }
        return s/n;
    }

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

    int16_t value(const uint16_t x, const uint16_t y)
    {
        return mField[x][y];
    }

public:
    Field()
    {
        init();
    }

    void calculateOne(const uint16_t x, const uint16_t y)
    {
        int16_t v = value(x,y);

        v  += (av(x,y) - value(x,y));

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
    Field f;
    int w	= f.mDim*f.mFactor;
    int h	= f.mDim*f.mFactor;

    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    int ms			= 1000;
    int quit		= -1;

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
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
    return 0;
};


