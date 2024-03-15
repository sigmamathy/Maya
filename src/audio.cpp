#include <maya/audio.hpp>
#include <portaudio.h>
#include <atomic>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <minimp3/minimp3.h>

namespace maya
{

// Warning: this assume little endian is employed in the system.
static Error s_ImportWav(char const* path, AudioSource& src)
{
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) return { Error::FileNotFound, "Required file \"" + stl::string(path) + "\" does not exists."};
	ifs.seekg(20, std::ios::cur); // skip useless header.

	uint16_t format, ch, bps;
	ifs.read(reinterpret_cast<char*>(&format), 2);
	ifs.read(reinterpret_cast<char*>(&ch), 2);
	ifs.read(reinterpret_cast<char*>(&src.SampleRate), 4);
	ifs.seekg(6, std::ios::cur);
	ifs.read(reinterpret_cast<char*>(&bps), 2);
	src.Channels = ch;

	char name[5];
	ifs.read(name, 4); // this may be "data" or "LIST"
	name[4] = '\0';
	if (strcmp(name, "LIST") == 0) { // if LIST chunk presents
		uint32_t toskip;
		ifs.read(reinterpret_cast<char*>(&toskip), 4);
		ifs.seekg(toskip + 4, std::ios::cur);
	}

	uint32_t datasize; // num samples * channels * bits per sample / 8
	ifs.read(reinterpret_cast<char*>(&datasize), 4);
	unsigned capacity = datasize / bps * 8;

	if (format == 1) // Pulse Code Modulation
	{
		switch (bps)
		{
			case 8: {
				std::vector<int8_t> tmp(capacity);
				ifs.read(reinterpret_cast<char*>(tmp.data()), datasize);
				std::transform(tmp.begin(), tmp.end(), std::back_inserter(src.Samples),
					[](int8_t element) { return static_cast<float>(element) / std::numeric_limits<int8_t>::max(); });
				break;
			}
			case 16: {
				std::vector<int16_t> tmp(capacity);
				ifs.read(reinterpret_cast<char*>(tmp.data()), datasize);
				std::transform(tmp.begin(), tmp.end(), std::back_inserter(src.Samples),
					[](int16_t element) { return static_cast<float>(element) / std::numeric_limits<int16_t>::max(); });
				break;
			}
			case 32: {
				std::vector<int32_t> tmp(capacity);
				ifs.read(reinterpret_cast<char*>(tmp.data()), datasize);
				std::transform(tmp.begin(), tmp.end(), std::back_inserter(src.Samples),
					[](int32_t element) { return static_cast<float>(element) / std::numeric_limits<int32_t>::max(); });
				break;
			}
			default: {
				return { Error::UnsupportedFileFormat,
					".wav with Pulse Code Modulation and " + std::to_string(bps) + " bits per sample is not supported." };
			}
		}
	}
	else if (format == 3) // IEEE floating point
	{
		switch (bps)
		{
			case 32: {
				ifs.read(reinterpret_cast<char*>(src.Samples.data()), datasize);
				break;
			}
			case 64: {
				std::vector<double> tmp(capacity);
				ifs.read(reinterpret_cast<char*>(tmp.data()), datasize);
				std::transform(tmp.begin(), tmp.end(), std::back_inserter(src.Samples),
					[](double element) { return static_cast<float>(element); });
				break;
			}
			default: {
				return { Error::UnsupportedFileFormat,
					".wav audio with IEEE 754 and " + std::to_string(bps) + " bits per sample is not supported." };
			}
		}
	}
	else
	{
		return { Error::UnsupportedFileFormat,
			"Unknown .wav format (" + std::to_string(format) + ")" };
	}

	return {};
}

static Error s_ImportMp3(char const* path, AudioSource& src)
{
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) return { Error::FileNotFound, "Required file \"" + stl::string(path) + "\" does not exists." };
	std::vector<uint8_t> data(std::filesystem::file_size(path));
	ifs.read(reinterpret_cast<char*>(data.data()), data.size());
	ifs.close();

	mp3dec_t decoder;
	mp3dec_init(&decoder);

	std::array<int16_t, MINIMP3_MAX_SAMPLES_PER_FRAME> pcm;  // PCM audio buffer
	mp3dec_frame_info_t frame_info;
	std::size_t offset = 0;

	int sc = mp3dec_decode_frame(&decoder, data.data(), static_cast<int>(data.size()), pcm.data(), &frame_info);
	src.Channels = frame_info.channels;
	src.SampleRate = frame_info.hz;
	src.Samples.insert(src.Samples.end(), pcm.begin(), pcm.begin() + sc * src.Channels);
	for (unsigned i = 0; i < sc * src.Channels; ++i)
		src.Samples[i] /= std::numeric_limits<int16_t>::max();
	offset += frame_info.frame_bytes;

	while (int sc = mp3dec_decode_frame(
		&decoder, data.data() + offset, static_cast<int>(data.size() - offset), pcm.data(), &frame_info))
	{
		auto x = src.Samples.size();
		src.Samples.insert(src.Samples.end(), pcm.begin(), pcm.begin() + sc * src.Channels);
		for (unsigned i = 0; i < sc * src.Channels; ++i)
			src.Samples[i + x] /= std::numeric_limits<int16_t>::max();
		offset += frame_info.frame_bytes;
	}

	return {};
}

void AudioSource::ReadFile(char const* path)
{
	namespace fs = std::filesystem;
	Error err;
	auto ext = fs::path(path).extension();

	if (ext == ".wav") err = s_ImportWav(path, *this);
	else if (ext == ".mp3") err = s_ImportMp3(path, *this);

	else { // not supported
		err.ErrorCode = Error::UnsupportedFileFormat;
		err.Details = "AudioSource::ReadFile(char const*): Unknown audio file format found.";
		Error::Send(err);
		return;
	}

	if (err) { // error found.
		err.Details.insert(0, "AudioSource::ReadFile(char const*): ");
		Error::Send(err);
	}
}

struct AudioStatus
{
	AudioSource const* Source;
	unsigned FramesPerBuffer;
	std::atomic<unsigned> SamplePosition;
	std::atomic<float> Volume;
};

static int s_AudioOutputCallback(const void* input_buffer, void* output_buffer,
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
		paNoFlag, s_AudioOutputCallback, status.get());

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

bool AudioPlayer::IsEndReached() const
{
	return status->SamplePosition == status->Source->Samples.size();
}

}