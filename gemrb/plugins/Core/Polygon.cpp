/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header: /data/gemrb/cvs2svn/gemrb/gemrb/gemrb/plugins/Core/Polygon.cpp,v 1.8 2004/08/05 16:02:07 avenger_teambg Exp $
 */
#include "../../includes/win32def.h"
#include "Polygon.h"
#include "Interface.h"

extern Interface* core;

Gem_Polygon::Gem_Polygon(Point* points, int cnt, bool precalculate, Color* color)
{
	if (cnt) {
		this->points = ( Point * ) malloc( cnt * sizeof( Point ) );
	} else {
		this->points = NULL;
	}
	memcpy( this->points, points, cnt * sizeof( Point ) );
	count = cnt;
	if (precalculate) {
		fill = core->GetVideoDriver()->PrecalculatePolygon( points, cnt, *color );
	} else {
		fill = NULL;
	}
}

Gem_Polygon::~Gem_Polygon(void)
{
	if (points) {
		free( points );
	}
	if (fill) {
		core->GetVideoDriver()->FreeSprite( fill );
	}
}

void Gem_Polygon::RecalcBBox()
{
	if(!count) {
		BBox.x=BBox.y=BBox.w=BBox.h=0;
		return;
	}
	BBox.x=points[0].x;
	BBox.y=points[0].y;
	BBox.w=0;
	BBox.h=0;
	for(int i=1; i<count; i++) {
		if(points[i].x<BBox.x) {
			BBox.w+=BBox.x-points[i].x;
			BBox.x=points[i].x;
		}
		if(points[i].y<BBox.y) {
			BBox.h+=BBox.y-points[i].y;
			BBox.y=points[i].y;
		}
	}
}

bool Gem_Polygon::PointIn(unsigned short x, unsigned short y)
{
	register int   j, yflag0, yflag1, xflag0 , index;
	register long ty, tx;
	bool inside_flag = false;
	Point* vtx0, * vtx1;

	index = 0;

	tx = x;
	ty = y;

	if(!BBox.PointInside(x,y) ) return false;
	vtx0 = &points[count - 1];
	yflag0 = ( vtx0->y >= ty );
	vtx1 = &points[index];

	for (j = count + 1; --j ;) {
		yflag1 = ( vtx1->y >= ty );
		if (yflag0 != yflag1) {
			xflag0 = ( vtx0->x >= tx );
			if (xflag0 == ( vtx1->x >= tx )) {
				if (xflag0)
					inside_flag = !inside_flag;
			} else {
				if (( vtx1->x -
					( vtx1->y - ty ) * ( vtx0->x - vtx1->x ) /
					( vtx0->y - vtx1->y ) ) >= tx) {
					inside_flag = !inside_flag;
				}
			}
		}
		yflag0 = yflag1;
		vtx0 = vtx1;
		vtx1 = &points[++index];
	}
	return inside_flag;
}
