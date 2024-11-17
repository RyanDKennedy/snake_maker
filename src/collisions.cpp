#include "collisions.hpp"

bool is_inside_collision_box(CollisionBox *box, Vec2i pos)
{
    if (pos[0] <= box->top_right[0] && pos[0] >= box->bottom_left[0] &&
	pos[1] <= box->top_right[1] && pos[1] >= box->bottom_left[1])
    {
	return true;
    }
    else
    {
	return false;
    }
}
