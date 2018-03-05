#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include "shapes/Shape.h"
#include "Transform.h"
#include <SDL2/SDL_render.h>

namespace grynca {

    class DebugDraw {
    public:
        struct SDLBlockColorMod {
            SDLBlockColorMod(SDL_Renderer* r, i8 dr, i8 dg, i8 db, i8 da);
            ~SDLBlockColorMod();

            SDL_Renderer* renderer_;
            u8 r_, g_, b_, a_;
        };

        enum Flags {
            fNone = 0,
            fAll = ~fNone,
            fShowStartEdge = BIT_MASK(0),
            fShowNormals = BIT_MASK(1),
            fShowRotation = BIT_MASK(2),
            fShowCenter = BIT_MASK(3),
            fShowPosition = BIT_MASK(4)
        };

        static void drawShape(SDL_Renderer* r, const Shape& s, const Transform& tr, u32 flags = DebugDraw::fNone);
        static void drawShape(SDL_Renderer* r, const Shape& s, u32 flags = DebugDraw::fNone);
        template <u32 Size>
        static void drawSquareAt(SDL_Renderer *r, f32 x, f32 y);
        template <u32 Size>
        static void drawCrossAt(SDL_Renderer *r, f32 x, f32 y);
        static void drawArrow(SDL_Renderer *r, const Vec2& start, const Vec2& end);
        static void drawPointCross(SDL_Renderer *r, const Vec2& pt);
    private:
        static void drawCircle_(SDL_Renderer* r, const Circle& c, const Transform& tr, u32 flags);
        static void drawARect_(SDL_Renderer* r, const ARect& ar, const Transform& tr, u32 flags);
        static void drawRect_(SDL_Renderer* r, const Rect& rect, u32 flags);
        static void drawPgon_(SDL_Renderer* r, const Pgon& pgon, const Transform& tr, u32 flags);
    };
}

#include "debug_draw.inl"
#endif //DEBUG_DRAW_H
