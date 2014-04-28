#ifndef LWF_LIBSDL2_FACTORY_H
#define LWF_LIBSDL2_FACTORY_H

#include <cassert>

#include "SDL.h"

#include "lwf_renderer.h"

namespace LWF {
    
    class LWFRendererFactory : public IRendererFactory
    {
    public:
        string m_basePath;
        SDL_Renderer *m_sdl;
        SDL_BlendMode m_blendMode;
        int m_maskMode;

        LWFRendererFactory(string basePath = "", SDL_Renderer *sdl = nullptr)
		: m_basePath(basePath), m_sdl(sdl), m_blendMode(SDL_BLENDMODE_BLEND), m_maskMode(0)
        {
        }
        
        shared_ptr<Renderer> ConstructBitmap(
                                             LWF *lwf, int objId, Bitmap *bitmap);
        shared_ptr<Renderer> ConstructBitmapEx(
                                               LWF *lwf, int objId, BitmapEx *bitmapEx);
        shared_ptr<TextRenderer> ConstructText(
                                               LWF *lwf, int objId, Text *text);
        shared_ptr<Renderer> ConstructParticle(
                                               LWF *lwf, int objId, Particle *particle);
        
        void Init(LWF *lwf);
        void BeginRender(LWF *lwf);
        void EndRender(LWF *lwf);
        void Destruct();
        void SetBlendMode(int blendMode)
        {
            switch (blendMode)
            {
                case Format::BLEND_MODE_NORMAL:
                    m_blendMode = SDL_BLENDMODE_BLEND;
                    break;
                case Format::BLEND_MODE_ADD:
                    m_blendMode = SDL_BLENDMODE_ADD;
                    break;
                case Format::BLEND_MODE_LAYER:
                    m_blendMode = SDL_BLENDMODE_MOD;
                    break;
                case Format::BLEND_MODE_ERASE:
                    m_blendMode = SDL_BLENDMODE_BLEND;
                    assert(!"Format::BLEND_MODE_ERASE not supported.");
                    break;
                case Format::BLEND_MODE_MASK:
                    m_blendMode = SDL_BLENDMODE_BLEND;
                    assert(!"Format::BLEND_MODE_MASK not supported.");
                    break;
            }
        }
        void SetMaskMode(int maskMode)
        {
            m_maskMode = maskMode;
        }
        int GetBlendMode() {return m_blendMode;}
        int GetMaskMode() {return m_maskMode;}
        
        void FitForHeight(LWF *lwf, float w, float h);
        void FitForWidth(LWF *lwf, float w, float h);
        void ScaleForHeight(LWF *lwf, float w, float h);
        void ScaleForWidth(LWF *lwf, float w, float h);
    };
    
}	// namespace LWF

#endif
