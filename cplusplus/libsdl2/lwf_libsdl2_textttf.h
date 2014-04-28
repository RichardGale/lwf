#ifndef LWF_COCOS2DX_TEXTTTF_H
#define LWF_COCOS2DX_TEXTTTF_H

#include "SDL.h"

#include "lwf_renderer.h"

namespace LWF {
    
    class LWFRendererFactory;
    class LWFTextTTF;
    
    class LWFTextTTFRenderer : public TextRenderer
    {
    protected:
        LWFRendererFactory *m_factory;
        LWFTextTTF *m_label;
        Text* m_text;
        
    public:
        LWFTextTTFRenderer(
                           LWF *l, Text *text, const char *fontName, SDL_Renderer *node);
        virtual ~LWFTextTTFRenderer();
        
        void Destruct();
        void Update(const Matrix *matrix, const ColorTransform *colorTransform);
        void Render(const Matrix *matrix, const ColorTransform *colorTransform,
                    int renderingIndex, int renderingCount, bool visible);
        
        void SetText(string text);
    private:
        int getLength(string text);
    };
    
}   // namespace LWF

#endif
