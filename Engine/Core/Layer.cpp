#include "Layer.h"

#include "Application.h"

namespace brnCore {
void Layer::QueueTransition(std::unique_ptr<Layer> toLayer) {
    // TODO: don't do this; make it async rather than immediate
    auto &layerStack = Application::Get().m_LayerStack;
    for (auto &layer : layerStack) {
        if (layer.get() == this) {
            layer = std::move(toLayer);
            return;
        }
    }
}
} // namespace brnCore
