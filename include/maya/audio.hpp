#pragma once

#include "./core.hpp"

namespace maya
{

// Play audio source on another thread.
class AudioPlayer
{
public:

	// Constructor.
	AudioPlayer();

	// Cleanup resources. Abort audio playing.
	~AudioPlayer();

	// No copy construct.
	AudioPlayer(AudioPlayer const&) = delete;
	AudioPlayer& operator=(AudioPlayer const&) = delete;

	// Set an audio source for the player to play.
	// One should not modify the source after this call.
	// Frames Per Buffer indicates the size of a buffer for each frame.
	void SetSource(struct AudioData const* src, unsigned framesperbuffer = 0x200);

	// Return the cuurent audio source, or nullptr if none.
	struct AudioData const* GetSource() const;

	// Get the frames per buffer.
	unsigned GetFramesPerBuffer() const;

	// Modify the volume, is used to multiply the samples.
	// By default is 1.0f.
	void SetVolume(float vol);

	// Retrieve the volume.
	float GetVolume() const;

	// Start and play the audio.
	// If already playing, this will interrupt and force to play.
	// pos indicates the seconds to start with.
	void Start(float pos = 0);

	// Resume where the player has stopped.
	// Would not interrupt if the audio is playing.
	void Resume();

	// Stop audio playing immediately.
	void Stop();

	// Check if an audio is playing currently.
	bool IsPlaying() const;

	// Get the current playing position in seconds.
	float GetPosition() const;

	// Get the duration of the audio track.
	float GetDuration() const;

	// Check whether the end is reached.
	bool IsEndReached() const;

private:

	void* nativeptr;
	stl::uptr<struct AudioStatus> status;
};

}