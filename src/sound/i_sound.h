/*
** i_sound.h
**
**---------------------------------------------------------------------------
** Copyright 1998-2006 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/


#ifndef __I_SOUND__
#define __I_SOUND__

#include <memory>

#include "doomtype.h"
#include "i_soundinternal.h"

class FileReader;

enum ECodecType
{
	CODEC_Unknown,
	CODEC_Vorbis,
};

enum EStartSoundFlags
{
	SNDF_LOOP=1,
	SNDF_NOPAUSE=2,
	SNDF_AREA=4,
	SNDF_ABSTIME=8,
	SNDF_NOREVERB=16,
};

class SoundStream
{
public:
	virtual ~SoundStream ();

	enum
	{	// For CreateStream
		Mono = 1,
		Bits8 = 2,
		Bits32 = 4,
		Float = 8,

		// For OpenStream
		Loop = 16
	};

	virtual bool Play(bool looping, float volume) = 0;
	virtual void Stop() = 0;
	virtual void SetVolume(float volume) = 0;
	virtual bool SetPaused(bool paused) = 0;
	virtual unsigned int GetPosition() = 0;
	virtual bool IsEnded() = 0;
	virtual bool SetPosition(unsigned int pos);
	virtual bool SetOrder(int order);
	virtual FString GetStats();
};

typedef bool (*SoundStreamCallback)(SoundStream *stream, void *buff, int len, void *userdata);

struct SoundDecoder;

class SoundRenderer
{
public:
	SoundRenderer ();
	virtual ~SoundRenderer ();

	virtual bool IsNull() { return false; }
	virtual void SetSfxVolume (float volume) = 0;
	virtual void SetMusicVolume (float volume) = 0;
	virtual SoundHandle LoadSound(BYTE *sfxdata, int length) = 0;
	SoundHandle LoadSoundVoc(BYTE *sfxdata, int length);
	virtual SoundHandle LoadSoundRaw(BYTE *sfxdata, int length, int frequency, int channels, int bits, int loopstart, int loopend = -1) = 0;
	virtual void UnloadSound (SoundHandle sfx) = 0;	// unloads a sound from memory
	virtual unsigned int GetMSLength(SoundHandle sfx) = 0;	// Gets the length of a sound at its default frequency
	virtual unsigned int GetSampleLength(SoundHandle sfx) = 0;	// Gets the length of a sound at its default frequency
	virtual float GetOutputRate() = 0;

	// Streaming sounds.
	virtual SoundStream *CreateStream (SoundStreamCallback callback, int buffbytes, int flags, int samplerate, void *userdata) = 0;
    virtual SoundStream *OpenStream (std::auto_ptr<FileReader> reader, int flags) = 0;
	virtual SoundStream *OpenStream (const char *url, int flags);

	// Starts a sound.
	virtual FISoundChannel *StartSound (SoundHandle sfx, float vol, int pitch, int chanflags, FISoundChannel *reuse_chan) = 0;
	virtual FISoundChannel *StartSound3D (SoundHandle sfx, SoundListener *listener, float vol, FRolloffInfo *rolloff, float distscale, int pitch, int priority, const FVector3 &pos, const FVector3 &vel, int channum, int chanflags, FISoundChannel *reuse_chan) = 0;

	// Stops a sound channel.
	virtual void StopChannel (FISoundChannel *chan) = 0;

	// Changes a channel's volume.
	virtual void ChannelVolume (FISoundChannel *chan, float volume) = 0;

	// Marks a channel's start time without actually playing it.
	virtual void MarkStartTime (FISoundChannel *chan) = 0;

	// Returns position of sound on this channel, in samples.
	virtual unsigned int GetPosition(FISoundChannel *chan) = 0;

	// Gets a channel's audibility (real volume).
	virtual float GetAudibility(FISoundChannel *chan) = 0;

	// Synchronizes following sound startups.
	virtual void Sync (bool sync) = 0;

	// Pauses or resumes all sound effect channels.
	virtual void SetSfxPaused (bool paused, int slot) = 0;

	// Pauses or resumes *every* channel, including environmental reverb.
	enum EInactiveState
	{
		INACTIVE_Active,		// sound is active
		INACTIVE_Complete,		// sound is completely paused
		INACTIVE_Mute			// sound is only muted
	};
	virtual void SetInactive(EInactiveState inactive) = 0;

	// Updates the volume, separation, and pitch of a sound channel.
	virtual void UpdateSoundParams3D (SoundListener *listener, FISoundChannel *chan, bool areasound, const FVector3 &pos, const FVector3 &vel) = 0;

	virtual void UpdateListener (SoundListener *) = 0;
	virtual void UpdateSounds () = 0;
	virtual void UpdateMusic() {}

	virtual bool IsValid () = 0;
	virtual void PrintStatus () = 0;
	virtual void PrintDriversList () = 0;
	virtual FString GatherStats ();
	virtual short *DecodeSample(int outlen, const void *coded, int sizebytes, ECodecType type);

	virtual void DrawWaveDebug(int mode);

protected:
    virtual SoundDecoder *CreateDecoder(FileReader *reader);
};

extern SoundRenderer *GSnd;
extern bool nosfx;
extern bool nosound;

void I_InitSound ();
void I_ShutdownSound ();

void S_ChannelEnded(FISoundChannel *schan);
void S_ChannelVirtualChanged(FISoundChannel *schan, bool is_virtual);
float S_GetRolloff(FRolloffInfo *rolloff, float distance, bool logarithmic);
FISoundChannel *S_GetChannel(void *syschan);

extern ReverbContainer *DefaultEnvironments[26];

#endif
