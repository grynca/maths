#ifndef MATHS_H
#define MATHS_H
#include "maths/maths_config.h"
#include "maths/common.h"
#include "maths/Angle.h"
#include "maths/Vec2.h"
#include "maths/Mat3.h"
#include "maths/Interval.h"
#include "maths/Transform.h"
#include "maths/shapes/Shape.h"
#include "maths/shapes/OverlapHelper.h"
#include "maths/shapes/GJK.h"
#include "maths/maths_funcs.h"

#if USE_SDL2 == 1
#include "maths/debug_draw.h"
#endif

#endif //MATHS_H
