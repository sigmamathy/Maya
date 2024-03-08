#include <maya/dataio.hpp>
#include <stb/stb_image.h>
#include <filesystem>

namespace fs = ::std::filesystem;

namespace maya
{

ImageImporter::ImageImporter(char const* path, int channels)
{
	MAYA_DIF(!fs::exists(path))
	{
		Error err (Error::MissingFile, "ImageImporter::ImageImporter(char const*, int): Desired file \"");
		err.Details += path;
		err.Details += "\" not found";
		Error::Send(err);
	}

	stbi_set_flip_vertically_on_load(true);
	int ch;
	Data = stbi_load(path, &Size.x, &Size.y, &ch, channels);
	Channels = channels == 0 || ch < channels ? ch : channels;

	if (!Data)
	{
		Error err (Error::ImageLoad,
			"ImageImporter::ImageImporter(char const*, int): Error while loading image file \"");
		err.Details += path;
		err.Details += "\"";
		Error::Send(err);
	}
}

ImageImporter::~ImageImporter()
{
	if (Data) stbi_image_free(Data);
}

}