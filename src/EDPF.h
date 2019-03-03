#ifndef EDPF_H_
#define EDPF_H_

// Parameter free for EDPF (refer to [52])
#define GAUSS_FILTER cv::Size(5, 5)
#define GAUSS_SIGMA 1.0
#define GRADIENT_THRES 25
#define ANCHOR_THRES 3
#define DETAIL_RATIO 1

#define MIN_EDGE_LEN 10

#include <stack>
#include <vector>

#include <opencv2/opencv.hpp>

static std::vector<cv::Vec3b> EdgeColors = {cv::Vec3b(255, 255, 255),  // White
                                            cv::Vec3b(255, 0, 0),      // Blue
                                            cv::Vec3b(0, 255, 0),      // Lime
                                            cv::Vec3b(0, 0, 255),      // Red
                                            cv::Vec3b(0, 255, 255),    // Yellow
                                            cv::Vec3b(255, 255, 0),    // Cyan
                                            cv::Vec3b(255, 0, 255),  // Magenta
                                            cv::Vec3b(0, 0, 128),    // Maroon
                                            cv::Vec3b(0, 128, 128),  // Olive
                                            cv::Vec3b(128, 128, 128)};  // Gray

// Edge direction
enum class EdgeDirection { NA = 0, Vertical, Horizontal };

// Which direction to traverse while drawing edges
enum class TraverseDirection { NA = 0, Up, Down, Left, Right };

// To which end the next hop will be added to current chain
enum class ChainEnd { NA = 0, Head, Tail };

struct TraverseNode {
  int32_t row, col;
  TraverseDirection dir;

  TraverseNode(int32_t x, int32_t y, TraverseDirection d)
      : row(x), col(y), dir(d) {}
};

struct EdgeChain {
  int32_t index;
  cv::Vec3b color;
  std::vector<cv::Point> points;

  EdgeChain(int32_t i, cv::Vec3b c) : index(i), color(c) {}
};

class EDPF {
 private:
  std::string src_path_;

  cv::Mat src_img_;
  cv::Mat smth_img_;
  int32_t height_, width_;
  uint8_t* smth_map_;
  int32_t* gradient_map_;
  int8_t* direction_map_;
  int8_t* anchor_map_;
  int8_t* edge_map_;

  std::vector<cv::Point> anchors_;
  std::vector<EdgeChain> chains_;

 private:
  EDPF(const EDPF&) = delete;
  EDPF& operator=(const EDPF&) = delete;

 private:
  int32_t& gradient_at(int32_t i, int32_t j);
  int8_t& direction_at(int32_t i, int32_t j);
  int8_t& anchor_at(int32_t i, int32_t j);
  int8_t& edge_at(int32_t i, int32_t j);
  int8_t smooth_at(int32_t i, int32_t j);

  void prewitt_filter(int32_t i, int32_t j, int32_t& Gx, int32_t& Gy);
  void sobel_filter(int32_t i, int32_t j, int32_t& Gx, int32_t& Gy);

  void sort_anchors();
  void traverse_up(std::stack<TraverseNode>& nodes, int32_t row, int32_t col);
  void traverse_down(std::stack<TraverseNode>& nodes, int32_t row, int32_t col);
  void traverse_left(std::stack<TraverseNode>& nodes, int32_t row, int32_t col);
  void traverse_right(std::stack<TraverseNode>& nodes,
                      int32_t row,
                      int32_t col);

  bool hit_border(int32_t row, int32_t col);
  std::vector<cv::Point> neighbors(int32_t row,
                                   int32_t col,
                                   TraverseDirection dir);
  int32_t find_next_hop(const std::vector<cv::Point>& pts);
  bool move_to_next_hop(const std::vector<cv::Point>& pts,
                        int32_t& row,
                        int32_t& col);
  void make_new_chain(const cv::Point& p);
  void append_to_current_chain(ChainEnd end, const cv::Point& p);
  ChainEnd belong_to_current_chain(const cv::Point& p);

  void suppress_noise();
  void build_gradient_n_direction_map();
  void scan_for_anchors();
  void draw_edges();
  void verify_edges();

 public:
  EDPF(const char* src_path);
  ~EDPF();

  void show_input();
  void show_output();
};

#endif