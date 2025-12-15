#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <stdint.h>

// 视频帧结构
typedef struct {
    uint8_t* data[3];      // YUV 数据指针 (Y, U, V)
    int linesize[3];       // 每个平面的行大小
    int width;             // 帧宽度
    int height;            // 帧高度
    int64_t pts;           // 显示时间戳
} VideoFrame;

// 解码器句柄
typedef struct VideoDecoder VideoDecoder;

// 回调函数类型：当有新帧解码完成时调用
typedef void (*FrameCallback)(VideoFrame* frame, void* user_data);

/**
 * 创建视频解码器
 * @param codec_type 编码类型: 1=H264, 2=H265, 3=JPEG
 * @param frame_callback 帧回调函数（可选，传 NULL 则手动获取帧）
 * @param user_data 用户数据指针，会传递给回调函数
 * @return 解码器句柄，失败返回 NULL
 */
VideoDecoder* video_decoder_create(int codec_type, 
                                   FrameCallback frame_callback, 
                                   void* user_data);

/**
 * 输入编码数据到解码器
 * @param decoder 解码器句柄
 * @param data 编码数据
 * @param size 数据大小
 * @param pts 时间戳
 * @return 0=成功, <0=失败
 */
int video_decoder_decode(VideoDecoder* decoder, 
                        const uint8_t* data, 
                        int size, 
                        int64_t pts);

/**
 * 获取解码后的帧（如果没有使用回调）
 * @param decoder 解码器句柄
 * @param frame 输出帧数据
 * @return 1=有帧, 0=无帧, <0=错误
 */
int video_decoder_get_frame(VideoDecoder* decoder, VideoFrame* frame);

/**
 * 销毁解码器
 * @param decoder 解码器句柄
 */
void video_decoder_destroy(VideoDecoder* decoder);

/**
 * 获取解码器信息
 * @param decoder 解码器句柄
 * @param width 输出宽度
 * @param height 输出高度
 * @return 0=成功
 */
int video_decoder_get_info(VideoDecoder* decoder, int* width, int* height);

/**
 * 设置输出缩放
 * @param decoder 解码器句柄
 * @param target_width 目标宽度（0=不缩放）
 * @param target_height 目标高度（0=不缩放）
 * @return 0=成功, <0=失败
 */
int video_decoder_set_scale(VideoDecoder* decoder, int target_width, int target_height);

#endif // VIDEO_DECODER_H
