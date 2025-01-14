//----------------------------------------------------------------------------
//  EDGE Sound System for SDL
//----------------------------------------------------------------------------
// 
//  Copyright (c) 1999-2009  The EDGE Team.
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

#include "i_defs.h"
#include "i_sound.h"

#include <fcntl.h>
#include <signal.h>

#include <sys/types.h>
#ifdef _MSC_VER
#include <ctime>
#else
#include <sys/time.h>
#endif

#include "file.h"
#include "filesystem.h"
#include "path.h"

#include "m_argv.h"
#include "m_misc.h"
#include "m_random.h"
#include "s_sound.h"
#include "s_cache.h"
#include "s_blit.h"
#include "s_opl.h"
#include "s_fluid.h"
#include "w_wad.h"

// If true, sound system is off/not working. Changed to false if sound init ok.
bool nosound = false;

static SDL_AudioSpec mydev;
SDL_AudioDeviceID mydev_id;

int dev_freq;
int dev_bytes_per_sample;
int dev_frag_pairs;
bool dev_stereo;

#define PRI_NOSOUND   -1
#define PRI_FINISHED  -2

// Error Description
static char errordesc[256] = "FOO";
static char scratcherror[256];

static bool audio_is_locked = false;

std::vector<std::string> available_soundfonts;
std::vector<std::string> available_genmidis;
extern std::string game_dir;


void SoundFill_Callback(void *udata, Uint8 *stream, int len)
{
	SDL_memset(stream, 0, len);
	S_MixAllChannels(stream, len);
}

static bool I_TryOpenSound(int want_freq, bool want_stereo)
{
	SDL_AudioSpec trydev;
	SDL_zero(trydev);

	int samples = 512;

	if (want_freq < 18000)
		samples = 256;
	else if (want_freq >= 40000)
		samples = 1024;

	I_Printf("I_StartupSound: trying %d Hz %s\n",
			 want_freq, want_stereo ? "Stereo" : "Mono");
	
	trydev.freq     = want_freq;
	trydev.format   = AUDIO_S16SYS;
	trydev.channels = want_stereo ? 2 : 1;
	trydev.samples  = samples;
	trydev.callback = SoundFill_Callback;

	// Ask for signed 16-bit @ 48khz by default; allow SDL to adjust frequency if needed but never the format
	mydev_id = SDL_OpenAudioDevice(NULL, 0, &trydev, &mydev, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

	if (mydev_id > 0)
		return true;

	I_Printf("  failed: %s\n", SDL_GetError());

	return false;
}

void I_StartupSound(void)
{
	if (nosound) return;

	if (argv::Find("waveout") > 0)
		force_waveout = true;

	if (argv::Find("dsound") > 0 || argv::Find("nowaveout") > 0)
		force_waveout = false;

	std::string driver = argv::Value("audiodriver");

	if (driver.empty())
		driver = SDL_getenv("SDL_AUDIODRIVER") ? SDL_getenv("SDL_AUDIODRIVER") : "";

	if (driver.empty())
	{
		driver = "default";

#ifdef WIN32
		if (force_waveout)
			driver = "waveout";
#endif
	}

	if (epi::case_cmp(driver, "default") != 0)
	{
		SDL_setenv("SDL_AUDIODRIVER", driver.c_str(), 1);
	}

	I_Printf("SDL_Audio_Driver: %s\n", driver.c_str());

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
	{
		I_Printf("I_StartupSound: Couldn't init SDL AUDIO! %s\n",
				 SDL_GetError());
		nosound = true;
		return;
	}

	int want_freq = 48000;
	bool want_stereo = (var_sound_stereo >= 1);

	std::string p = argv::Value("freq");

	if (!p.empty())
		want_freq = atoi(p.c_str());

	if (argv::Find("mono")   > 0) want_stereo = false;
	if (argv::Find("stereo") > 0) want_stereo = true;

	bool success = false;
	
	if (I_TryOpenSound(want_freq, want_stereo))
		success = true;
	
	if (! success)
	{
		I_Printf("I_StartupSound: Unable to find a working sound mode!\n");
		nosound = true;
		return;
	}

	// These checks shouldn't really fail, as SDL2 allows us to force our desired format and convert silently if needed,
	// but they might end up being a good safety net - Dasho

	if (mydev.format != AUDIO_S16SYS)
	{
		I_Printf("I_StartupSound: unsupported format: %d\n", mydev.format);
		SDL_CloseAudio();
		nosound = true;
		return;
	}

	if (mydev.channels >= 3)
	{
		I_Printf("I_StartupSound: unsupported channel num: %d\n", mydev.channels);
		SDL_CloseAudio();

		nosound = true;
		return;
	}

	if (want_stereo && mydev.channels != 2)
		I_Printf("I_StartupSound: stereo sound not available.\n");
	else if (!want_stereo && mydev.channels != 1)
		I_Printf("I_StartupSound: mono sound not available.\n");

	if (mydev.freq < (want_freq - want_freq/100) || 
		mydev.freq > (want_freq + want_freq/100))
	{
		I_Printf("I_StartupSound: %d Hz sound not available.\n", want_freq);
	}

	dev_bytes_per_sample = (mydev.channels) * 2;
	dev_frag_pairs = mydev.size / dev_bytes_per_sample;

	SYS_ASSERT(dev_bytes_per_sample > 0);
	SYS_ASSERT(dev_frag_pairs > 0);

	dev_freq   = mydev.freq;
	dev_stereo = (mydev.channels == 2);

	// update Sound Options menu
	if (dev_stereo != (var_sound_stereo >= 1))
		var_sound_stereo = dev_stereo ? 1 : 0;

	// display some useful stuff
	I_Printf("I_StartupSound: Success @ %d Hz %s\n",
			dev_freq, dev_stereo ? "Stereo" : "Mono");

	return;
}


void I_ShutdownSound(void)
{
	if (nosound)
		return;

	S_Shutdown();

	nosound = true;

	SDL_CloseAudio();
}


const char *I_SoundReturnError(void)
{
	memcpy(scratcherror, errordesc, sizeof(scratcherror));
	memset(errordesc, '\0', sizeof(errordesc));

	return scratcherror;
}


void I_LockAudio(void)
{
	if (audio_is_locked)
	{
		I_UnlockAudio();
		I_Error("I_LockAudio: called twice without unlock!\n");
	}

	SDL_LockAudio();
	audio_is_locked = true;
}

void I_UnlockAudio(void)
{
	if (audio_is_locked)
	{
		SDL_UnlockAudio();
		audio_is_locked = false;
	}
}

void I_StartupMusic(void)
{
	// Check for SF2 soundfonts
	std::vector<epi::dir_entry_c> sfd;
	std::string soundfont_dir = epi::PATH_Join(game_dir.c_str(), "soundfont");

	if (!FS_ReadDir(sfd, soundfont_dir.c_str(), "*.sf2"))
	{
		I_Warning("FluidLite: Failed to read '%s' directory!\n", soundfont_dir.c_str());
	}
	else
	{
		for (size_t i = 0 ; i < sfd.size() ; i++) 
		{
			if(!sfd[i].is_dir)
			{
				available_soundfonts.push_back(epi::PATH_GetFilename(sfd[i].name.c_str()));
			}
		}
	}

	// Check for SF3 soundfonts
	sfd.clear();

	if (!FS_ReadDir(sfd, soundfont_dir.c_str(), "*.sf3"))
	{
		I_Warning("FluidLite: Failed to read '%s' directory!\n", soundfont_dir.c_str());
	}
	else
	{
		for (size_t i = 0 ; i < sfd.size() ; i++) 
		{
			if(!sfd[i].is_dir)
			{
				available_soundfonts.push_back(epi::PATH_GetFilename(sfd[i].name.c_str()));
			}
		}
	}

	// Check for OP2 instrument banks
	sfd.clear();

	// Start with empty string to represent using whichever GENMIDI
	// is found in the load order (usually just the IWAD)
	available_genmidis.push_back("");

	if (!FS_ReadDir(sfd, soundfont_dir.c_str(), "*.op2"))
	{
		I_Warning("OPL: Failed to read '%s' directory!\n", soundfont_dir.c_str());
	}
	else
	{
		for (size_t i = 0 ; i < sfd.size() ; i++) 
		{
			if(!sfd[i].is_dir)
			{
				available_genmidis.push_back(epi::PATH_GetFilename(sfd[i].name.c_str()));
			}
		}
	}

	// Startup both FluidLite and OPL, as some formats require OPL now (IMF/CMF)

	if (!S_StartupFluid())
		fluid_disabled = true;

	if (!S_StartupOPL())
		opl_disabled = true;

	return;
}

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
