#include <maya/font.hpp>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

MayaFontUptr MayaCreateFontUptr(MayaWindow& window, MayaStringCR path, int pixelsize)
{
    return std::make_unique<MayaFont>(window, path, pixelsize);
}

MayaFontSptr MayaCreateFontSptr(MayaWindow& window, MayaStringCR path, int pixelsize)
{
    return std::make_shared<MayaFont>(window, path, pixelsize);
}

MayaFont::MayaFont(MayaWindow& window, MayaStringCR path, int pixelsize)
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

        textures[c] = MayaCreateTextureUptr(window, image.data(), MayaIvec2(map.width, map.rows), 4);
        glyph_infos[c] = {
            .Texture = textures[c].get(),
            .Size = MayaIvec2(map.width, map.rows),
            .Bearing = MayaIvec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            .Advance = unsigned(face->glyph->advance.x >> 6)
        };
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

MayaGlyphInfo const& MayaFont::operator[](char c) const
{
    return glyph_infos.at(c);
}