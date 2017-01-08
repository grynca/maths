#ifndef TEST_POLYGONS_H
#define TEST_POLYGONS_H
#ifdef VISUAL_UPDATE

#include "SDL2/SDL.h"
#define WIDTH 1024
#define HEIGHT 768
//#define CONSTANT_DT 1.0f/60

#ifdef WEB
#include <emscripten/emscripten.h>
    static void dispatch_main(void* fp) {
        std::function<void()>* func = (std::function<void()>*)fp;
        (*func)();
    }
#endif

static inline void drawSmallRect(SDL_Renderer *r, f32 x, f32 y) {
    SDL_Rect rect;
    rect.x = (int)roundf(x)-2;
    rect.y = (int)roundf(y)-2;
    rect.w = 4;
    rect.h = 4;
    SDL_RenderDrawRect(r, &rect);
}

static inline void startPolygonsLoop() {
    srand(time(NULL));
    SDL_Window *win = SDL_CreateWindow("Polygons playground", 100, 100, WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // set vsync
    SDL_GL_SetSwapInterval(1);

    // main loop
    bool exit = false;
    bool mouse_down = false;
    int drag_pid = -1;
    u32 hl_pgon = 0;
    u32 hl_pid = 0;
    u32 frame_id = 0;

    fast_vector<Pgon> pgons;
    PgonModifier mod(pgons);
    pgons.push_back();
    for (u32 i=0; i<10; ++i) {
        Vec2 v(randFloatMinMax(50, 900), randFloatMinMax(50, 700));
        pgons[0].addPoint(v);
    }

    std::cout << "Controls: number keys: change preconstructed polygon" << std::endl
              << "          g: generate new polygon" << std::endl
              << "          s: split polygon to simple polygons" << std::endl
              << "          c: toggle clockwise/counter clockwise" << std::endl
              << "          h: polygon halving" << std::endl
              << "          d: decompose to convex polygons" << std::endl;

    std::function<void()> main_loop = [&]() {
        SDL_Event evt;

        while ( SDL_PollEvent(&evt) ) {
            switch (evt.type) {
                case (SDL_QUIT): {
                    exit = true;
                }break;
                case (SDL_KEYDOWN): {
                    switch (evt.key.keysym.sym) {
                        case (SDLK_c): {
                            if (pgons[hl_pgon].isClockwise()) {
                                std::cout << "already clockwise." << std::endl;
                            }
                            else {
                                std::cout << "reversing to clockwise." << std::endl;
                                pgons[hl_pgon].reverse();
                            }
                        }break;
                        case (SDLK_s): {
                            BlockMeasure m("polygon split");
                            u32 prev_size = pgons.size();
                            mod.simplify(hl_pgon);
                            std::cout << " Created " << pgons.size()-prev_size << " new polygons. " << std::endl;
                            hl_pid = 0;
                        }break;
                        case (SDLK_d): {
                            if (!pgons[hl_pgon].isSimple()) {
                                std::cout << "polygon must be simple." << std::endl;
                                break;
                            }
                            if (!pgons[hl_pgon].isClockwise()) {
                                std::cout << "polygon must be clockwise." << std::endl;
                                break;
                            }
                            BlockMeasure m("polygon decompose to convex.");
                            u32 prev_size = pgons.size();
                            mod.convexize(hl_pgon);
                            std::cout << " Created " << pgons.size()-prev_size << " new polygons. " << std::endl;
                            hl_pid = 0;
                        }break;
                        case (SDLK_h): {
                            if (!pgons[hl_pgon].isClockwise()) {
                                std::cout << "polygon must be clockwise." << std::endl;
                                break;
                            }
                            BlockMeasure m("polygon halving.");
                            mod.half(hl_pgon);
                            std::cout << " Created " << 1 << " new polygons. " << std::endl;
                            hl_pid = 0;
                        }break;
                        case (SDLK_g): {
                            pgons.clear();
                            pgons.push_back();

                            for (u32 i=0; i<10; ++i) {
                                Vec2 v(randFloatMinMax(50, 900), randFloatMinMax(50, 700));
                                pgons[0].addPoint(v);
                            }
                            hl_pgon = hl_pid = 0;
                        }break;
                        case (SDLK_1): {
                            pgons.clear();
                            pgons.push_back();

                            pgons[0].addPoint({296, 403});
                            pgons[0].addPoint({435, 162});
                            pgons[0].addPoint({564, 486});
                            pgons[0].addPoint({457, 328});
                            pgons[0].addPoint({388, 312});
                            hl_pgon = hl_pid = 0;
                        }break;
                        case (SDLK_2): {
                            pgons.clear();
                            pgons.push_back();

                            pgons[0].addPoint({596, 454});
                            pgons[0].addPoint({458, 352});
                            pgons[0].addPoint({316, 440});
                            pgons[0].addPoint({461, 116});
                            pgons[0].addPoint({568, 272});
                            pgons[0].addPoint({483, 192});
                            hl_pgon = hl_pid = 0;
                        }break;
                        case (SDLK_3): {
                            pgons.clear();
                            pgons.push_back();

                            pgons[0].addPoint({232, 506});
                            pgons[0].addPoint({237, 195});
                            pgons[0].addPoint({514, 171});
                            pgons[0].addPoint({568, 274});
                            pgons[0].addPoint({615, 171});
                            pgons[0].addPoint({714, 162});
                            pgons[0].addPoint({731, 239});
                            pgons[0].addPoint({849, 264});
                            pgons[0].addPoint({814, 531});
                            pgons[0].addPoint({688, 527});
                            pgons[0].addPoint({652, 460});
                            pgons[0].addPoint({560, 511});
                            pgons[0].addPoint({518, 445});
                            pgons[0].addPoint({380, 507});
                            hl_pgon = hl_pid = 0;
                        }break;
                        case (SDLK_4): {
                            pgons.clear();
                            pgons.push_back();

                            pgons[0].addPoint({372, 249});
                            pgons[0].addPoint({318, 141});
                            pgons[0].addPoint({414, 71});
                            pgons[0].addPoint({492, 39});
                            pgons[0].addPoint({642, 224});
                            pgons[0].addPoint({663, 45});
                            pgons[0].addPoint({878, 222});
                            pgons[0].addPoint({714, 375});
                            pgons[0].addPoint({978, 348});
                            pgons[0].addPoint({1020, 539});
                            pgons[0].addPoint({943, 690});
                            pgons[0].addPoint({524, 375});
                            pgons[0].addPoint({420, 570});
                            pgons[0].addPoint({570, 713});
                            pgons[0].addPoint({162, 461});
                            hl_pgon = hl_pid = 0;
                        }break;
                    }
                }break;
                case (SDL_KEYUP): {
                    switch (evt.key.keysym.sym) {
                        case (SDLK_ESCAPE): {
                            exit = true;
                        }break;
                    }
                }break;
                case (SDL_MOUSEBUTTONDOWN): {
                    if (evt.button.button == SDL_BUTTON_LEFT) {
                        Vec2 mouse_pos(evt.button.x, evt.button.y);
                        mouse_down = true;
                        drag_pid = -1;
                        u32 best_pgon = 0;
                        f32 best_d = std::numeric_limits<f32>::max();
                        for (u32 i=0; i<pgons.size(); ++i) {
                            for (u32 j=0; j<pgons[i].getSize(); ++j) {
                                f32 d = (pgons[i].getPoint(j)-mouse_pos).getSqrLen();
                                if (d < best_d || (d == best_d && hl_pgon == i)) {
                                    best_d = d;
                                    drag_pid = j;
                                    best_pgon = i;
                                }
                            }
                        }

                        if (best_d >= 50) {
                            drag_pid = -1;
                        }
                        else {
                            hl_pid = drag_pid;
                            hl_pgon = best_pgon;
                        }
                    }
                }break;
                case (SDL_MOUSEBUTTONUP): {
                    if (evt.button.button == SDL_BUTTON_LEFT && mouse_down) {
                        drag_pid = -1;
                        mouse_down = false;
                    }
                }break;
                case (SDL_MOUSEMOTION): {
                    if (drag_pid>=0) {
                        pgons[hl_pgon].getPoint(drag_pid).setX(evt.button.x);
                        pgons[hl_pgon].getPoint(drag_pid).setY(evt.button.y);
                    }
                }break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (u32 i=0; i<pgons.size(); ++i) {
            Pgon& p = pgons[i];
            for (u32 j=0; j<p.getSize(); ++j) {
                Vec2& p1 = p.getPoint(j);
                Vec2& p2 = p.getPoint((j+1)%p.getSize());
                SDL_RenderDrawLine(renderer, (int)p1.getX(), (int)p1.getY(), (int)p2.getX(), (int)p2.getY());
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (u32 i=0; i<pgons[hl_pgon].getSize(); ++i) {
            drawSmallRect(renderer, pgons[hl_pgon].getPoint(i).getX(), pgons[hl_pgon].getPoint(i).getY());
        }

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        drawSmallRect(renderer, pgons[hl_pgon].getPoint(hl_pid).getX(), pgons[hl_pgon].getPoint(hl_pid).getY());

//        Vec2& sp = pgons[hl_pgon].getPoint(hl_pid);
//        Vec2& p1 = pgons[hl_pgon].getPoint(wrap(hl_pid-1, pgons[hl_pgon].getSize()));
//        Vec2& p2 = pgons[hl_pgon].getPoint(wrap(hl_pid+1, pgons[hl_pgon].getSize()));

        SDL_RenderPresent(renderer);
        ++frame_id;
    };

#ifdef WEB
    emscripten_set_main_loop_arg(dispatch_main, &main_loop,
                                     0 /* use browser's framerate */,
                                     1 /* simulate infinite loop */);
#else
    while (!exit) {
        main_loop();
    }
#endif

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
}


#endif  // VISUAL_UPDATE
#endif //TEST_POLYGONS_H
