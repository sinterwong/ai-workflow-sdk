/**
 * @file thy_types.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __THY_TYPES_HPP__
#define __THY_TYPES_HPP__

#include "pipe_types.hpp"
#include <optional>
namespace us_pipe {
struct ThyroidBreastParams {
  std::size_t stableFrames = 0;
  std::array<std::size_t, BIRADS_KEYS.size()> biradsCounter = {};
  std::map<SignAttribute, std::size_t> signCounter;
  cv::Mat vesselMask;
};

// 甲状腺征象;
struct ThyroidLesionSign {
  Structure structure = Structure::UNKNOWN;
  Echogenicity echogenicity = Echogenicity::UNKNOWN;
  AspectRatio aspect_ratio = AspectRatio::UNKNOWN;
  Eccentric eccentric = Eccentric::UNKNOWN;
  std::vector<Margin> margin;
  std::vector<FocalHyperechogenicity> focal_hyperechogenicity;
  HaloSign halo_sign = HaloSign::UNKNOWN;          // 目前算法暂未用到;
  HaloTexture halo_texture = HaloTexture::UNKNOWN; // 目前算法暂未用到;
  EchogenicityTexture echogenicity_texture =
      EchogenicityTexture::UNKNOWN; // 目前算法暂未用到;
  PostLesionalEcho post_lesional_echo =
      PostLesionalEcho::UNKNOWN; // 目前算法暂未用到;

  ThyroidLesionSign() {}
};

// 甲状腺征象概率;
struct ThyroidLesionSignProbs {
  float structure = 0.f;
  float echogenicity = 0.f;
  float aspect_ratio = 0.f;
  float eccentric = 0.f; // bias
  std::vector<float> margin;
  std::vector<float> focal_hyperechogenicity;
  float halo_sign = 0.0f;            // 目前算法暂未用到;
  float halo_texture = 0.0f;         // 目前算法暂未用到;
  float echogenicity_texture = 0.0f; // 目前算法暂未用到;
  float post_lesional_echo = 0.0f;   // 目前算法暂未用到;

  ThyroidLesionSignProbs() {}
};

struct ThyroidDiagMeta {
  bool is_valid = false;
  float bbox_area = -1;
  std::optional<float> reweight_tirads_score = std::nullopt;
  CTypeTIRADS reweight_tirads = CTypeTIRADS::UNKNOWN;

  ThyroidDiagMeta() {}
};

// 甲状腺诊断, 在s4_diagnoser模块出现;
struct ThyroidLesionTirads {
  float score;
  float confidence = 0.0f;
  CTypeTIRADS tirads = CTypeTIRADS::UNKNOWN;
  ThyroidDiagMeta diag_meta;

  ThyroidLesionTirads() {}
};

// 基于征象计算出的甲状腺诊断结果;
struct ThyroidTirads {
  AcrTypeTIRADS acr_type_tirads = AcrTypeTIRADS::UNKNOWN;
  AtaTypeTIRADS ata_type_tirads = AtaTypeTIRADS::UNKNOWN;
  CTypeTIRADS c_type_tirads = CTypeTIRADS::UNKNOWN;

  ThyroidTirads() {}
};

// 甲状腺诊断后处理中相关超参,在s5_displayer中, 对tirads结果进行后处理时使用;
struct ThyroidTiradsPostThresh {
  int keep_frame_num_for_2;
  int keep_frame_num_for_3;
  int keep_frame_num_for_4a;
  int keep_frame_num_for_4b;
  int keep_frame_num_for_4c;
  int keep_frame_num_for_5;

  float thresh_between_2_and_3;
  float thresh_between_3_and_4a;
  float thresh_between_4a_and_4b;
  float thresh_between_4b_and_4c;
  float thresh_between_4c_and_5;
};

// 甲状腺s4_diagnoser模块输出结果;
struct ThyroidLesionDiagnoseResult {
  ThyroidLesionSign thyroid_signs;
  ThyroidLesionSignProbs thyroid_signs_probs;
  ThyroidLesionTirads thyroid_tirads;

  ThyroidLesionDiagnoseResult() {}

  ThyroidLesionDiagnoseResult(const ThyroidLesionSign &thyroid_signs,
                              const ThyroidLesionSignProbs &thyroid_signs_probs,
                              const ThyroidLesionTirads &thyroid_tirads)
      : thyroid_signs(thyroid_signs), thyroid_signs_probs(thyroid_signs_probs),
        thyroid_tirads(thyroid_tirads) {}
};

struct ThyroidDiagResult {};

// 一个finished track里挑选出的3帧典型病灶截图;
struct ThyroidLesionRepr {
  std::vector<cv::Mat> positive_imgs;

  ThyroidLesionRepr() = default;
};

// 5.2 甲状腺展示结果
struct ThyroidDisplayResult : public DisplayResult {

  // {acr/ata/c}-Tirads;
  std::optional<ThyroidTirads> diag_tirads;
  std::optional<ThyroidLesionSign> thyroid_signs;
  std::optional<ThyroidLesionSignProbs> thyroid_signs_probs;
  float malig_prob = -1.0f; // 良恶性概率;

  ThyroidDisplayResult(int track_id, const cv::Rect &box,
                       const cv::Rect &smooth_box, int label, float score,
                       const cv::Mat &best_view, float max_area,
                       int fpr_model_birads, int mode,
                       LesionNotifyStatus notify_status, float last_det_score,
                       float last_fpr_score,
                       const std::optional<ThyroidTirads> &diag_tirads,
                       const std::optional<ThyroidLesionSign> &thyroid_signs,
                       const std::optional<ThyroidLesionSignProbs> &probs,
                       float malig_prob)
      : DisplayResult(track_id, box, smooth_box, label, score, best_view,
                      max_area, fpr_model_birads, mode, notify_status,
                      last_det_score, last_fpr_score),
        diag_tirads(diag_tirads), thyroid_signs(thyroid_signs),
        thyroid_signs_probs(probs), malig_prob(malig_prob) {}
};

struct ThyroidLesion {
  // 甲状腺结节基本信息
  int track_id = -1;
  cv::Rect box;
  cv::Rect smooth_box;
  int label;
  float score = 0.f;
  float last_det_score = -1.0f;
  float last_fpr_score = -1.0f;
  int fpr_model_birads = -1;
  LesionNotifyStatus notify_status = LesionNotifyStatus::NONE;

  cv::Mat best_view;
  float max_area = 0.f;

  // int diag_tirads = -1;
  ThyroidTirads diag_tirads;
  ThyroidLesionSign thyroid_signs;
  int mode;
  float malig_prob = -1.0f;

  ThyroidLesion() {}

  ThyroidLesion(int track_id, const cv::Rect &box, const cv::Rect &smooth_box,
                int label, float score, float last_det_score,
                float last_fpr_score, const ThyroidTirads &diag_tirads)
      : track_id(track_id), box(box), smooth_box(smooth_box), label(label),
        score(score), last_det_score(last_det_score),
        last_fpr_score(last_fpr_score), diag_tirads(diag_tirads) {}

  ThyroidLesion(int track_id, const cv::Rect &box, const cv::Rect &smooth_box,
                int label, float score, cv::Mat const &best_view,
                float max_area, const ThyroidTirads &diag_tirads,
                const ThyroidLesionSign &thyroid_signs, int fpr_model_birads,
                int mode, LesionNotifyStatus notify_status,
                float last_det_score, float last_fpr_score, float malig_prob)
      : track_id(track_id), box(box), smooth_box(smooth_box), label(label),
        score(score), best_view(best_view), max_area(max_area),
        diag_tirads(diag_tirads), thyroid_signs(thyroid_signs),
        fpr_model_birads(fpr_model_birads), mode(mode),
        notify_status(notify_status), last_det_score(last_det_score),
        last_fpr_score(last_fpr_score), malig_prob(malig_prob) {}

  std::pair<Line, Line> get_long_short_diameter() const;

  static ThyroidLesion
  convert_from_display_result(const ThyroidDisplayResult &rhs) {
    auto diag_tirads = ThyroidTirads();
    if (rhs.diag_tirads.has_value())
      diag_tirads = *rhs.diag_tirads;

    auto thyroid_signs = ThyroidLesionSign();
    if (rhs.thyroid_signs.has_value())
      thyroid_signs = *rhs.thyroid_signs;

    return ThyroidLesion{rhs.track_id,       rhs.box,
                         rhs.smooth_box,     rhs.label,
                         rhs.score,          rhs.best_view,
                         rhs.max_area,       diag_tirads,
                         thyroid_signs,      rhs.fpr_model_birads,
                         rhs.mode,           rhs.notify_status,
                         rhs.last_det_score, rhs.last_fpr_score,
                         rhs.malig_prob};
  }

  // TODO: 现在这个构造函数几乎没用... 因为 ThyroidLesion
  // 并没有接受右值参数的构造函数
  static ThyroidLesion convert_from_display_result(ThyroidDisplayResult &&rhs) {
    auto diag_tirads = ThyroidTirads();
    if (rhs.diag_tirads.has_value())
      diag_tirads = std::move(*rhs.diag_tirads);

    auto thyroid_signs = ThyroidLesionSign();
    if (rhs.thyroid_signs.has_value())
      thyroid_signs = std::move(*rhs.thyroid_signs);

    return ThyroidLesion{std::move(rhs.track_id),
                         std::move(rhs.box),
                         std::move(rhs.smooth_box),
                         std::move(rhs.label),
                         std::move(rhs.score),
                         std::move(rhs.best_view),
                         std::move(rhs.max_area),
                         diag_tirads,
                         thyroid_signs,
                         std::move(rhs.fpr_model_birads),
                         std::move(rhs.mode),
                         std::move(rhs.notify_status),
                         std::move(rhs.last_det_score),
                         std::move(rhs.last_fpr_score),
                         std::move(rhs.malig_prob)};
  }
};

// 每帧返回的甲状腺保险结果;
struct ThyroidInsu {
  // 每帧的病灶检测结果;
  std::vector<ThyroidLesion> lesions;

  // ThyroidLesionRepr: 一个finished track中典型的3帧病灶图像;
  std::vector<ThyroidLesionRepr> lesion_reprs;

  // 阴性图像;
  cv::Mat negative_img;

  ThyroidInsu() = default;

  // 为了适配 pipeline_runner 模版里的 OutterDisplayResultT;
  static ThyroidInsu convert_from_display_result(const ThyroidInsu &rhs) {
    return rhs;
  }
};

struct ThyroidInsurancePipelineConfig {
  float track_active_thresh;
  float track_inactive_thresh;
  float track_finish_thresh;
  float track_det_weak_thresh;

  int lesion_diag_timeout_frames;

  friend std::ostream &
  operator<<(std::ostream &os, const ThyroidInsurancePipelineConfig &config) {
    os << "track_active_thresh: " << config.track_active_thresh << std::endl;
    os << "track_inactive_thresh: " << config.track_inactive_thresh
       << std::endl;
    os << "track_finish_thresh: " << config.track_finish_thresh << std::endl;
    os << "track_det_weak_thresh: " << config.track_det_weak_thresh
       << std::endl;
    os << "lesion_diag_timeout_frames: " << config.lesion_diag_timeout_frames
       << std::endl;
    return os;
  }
};
} // namespace us_pipe

#endif // __THY_TYPES_HPP__