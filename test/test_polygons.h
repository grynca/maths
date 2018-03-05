#ifndef TEST_POLYGONS_H
#define TEST_POLYGONS_H

#include "maths.h"

//#define CONSTANT_DT 1.0f/60

Vec2 points1[] = {
    {296, 403}, {435, 162}, {564, 486}, {457, 328}, {388, 312}
};
Vec2 points2[] = {
    {596, 454}, {458, 352}, {316, 440}, {461, 116}, {568, 272}, {483, 192}
};
Vec2 points3[] = {
    {232, 506}, {237, 195}, {514, 171}, {568, 274}, {615, 171}, {714, 162}, {731, 239}, {849, 264},
    {814, 531}, {814, 531}, {688, 527}, {652, 460}, {560, 511}, {518, 445}, {380, 507}
};
Vec2 points4[] = {
    {372, 249}, {318, 141}, {414, 71}, {492, 39}, {642, 224}, {663, 45}, {878, 222}, {714, 375},
    {978, 348}, {1020, 539}, {943, 690}, {524, 375}, {420, 570}, {570, 713}, {162, 461}
};

class TestPolygonsFixture : public SDLTest {
public:
    bool mouse_down;
    i32 drag_pid;
    u32 hl_pgon;
    u32 hl_pid;

    fast_vector<Pgon> pgons;
    PgonModifier pgons_mod;

    void init(Config::ConfigSectionMap& cfg) {
        mouse_down = false;
        drag_pid = -1;
        hl_pgon = hl_pid = 0;

        pgons.clear();
        pgons_mod.setPgons(pgons);

        // add random polygon
        pgons.push_back();
        for (u32 i=0; i<10; ++i) {
            Vec2 v(randFloat(50, 900), randFloat(50, 700));
            pgons[0].addPoint(v);
        }

        std::cout << "Controls: number keys: change preconstructed polygon" << std::endl
                  << "          g: generate new polygon" << std::endl
                  << "          s: split polygon to simple polygons" << std::endl
                  << "          c: toggle clockwise/counter clockwise" << std::endl
                  << "          h: polygon halving" << std::endl
                  << "          d: decompose to convex polygons" << std::endl;
    }

    void close() {
    }

    void handleEvent(SDL_Event& evt) {
        switch (evt.type) {
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
                        pgons_mod.simplify(hl_pgon);
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
                        pgons_mod.convexize(hl_pgon);
                        std::cout << " Created " << pgons.size()-prev_size << " new polygons. " << std::endl;
                        hl_pid = 0;
                    }break;
                    case (SDLK_h): {
                        if (!pgons[hl_pgon].isClockwise()) {
                            std::cout << "polygon must be clockwise." << std::endl;
                            break;
                        }
                        if (!pgons[hl_pgon].isSimple()) {
                            std::cout << "polygon must be simple." << std::endl;
                            break;
                        }
                        BlockMeasure m("polygon halving.");
                        pgons_mod.half(hl_pgon);
                        std::cout << " Created " << 1 << " new polygons. " << std::endl;
                        hl_pid = 0;
                    }break;
                    case (SDLK_g): {
                        pgons.clear();
                        pgons.push_back();

                        for (u32 i=0; i<10; ++i) {
                            Vec2 v(randFloat(50, 900), randFloat(50, 700));
                            pgons[0].addPoint(v);
                        }
                        hl_pgon = hl_pid = 0;
                    }break;
                    case (SDLK_1): {
                        pgons.clear();
                        pgons.push_back();
                        for (u32 i=0; i<ARRAY_SIZE(points1); ++i) {
                            pgons[0].addPoint(points1[i]);
                        }
                        hl_pgon = hl_pid = 0;
                    }break;
                    case (SDLK_2): {
                        pgons.clear();
                        pgons.push_back();
                        for (u32 i=0; i<ARRAY_SIZE(points2); ++i) {
                            pgons[0].addPoint(points2[i]);
                        }
                        hl_pgon = hl_pid = 0;
                    }break;
                    case (SDLK_3): {
                        pgons.clear();
                        pgons.push_back();
                        for (u32 i=0; i<ARRAY_SIZE(points3); ++i) {
                            pgons[0].addPoint(points3[i]);
                        }
                        hl_pgon = hl_pid = 0;
                    }break;
                    case (SDLK_4): {
                        pgons.clear();
                        pgons.push_back();
                        for (u32 i=0; i<ARRAY_SIZE(points4); ++i) {
                            pgons[0].addPoint(points4[i]);
                        };
                        hl_pgon = hl_pid = 0;
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
                    pgons[hl_pgon].accPoint(drag_pid).setX(evt.button.x);
                    pgons[hl_pgon].accPoint(drag_pid).setY(evt.button.y);
                }
            }break;
        }
    }

    void update(SDL_Renderer* r, f32 dt) {
        PROFILE_BLOCK("update");

        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderClear(r);

        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        for (u32 i=0; i<pgons.size(); ++i) {
            Pgon& p = pgons[i];
            for (u32 j=0; j<p.getSize(); ++j) {
                const Vec2& p1 = p.getPoint(j);
                const Vec2& p2 = p.getPoint((j+1)%p.getSize());
                SDL_RenderDrawLine(r, (int)p1.getX(), (int)p1.getY(), (int)p2.getX(), (int)p2.getY());
            }
        }

        SDL_SetRenderDrawColor(r, 0, 0, 255, 255);
        for (u32 i=0; i<pgons[hl_pgon].getSize(); ++i) {
            DebugDraw::drawSquareAt<4>(r, pgons[hl_pgon].getPoint(i).getX(), pgons[hl_pgon].getPoint(i).getY());
        }

        SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
        DebugDraw::drawSquareAt<4>(r, pgons[hl_pgon].getPoint(hl_pid).getX(), pgons[hl_pgon].getPoint(hl_pid).getY());
    }
};

#endif //TEST_POLYGONS_H
