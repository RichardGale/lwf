#include "SDL.h"
//#include "SDL_image.h"

#define STBI_HEADER_FILE_ONLY
#include "stb/stb_image.c"

#include "lwf_bitmap.h"
#include "lwf_bitmapex.h"
#include "lwf_libsdl2_bitmap.h"
#include "lwf_libsdl2_factory.h"
#include "lwf_core.h"
#include "lwf_data.h"
#include "lwf_utility.h"

namespace LWF {
    
    class LWFBitmap
    {
    public:
        Matrix m_matrix;
        SDL_Texture *m_texture;
        
    public:
        static LWFBitmap *create(const char *filename,
                                 const Format::Texture &texture,
                                 const Format::TextureFragment &fragment,
                                 const Format::BitmapEx &bitmapEx,
                                 SDL_Renderer *sdl)
        {
            LWFBitmap *bitmap = new LWFBitmap();
            if (bitmap && bitmap->initWithFileEx(filename, texture, fragment, bitmapEx, sdl)) {
                return bitmap;
            }
            delete bitmap;
            return NULL;
        }
        
        bool initWithFileEx(const char *filename,
                            const Format::Texture &t,
                            const Format::TextureFragment &f,
                            const Format::BitmapEx &bx,
                            SDL_Renderer *sdl)
        {
            if (m_texture)
            {
                SDL_DestroyTexture(m_texture);
                m_texture = NULL;
            }
            
            int x;
            int y;
            int comp;
            stbi_uc* image = stbi_load(filename, &x, &y, &comp, STBI_rgb_alpha);
            if (image)
            {
                m_texture = SDL_CreateTexture(sdl, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, x, y);
                if (m_texture)
                {
                    SDL_Rect rect;
                    rect.x = 0;
                    rect.y = 0;
                    rect.w = x;
                    rect.h = y;
                    SDL_UpdateTexture(m_texture, &rect, image, x*4);
                }
                
                stbi_image_free(image);
            }
            
            return (m_texture != NULL);
        }
    };
    
    LWFBitmapRenderer::LWFBitmapRenderer(LWF *l,
                                         Bitmap *bitmap,
                                         SDL_Renderer *node)
	: Renderer(l), m_sprite(0)
    {
        const Format::Bitmap &b = l->data->bitmaps[bitmap->objectId];
        if (b.textureFragmentId == -1)
            return;
        
        Format::BitmapEx bx;
        bx.matrixId = b.matrixId;
        bx.textureFragmentId = b.textureFragmentId;
        bx.u = 0;
        bx.v = 0;
        bx.w = 1;
        bx.h = 1;
        
        const Format::TextureFragment &f = l->data->textureFragments[b.textureFragmentId];
        const Format::Texture &t = l->data->textures[f.textureId];
        string texturePath = t.GetFilename(l->data.get());
        m_factory = (LWFRendererFactory *)l->rendererFactory.get();
        string filename = m_factory->m_basePath + texturePath;
        
        if (LWF::GetTextureLoadHandler())
        {
            filename = LWF::GetTextureLoadHandler()(filename, m_factory->m_basePath, texturePath);
        }
        
        m_sprite = LWFBitmap::create(filename.c_str(), t, f, bx, m_factory->m_sdl);
        if (!m_sprite)
            return;
        
        l->data->resourceCache[filename] = true;
    }
    
    LWFBitmapRenderer::LWFBitmapRenderer(LWF *l,
                                         BitmapEx *bitmapEx,
                                         SDL_Renderer *node)
	: Renderer(l), m_sprite(0)
    {
        const Format::BitmapEx &bx = l->data->bitmapExs[bitmapEx->objectId];
        if (bx.textureFragmentId == -1)
            return;
        
        const Format::TextureFragment &f =
		l->data->textureFragments[bx.textureFragmentId];
        const Format::Texture &t = l->data->textures[f.textureId];
        string texturePath = t.GetFilename(l->data.get());
        m_factory = (LWFRendererFactory *)l->rendererFactory.get();
        string filename = m_factory->m_basePath + texturePath;
        
        if (LWF::GetTextureLoadHandler())
            filename = LWF::GetTextureLoadHandler()(
                                                    filename, m_factory->m_basePath, texturePath);
        
        m_sprite = LWFBitmap::create(filename.c_str(), t, f, bx, m_factory->m_sdl);
        if (!m_sprite)
            return;
    }
    
    void LWFBitmapRenderer::Destruct()
    {
        if (!m_sprite)
            return;
        
        SDL_DestroyTexture(m_sprite->m_texture);
        delete m_sprite;
        m_sprite = nullptr;
    }
    
    void LWFBitmapRenderer::Update(const Matrix *matrix,
                                   const ColorTransform *colorTransform)
    {
    }
    
    void LWFBitmapRenderer::Render(const Matrix *matrix,
                                   const ColorTransform *colorTransform,
                                   int renderingIndex,
                                   int renderingCount,
                                   bool visible)
    {
        if (!m_sprite)
            return;
        
        if (!visible)
            return;
        
        SDL_SetTextureColorMod(m_sprite->m_texture, colorTransform->multi.red*255, colorTransform->multi.green*255, colorTransform->multi.blue*255);
        SDL_SetTextureAlphaMod(m_sprite->m_texture, colorTransform->multi.alpha*255);
        SDL_SetTextureBlendMode(m_sprite->m_texture, m_factory->m_blendMode);

        Uint32 format;
        int access;
        int w;
        int h;
        SDL_QueryTexture(m_sprite->m_texture, &format, &access, &w, &h);
        
        SDL_Rect srcRect;
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.w = w;
        srcRect.h = h;
        
        SDL_Rect dstRect;
        dstRect.x = matrix->translateX;
        dstRect.y = matrix->translateY;
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
        
        SDL_RenderCopyEx(m_factory->m_sdl, m_sprite->m_texture, &srcRect, &dstRect, rotation, &center, SDL_FLIP_NONE);

#if 0
        m_sprite->setZOrder(renderingIndex);
#endif
    }
    
}   // namespace LWF
