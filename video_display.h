#ifndef VIDEO_DISPLAY_H
#define VIDEO_DISPLAY_H

#include "video_decoder.h"
#include <stdint.h>

// 显示窗口句柄
typedef struct VideoDisplay VideoDisplay;

/**
 * 创建显示窗口
 * @param title 窗口标题
 * @param width 窗口宽度
 * @param height 窗口高度
 * @return 显示窗口句柄，失败返回 NULL
 */
VideoDisplay* video_display_create(const char* title, int width, int height);

/**
 * 显示一帧 YUV 数据
 * @param display 显示窗口句柄
 * @param frame 视频帧数据
 * @return 0=成功, <0=失败
 */
int video_display_render(VideoDisplay* display, VideoFrame* frame);

/**
 * 处理窗口事件（非阻塞）
 * @param display 显示窗口句柄
 * @return 1=窗口仍然打开, 0=窗口已关闭
 */
int video_display_poll_events(VideoDisplay* display);

/**
 * 销毁显示窗口
 * @param display 显示窗口句柄
 */
void video_display_destroy(VideoDisplay* display);

/**
 * 设置窗口标题
 * @param display 显示窗口句柄
 * @param title 新标题
 */
void video_display_set_title(VideoDisplay* display, const char* title);

#endif // VIDEO_DISPLAY_H
