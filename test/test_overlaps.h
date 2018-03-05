#ifndef TEST_OVERLAPS_H
#define TEST_OVERLAPS_H

#include "maths.h"

class TestOverlapsFixture : public SDLTest {
public:
    TightArray<Shape> shapes_pool;
    struct ShapeCtx {
        ShapeCtx(Index idx) : pool_idx(idx) {}

        Index pool_idx;
        Transform tr;
    };
    fast_vector<ShapeCtx> shapes;

    bool mouse_down;
    bool rotate_down;
    bool resolve_collision;
    bool show_local_frame;
    Vec2 local_frame_pos;
    Vec2 local_frame_size;
    F8x8::SDL2Text local_frame_lbl;
    i32 selected_shape_id;
    Dir2 grab_offset;
    bool show_contacts;

    void init(Config::ConfigSectionMap& cfg) {
        mouse_down = false;
        selected_shape_id = -1;
        rotate_down = false;
        resolve_collision = false;
        show_local_frame = true;
        show_contacts = true;

        int ww, wh;
        SDL_GetWindowSize(accTestBench().getWindow(), &ww, &wh);
        local_frame_size.set(250, 250);
        local_frame_pos = Vec2(ww-local_frame_size.getX(), 0);
        local_frame_lbl.setColor(0, 0, 0, 255);

        Vec2 pos{60, 300};
        Vec2 rsize{50, 100};
        f32 cradius = 35;
        f32 x_gap = 20;
        f32 row_h = 200;
        f32 ray_len = 100;

        addShape<ARect>(-rsize/2, rsize);
        shapes.back().tr.setPosition(pos);
        pos.accX() += rsize.getX() + x_gap;
        std::swap(rsize.accX(), rsize.accY());
        pos.accX() += rsize.getX()*0.5f;

        addShape<ARect>(-rsize/2, rsize);
        shapes.back().tr.setPosition(pos);
        pos.accX() += rsize.getX() + x_gap;

        addShape<Circle>(Vec2(0, 0), cradius);
        shapes.back().tr.setPosition(pos);
        pos.accX() += cradius*2 + x_gap;
        addShape<Circle>(Vec2(0, 0), cradius);
        shapes.back().tr.setPosition(pos);
        pos.accX() += cradius*2 + x_gap;

        Angle rot = Angle::Pi*0.25f;
        addShape<Rect>(Vec2(0, 0), rsize, -rsize/2, rot);
        shapes.back().tr.setPosition(pos);
        pos.accX() +=rsize.getX() + x_gap;
        std::swap(rsize.accX(), rsize.accY());
        addShape<Rect>(Vec2(0, 0), rsize, -rsize/2, rot);
        shapes.back().tr.setPosition(pos);
        pos.accX() += rsize.getX() + x_gap;

        addShape<Ray>(Vec2(0, 0), rot.getDir(), ray_len, Ray::CalcDirInfo());
        shapes.back().tr.setPosition(pos);
        pos.accX() = 100;
        pos.accY() += row_h;

        Vec2 p1verts[] = {
                {-33, -42}, {38, -63}, {62, -12}, {44, 32}, {-44, 54}, {-57, 28}
        };

        addShape<Pgon>(p1verts, ARRAY_SIZE(p1verts));
        Pgon& pgon1 = shapes_pool.accItem(shapes.back().pool_idx).acc<Pgon>();
        pgon1.calculateNormalsIfNeeded();
        shapes.back().tr.setPosition(pos);
        pos.accX() += 200;

        Vec2 p2verts[] = {
                {-43, 11}, {-26, -20}, {2, -31}, {69, -13}, {-19, 37}
        };

        addShape<Pgon>(p2verts, ARRAY_SIZE(p2verts));
        Pgon& pgon2 = shapes_pool.accItem(shapes.back().pool_idx).acc<Pgon>();
        pgon2.calculateNormalsIfNeeded();
        shapes.back().tr.setPosition(pos);

        std::cout << "Controls: c: show/hide contacts" << std::endl
                  << "          r: rotate selected shape (oriented rect, ray, pgon)" << std::endl
                  << "          f: resolve collision for currently selected shape" << std::endl
                  << "          l: toggle local-frame" << std::endl;
    }

    void close() {
        shapes_pool.clear();
        shapes.clear();
    }

    void handleEvent(SDL_Event& evt) {
        switch (evt.type) {
            case (SDL_MOUSEBUTTONDOWN): {
                Vec2 mouse_pos(evt.button.x, evt.button.y);
                mouse_down = true;
                selected_shape_id = -1;
                for (u32 i=0; i<shapes.size(); ++i) {
                    const Shape& s = shapes_pool.getItem(shapes[i].pool_idx);
                    Vec2 mouse_pos_in_shape = (-shapes[i].tr).calcMatrix()*mouse_pos;
                    if ( (s.is<Ray>() && s.get<Ray>().isPointInside(mouse_pos_in_shape, 10))
                         || s.isPointInside(mouse_pos_in_shape))
                    {
                        grab_offset = mouse_pos_in_shape;
                        grab_offset = shapes[i].tr.calcMatrix()*grab_offset;
                        selected_shape_id = i;
                    }
                }

            }break;
            case (SDL_MOUSEBUTTONUP): {
                if (evt.button.button == SDL_BUTTON_LEFT && mouse_down) {
                    mouse_down = false;
                }
            }break;
            case (SDL_MOUSEMOTION): {
                if (selected_shape_id != -1 && mouse_down) {
                    Vec2 mouse_pos(evt.button.x, evt.button.y);
                    shapes[selected_shape_id].tr.setPosition(mouse_pos - grab_offset);
                }
            }break;
            case (SDL_KEYDOWN): {
                switch (evt.key.keysym.sym) {
                    case SDLK_r:
                        rotate_down = true;
                        break;
                    case SDLK_f:
                        resolve_collision = true;
                        break;
                    case SDLK_c: {
                        show_contacts = !show_contacts;
                        break;
                    }
                    case SDLK_l:
                        show_local_frame = !show_local_frame;
                        break;
                }
            }break;
            case (SDL_KEYUP): {
                if (evt.key.keysym.sym == SDLK_r) {
                    rotate_down = false;
                }
            }break;
        }
    }

    void update(SDL_Renderer* r, f32 dt) {
        PROFILE_BLOCK("update");

        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderClear(r);

        if (rotate_down && selected_shape_id!=-1) {
            const Shape& sh = shapes_pool.getItem(shapes[selected_shape_id].pool_idx);
            Angle d_rot = dt*Angle::Pi_2;
            switch (sh.getTypeId()) {
                case Shape::getTypeIdOf<ARect>():
                    // dont rotate ARect
                    break;
                default:
                    shapes[selected_shape_id].tr.setRotation(shapes[selected_shape_id].tr.getRotation() + d_rot);
                    break;
            }
        }

        for (u32 i=0; i<shapes.size(); ++i) {
            if (i != u32(selected_shape_id))
                SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
            else {
                if (show_local_frame) {
                    SDL_Rect lf_bckg_rect{i32(local_frame_pos.getX()), i32(local_frame_pos.getY()), i32(local_frame_size.getX()), i32(local_frame_size.getY())};
                    SDL_SetRenderDrawColor(r, 255, 255, 255, 200);
                    SDLCall(SDL_RenderFillRect(r, &lf_bckg_rect));

                    local_frame_lbl.setRenderer(r);
                    local_frame_lbl.setText("Local Frame:");
                    local_frame_lbl.draw(local_frame_pos.getX()+5, 5);

                    SDL_SetRenderDrawColor(r, 0, 170, 50, 255);
                    Shape shape_copy(shapes_pool.getItem(shapes[i].pool_idx));
                    DebugDraw::drawShape(r, shape_copy, Transform::createTranslation(Vec2(local_frame_pos+Vec2(125, 125))), DebugDraw::fAll);
                }
                SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
            }
            Shape shape_copy(shapes_pool.getItem(shapes[i].pool_idx));
            DebugDraw::drawShape(r, shape_copy, shapes[i].tr, DebugDraw::fAll);
        }

        for (u32 i=0; i<shapes.size(); ++i) {
            for (u32 j=i+1; j<shapes.size(); ++j) {
                u32 s1_id = i;
                u32 s2_id = j;
                if (u32(selected_shape_id) == s2_id) {
                    std::swap(s1_id, s2_id);
                }
                bool selected_colliding = (u32(selected_shape_id) == s1_id);

                Shape& s1 = shapes_pool.accItem(shapes[s1_id].pool_idx);
                Shape& s2 = shapes_pool.accItem(shapes[s2_id].pool_idx);

                OverlapHelper oh;
                oh.set(s1, s2, shapes[s1_id].tr, shapes[s2_id].tr);

                bool rslt = oh.overlaps();
                if (rslt) {
                    oh.calcContactG();
                    const ContactManifold& cm = oh.getContactManifoldG();
                    if (show_contacts) {
                        SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
                        Vec2 avg_cp;
                        for (u32 cpid=0; cpid<cm.size; ++cpid) {
                            const Vec2& cp = cm.points[cpid].position;
                            Vec2 end = cp + cm.points[cpid].penetration * cm.normal;
                            DebugDraw::drawArrow(r, cp, end);
                        }
                    }

                    if (resolve_collision) {
                        if (selected_colliding) {
                            f32 pen = cm.points[0].penetration;
                            if (cm.size > 1 && cm.points[1].penetration > pen)
                                pen = cm.points[1].penetration;
                            shapes[s1_id].tr = Transform::createTranslation(pen*cm.normal)*shapes[s1_id].tr;
                            resolve_collision = false;
                        }
                    }

                    if (show_local_frame && selected_colliding) {
                        SDL_SetRenderDrawColor(r, 0, 128, 128, 255);
                        Shape shape_copy(s2);
                        shape_copy.transform(
                                Transform::createTranslation(Vec2(local_frame_pos+Vec2(125, 125)))
                                *(-shapes[s1_id].tr)
                                *shapes[s2_id].tr);
                        DebugDraw::drawShape(r, shape_copy, DebugDraw::fShowNormals | DebugDraw::fShowStartEdge | DebugDraw::fShowCenter | DebugDraw::fShowPosition);

                        std::string feature_ids_str;
                        for (u32 pti=0; pti<cm.size; ++pti) {
                            struct ClipVertex {
                                u8 in_edge1, out_edge1, in_edge2, out_edge2;
                            };
                            ClipVertex* cv = (ClipVertex*)&cm.points[pti].feature_id;
                            feature_ids_str += ssu::formatA("FID %u: %u, %u, %u, %u\n", pti, cv->in_edge1, cv->out_edge1, cv->in_edge2, cv->out_edge2);
                        }
                        local_frame_lbl.setText(feature_ids_str);
                        local_frame_lbl.draw(local_frame_pos.getX()+5, 20);
                    }
                }

            }
        }
    }

    template <typename ShapeT, typename... ConstrArgs>
    void addShape(ConstrArgs&&... args) {
        Index shape_id;
        Shape& s = shapes_pool.add2(shape_id);
        s.create<ShapeT>(std::forward<ConstrArgs>(args)...);
        shapes.push_back(shape_id);
    };
};

#endif //TEST_OVERLAPS_H
