#include "lwf_libsdl2_factory.h"
#include "lwf_libsdl2_textttf.h"
#include "lwf_core.h"
#include "lwf_data.h"
#include "lwf_text.h"
#include "lwf_utility.h"

#include <iostream>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

namespace LWF {
    
    static std::map<std::string, std::pair<stbtt_fontinfo, unsigned char*> > s_cachedFonts;

    class LWFTextTTF
    {
    public:
        SDL_Texture *m_texture;
        stbtt_fontinfo m_font;
        unsigned char* m_ttfFile;
        int m_offsetX;
        int m_offsetY;
        
    protected:
        Matrix m_matrix;
        
    private:
        SDL_Renderer *m_sdl;
        std::string m_text;
        int m_width;
        int m_height;
        int m_hAlignment;
        int m_vAlignment;
        float m_fontSize;
        
    public:
        LWFTextTTF(SDL_Renderer* sdl) :
        m_texture(NULL),
        m_offsetX(0),
        m_offsetY(0),
        m_matrix(),
        m_sdl(sdl),
        m_text(),
        m_width(0),
        m_height(0),
        m_hAlignment(0),
        m_vAlignment(0)
        {
            memset(&m_font, 0, sizeof(m_font));
        }
        
        ~LWFTextTTF()
        {
            if (m_texture)
            {
                SDL_DestroyTexture(m_texture);
            }
        }
        
        bool initWithString(const char *string,
                            const char *fontName,
                            float fontSize,
                            int width,
                            int height,
                            int hAlignment,
                            int vAlignment,
                            float red, float green, float blue)
        {
            m_width = width;
            m_height = height;
            m_hAlignment = hAlignment;
            m_vAlignment = vAlignment;
            m_fontSize = fontSize;
            
            char *basePath = SDL_GetBasePath();
            if (basePath)
            {
                std::string path = basePath;
                path += "res/Verdana.ttf";
                
                if (s_cachedFonts.count(path))
                {
                    m_font = s_cachedFonts[path].first;
                    m_ttfFile = s_cachedFonts[path].second;
                }
                else
                {
                    SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "r");
                    if (rw)
                    {
                        Sint64 result;
                        
                        result = SDL_RWseek(rw, 0, RW_SEEK_END);
                        Sint64 size = SDL_RWtell(rw);
                        result = SDL_RWseek(rw, 0, RW_SEEK_SET);
                        
                        m_ttfFile = static_cast<unsigned char*>(malloc(size));
                        if (m_ttfFile)
                        {
                            size_t bytesRead = SDL_RWread(rw, m_ttfFile, 1, size);
                            if (bytesRead != size)
                            {
                                SDL_RWclose(rw);
                                return true;
                            }
                            
                            int ret = stbtt_InitFont(&m_font, m_ttfFile, 0);
                            assert(ret == 1);

                            s_cachedFonts[path] = std::pair<stbtt_fontinfo, unsigned char*>(m_font, m_ttfFile);
                        }
                        
                        SDL_RWclose(rw);
                    }
                }

                setString(string);
            }
            
            return false;
        }
        
        static SDL_bool CharacterIsDelimiter(char c, const char *delimiters)
        {
            while (*delimiters) {
                if (c == *delimiters) {
                    return SDL_TRUE;
                }
                ++delimiters;
            }
            return SDL_FALSE;
        }

        int getLength(string text)
        {
            int ascent,baseline,ch=0;
            float scale, xpos=0;
            
            scale = stbtt_ScaleForPixelHeight(&m_font, m_fontSize);
            stbtt_GetFontVMetrics(&m_font, &ascent,0,0);
            baseline = (int) (ascent*scale);

            while (text[ch])
            {
                int advance,lsb;
                stbtt_GetCodepointHMetrics(&m_font, text[ch], &advance, &lsb);
                
                xpos += (advance * scale);
                if (text[ch+1])
                    xpos += scale*stbtt_GetCodepointKernAdvance(&m_font, text[ch],text[ch+1]);
                ++ch;
            }
            xpos += 1;
            return xpos;
        }
        
        void setString(const char* text_)
        {
            char* text = strdup(text_);
            
            if (m_texture)
            {
                SDL_DestroyTexture(m_texture);
                m_texture = NULL;
            }
            
            int width = 0, height = 0;
            int numLines;
            char *str, **strLines;
            
            int wrapLength = m_width;
            numLines = 1;
            str = NULL;
            strLines = NULL;

            width = getLength(text);
            if (width > wrapLength)
            {
                width = wrapLength;
            }
            
            if ( wrapLength > 0 && *text ) {
                const char *wrapDelims = " \t\r\n";
                int w;
                char *spot, *tok, *next_tok, *end;
                char delim;
                size_t str_len = SDL_strlen(text);
                
                numLines = 0;
                
                str = SDL_stack_alloc(char, str_len+1);
                if ( str == NULL ) {
                    return;
                }
                
                SDL_strlcpy(str, text, str_len+1);
                tok = str;
                end = str + str_len;
                do {
                    strLines = (char **)SDL_realloc(strLines, (numLines+1)*sizeof(*strLines));
                    if (!strLines) {
                        return;
                    }
                    strLines[numLines++] = tok;
                    
                    /* Look for the end of the line */
                    if ((spot = SDL_strchr(tok, '\r')) != NULL ||
                        (spot = SDL_strchr(tok, '\n')) != NULL) {
                        if (*spot == '\r') {
                            ++spot;
                        }
                        if (*spot == '\n') {
                            ++spot;
                        }
                    } else {
                        spot = end;
                    }
                    next_tok = spot;
                    
                    /* Get the longest string that will fit in the desired space */
                    for ( ; ; ) {
                        /* Strip trailing whitespace */
                        while ( spot > tok &&
                               CharacterIsDelimiter(spot[-1], wrapDelims) ) {
                            --spot;
                        }
                        if ( spot == tok ) {
                            if (CharacterIsDelimiter(*spot, wrapDelims)) {
                                *spot = '\0';
                            }
                            break;
                        }
                        delim = *spot;
                        *spot = '\0';
                        
                        w = getLength(tok);
                        if ((Uint32)w <= wrapLength) {
                            break;
                        } else {
                            /* Back up and try again... */
                            *spot = delim;
                        }
                        
                        while ( spot > tok &&
                               !CharacterIsDelimiter(spot[-1], wrapDelims) ) {
                            --spot;
                        }
                        if ( spot > tok ) {
                            next_tok = spot;
                        }
                    }
                    tok = next_tok;
                } while (tok < end);
            }
            else
            {
                numLines = 0;
                strLines = (char **)SDL_realloc(strLines, (numLines+1)*sizeof(*strLines));
                strLines[numLines++] = text;
            }
            
            int ascent,descent,lineGap,baseline;
            float xpos=0;
            
            float scale = stbtt_ScaleForPixelHeight(&m_font, m_fontSize);
            stbtt_GetFontVMetrics(&m_font, &ascent,&descent,&lineGap);
            baseline = (int) (ascent*scale);

            unsigned char *pixels = static_cast<unsigned char *>(calloc(m_width*m_height,1));
            for (int i = 0; i < numLines; ++i)
            {
                char* str = strLines[i];
                xpos = 0;
                int ch = 0;
                int h;
                while (str[ch])
                {
                    int advance,lsb,x0,y0,x1,y1;
                    float x_shift = xpos - (float) floor(xpos);
                    stbtt_GetCodepointHMetrics(&m_font, str[ch], &advance, &lsb);
                    stbtt_GetCodepointBitmapBoxSubpixel(&m_font, str[ch], scale,scale,x_shift,0, &x0,&y0,&x1,&y1);
                    h = y1-y0;
                    unsigned char* pos = pixels;
                    pos += (baseline+y0)*m_width;
                    pos += (int)xpos + x0;
                    stbtt_MakeCodepointBitmapSubpixel(&m_font, pos, x1-x0,y1-y0, m_width*1, scale,scale,x_shift,0, str[ch]);
                    
                    // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
                    // because this API is really for baking character bitmaps into textures. if you want to render
                    // a sequence of characters, you really need to render each bitmap to a temp buffer, then
                    // "alpha blend" that into the working buffer
                    xpos += (advance * scale);
                    if (str[ch+1])
                        xpos += scale*stbtt_GetCodepointKernAdvance(&m_font, str[ch],str[ch+1]);
                    ++ch;
                }
                
                baseline += (ascent*scale+h+descent*scale)+lineGap*scale;
            }
            
            unsigned char *pixelsRGBA8888 = static_cast<unsigned char *>(calloc(m_width*m_height,4));
            for (int y = 0; y < m_height; ++y)
            {
                for (int x = 0; x < m_width; ++x)
                {
                    if (pixels[y*m_width+x])
                    {
                        pixelsRGBA8888[y*m_width*4+x*4+0] = //0xff;
                        pixelsRGBA8888[y*m_width*4+x*4+1] = //0xff;
                        pixelsRGBA8888[y*m_width*4+x*4+2] = //0xff;
                        pixelsRGBA8888[y*m_width*4+x*4+3] = pixels[y*m_width+x];
                    }
                }
            }
            
            free(pixels);
            
            SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(pixelsRGBA8888, m_width, m_height, 4*8, m_width*4, 0xff, 0xff00, 0xff0000, 0xff000000);
            m_texture = SDL_CreateTextureFromSurface(m_sdl, surf);
            SDL_FreeSurface(surf);
            
            free(pixelsRGBA8888);
            
            switch (m_hAlignment)
            {
                case -1:
                    m_offsetX = 0;
                    break;
                case 0:
                    m_offsetX = m_width/2-width/2;
                    break;
                case 1:
                    m_offsetX = m_width - width;
                    break;
            }
            switch (m_vAlignment)
            {
                case -1:
                    m_offsetY = 0;
                    break;
                case 0:
                    m_offsetY = m_height/2-height/2;
                    break;
                case 1:
                    m_offsetY = m_height - height;
                    break;
            }
        }
    };
    
    LWFTextTTFRenderer::LWFTextTTFRenderer(LWF *l,
                                           Text *text,
                                           const char *fontName,
                                           SDL_Renderer *node)
	: TextRenderer(l), m_label(0), m_text(text)
    {
        const Format::Text &t = l->data->texts[text->objectId];
        const Color &c = l->data->colors[t.colorId];
        const Format::TextProperty &p = l->data->textProperties[t.textPropertyId];
        int hAlignment;
        int vAlignment;
        
        switch (p.align & Format::TextProperty::ALIGN_MASK) {
            default:
            case Format::TextProperty::LEFT:
                hAlignment = -1;
                break;
            case Format::TextProperty::RIGHT:
                hAlignment = 1;
                break;
            case Format::TextProperty::CENTER:
                hAlignment = 0;
                break;
        }
        
        switch (p.align & Format::TextProperty::VERTICAL_MASK) {
            default:
                vAlignment = -1;
                break;
            case Format::TextProperty::VERTICAL_BOTTOM:
                vAlignment = 0;
                break;
            case Format::TextProperty::VERTICAL_MIDDLE:
                vAlignment = 1;
                break;
        }
        
        std::string str = l->data->strings[t.stringId];
        
        m_label = new LWFTextTTF(node);
        m_label->initWithString(str.c_str(),
                                fontName, p.fontHeight, t.width, t.height, hAlignment, vAlignment,
                                c.red, c.green, c.blue);
        
        if (!m_label)
            return;
        
        m_factory = (LWFRendererFactory *)l->rendererFactory.get();
    }
    
    LWFTextTTFRenderer::~LWFTextTTFRenderer()
    {
    }
    
    void LWFTextTTFRenderer::Destruct()
    {
        if (!m_text->name.empty())
        {
            lwf->ClearTextRenderer(m_text->name);
        }
        
        if (!m_label)
            return;
        
        delete m_label;
        m_label = 0;
    }
    
    void LWFTextTTFRenderer::Update(const Matrix *matrix,
                                    const ColorTransform *colorTransform)
    {
//        std::cout << "update mult " << colorTransform->multi.red << " " << colorTransform->multi.green << " " << colorTransform->multi.blue << " " << colorTransform->multi.alpha << "\n";
//        std::cout << "update add  " << colorTransform->add.red << " " << colorTransform->add.green << " " << colorTransform->add.blue << " " << colorTransform->add.alpha << "\n";
    }
    
    void LWFTextTTFRenderer::Render(const Matrix *matrix,
                                    const ColorTransform *colorTransform,
                                    int renderingIndex,
                                    int renderingCount,
                                    bool visible)
    {
//        std::cout << "render mult " << colorTransform->multi.red << " " << colorTransform->multi.green << " " << colorTransform->multi.blue << " " << colorTransform->multi.alpha << "\n";
//        std::cout << "render add  " << colorTransform->add.red << " " << colorTransform->add.green << " " << colorTransform->add.blue << " " << colorTransform->add.alpha << "\n";
        
        if (!m_label)
            return;
        
        if (!visible)
            return;
        
        SDL_SetTextureColorMod(m_label->m_texture, colorTransform->multi.red*255, colorTransform->multi.green*255, colorTransform->multi.blue*255);
        SDL_SetTextureAlphaMod(m_label->m_texture, colorTransform->multi.alpha*255);
        SDL_SetTextureBlendMode(m_label->m_texture, m_factory->m_blendMode);

        Uint32 format;
        int access;
        int w;
        int h;
        SDL_QueryTexture(m_label->m_texture, &format, &access, &w, &h);
        
        SDL_Rect srcRect;
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.w = w;
        srcRect.h = h;
        
        SDL_Rect dstRect;
        dstRect.x = matrix->translateX+m_label->m_offsetX;
        dstRect.y = matrix->translateY+m_label->m_offsetY;
        dstRect.w = w;
        dstRect.h = h;
        
		bool md = Utility::GetMatrixDeterminant(matrix);
		float rotation;
		if (md)
			rotation = atan2f(matrix->skew1, -matrix->scaleX);
		else
			rotation = atan2f(matrix->skew1, matrix->scaleX);
		rotation = rotation / M_PI * 180.0f;
        
        SDL_Point center;
        center.x = 0;
        center.y = 0;
        
        SDL_RenderCopyEx(m_factory->m_sdl, m_label->m_texture, &srcRect, &dstRect, rotation, &center, SDL_FLIP_NONE);
    }
    
    void LWFTextTTFRenderer::SetText(string text)
    {
        if (!m_label)
            return;
        
        m_label->setString(text.c_str());
    }
    
}   // namespace LWF
