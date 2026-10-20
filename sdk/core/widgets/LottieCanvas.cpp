#include "LottieCanvas.h"
#include "core/base/LvObjFactory.h"
#include "core/log/log.h"
#include "core/tools/AssetManager.h"

namespace lvglpp::widgets {

LottieCanvas::LottieCanvas(const std::string &assetPath, BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglLottie, parent)
{
    setAsset(assetPath);
}

void LottieCanvas::setAsset(const std::string &assetPath)
{
    src_path_ = assetPath;
    if (getLvglItem() == nullptr || assetPath.empty())
    {
        return;
    }

    tools::AssetHandle asset = tools::AssetManager::getInstance().open(assetPath);
    if (!asset || asset->size() == 0)
    {
        LOG_ERROR() << "lottie asset not found: " << assetPath;
        return;
    }

    // ThorVG parses the JSON immediately (copy=true); the bytes need not stay
    // alive after this call.
    lv_lottie_set_src_data(getLvglItem(), asset->data(), asset->size());
}

void LottieCanvas::setFile(const std::string &fsPath)
{
    src_path_ = fsPath;
    if (getLvglItem() != nullptr && !fsPath.empty())
    {
        lv_lottie_set_src_file(getLvglItem(), fsPath.c_str());
    }
}

void LottieCanvas::setCacheSize(int32_t width, int32_t height)
{
    auto lottie_obj = getLvglItem();
    if (lottie_obj == nullptr)
    {
        return;
    }

#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    cache_.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4);
    lv_lottie_set_buffer(lottie_obj, width, height, cache_.data());
#else
    /*For GPUs and special alignment/strided settings use a draw_buf instead*/
    LV_DRAW_BUF_DEFINE(draw_buf, width, height, LV_COLOR_FORMAT_ARGB8888);
    lv_lottie_set_draw_buf(lottie_obj, &draw_buf);
#endif
}

} // namespace lvglpp::widgets
