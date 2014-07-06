#include "lwf_libsdl2_bitmap.h"
#include "lwf_libsdl2_factory.h"
#include "lwf_libsdl2_particle.h"
#include "lwf_libsdl2_textttf.h"
#include "lwf_core.h"
#include "lwf_data.h"
#include "lwf_property.h"
#include "lwf_libsdl2_factory.h"
#include "lwf_text.h"

namespace LWF {
    
    shared_ptr<Renderer> LWFRendererFactory::ConstructBitmap(LWF *lwf, int objId, Bitmap *bitmap)
    {
        return make_shared<LWFBitmapRenderer>(lwf, bitmap, m_sdl);
    }
    
    shared_ptr<Renderer> LWFRendererFactory::ConstructBitmapEx(LWF *lwf, int objId, BitmapEx *bitmapEx)
    {
        return make_shared<LWFBitmapRenderer>(lwf, bitmapEx, m_sdl);
    }
    
    shared_ptr<TextRenderer> LWFRendererFactory::ConstructText(LWF *lwf, int objId, Text *text)
    {
        const Format::Text &t = lwf->data->texts[text->objectId];
        const Format::TextProperty &p = lwf->data->textProperties[t.textPropertyId];
        const Format::Font &f = lwf->data->fonts[p.fontId];
        string fontName = lwf->data->strings[f.stringId];
        
        return make_shared<LWFTextTTFRenderer>(lwf, text, fontName.c_str(), m_sdl);
    }
    
    shared_ptr<Renderer> LWFRendererFactory::ConstructParticle(LWF *lwf, int objId, Particle *particle)
    {
        return make_shared<LWFParticleRenderer>(lwf, particle, m_sdl);
    }
    
    void LWFRendererFactory::Init(LWF *lwf)
    {
    }
    
    void LWFRendererFactory::BeginRender(LWF *lwf)
    {
    }
    
    void LWFRendererFactory::EndRender(LWF *lwf)
    {
    }
    
    void LWFRendererFactory::Destruct()
    {
    }
    
    void LWFRendererFactory::FitForHeight(class LWF *lwf, float w, float h)
    {
        ScaleForHeight(lwf, w, h);
        float offsetX = (w - lwf->width * lwf->scaleByStage) / 2.0f;
        float offsetY = -h;
        lwf->property->Move(offsetX, offsetY);
    }
    
    void LWFRendererFactory::FitForWidth(class LWF *lwf, float w, float h)
    {
        ScaleForWidth(lwf, w, h);
        float offsetX = (w - lwf->width * lwf->scaleByStage) / 2.0f;
        float offsetY = -h + (h - lwf->height * lwf->scaleByStage) / 2.0f;
        lwf->property->Move(offsetX, offsetY);
    }
    
    void LWFRendererFactory::ScaleForHeight(class LWF *lwf, float w, float h)
    {
        float scale = h / lwf->height;
        lwf->scaleByStage = scale;
        lwf->property->Scale(scale, scale);
    }
    
    void LWFRendererFactory::ScaleForWidth(class LWF *lwf, float w, float h)
    {
        float scale = w / lwf->width;
        lwf->scaleByStage = scale;
        lwf->property->Scale(scale, scale);
    }
    
}	// namespace LWF

