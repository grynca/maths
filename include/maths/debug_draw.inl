#include "debug_draw.h"

namespace grynca {

    inline void DebugDraw::drawShape(SDL_Renderer* r, const Shape& s, const Transform& tr, u32 flags) {
        switch (s.getTypeId()) {
            case Shape::getTypeIdOf<ARect>(): {
                const ARect& arect = s.get<ARect>();
                drawARect_(r, arect, tr, flags);
            }break;
            case Shape::getTypeIdOf<Circle>(): {
                const Circle& circle = s.get<Circle>();
                drawCircle_(r, circle, tr, flags);
            }break;
            case Shape::getTypeIdOf<Rect>(): {
                Rect rect = s.get<Rect>().transformOut(tr);
                drawRect_(r, rect, flags);
            }break;
            case Shape::getTypeIdOf<Ray>(): {
                const Ray& ray = s.get<Ray>();
                Mat3 trm = tr.calcMatrix();
                drawArrow(r, trm*ray.getStart(), trm*ray.getEnd());
            }break;
            case Shape::getTypeIdOf<Pgon>(): {
                drawPgon_(r, s.get<Pgon>(), tr, flags);
            }break;
            default:
                break;
        }
    }

    inline void DebugDraw::drawShape(SDL_Renderer* r, const Shape& s, u32 flags) {
        drawShape(r, s, Transform(), flags);
    }

    template <u32 Size>
    inline void DebugDraw::drawSquareAt(SDL_Renderer *r, f32 x, f32 y) {
        SDL_Rect rect;
        rect.x = (int)roundf(x)-(Size/2);
        rect.y = (int)roundf(y)-(Size/2);
        rect.w = Size;
        rect.h = Size;
        SDL_RenderDrawRect(r, &rect);
    }

    template <u32 Size>
    inline void DebugDraw::drawCrossAt(SDL_Renderer *r, f32 x, f32 y) {
        SDL_RenderDrawLine(r, (i32)x, (i32)y-(Size/2), (i32)x, (i32)y+(Size/2));
        SDL_RenderDrawLine(r, (i32)x-(Size/2), (i32)y, (i32)x+(Size/2), (i32)y);
    }

    inline void DebugDraw::drawArrow(SDL_Renderer *r, const Vec2& start, const Vec2& end) {
        static Dir2 rd = Angle::degrees(25).getDir();

        f32 len = (end-start).getLen();
        if (len > 0.0f) {
            Dir2 dir = (end-start)/len;
            SDL_RenderDrawLine(r, (i32)start.getX(), (i32)start.getY(), (i32)end.getX(), (i32)end.getY());
            Dir2 arrow_top_dir = (-dir).rotate(rd);
            Dir2 arrow_bot_dir = (-dir).rotateInverse(rd);

            Vec2 end2 = end + arrow_top_dir*5;
            SDL_RenderDrawLine(r, (i32)end.getX(), (i32)end.getY(), (i32)end2.getX(), (i32)end2.getY());

            end2 = end + arrow_bot_dir*5;
            SDL_RenderDrawLine(r, (i32)end.getX(), (i32)end.getY(), (i32)end2.getX(), (i32)end2.getY());
        }
        else {
            drawPointCross(r, start);
        }
    }

    inline void DebugDraw::drawPointCross(SDL_Renderer *r, const Vec2& pt) {
        static Dir2 dir1(Angle::degrees(45).getDir());
        static Dir2 dir2(Angle::degrees(135).getDir());

        Vec2 start = pt-dir1*2;
        Vec2 end = pt+dir1*2;
        SDL_RenderDrawLine(r, (i32)start.getX(), (i32)start.getY(), (i32)end.getX(), (i32)end.getY());
        start = pt-dir2*2;
        end = pt+dir2*2;
        SDL_RenderDrawLine(r, (i32)start.getX(), (i32)start.getY(), (i32)end.getX(), (i32)end.getY());
    }

    inline void DebugDraw::drawCircle_(SDL_Renderer* r, const Circle& c, const Transform& tr, u32 flags) {
        u32 sides = Angle::Pi*c.getRadius();
        f32 da = 2*Angle::Pi/sides;
        f32 angle = da;

        Vec2 center = c.getCenter() + tr.getPosition();
        f32 radius = c.getRadius()*tr.getScale().getX();

        Vec2 start, end(radius, 0.0f);
        end += center;
        for (u32 i=0; i!=sides; i++) {
            start = end;
            end.accX() = cosf(angle) * radius;
            end.accY() = sinf(angle) * radius;
            end = end + center;
            angle += da;
            SDL_RenderDrawLine(r, (i32)start.getX(), (i32)start.getY(), (i32)end.getX(), (i32)end.getY());
        }
        if (flags&DebugDraw::fShowRotation) {
            SDLBlockColorMod mod(r, 0, 20, -20, 0);
            drawArrow(r, center, center+tr.getRotDir()*radius);
        }
        if (flags&(DebugDraw::fShowCenter|DebugDraw::fShowPosition)) {
            SDLBlockColorMod mod(r, 0, -20, 20, 0);
            drawCrossAt<4>(r, center.getX(), center.getY());
        }
    }

    inline void DebugDraw::drawARect_(SDL_Renderer* r, const ARect& ar, const Transform& tr, u32 flags) {
        Rect rect(tr.getPosition(), ar.getSize()*tr.getScale(), ar.getLeftTop()*tr.getScale(), tr.getRotDir());
        drawRect_(r, rect, flags);
    }

    inline void DebugDraw::drawRect_(SDL_Renderer* r, const Rect& rect, u32 flags) {
        Vec2 corners[4];
        rect.getCorners(corners);

        if (flags&DebugDraw::fShowStartEdge) {
            SDLBlockColorMod mod(r, 0, 20, -20, 0);
            drawArrow(r, corners[0], corners[1]);
        }
        else {
            SDL_RenderDrawLine(r, (i32)corners[0].getX(), (i32)corners[0].getY(), (i32)corners[1].getX(), (i32)corners[1].getY());
        }
        for (u32 i=1; i<3; ++i) {
            SDL_RenderDrawLine(r, (i32)corners[i].getX(), (i32)corners[i].getY(), (i32)corners[i+1].getX(), (i32)corners[i+1].getY());
        }
        SDL_RenderDrawLine(r, (i32)corners[3].getX(), (i32)corners[3].getY(), (i32)corners[0].getX(), (i32)corners[0].getY());

        Vec2 center;
        if (flags&DebugDraw::fShowCenter) {
            SDLBlockColorMod mod(r, 0, -20, 20, 0);
            center = rect.getCenter();
            drawCrossAt<4>(r, center.getX(), center.getY());
        }
        if (flags&DebugDraw::fShowPosition) {
            Vec2 pos = rect.getPosition();
            if (!(flags&DebugDraw::fShowCenter) || !pos.isSame(center)) {
                SDLBlockColorMod mod(r, 0, -20, -20, 0);
                drawCrossAt<4>(r, pos.getX(), pos.getY());
            }
        }
    }

    inline void DebugDraw::drawPgon_(SDL_Renderer* r, const Pgon& p, const Transform& tr, u32 flags) {
        Pgon pgon = p.transformOut(tr.calcMatrix());
        pgon.calculateNormalsIfNeeded();

        u32 j = 0;
        if (flags&DebugDraw::fShowStartEdge) {
            SDLBlockColorMod mod(r, 0, 20, -20, 0);
            const Vec2& p1 = pgon.getPoint(0);
            const Vec2& p2 = pgon.getPoint(1);
            drawArrow(r, p1, p2);
            ++j;
        }

        for (; j<pgon.getSize(); ++j) {
            const Vec2& p1 = pgon.getPoint(j);
            const Vec2& p2 = pgon.getPoint((j+1)%pgon.getSize());
            SDL_RenderDrawLine(r, (int)p1.getX(), (int)p1.getY(), (int)p2.getX(), (int)p2.getY());
        }

        static u32 normals_len = 10;
        if (flags&DebugDraw::fShowNormals) {
            for (u32 i=0; i<pgon.getSize(); ++i) {
                Vec2 p1 = (pgon.getPoint(i) + pgon.getPoint(wrap(i+1, pgon.getSize())))*0.5f;
                Dir2 normal = pgon.getNormal(i);
                Vec2 p2 = p1+normal*normals_len;
                drawArrow(r, p1, p2);
            }
        }
        Vec2 center;
        if (flags&DebugDraw::fShowCenter) {
            SDLBlockColorMod mod(r, 0, -20, 20, 0);
            center = pgon.calcCenter();
            drawCrossAt<4>(r, center.getX(), center.getY());
        }
        if (flags&DebugDraw::fShowPosition) {
            Vec2 pos = tr.getPosition();
            if (!(flags&DebugDraw::fShowCenter) || !pos.isSame(center)) {
                SDLBlockColorMod mod(r, 0, -20, -20, 0);
                drawCrossAt<4>(r, pos.getX(), pos.getY());
            }
        }
    }

    inline DebugDraw::SDLBlockColorMod::SDLBlockColorMod(SDL_Renderer* r, i8 dr, i8 dg, i8 db, i8 da)
     : renderer_(r)
    {
        SDL_GetRenderDrawColor(r, &r_, &g_, &b_, &a_);
        SDL_SetRenderDrawColor(r, r_+dr, g_+dg, b_+db, a_+da);
    }

    inline DebugDraw::SDLBlockColorMod::~SDLBlockColorMod() {
        SDL_SetRenderDrawColor(renderer_, r_, g_, b_, a_);
    }

}