#include "SkCanvas.h"
#include "SkImageInfo.h"
#include "SkSurface.h"
#include "SkPath.h"
#include "SkRect.h"
#include "SkColor.h"
#include "SkPaint.h"
#include "SkTypes.h"

#include <iostream>
#include <vector>

using namespace std;

#if defined(SK_VULKAN)
#include "GrContext.h"
  #include "GrBackendSurface.h"
  #include "vk/GrVkBackendContext.h"
#endif

extern "C" SkSurface* C_SkSurface_MakeRasterN32Premul(int width, int height, const SkSurfaceProps* surfaceProps) {
    return SkSurface::MakeRasterN32Premul(width, height, surfaceProps).release();
}

extern "C" SkSurface* C_SkSurface_MakeRenderTarget(
    GrContext* context,
    SkBudgeted budgeted,
    const SkImageInfo* imageInfo) {
    return SkSurface::MakeRenderTarget(context, budgeted, *imageInfo).release();
}

extern "C" SkSurface* C_SkSurface_MakeFromBackendTexture(
    GrContext* context,
    const GrBackendTexture* backendTexture,
    GrSurfaceOrigin origin,
    int sampleCnt,
    SkColorType colorType) {
    return SkSurface::MakeFromBackendTexture(context, *backendTexture, origin, sampleCnt, colorType, nullptr, nullptr).release();
}

extern "C" SkImage* C_SkSurface_makeImageSnapshot(SkSurface* self) {
    return self->makeImageSnapshot().release();
}

extern "C" SkData* C_SkImage_encodeToData(SkImage* self) {
    return self->encodeToData().release();
}

extern "C" void C_SkData_unref(const SkData* self) {
    self->unref();
}

extern "C" void C_SkPaint_destruct(const SkPaint* self) {
    self->~SkPaint();
}

extern "C" void C_SkPath_destruct(const SkPath* self) {
    self->~SkPath();
}

extern "C" void C_GrBackendTexture_destruct(const GrBackendTexture* self) {
    self->~GrBackendTexture();
}

#if defined(SK_VULKAN)

// The GrVkBackendContext struct binding's length is too short
// because of the std::function that is used in it.

extern "C" void* C_GrVkBackendContext_New(
        void* instance,
        void* physicalDevice,
        void* device,
        void* queue,
        uint32_t graphicsQueueIndex) {

    auto& context = *new GrVkBackendContext();
    context.fInstance = static_cast<VkInstance>(instance);
    context.fPhysicalDevice = static_cast<VkPhysicalDevice>(physicalDevice);
    context.fDevice = static_cast<VkDevice>(device);
    context.fQueue = static_cast<VkQueue>(queue);
    context.fGraphicsQueueIndex = graphicsQueueIndex;
    return &context;
}

extern "C" void C_GrVkBackendContext_Delete(void* vkBackendContext) {
    delete static_cast<GrVkBackendContext*>(vkBackendContext);
}

extern "C" GrContext* C_GrContext_MakeVulkan(const void* vkBackendContext) {
    return GrContext::MakeVulkan(*static_cast<const GrVkBackendContext*>(vkBackendContext)).release();
}

#endif

typedef struct SkCanvasBindings {
    SkSurface* surface;
    void (*release_fn)();
    SkCanvas* canvas;
} SkCanvasBindings;

typedef struct SkSurfaceData {
    const unsigned char* data;
    size_t size;
} SkSurfaceData;

extern "C" SkCanvasBindings SkiaCreateCanvas(int width, int height) {
  auto surface = SkSurface::MakeRasterN32Premul(width, height);
  auto canvas = surface->getCanvas();
  auto release = [surface]() {
    surface->unref();
  };
  static auto static_release = release;
  void (*ptr)() = []() { static_release(); };
  SkCanvasBindings sk_canvas_bindings = { surface.get(), ptr, canvas };
  surface->ref();
  return sk_canvas_bindings;
}

extern "C" SkRect SkiaCreateRect(float width, float height) {
  return SkRect::MakeWH(width, height);
}

extern "C" void SkiaClearCanvas(SkCanvas* canvas, SkColor color) {
  canvas->clear(color);
}

extern "C" SkSurfaceData SkiaGetSurfaceData(SkSurface* surface) {
  sk_sp<SkImage> img(surface->makeImageSnapshot());
  if (!img) { return { nullptr, 0 }; }
  sk_sp<SkData> png(img->encodeToData());
  if (!png) { return { nullptr, 0 }; }
  SkSurfaceData result = { png->bytes(), png->size() };
  return result;
}
