#include "RenderContainer.h"
#include "../Util/Render/SFrameBuffer.h"
#include <iostream>

using namespace CSE;

RenderContainer::RenderContainer() {}


RenderContainer::~RenderContainer() {}


void RenderContainer::Register(SIRender* object) {
    SMaterial* material = object->material;
    if(material == nullptr) return;

    short orderLayer = material->GetOrderLayer();
    GLProgramHandle* handler = material->GetHandle();

    auto& layer = m_rendersLayer[orderLayer][handler];
    object->programRenderIndex = layer.size();
    layer.push_back(object);

}


void RenderContainer::Remove(SIRender* object) {
    SMaterial* material = object->material;
    if(material == nullptr) return;

    short orderLayer = material->GetOrderLayer();
    GLProgramHandle* handler = material->GetHandle();

    //1. Order Render Layer Level
    auto orderLayerIter = m_rendersLayer.find(orderLayer);
    if (orderLayerIter != m_rendersLayer.end()) {
        {
            //2. Program Render Layer Level
            ProgramRenderLayer& programLayer = orderLayerIter->second;
            auto handlerPair = programLayer.find(handler);
            if (handlerPair != programLayer.end()) {
                auto& layerVector = handlerPair->second;
                handlerPair->second.erase(layerVector.begin() + object->programRenderIndex);

                //빈공간은 제거
                if (layerVector.empty()) {
                    programLayer.erase(handlerPair);
                }
            }
        }
        //빈공간은 제거
        if (orderLayerIter->second.empty()) {
            m_rendersLayer.erase(orderLayer);
        }
    }
}

void RenderContainer::RegisterFrameBuffer(SFrameBuffer* buffer) {
    m_frameBuffer.push_back(buffer);
}

void RenderContainer::RemoveFrameBuffer(SFrameBuffer* buffer) {
    auto it = std::find(m_frameBuffer.begin(), m_frameBuffer.end(), buffer);
    m_frameBuffer.erase(it);
}
