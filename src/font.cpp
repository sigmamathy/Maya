#include <maya/font.hpp>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace maya
{

Font::Font()
    : ft(0), face(0), max_height(0)
{
    glyphs.reserve(96);
}

Font::~Font()
{
    CloseStream();
}

void Font::OpenFileStream(char const* path, int pixelsize)
{
    FT_Library ft;
    FT_Init_FreeType(&ft);
    FT_Face face;
    FT_New_Face(ft, path, 0, &face);
    FT_Set_Pixel_Sizes(face, 0, pixelsize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    this->ft = ft;
    this->face = face;
}

void Font::OpenMemoryStream(ConstBuffer<void> data, int pixelsize)
{
    FT_Library ft;
    FT_Init_FreeType(&ft);
    FT_Face face;
    FT_New_Memory_Face(ft, static_cast<FT_Byte const*>(data.Data), data.Size, 0, &face);
    FT_Set_Pixel_Sizes(face, 0, pixelsize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    this->ft = ft;
    this->face = face;
}

void Font::CloseStream()
{
    if (!ft) return;
    FT_Done_Face(static_cast<FT_Face>(face));
    FT_Done_FreeType(static_cast<FT_Library>(ft));
    ft = 0;
}

void Font::EmptyChar(unsigned c)
{
    glyphs[c];
}

void Font::LoadChar(unsigned c, RenderContext& rc)
{
    FT_Face face = static_cast<FT_Face>(this->face);
    FT_Load_Char(face, c, FT_LOAD_RENDER);
    auto& map = face->glyph->bitmap;

    std::vector<unsigned char> image;
    image.reserve(map.width * map.rows * 4);

    for (unsigned j = 0; j < map.rows; j++)
        for (unsigned i = 0; i < map.width; i++)
            for (unsigned k = 0; k < 4; k++)
                image.emplace_back(map.buffer[(map.rows - j - 1) * map.width + i]);

    glyphs[c] = {
        .Texture = Texture::MakeUnique(rc),
        .Size = Ivec2(map.width, map.rows),
        .Bearing = Ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        .Advance = unsigned(face->glyph->advance.x >> 6)
    };

    auto& t = *glyphs.at(c).Texture;
    t.CreateContent(image.data(), Ivec2(map.width, map.rows), 4);
    t.SetClampToEdge();
    t.SetFilterLinear();

    if (face->glyph->bitmap_top > max_height)
        max_height = face->glyph->bitmap_top;
}

void Font::LoadAsciiChars(RenderContext& rc)
{
    for (char c = 32; c < 127; c++)
        LoadChar(c, rc);
}

bool Font::IsCharLoaded(unsigned c) const
{
    return glyphs.contains(c);
}

Glyph const& Font::operator[](unsigned c) const
{
    return glyphs.at(c);
}

Glyph& Font::operator[](unsigned c)
{
    return glyphs.at(c);
}

int Font::GetMaxHeight() const
{
    return max_height;
}

}