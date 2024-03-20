#include <maya/dataio.hpp>
#include <maya/texture.hpp>
#include <stb/stb_image.h>
#include <filesystem>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <fstream>
#include <cstring>
#include <minimp3/minimp3.h>
#include <algorithm>
#include <iterator>

#ifdef _MSC_VER
#pragma warning (disable: 4244)
#endif

namespace maya
{

void ImageData::Import(char const* path, int channels)
{
	Data.clear();

	MAYA_DIF(!std::filesystem::exists(path))
	{
		Error err(Error::FileNotFound, "ImageData::Import(char const*, int): Desired file \"");
		err.Details += path;
		err.Details += "\" not found";
		Error::Send(err);
	}

	stbi_set_flip_vertically_on_load(true);
	int ch;
	stbi_uc* dat = stbi_load(path, &Size.x, &Size.y, &ch, channels);
	Channels = channels == 0 || ch < channels ? ch : channels;

	if (!dat) [[unlikely]] {
		Error err(Error::UnsupportedFileFormat,
			"ImageData::Import(char const*, int): Error while loading image file \"");
		err.Details += path;
		err.Details += "\": ";
		err.Details += stbi_failure_reason();
		Error::Send(err);
	}

	Data.insert(Data.end(), dat, dat + Size.x * Size.y);
	stbi_image_free(dat);
}

static void s_LoadChars(RenderContext& rc, FT_Face face, FontData& font)
{
	FT_UInt index;
	FT_ULong charcode = FT_Get_First_Char(face, &index);

	RenderContext::QuietWait qw(rc);

	while (index != 0)
	{
		FT_Load_Glyph(face, index, FT_LOAD_RENDER);
		auto& map = face->glyph->bitmap;

		std::vector<std::uint32_t> image;
		image.reserve(map.width * map.rows);

		for (unsigned j = map.rows - 1; j != ~0u; j--) {
			auto x = &map.buffer[j * map.width];
			image.insert(image.end(), x, x + map.width);
		}

		rc.WaitForSyncExec([&, charcode, face]()
		{
			font.Data[charcode] = {
				.Texture = Texture::MakeUnique(rc),
				.Size = Ivec2(map.width, map.rows),
				.Bearing = Ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				.Advance = unsigned(face->glyph->advance.x >> 6)
			};

			auto& t = *font.Data.at(charcode).Texture;
			t.CreateContent(image.data(), Ivec2(map.width, map.rows), 4);
			t.SetClampToEdge();
			t.SetFilterLinear();
		});

		charcode = FT_Get_Next_Char(face, charcode, &index);
	}
}

void FontData::Import(char const* path, int pixelsize, RenderContext& rc)
{
	FT_Library ft;
	FT_Init_FreeType(&ft);
	FT_Face face;
	FT_New_Face(ft, path, 0, &face);
	FT_Set_Pixel_Sizes(face, 0, pixelsize);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	s_LoadChars(rc, face, *this);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void FontData::Import(ConstBuffer<void> data, int pixelsize, class RenderContext& rc)
{
	FT_Library ft;
	FT_Init_FreeType(&ft);
	FT_Face face;
	FT_New_Memory_Face(ft, static_cast<FT_Byte const*>(data.Data), data.Size, 0, &face);
	FT_Set_Pixel_Sizes(face, 0, pixelsize);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	s_LoadChars(rc, face, *this);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

// Warning: this assume little endian is employed in the system.
static Error s_ImportWav(char const* path, AudioData& audio)
{
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) return { Error::FileNotFound, "Required file \"" + stl::string(path) + "\" does not exists." };
	ifs.seekg(20, std::ios::cur); // skip useless header.

	uint16_t format, ch, bps;
	ifs.read(reinterpret_cast<char*>(&format), 2);
	ifs.read(reinterpret_cast<char*>(&ch), 2);
	ifs.read(reinterpret_cast<char*>(&audio.SampleRate), 4);
	ifs.seekg(6, std::ios::cur);
	ifs.read(reinterpret_cast<char*>(&bps), 2);
	audio.Channels = ch;

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
				audio.Samples.insert(audio.Samples.end(), tmp.begin(), tmp.end());
				for (auto& x: audio.Samples)
					x /= std::numeric_limits<int8_t>::max();

				break;
			}
			case 16: {
				std::vector<int16_t> tmp(capacity);
				ifs.read(reinterpret_cast<char*>(tmp.data()), datasize);
				audio.Samples.insert(audio.Samples.end(), tmp.begin(), tmp.end());
				for (auto& x : audio.Samples)
					x /= std::numeric_limits<int16_t>::max();
				break;
			}
			case 32: {

				std::vector<int32_t> tmp(capacity);
				ifs.read(reinterpret_cast<char*>(tmp.data()), datasize);
				audio.Samples.insert(audio.Samples.end(), tmp.begin(), tmp.end());
				for (auto& x : audio.Samples)
					x /= std::numeric_limits<int32_t>::max();
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
				ifs.read(reinterpret_cast<char*>(audio.Samples.data()), datasize);
				break;
			}
			case 64: {
				std::vector<double> tmp(capacity);
				ifs.read(reinterpret_cast<char*>(tmp.data()), datasize);
				audio.Samples.insert(audio.Samples.end(), tmp.begin(), tmp.end());
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

static Error s_ImportMp3(char const* path, AudioData& src)
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

	while (int sc = mp3dec_decode_frame(
		&decoder, data.data() + offset, static_cast<int>(data.size() - offset), pcm.data(), &frame_info))
	{
		if (!offset) {
			src.Channels = frame_info.channels;
			src.SampleRate = frame_info.hz;
		}
		src.Samples.insert(src.Samples.end(), pcm.begin(), pcm.begin() + sc * src.Channels);
		offset += frame_info.frame_bytes;
	}

	for (float& x: src.Samples)
		x /= std::numeric_limits<int16_t>::max();

	return {};
}

void AudioData::Import(char const* path)
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

}