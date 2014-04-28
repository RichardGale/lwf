#ifndef LWF_COCOS2DX_BITMAP_H
#define LWF_COCOS2DX_BITMAP_H

#include "SDL.h"

#include "lwf_renderer.h"

namespace LWF {
    
    class LWFBitmap;
    class LWFRendererFactory;
    
    class LWFBitmapRenderer : public Renderer
    {
    protected:
        LWFRendererFactory *m_factory;
        LWFBitmap *m_sprite;
        
    public:
        LWFBitmapRenderer(LWF *l, Bitmap *bitmap, SDL_Renderer *node);
        LWFBitmapRenderer(LWF *l, BitmapEx *bitmapEx, SDL_Renderer *node);
        
        void Destruct();
        void Update(const Matrix *matrix, const ColorTransform *colorTransform);
        void Render(const Matrix *matrix, const ColorTransform *colorTransform,
                    int renderingIndex, int renderingCount, bool visible);
    };
    
}   // namespace LWF

#endif
