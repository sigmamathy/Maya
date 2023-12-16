#include <Maya/saveio.hpp>
#include <cstdio>
#include <fstream>

MayaSaveIO::MayaSaveIO(std::string const& filepath)
	: filepath(filepath)
{
}

// -1: Not found
// -2: Empty or invalid
// 0: Text based
// > 0: binary obfuscated
static int GetFileMode(char const* filepath)
{
    FILE* file = std::fopen(filepath, "r");
    if (!file)
        return -1;
    int first = std::fgetc(file);
    if (first == EOF) {
        std::fclose(file);
        return -2;
    }
    if (first != 0) {
        std::fclose(file);
        return 0;
    }
    int mode;
    if (std::fread(&mode, sizeof(int), 1, file) != 1) {
        std::fclose(file);
        return -2;
    }
    return mode;
}

static void TextRead(MayaSaveSlot& slot, std::string const path)
{
    std::ifstream ifs(path);
    // ...
    ifs.close();
}

void MayaSaveIO::Import(MayaSaveSlot& slot)
{
    int mode = GetFileMode(filepath.c_str());
#if MAYA_DEBUG
    if (mode == -1) 
        MAYA_DERR(MAYA_MISSING_FILE_ERROR,
            "MayaSaveIO::Import(MayaSaveSlot&): Required file cannot be found.");
#endif
    if (mode == -2)
        return;
    if (mode == 0) {
        TextRead(slot, filepath);
        return;
    }
    // ...
}