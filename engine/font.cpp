#include "common/fontfile.h"
#include "common/fileio.h"
#include "common/rle.h"
#include "common/log.h"
#include "common/Canvas.h"
#include "video/Driver.h"
#include "video/Image.h"

#include "font.h"

#include <cassert>

namespace Ika {
    static const char subsetMarker = '~';
    static const char colourMarker = '#';

    Font::Font(const std::string& filename, Video::Driver* v)
        : _video(v)
        , _width(0)
        , _height(0)
        , _tabSize(30)
        , _letterSpacing(0)
        , _wordSpacing(0)
        , _lineSpacing(0)
    {
        CDEBUG("cfont::loadfnt");

        if (!_fontFile.Load(filename.c_str())) {
            throw FontException();
        }

        _glyphs.resize(_fontFile.NumGlyphs());

        for (uint i = 0; i < _fontFile.NumGlyphs(); i++) {
            const Canvas& glyph = _fontFile.GetGlyph(i);
            _width = max<uint>(_width, glyph.Width());
            _height = max<uint>(_height, glyph.Height());
        }
    }

    Font::~Font() {
        for (uint i = 0; i < _glyphs.size(); i++) {
            _video->FreeImage(_glyphs[i]);
        }

        _glyphs.clear();
    }

    uint Font::GetGlyphIndex(char c, uint subset) const {
        if (subset >= 0 && subset < _fontFile.NumSubSets()) {
            return _fontFile.GetSubSet(subset).glyphIndex[(int)c];
        } else {
            return 0;
        }
    }

    Video::Image* Font::GetGlyphImage(char c, uint subset) {
        uint glyphIndex = GetGlyphIndex(c, subset);

        if (glyphIndex >= _glyphs.size()) {
            return 0;
        }

        Video::Image* img = _glyphs[glyphIndex];

        if (!img) {
            img = _video->CreateImage(const_cast<Canvas&>(_fontFile.GetGlyph(glyphIndex)));
            _glyphs[glyphIndex] = img;
        }

        return img;
    }

    const Canvas& Font::GetGlyphCanvas(char c, uint subset) const {
        return _fontFile.GetGlyph(GetGlyphIndex(c, subset));
    }

    void Font::PrintChar(int& x, int y, uint subset, char c, RGBA colour) {
        Video::Image* img = GetGlyphImage(c, subset);

        if (img) {
            _video->TintBlitImage(img, x, y, colour.i);
            x += img->Width() + _letterSpacing;
            if (c == ' ')
                x += _wordSpacing;
        }
    }
    
    void Font::PrintChar(int& x, int y, uint subset, char c, RGBA /*colour*/, Canvas& dest, Video::BlendMode blendMode) {
        //if (c < 0 || c > 96) {
        //    return;
        //}

        assert(GetGlyphIndex(c, subset) < _fontFile.NumGlyphs());  // paranoia check

        Canvas& glyph = const_cast<Canvas&>(GetGlyphCanvas(c, subset));

        switch (blendMode) {
            default:
            case Video::None:
                Blitter::Blit(glyph, dest, x, y, Blitter::OpaqueBlend());
                break;
            case Video::Add:
                Blitter::Blit(glyph, dest, x, y, Blitter::AddBlend());
                break;
            case Video::Matte:
                Blitter::Blit(glyph, dest, x, y, Blitter::MatteBlend());
                break;
            case Video::Normal:
                Blitter::Blit(glyph, dest, x, y, Blitter::AlphaBlend());
                break;
            case Video::Subtract:
                Blitter::Blit(glyph, dest, x, y, Blitter::SubtractBlend());
                break;
        }

        x += glyph.Width() + _letterSpacing;
        
        if (c == ' ')
            x += _wordSpacing;
    }

    template <typename Printer>
    void Font::PaintString(int startx, int starty, const std::string& s, Printer& print) {
        int cursubset = 0;
        int x = startx;
        int y = starty;
        uint len = s.length();
        RGBA colour(255, 255, 255, 255);

        for (uint i = 0; i < len; i++) {
            switch (s[i]) {
                case '\n': {  // newline
                    y += int(_height) + _lineSpacing;
                    x = startx;
                    continue;
                }

                case '\t': {  // tab
                    x += _tabSize - (x - startx) % _tabSize;
                    continue;
                }

                case subsetMarker: {
                    i++;

                    if (i >= len) {
                        break;  // Subset marker at end of string.  Just print it.
                    }

                    if (i < len && s[i] >= '0' && s[i] <= '0' + static_cast<char>(_fontFile.NumSubSets())) {
                        // ~ followed by a digit is not printed.  the subset is instead changed.
                        cursubset = s[i] - '0';
                        continue;
                    } else {
                        // ~ followed by anything else (or nothing at all) is printed like any other character.
                        i--;
                        break;
                    }
                }

                case colourMarker: {
                        i++;
                        uint pos = s.find(']', i);
                        if (i >= len || s[i] != '[' || pos == std::string::npos) {
                            i--;
                            break;
                        }

                        std::string t(s.substr(i + 1, pos - i - 1));

                        if (_video->hasColour(t)) {
                            colour = _video->getColour(t);
                        } else if (isHexNumber(t)) {
                            t.reserve(8);
                            switch (t.length()) {
                                /*
                                 * Shorthand rules for hex colours:
                                 *  8 digits:  RRGGBBAA
                                 *  6 digits:  RRGGBB   Alpha is always full.
                                 *  4 digits:  RGBA     Same as if each digit were repeated:  ABCD is shorthand for AABBCCDD
                                 *  3 digits:  RGB      RGB digits are doubled, alpha is full.
                                 */
                                case 3:
                                    t += 'F'; 
                                    // fall-through
                                case 4:
                                    // #[rgba] becomes #[rrggbbaa]
                                    t.resize(8);
                                    t[7] = t[6] = t[3];
                                    t[5] = t[4] = t[2];
                                    t[3] = t[2] = t[1];
                                    t[1] = t[0];
                                    colour = RGBA(hexToInt(t));
                                    break;
                                case 6:
                                    colour = hexToInt(t); colour.a = 255;
                                    break;
                                case 8:
                                    colour = hexToInt(t);
                                    break;
                                default: 
                                    colour = RGBA(255, 255, 255); // malformed colour string: White.
                                    break;
                            }
                        } else {
                            colour = RGBA(255, 255, 255);
                        }

                        i = pos;
                        continue;
                    }
            };

            // if execution gets here, we found no control character
            print(x, y, cursubset, s[i], colour, this);
        }
    }

    namespace {
        struct PrintToVideo {
            inline void operator ()(int& x, int y, int subset, char c, RGBA colour, Font* font) {
                font->PrintChar(x, y, subset, c, colour);
            }
        };

        struct PrintToCanvas {
            Canvas& _dest;
            Video::BlendMode _blendMode;

            PrintToCanvas(Canvas& dest, Video::BlendMode blendMode)
                : _dest(dest)
                , _blendMode(blendMode)
            {}

            inline void operator ()(int& x, int y, int subset, char c, RGBA colour, Font* font) {
                font->PrintChar(x, y, subset, c, colour, _dest, _blendMode);
            }
        };

        struct CountWidth {
            int width;

            CountWidth()
                : width(0)
            {}

            inline void operator ()(int& x, int /*y*/, int subset, char c, RGBA /*colour*/, Font* font) {
                const Canvas& glyph = font->GetGlyphCanvas(c, subset);
                x += glyph.Width() + font->LetterSpacing();
                if (c == ' ')
                    x += font->WordSpacing();
                width = max(width, x);
            }
        };
        
        struct CountHeight {
            int height;

            CountHeight()
                : height(0)
            {}

            inline void operator ()(int /*x*/, int& y, int subset, char c, RGBA /*colour*/, Font* font) {
                if (y == 0)
                    y += font->Height() + font->LineSpacing();
                if (c == '\n')
                    y += font->Height() + font->LineSpacing();
                height = max(height, y);
            }
        };        
    }

    void Font::PrintString(int x, int y, const std::string& s) {
        PrintToVideo printer;
        _video->SetBlendMode(Video::Normal);
        PaintString(x, y, s, printer);
    }

    void Font::PrintString(int x, int y, const std::string& s, Canvas& dest, Video::BlendMode blendMode) {
        PrintToCanvas printer(dest, blendMode);
        PaintString(x, y, s, printer);
    }

    int Font::StringWidth(const std::string& s) {
        CountWidth counter;
        PaintString(0, 0, s, counter);
        return counter.width;
    }
    
    int Font::StringHeight(const std::string& s) {
        CountHeight counter;
        PaintString(0, 0, s, counter);
        return counter.height;
    }    

}
