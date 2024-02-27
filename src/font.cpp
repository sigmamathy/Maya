#include <maya/font.hpp>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

MayaFontUptr MayaCreateFontUptr(MayaWindow& window, MayaStringCR path, int pixelsize)
{
    return std::make_unique<MayaFont>(window, path, pixelsize);
}

MayaFontUptr MayaCreateFontUptr(MayaWindow& window, void* data, unsigned size, int pixelsize)
{
    return std::make_unique<MayaFont>(window, data, size, pixelsize);
}

MayaFontSptr MayaCreateFontSptr(MayaWindow& window, MayaStringCR path, int pixelsize)
{
    return std::make_shared<MayaFont>(window, path, pixelsize);
}

MayaFontSptr MayaCreateFontSptr(MayaWindow& window, void* data, unsigned size, int pixelsize)
{
    return std::make_shared<MayaFont>(window, data, size, pixelsize);
}

MayaFont::MayaFont(MayaWindow& window, MayaStringCR path, int pixelsize)
    : max_height(0)
{
    FT_Library ft;
    FT_Init_FreeType(&ft);
    FT_Face face;
    FT_New_Face(ft, path.c_str(), 0, &face);
    FT_Set_Pixel_Sizes(face, 0, pixelsize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 32; c < 128; c++)
    {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        auto& map = face->glyph->bitmap;

        std::vector<unsigned char> image;
        image.reserve(map.width * map.rows * 4);

        for (unsigned j = 0; j < map.rows; j++)
            for (unsigned i = 0; i < map.width; i++)
                for (unsigned k = 0; k < 4; k++)
                    image.emplace_back(map.buffer[(map.rows - j - 1) * map.width + i]);

        MayaTextureParameters param;
        param.Source = MayaTextureParameters::FromMemory;
        param.RawData.Data = image.data();
        param.RawData.Size = MayaIvec2(map.width, map.rows);
        param.RawData.Channels = 4;
        param.Repeat = false;
        textures[c] = MayaCreateTextureUptr(window, param);

        glyph_infos[c] = {
            .Texture = textures[c].get(),
            .Size = MayaIvec2(map.width, map.rows),
            .Bearing = MayaIvec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            .Advance = unsigned(face->glyph->advance.x >> 6)
        };

        if (face->glyph->bitmap_top > max_height)
            max_height = face->glyph->bitmap_top;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

MayaFont::MayaFont(MayaWindow& window, void* data, unsigned size, int pixelsize)
    : max_height(0)
{
    FT_Library ft;
    FT_Init_FreeType(&ft);
    FT_Face face;
    FT_New_Memory_Face(ft, static_cast<FT_Byte*>(data), size, 0, &face);
    FT_Set_Pixel_Sizes(face, 0, pixelsize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 32; c < 128; c++)
    {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        auto& map = face->glyph->bitmap;

        std::vector<unsigned char> image;
        image.reserve(map.width * map.rows * 4);

        for (unsigned j = 0; j < map.rows; j++)
            for (unsigned i = 0; i < map.width; i++)
                for (unsigned k = 0; k < 4; k++)
                    image.emplace_back(map.buffer[(map.rows - j - 1) * map.width + i]);

        MayaTextureParameters param;
        param.Source = MayaTextureParameters::FromMemory;
        param.RawData.Data = image.data();
        param.RawData.Size = MayaIvec2(map.width, map.rows);
        param.RawData.Channels = 4;
        textures[c] = MayaCreateTextureUptr(window, param);

        glyph_infos[c] = {
            .Texture = textures[c].get(),
            .Size = MayaIvec2(map.width, map.rows),
            .Bearing = MayaIvec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            .Advance = unsigned(face->glyph->advance.x >> 6)
        };

        if (face->glyph->bitmap_top > max_height)
            max_height = face->glyph->bitmap_top;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

MayaGlyphInfo const& MayaFont::operator[](char c) const
{
    return glyph_infos.at(c);
}

int MayaFont::GetMaxHeight() const
{
    return max_height;
}