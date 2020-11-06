//
// Created by denn nevera on 06/11/2020.
//

#pragma once

#include <string>
#include "clHelper/buffer.h"
#include "clHelper/embeddedProgram.h"

struct Texture {
    cl_mem buffer = nullptr;
    size_t width = 0;
    size_t height = 0;
    size_t depth = 0;

    size_t get_length() const { return width*height*depth*3*sizeof(float);}
};

inline void release(const Texture& texture) {
  if (texture.buffer) free(texture.buffer);
}

class BenchKernel {

public:
    BenchKernel(const void* command_queue,
                const std::string& kernel_name,
                size_t width = 800,
                size_t height = 600,
                size_t samples = 4
                );

    [[nodiscard]] cl_command_queue get_command_queue() const ;

    void process();
    Texture make_texture(size_t width, size_t height, size_t depth = 1);
    [[nodiscard]] Texture get_destination() const;

    ~BenchKernel();

private:
    const void *command_queue_;
    std::string kernel_name_;
    size_t width_;
    size_t height_;
    size_t samples_;
    cl_device_id device_id_{};
    cl_context context_{};
    cl_int last_error_;
    cl_program program_;
    cl_kernel kernel_;
    Texture ao_texture_;
};

