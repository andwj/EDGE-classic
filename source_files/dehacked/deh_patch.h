//------------------------------------------------------------------------
//  PATCH Loading
//------------------------------------------------------------------------
// 
//  DEH_EDGE  Copyright (C) 2004-2005  The EDGE Team
// 
//  This program is under the GNU General Public License.
//  It comes WITHOUT ANY WARRANTY of any kind.
//  See COPYING.txt for the full details.
//
//------------------------------------------------------------------------

#ifndef __DEH_PATCH_HDR__
#define __DEH_PATCH_HDR__

namespace Deh_Edge
{

namespace Patch
{
	extern char line_buf[];
	extern int line_num;

	extern int active_obj;
	extern int patch_fmt;

	dehret_e Load(input_buffer_c *buf);
}

}  // Deh_Edge

#endif /* __DEH_PATCH_HDR__ */
