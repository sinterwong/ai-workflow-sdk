/**
 * @file pipe_types.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __PIPE_TYPES_HPP__
#define __PIPE_TYPES_HPP__
#include "core/infer_types.hpp"

namespace us_pipe {
struct Line {
  cv::Point2i start;
  cv::Point2i end;
};

enum class LesionNotifyStatus : std::uint8_t {
  NONE = 0,
  WEAK_PASS_DET_THRESH = 1 << 0,
  WEAK_PASS_TRACK_THRESH = 1 << 1,
  STRONG_PASS = 1 << 7
};

struct LesionFprResult {
  infer::ClsRet clsRet;
  int clsBirads;

  LesionFprResult(const infer::ClsRet &clsRet, int clsBirads = -1)
      : clsRet(clsRet), clsBirads(clsBirads) {}
};

constexpr float EPSILON = 1e-5f;
constexpr std::array<int, 7> BIRADS_KEYS = {-1, 0, 1, 2, 3, 4, 5};

enum class AtaTypeTIRADS {
  UNKNOWN = -1,
  BENIGN = 0,
  EXTREMELY_LOW_RISK,
  LOW_RISK,
  MODERATE_RISK,
  HIGH_RISK
};

enum class AcrTypeTIRADS { UNKNOWN = -1, TR1 = 0, TR2, TR3, TR4, TR5 };

enum class CTypeTIRADS { UNKNOWN = -1, TR2 = 0, TR3, TR4A, TR4B, TR4C, TR5 };

// 甲状腺结节的结构特征 (0-4枚举顺序与模型预测顺序相关, 不能变!!!!!!)
enum class Structure {
  UNKNOWN = -1,
  CYSTIC = 0,
  SOLID = 1,
  PREDOMINANTLY_CYSTIC = 2,
  PREDOMINANTLY_SOLID = 3,
  SPONGY = 4
};

// 甲状腺结节的横纵比; (0-1枚举顺序与模型预测顺序相关, 不能变!!!!!!)
enum class AspectRatio { UNKNOWN = -1, VERTICAL = 0, HORIZONTAL = 1 };

// 超声回声强度: 甲状腺结节的回声比周围组织的高还是低;
// (0-4枚举顺序与模型预测顺序相关, 不能变!!!!!!)
enum class Echogenicity {
  UNKNOWN = -1,             // 默认值, 表示未计算;
  ANECHOIC = 0,             // 无回声;
  EXTREMELY_HYPOECHOIC = 1, // 极低回声;
  HYPOECHOIC = 2,           // 低回声;
  ISOECHOIC = 3,            // 等回声;
  HYPERECHOIC = 4           // 高回声;
};

// 局灶性强回声:某个特定的小区域（局灶）显示出比周围组织更强的回声（即反射超声波的能力更强），在图像上表现为更亮的区域
// (0-4枚举顺序与模型预测顺序相关, 不能变!!!!!!)
enum class FocalHyperechogenicity {
  UNKNOWN = -1,                 // 默认值, 表示未计算;
  COMET_TAIL = 0,               // 彗星尾
  MACROCALCIFICATION = 1,       // 粗钙化
  PERIPHERAL_CALCIFICATION = 2, // 周边钙化
  PUNCTATE_ECHOGENIC_FOCI = 3,  // 微钙化
  UNKNOWN_FOCI = 4              // 意义不明点状强钙化
};

// 甲状腺结节的边缘特征edge; (0-2枚举顺序与模型预测顺序相关, 不能变!!!!!!)
enum class Margin {
  UNKNOWN = -1,  // 默认值, 表示未计算;
  IRREGULAR = 0, // 不规则(小分叶、毛刺)
  BLUR = 1,      // 模糊
  INVASIVE = 2   // 甲状腺外侵犯
};

// 甲状腺结节的bias; (0-1枚举顺序与模型预测顺序相关, 不能变!!!!!!)
enum class Eccentric { UNKNOWN = -1, FALSE = 0, TRUE = 1 };

// 声晕:结节周围环绕的低回声或者无回声区域, (目前没用);
enum class HaloSign { UNKNOWN = -1, NONE = 0, THICK = 1, THIN = 2 };

// 声晕质地:结节周围环绕的低回声或者无回声区域表现的一致性, (目前没用);
enum class HaloTexture {
  UNKNOWN = -1,
  NONE = 0,
  HOMOGENEOUS = 1,  // 均匀
  HETEROGENEOUS = 2 // 不均匀
};

// 后方回声特征:结节后方回声和同一深度周围组织回声的对比, (目前没用);
enum class PostLesionalEcho {
  UNKNOWN = -1,
  NONE = 0,       // 什么都没有
  ENHANCED = 1,   // 增强
  ATTENUATED = 2, // 衰减
  MIXED = 3       // 混合性改变：增强和衰减都有
};

// 回声质地:病灶回声区域表现的一致性, (目前没用);
enum class EchogenicityTexture {
  UNKNOWN = -1,
  HOMOGENEOUS = 0,  // 均匀
  HETEROGENEOUS = 1 // 不均匀
};

using SignAttribute = std::variant<Structure, AspectRatio, Echogenicity,
                                   FocalHyperechogenicity, Margin, Eccentric>;

struct Frame {
  int index;
  cv::Mat image;
  cv::Rect roi;

  Frame(int frame_index, const cv::Mat &image, const cv::Rect &roi)
      : index(frame_index), image(image), roi(roi) {}

  Frame(int frame_index, cv::Mat &&image, cv::Rect &&roi)
      : index(frame_index), image(std::move(image)), roi(std::move(roi)) {}

  Frame() : index(-1), image(), roi() {}
};

struct DisplayResult {
  int track_id;
  cv::Rect box;
  cv::Rect smooth_box;
  int label;
  float score;
  cv::Mat best_view;
  float max_area;
  int fpr_model_birads;
  int mode;
  LesionNotifyStatus notify_status = LesionNotifyStatus::NONE;
  float last_det_score;
  float last_fpr_score;

  DisplayResult(int track_id, const cv::Rect &box, const cv::Rect &smooth_box,
                int label, float score, const cv::Mat &best_view,
                float max_area, int fpr_model_birads, int mode,
                LesionNotifyStatus notify_status, float last_det_score,
                float last_fpr_score)
      : track_id(track_id), box(box), smooth_box(smooth_box), label(label),
        score(score), best_view(best_view), max_area(max_area),
        fpr_model_birads(fpr_model_birads), mode(mode),
        notify_status(notify_status), last_det_score(last_det_score),
        last_fpr_score(last_fpr_score) {}
};

enum class VideoStatus {
  NONE = -1,
  SCANNING,
  OPERATING,
  BLACK_SCAEEN,
  MEASURING,
  FREEZE,
  UNKNOW
};

// TODO: 整个视频结束后需要进行处理的信息,需要补充;
struct VideoRepr {};

struct ThyDispatchConfig {
  std::string modelRoot;
  std::string algoConfPath;
  bool adaStride = true;
};

} // namespace us_pipe

#endif
