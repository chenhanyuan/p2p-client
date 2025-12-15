#include "video_display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SDL2 头文件（需要从官网下载）
#include <SDL2/SDL.h>

// 显示窗口结构
struct VideoDisplay {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    
    int width;
    int height;
    int initialized;
};

/**
 * 创建显示窗口
 */
VideoDisplay* video_display_create(const char* title, int width, int height) {
    VideoDisplay* display = (VideoDisplay*)malloc(sizeof(VideoDisplay));
    if (!display) {
        printf("[Display] Failed to allocate display\n");
        return NULL;
    }
    
    memset(display, 0, sizeof(VideoDisplay));
    display->width = width;
    display->height = height;
    
    printf("[Display] Creating window: %s (%dx%d)\n", title, width, height);
    
    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("[Display] SDL init failed: %s\n", SDL_GetError());
        free(display);
        return NULL;
    }
    
    // 创建窗口
    display->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (!display->window) {
        printf("[Display] Create window failed: %s\n", SDL_GetError());
        SDL_Quit();
        free(display);
        return NULL;
    }
    
    // 创建渲染器
    display->renderer = SDL_CreateRenderer(
        display->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!display->renderer) {
        printf("[Display] Create renderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        free(display);
        return NULL;
    }
    
    // 创建纹理（YUV420P 格式）
    display->texture = SDL_CreateTexture(
        display->renderer,
        SDL_PIXELFORMAT_IYUV,  // YUV420P
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );
    
    if (!display->texture) {
        printf("[Display] Create texture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        free(display);
        return NULL;
    }
    
    display->initialized = 1;
    printf("[Display] Window created successfully\n");
    
    return display;
}

/**
 * 显示一帧 YUV 数据
 */
int video_display_render(VideoDisplay* display, VideoFrame* frame) {
    if (!display || !display->initialized || !frame) {
        return -1;
    }
    
    // 更新纹理数据（YUV420P）
    int ret = SDL_UpdateYUVTexture(
        display->texture,
        NULL,
        frame->data[0], frame->linesize[0],  // Y
        frame->data[1], frame->linesize[1],  // U
        frame->data[2], frame->linesize[2]   // V
    );
    
    if (ret < 0) {
        printf("[Display] Update texture failed: %s\n", SDL_GetError());
        return -1;
    }
    
    // 清空渲染器
    SDL_RenderClear(display->renderer);
    
    // 渲染纹理
    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
    
    // 呈现到屏幕
    SDL_RenderPresent(display->renderer);
    
    return 0;
}

/**
 * 处理窗口事件
 */
int video_display_poll_events(VideoDisplay* display) {
    if (!display || !display->initialized) {
        return 0;
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                printf("[Display] Window closed by user\n");
                return 0;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE ||
                    event.key.keysym.sym == SDLK_q) {
                    printf("[Display] Quit key pressed\n");
                    return 0;
                }
                break;
                
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    printf("[Display] Window resized: %dx%d\n",
                           event.window.data1, event.window.data2);
                }
                break;
        }
    }
    
    return 1;  // 窗口仍然打开
}

/**
 * 设置窗口标题
 */
void video_display_set_title(VideoDisplay* display, const char* title) {
    if (display && display->window && title) {
        SDL_SetWindowTitle(display->window, title);
    }
}

/**
 * 销毁显示窗口
 */
void video_display_destroy(VideoDisplay* display) {
    if (!display) return;
    
    printf("[Display] Destroying window...\n");
    
    if (display->texture) {
        SDL_DestroyTexture(display->texture);
    }
    
    if (display->renderer) {
        SDL_DestroyRenderer(display->renderer);
    }
    
    if (display->window) {
        SDL_DestroyWindow(display->window);
    }
    
    if (display->initialized) {
        SDL_Quit();
    }
    
    free(display);
    printf("[Display] Window destroyed\n");
}
