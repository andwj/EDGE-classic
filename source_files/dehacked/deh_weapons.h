//------------------------------------------------------------------------
//  WEAPON conversion
//------------------------------------------------------------------------
// 
//  DEH_EDGE  Copyright (C) 2004-2005  The EDGE Team
// 
//  This program is under the GNU General Public License.
//  It comes WITHOUT ANY WARRANTY of any kind.
//  See COPYING.txt for the full details.
//
//------------------------------------------------------------------------

#ifndef __DEH_WEAPONS_HDR__
#define __DEH_WEAPONS_HDR__

namespace Deh_Edge
{

//
// MBF21 weapon flags
//
typedef enum
{
	// Doesn't thrust things
	MBF21_NOTHRUST = 1,

	// Weapon is silent
	MBF21_SILENT = 2,

	// Weapon won't autofire when swapped to
	MBF21_NOAUTOFIRE = 4,

	// Monsters consider it a melee weapon (currently unused)
	MBF21_FLEEMELEE = 8,

	// Can be switched away from when ammo is picked up
	MBF21_AUTOSWITCHFROM = 16,

	// Cannot be switched to when ammo is picked up
	MBF21_NOAUTOSWITCHTO = 32,
}
weapmbf21flag_t;

// Weapon info: sprite frames, ammunition use.
typedef struct
{
	const char *ddf_name;

    int ammo, ammo_per_shot;
	int bind_key, priority;
	
	const char *flags;

    int upstate;
    int downstate;
    int readystate;
    int atkstate;
    int flashstate;
    int mbf21_flags;
}
weaponinfo_t;

// The defined weapons...
typedef enum
{
    wp_fist,
    wp_pistol,
    wp_shotgun,
    wp_chaingun,
    wp_missile,
    wp_plasma,
    wp_bfg,
    wp_chainsaw,
    wp_supershotgun,
                                                                                            
    NUMWEAPONS
}
weapontype_e;

extern weaponinfo_t weapon_info[NUMWEAPONS];


namespace Weapons
{
	void Init();
	void Shutdown();

	void MarkWeapon(int wp_num);

	void AlterWeapon(int new_val);

	void ConvertWEAP(void);
}

}  // Deh_Edge

#endif /* __DEH_WEAPONS_HDR__ */
