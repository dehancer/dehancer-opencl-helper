//
// Created by denn nevera on 06/11/2020.
//

#pragma once

#include <string>
#include <functional>

#include "clHelper/buffer.h"
#include "clHelper/embeddedProgram.h"

namespace dehancer::opencl::example {

    struct Texture {
        cl_mem buffer = nullptr;
        size_t width = 0;
        size_t height = 0;
        size_t depth = 1;

        size_t get_length() const { return width * height * depth * 3 * sizeof(float); }
    };

    typedef std::function<Texture (cl_kernel& compute_kernel)> FunctionHandler;

    inline void release(const Texture &texture) {
      if (texture.buffer) clReleaseMemObject(texture.buffer);
    }

    class Function {

    public:
        Function(const void *command_queue, const std::string &kernel_name);

        [[nodiscard]] cl_command_queue get_command_queue() const;

        void execute(const FunctionHandler& block);
        Texture make_texture(size_t width, size_t height, size_t depth = 1);

        ~Function();

    private:
        const void *command_queue_;
        std::string kernel_name_;
        cl_device_id device_id_{};
        cl_context context_{};
        cl_int last_error_;
        cl_program program_;
        cl_kernel kernel_;
    };
}
