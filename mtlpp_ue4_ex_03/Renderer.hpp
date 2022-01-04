//
//  Renderer.hpp
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 15/12/2564 BE.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include <simd/simd.h>
#include <MetalKit/MetalKit.h>
#include <ModelIO/ModelIO.h>

#include <chrono>

#include "ShaderTypes.h"
#include "renderer/mtlpp/mtlpp.hpp"
#include "ShaderCache.hpp"

constexpr int32_t max_buffers_in_flight = 3;

class renderer {
public:
    renderer();
    renderer(CAMetalLayer* metal_layer);
    ~renderer();
    
    void render();
    void drawable_resize(CGSize drawable_size);
    
    void load_metal();
    void load_assets();
    void update_game_state(double delta);
    void build_depth_texture(CGSize drawable_size);
    void update_current_drawable(mtlpp::Drawable next_drawable);
    void update_pass_descriptor(mtlpp::Texture texture);
    
private:
    dispatch_semaphore_t in_flight_semaphore_;
    mtlpp::Device device_;
    mtlpp::CommandQueue commandQueue_;
    mtlpp::RenderPassDescriptor render_pass_descriptor_;
    mtlpp::Buffer dynamicUniformBuffer_[max_buffers_in_flight];
    mtlpp::Buffer dynamicUniformBuffer2_[max_buffers_in_flight];
    mtlpp::RenderPipelineState pipelineState_;
    mtlpp::DepthStencilState depthState_;
    mtlpp::RenderPipelineState pipelineState2_;
    mtlpp::DepthStencilState depthState2_;
    mtlpp::Texture depthTexture_;
    mtlpp::Texture colorMap_;
    mtlpp::VertexDescriptor mtlVertexDescriptor_;
    
    mtlpp::Drawable currentDrawable_;
    
    ShaderCache shader_;
    
    float depth_;
    float rotation_;
    
    uint8_t uniformBufferIndex_;
    matrix_float4x4 projectionMatrix_;
    MTKMesh *mesh_;
    
};

#endif /* Renderer_hpp */
