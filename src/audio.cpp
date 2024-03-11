#include <maya/audio.hpp>
#include <sndfile.h>
#include <portaudio.h>
#include <atomic>

namespace maya
{

void AudioSource::ReadFile(char const* path)
{
	SF_INFO info;
	SNDFILE* file = sf_open(path, SFM_READ, &info);
	Samples.resize(info.frames * info.channels);
	sf_readf_float(file, Samples.data(), info.frames);
	sf_close(file);
	SampleRate = info.samplerate;
	Channels = info.channels;
}

struct AudioStatus
{
	AudioSource const* Source;
	unsigned FramesPerBuffer;
	std::atomic<unsigned> SamplePosition;
	std::atomic<float> Volume;
};

static int PortAudio_audioCallback(const void* input_buffer, void* output_buffer,
								unsigned long frames_per_buffer,
								const PaStreamCallbackTimeInfo* time_info,
								PaStreamCallbackFlags status_flags,
								void* userdata)
{
	float* out = static_cast<float*>(output_buffer);
	AudioStatus* status = static_cast<AudioStatus*>(userdata);
	unsigned int remaining = static_cast<unsigned>(status->Source->Samples.size() - status->SamplePosition);

	if (remaining >= frames_per_buffer * status->Source->Channels)
	{
		unsigned pos = status->SamplePosition;
		for (unsigned int i = 0; i < frames_per_buffer * status->Source->Channels; i++)
			out[i] = status->Source->Samples[pos++] * status->Volume;
		status->SamplePosition = pos;
		return paContinue;
	}

	// The end is reached. Play the remaining samples.
	unsigned pos = status->SamplePosition;
	for (unsigned int i = 0; i < remaining; i++)
		out[i] = status->Source->Samples[pos++] * status->Volume;
	status->SamplePosition = pos;

	return paComplete;
}

AudioPlayer::AudioPlayer()
	: nativeptr(0)
{
	status = std::make_unique<AudioStatus>();
	status->Source = 0;
	status->Volume = 1.f;
}

AudioPlayer::~AudioPlayer()
{
	if (nativeptr)
		Pa_CloseStream(static_cast<PaStream*>(nativeptr));
}

void AudioPlayer::SetSource(AudioSource const* src, unsigned framesperbuffer)
{
	if (status->Source == src)
		return;

	if (nativeptr) {
		Pa_CloseStream(static_cast<PaStream*>(nativeptr));
	}

	if (!src) {
		nativeptr = 0;
		status->Source = 0;
		return;
	}

	PaStreamParameters outputParameters;
	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = src->Channels;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	status->Source = src;
	status->SamplePosition = 0;
	status->FramesPerBuffer = framesperbuffer;

	PaStream* stream;
	Pa_OpenStream(&stream, NULL, &outputParameters, src->SampleRate, framesperbuffer,
		paNoFlag, PortAudio_audioCallback, status.get());

	nativeptr = stream;
}

AudioSource const* AudioPlayer::GetSource() const
{
	return status->Source;
}

unsigned AudioPlayer::GetFramesPerBuffer() const
{
	return status->FramesPerBuffer;
}

void AudioPlayer::SetVolume(float vol)
{
	status->Volume = vol;
}

float AudioPlayer::GetVolume() const
{
	return status->Volume;
}

void AudioPlayer::Start(float pos)
{
	status->SamplePosition = static_cast<unsigned>(pos * status->Source->SampleRate * status->Source->Channels);
	if (!IsPlaying())
		Pa_StartStream(static_cast<PaStream*>(nativeptr));
}

void AudioPlayer::Resume()
{
	if (!IsPlaying())
		Pa_StartStream(static_cast<PaStream*>(nativeptr));
}

void AudioPlayer::Stop()
{
	Pa_StopStream(static_cast<PaStream*>(nativeptr));
}

bool AudioPlayer::IsPlaying() const
{
	return Pa_IsStreamActive(static_cast<PaStream*>(nativeptr));
}

float AudioPlayer::GetPosition() const
{
	auto& src = status->Source;
	return (float) status->SamplePosition / src->SampleRate / src->Channels;
}

float AudioPlayer::GetDuration() const
{
	auto& src = status->Source;
	return (float) src->Samples.size() / src->SampleRate / src->Channels;
}

}