//----------------------------------------------------------------------------
//  EDGE Navigation System
//----------------------------------------------------------------------------
//
//  Copyright (c) 2022  The EDGE Team.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//----------------------------------------------------------------------------

#ifndef __P_NAVIGATE_H__
#define __P_NAVIGATE_H__

#include "types.h"
#include "p_mobj.h"
#include "r_defs.h"

struct bot_t;

// a path from a start subsector to a finish one.
// elements are indices into the subsectors[] array.
// start is NOT included, but finish is.
class bot_path_c
{
public:
	std::vector<int> subs {};

	size_t along = 0;

	bool finished() const
	{
		return along == subs.size();
	}

	position_c calc_target() const;
};


void NAV_AnalyseLevel();
void NAV_FreeLevel();

float NAV_EvaluateBigItem(const mobj_t *mo);
bool  NAV_NextRoamPoint(position_c& out);

// attempt to find a traversible path, returns NULL if failed.
bot_path_c * NAV_FindPath(subsector_t *start, subsector_t *finish, int flags);

// find an pickup item in a nearby area, returns NULL if none found.
bot_path_c * NAV_FindThing(bot_t *bot, float radius, mobj_t*& best);

// find an enemy to fight, or NULL if none found.
// caller is responsible to do a sight checks.
mobj_t * NAV_FindEnemy(bot_t *bot, float radius);

#endif  /*__P_NAVIGATE_H__*/

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab