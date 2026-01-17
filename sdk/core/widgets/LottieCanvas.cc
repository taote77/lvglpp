#include "LottieCanvas.h"

namespace lvglpp::widgets {

LottieCanvas::LottieCanvas(const std::string &path, BaseItem *parent) : BaseItem(BaseItem::ItemType::LottieCanvas, parent)
{
    setImageSrcPath(path);
}

void LottieCanvas::setImageSrcPath(const std::string &path)
{
    src_path_ = path;
    if (getLvglItem() != nullptr && !src_path_.empty())
    {
        auto lottie_obj = getLvglItem();
        lv_lottie_set_src_file(lottie_obj, src_path_.c_str());
    }
}

void LottieCanvas::setCacheSize(int32_t width, int32_t height)
{
    auto lottie_obj = getLvglItem();

#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    cache_.resize(width * height * 4);
    lv_lottie_set_buffer(lottie_obj, width, height, cache_.data());
#else
    /*For GPUs and special alignment/strid setting use a draw_buf instead*/
    LV_DRAW_BUF_DEFINE(draw_buf, width, height, LV_COLOR_FORMAT_ARGB8888);
    lv_lottie_set_draw_buf(lottie, &draw_buf);
#endif
}

} // namespace lvglpp::widgets
