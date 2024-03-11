#pragma once

#include "./core.hpp"

namespace maya
{

struct AudioSource
{
	stl::list<float> Samples;
	unsigned SampleRate;
	unsigned Channels;

	void ReadFile(char const* path);
};

class AudioPlayer
{
public:

	AudioPlayer();

	~AudioPlayer();

	AudioPlayer(AudioPlayer const&) = delete;
	AudioPlayer& operator=(AudioPlayer const&) = delete;

	void SetSource(AudioSource const* src, unsigned framesperbuffer = 0x200);

	AudioSource const* GetSource() const;

	unsigned GetFramesPerBuffer() const;

	void SetVolume(float vol);

	float GetVolume() const;

	void Start(float pos = 0);

	void Resume();

	void Stop();

	bool IsPlaying() const;

	float GetPosition() const;

	float GetDuration() const;

private:

	void* nativeptr;
	stl::uptr<struct AudioStatus> status;
};

}