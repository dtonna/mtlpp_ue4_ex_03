//
//  Renderer.cpp
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 15/12/2564 BE.
//

#include "Renderer.hpp"
#include "math.hpp"
#include <iostream>

using namespace std::chrono_literals;
template<typename T1, typename T2>
using mul = std::ratio_multiply<T1, T2>;

extern mtlpp::Device g_device;


renderer::renderer()
: device_(g_device)
, depth_(0.0)
, rotation_(0.0)
, uniformBufferIndex_(0)
, currentDrawable_()
, in_flight_semaphore_(dispatch_semaphore_create(max_buffers_in_flight))
{
    load_metal();
    load_assets();
}

renderer::renderer(CAMetalLayer* metal_layer)
: device_(metal_layer.device)
, depth_(0.0)
, rotation_(0.0)
, uniformBufferIndex_(0)
, currentDrawable_()
, in_flight_semaphore_(dispatch_semaphore_create(max_buffers_in_flight))
{
    load_metal();
    load_assets();
}

renderer::~renderer() {
    dispatch_semaphore_signal(in_flight_semaphore_);
}


void renderer::load_metal() {
    ns::Array<mtlpp::VertexBufferLayoutDescriptor> vertexLayouts = mtlVertexDescriptor_.GetLayouts();
    ns::Array<mtlpp::VertexAttributeDescriptor> attribs = mtlVertexDescriptor_.GetAttributes();
    
    uint64_t attrib_idx = VertexAttributePosition;
    attribs[attrib_idx].SetFormat(mtlpp::VertexFormat::Float3);
    attribs[attrib_idx].SetOffset(0);
    attribs[attrib_idx].SetBufferIndex(BufferIndexMeshPositions);
    
    attribs[(uint64_t)VertexAttributeTexcoord].SetFormat(mtlpp::VertexFormat::Float2);
    attribs[(uint64_t)VertexAttributeTexcoord].SetOffset(0);
    attribs[(uint64_t)VertexAttributeTexcoord].SetBufferIndex(BufferIndexMeshGenerics);
    
    vertexLayouts[(uint64_t)BufferIndexMeshPositions].SetStride(12);
    vertexLayouts[(uint64_t)BufferIndexMeshPositions].SetStepRate(1);
    vertexLayouts[(uint64_t)BufferIndexMeshPositions].SetStepFunction(mtlpp::VertexStepFunction::PerVertex);
    
    vertexLayouts[(uint64_t)BufferIndexMeshGenerics].SetStride(8);
    vertexLayouts[(uint64_t)BufferIndexMeshGenerics].SetStepRate(1);
    vertexLayouts[(uint64_t)BufferIndexMeshGenerics].SetStepFunction(mtlpp::VertexStepFunction::PerVertex);
    
    shader_.CreateShader(device_);
    
    mtlpp::RenderPipelineDescriptor pipelineStateDescriptor;
    pipelineStateDescriptor.SetLabel("MyPipeline");
    pipelineStateDescriptor.SetSampleCount(1);
    pipelineStateDescriptor.SetVertexFunction(shader_.vertexFunction_);
    pipelineStateDescriptor.SetFragmentFunction(shader_.fragmentFunction_);
    pipelineStateDescriptor.SetVertexDescriptor(mtlVertexDescriptor_);
    pipelineStateDescriptor.GetColorAttachments()[0].SetPixelFormat(mtlpp::PixelFormat::BGRA8Unorm);
    pipelineStateDescriptor.SetDepthAttachmentPixelFormat(mtlpp::PixelFormat::Depth32Float_Stencil8);
    pipelineStateDescriptor.SetStencilAttachmentPixelFormat(mtlpp::PixelFormat::Depth32Float_Stencil8);
    
    ns::AutoReleasedError *error = NULL;
    pipelineState_ = device_.NewRenderPipelineState(pipelineStateDescriptor, error);
    if (!pipelineState_ || error != NULL)
    {
        NSLog(@"Failed to created pipeline state, error %s", error->GetLocalizedDescription().GetCStr());
    }
    
    mtlpp::DepthStencilDescriptor depthStateDesc;
    depthStateDesc.SetDepthCompareFunction(mtlpp::CompareFunction::Less);
    depthStateDesc.SetDepthWriteEnabled(true);
    
    mtlpp::StencilDescriptor frontStencilDesc;
    frontStencilDesc.SetStencilCompareFunction(mtlpp::CompareFunction::Always);
    frontStencilDesc.SetStencilFailureOperation(mtlpp::StencilOperation::Keep);
    frontStencilDesc.SetDepthFailureOperation(mtlpp::StencilOperation::Keep);
    frontStencilDesc.SetDepthStencilPassOperation(mtlpp::StencilOperation::Replace);
    frontStencilDesc.SetReadMask(0xFF);
    frontStencilDesc.SetWriteMask(1);
    
    depthStateDesc.SetFrontFaceStencil(frontStencilDesc);
    depthStateDesc.SetBackFaceStencil(nil);
    
    depthState_ = device_.NewDepthStencilState(depthStateDesc);
    
    /* Frame */
    mtlpp::RenderPipelineDescriptor pipelineStateDescriptor2;
    pipelineStateDescriptor2.SetLabel(@"MyPipeline2");
    pipelineStateDescriptor2.SetSampleCount(1);
    pipelineStateDescriptor2.SetVertexFunction(shader_.vertexFunction_);
    pipelineStateDescriptor2.SetFragmentFunction(shader_.fragmentBorderFunction_);
    pipelineStateDescriptor2.SetVertexDescriptor(mtlVertexDescriptor_);
    pipelineStateDescriptor2.GetColorAttachments()[0].SetPixelFormat(mtlpp::PixelFormat::BGRA8Unorm);
    pipelineStateDescriptor2.SetDepthAttachmentPixelFormat(mtlpp::PixelFormat::Depth32Float_Stencil8);
    pipelineStateDescriptor2.SetStencilAttachmentPixelFormat(mtlpp::PixelFormat::Depth32Float_Stencil8);
    
    pipelineState2_ = device_.NewRenderPipelineState(pipelineStateDescriptor2, error);
    if (!pipelineState2_ || error != NULL)
    {
        NSLog(@"Failed to created pipeline state, error %s", error->GetLocalizedDescription().GetCStr());
    }
    
    mtlpp::DepthStencilDescriptor depthStateDesc2;
    depthStateDesc2.SetDepthWriteEnabled(NO);
    
    mtlpp::StencilDescriptor frontStencilDesc2;
    frontStencilDesc2.SetStencilCompareFunction(mtlpp::CompareFunction::NotEqual);
    frontStencilDesc2.SetStencilFailureOperation(mtlpp::StencilOperation::Keep);
    frontStencilDesc2.SetDepthFailureOperation(mtlpp::StencilOperation::Keep);
    frontStencilDesc2.SetDepthStencilPassOperation(mtlpp::StencilOperation::Replace);
    frontStencilDesc2.SetReadMask(0xFF);
    frontStencilDesc2.SetWriteMask(1);
    
    depthStateDesc2.SetFrontFaceStencil(frontStencilDesc2);
    depthStateDesc2.SetBackFaceStencil(nil);
    
    depthState2_ = device_.NewDepthStencilState(depthStateDesc2);
    
    for(NSUInteger i = 0; i < max_buffers_in_flight; i++)
    {
        dynamicUniformBuffer_[i] = device_.NewBuffer(sizeof(Uniforms), mtlpp::ResourceOptions::StorageModeShared);
        dynamicUniformBuffer_[i].SetLabel(@"UniformBuffer");
        dynamicUniformBuffer2_[i] = device_.NewBuffer(sizeof(Uniforms), mtlpp::ResourceOptions::StorageModeShared);
        dynamicUniformBuffer2_[i].SetLabel(@"UniformBuffer2");
    }
    
    commandQueue_ = device_.NewCommandQueue();
    
    render_pass_descriptor_.GetColorAttachments()[0].SetLoadAction(mtlpp::LoadAction::Clear);
    render_pass_descriptor_.GetColorAttachments()[0].SetStoreAction(mtlpp::StoreAction::Store);
    render_pass_descriptor_.GetColorAttachments()[0].SetClearColor(mtlpp::ClearColor(0, 0, 0, 1.0));
    
}

void renderer::load_assets() {
    ns::AutoReleasedError a_error;
    
    MTKMeshBufferAllocator *metalAllocator = [[[MTKMeshBufferAllocator alloc] initWithDevice:device_] autorelease];
    MDLMesh *mdlMesh = [[MDLMesh newBoxWithDimensions:(vector_float3){4, 4, 4}
                                             segments:(vector_uint3){2, 2, 2}
                                         geometryType:MDLGeometryTypeTriangles
                                        inwardNormals:NO
                                            allocator:metalAllocator] autorelease];
    MDLVertexDescriptor *mdlVertexDescriptor = MTKModelIOVertexDescriptorFromMetal(mtlVertexDescriptor_);
    
    mdlVertexDescriptor.attributes[VertexAttributePosition].name = MDLVertexAttributePosition;
    mdlVertexDescriptor.attributes[VertexAttributeTexcoord].name = MDLVertexAttributeTextureCoordinate;
    
    mdlMesh.vertexDescriptor = mdlVertexDescriptor;
    
    mesh_ = [[MTKMesh alloc] initWithMesh:mdlMesh device:device_ error:a_error.GetInnerPtr()];
    if(!mesh_) {
        NSLog(@"Error creating MetalKit mesh %@", a_error.GetPtr().localizedDescription);
    }
    
    MTKTextureLoader* textureLoader = [[[MTKTextureLoader alloc] initWithDevice:device_] autorelease];
    
    NSDictionary *textureLoaderOptions =
    @{
        MTKTextureLoaderOptionTextureUsage       : @(MTLTextureUsageShaderRead),
        MTKTextureLoaderOptionTextureStorageMode : @(MTLStorageModePrivate)
    };
    
    colorMap_ = [[textureLoader newTextureWithName:@"ColorMap" scaleFactor:1.0 bundle:nil options:textureLoaderOptions error:a_error.GetInnerPtr()] autorelease];
    if (!colorMap_) {
        NSLog(@"Error creating texture %@", a_error.GetPtr().localizedDescription);
    }
}

void renderer::update_game_state(double delta)
{
//    std::cout << "delta : " << delta << std::endl;
    
    
    Uniforms * uniforms = (Uniforms*)dynamicUniformBuffer_[uniformBufferIndex_].GetContents();
    Uniforms * uniforms2 = (Uniforms*)dynamicUniformBuffer2_[uniformBufferIndex_].GetContents();
    
    uniforms->projectionMatrix = projectionMatrix_;
    uniforms2->projectionMatrix = projectionMatrix_;
    
    vector_float3 rotationAxis = {1, 1, 0};
    matrix_float4x4 modelMatrix = ::matrix4x4_rotation(rotation_, rotationAxis);
    matrix_float4x4 viewMatrix = matrix4x4_translation(0.0, 0.0, -8.0);
    
    matrix_float4x4 scale = matrix4x4_scale(1.1, 1.1, 1.1);
    matrix_float4x4 modelMatrixScaled = matrix_multiply(modelMatrix, scale);
    
    uniforms->modelViewMatrix = matrix_multiply(viewMatrix, modelMatrix);
    uniforms2->modelViewMatrix = matrix_multiply(viewMatrix, modelMatrixScaled);

//    rotation_ += .01;
    rotation_ += delta;
}

void renderer::build_depth_texture(CGSize drawable_size)
{
    mtlpp::TextureDescriptor desc = mtlpp::TextureDescriptor::Texture2DDescriptor(mtlpp::PixelFormat::Depth32Float_Stencil8, drawable_size.width, drawable_size.height, NO);
    desc.SetUsage(mtlpp::TextureUsage::RenderTarget);
    desc.SetStorageMode(mtlpp::StorageMode::Memoryless);
    depthTexture_ = device_.NewTexture(desc);
    depthTexture_.SetLabel("Depth Texture");
    
    mtlpp::RenderPassDepthAttachmentDescriptor depthAttachment;
    mtlpp::RenderPassStencilAttachmentDescriptor stencilAttachment;
    depthAttachment.SetTexture(depthTexture_);
    depthAttachment.SetLoadAction(mtlpp::LoadAction::Clear);
    depthAttachment.SetStoreAction(mtlpp::StoreAction::DontCare);
    render_pass_descriptor_.SetDepthAttachment(depthAttachment);
    
    stencilAttachment.SetTexture(depthTexture_);
    stencilAttachment.SetLoadAction(mtlpp::LoadAction::Clear);
    stencilAttachment.SetStoreAction(mtlpp::StoreAction::DontCare);
    render_pass_descriptor_.SetStencilAttachment(stencilAttachment);
}

void renderer::drawable_resize(CGSize drawable_size) {
    std::cout << "drawable resize : width : " << drawable_size.width << " height : " << drawable_size.height << std::endl;
    float aspect = drawable_size.width / (float)drawable_size.height;
    projectionMatrix_ = matrix_perspective_right_hand(65.0f * (M_PI / 180.0f), aspect, 0.1f, 100.0f);
    build_depth_texture(drawable_size);
}

void renderer::update_current_drawable(mtlpp::Drawable next_drawable) {
    currentDrawable_ = next_drawable;
}

void renderer::update_pass_descriptor(mtlpp::Texture texture) {
    render_pass_descriptor_.GetColorAttachments()[0].SetTexture(texture);
}

void renderer::render(/*double delta*/) {
    dispatch_semaphore_wait(in_flight_semaphore_, DISPATCH_TIME_FOREVER);
    
    uniformBufferIndex_ = (uniformBufferIndex_ + 1) % max_buffers_in_flight;
    
    mtlpp::CommandBuffer commandBuffer = commandQueue_.CommandBuffer();
    commandBuffer.SetLabel("MyCommand");
    
    __block dispatch_semaphore_t block_sema = in_flight_semaphore_;
    commandBuffer.AddCompletedHandler(^(const mtlpp::CommandBuffer& buffer) {
        dispatch_semaphore_signal(block_sema);
    });
    
//    if (!currentDrawable_.GetPtr()) {
//        return;
//    }
//    mtlpp::Drawable currentDrawable = metal_layer.nextDrawable;
//
//    if (!currentDrawable.GetPtr()) {
//        return;
//    }
//    update_game_state(delta);
//    CGSize drawable_size = metal_layer.drawableSize;
//    if ([depthTexture_.GetPtr() width] != drawable_size.width || [depthTexture_.GetPtr() height] != drawable_size.height)
//    {
//        std::cout << "drawable not equal texture : width : " << drawable_size.width << " height : " << drawable_size.height << std::endl;
//        build_depth_texture(drawable_size);
//    }
    /// Delay getting the currentRenderPassDescriptor until absolutely needed. This avoids
    ///   holding onto the drawable and blocking the display pipeline any longer than necessary
    
//    mtlpp::Texture texture = ((id<CAMetalDrawable>)currentDrawable_.GetPtr()).texture;
//    drawableRenderDescriptor_.GetColorAttachments()[0].SetTexture(texture);
    
    /// Final pass rendering code here
    
    mtlpp::RenderCommandEncoder renderEncoder = commandBuffer.RenderCommandEncoder(render_pass_descriptor_);
    renderEncoder.SetLabel("MyRenderEncoder");
//    mtlpp::Viewport viewport(0, 0, (float)drawable_size.width, (float)drawable_size.height, 0.1, 1);
//    renderEncoder.SetViewport(viewport);
    renderEncoder.PushDebugGroup("DrawBox");
    
    renderEncoder.SetFrontFacingWinding(mtlpp::Winding::CounterClockwise);
    renderEncoder.SetCullMode(mtlpp::CullMode::Back);
    renderEncoder.SetRenderPipelineState(pipelineState_);
    renderEncoder.SetDepthStencilState(depthState_);
    renderEncoder.SetStencilReferenceValue(1, 1);
    
    renderEncoder.SetVertexBuffer(dynamicUniformBuffer_[uniformBufferIndex_]
                                  ,0
                                  ,BufferIndexUniforms);
    
    renderEncoder.SetFragmentBuffer(dynamicUniformBuffer_[uniformBufferIndex_]
                                    ,0
                                    ,BufferIndexUniforms);
    
    for (NSUInteger bufferIndex = 0; bufferIndex < mesh_.vertexBuffers.count; bufferIndex++)
    {
        MTKMeshBuffer *vertexBuffer = mesh_.vertexBuffers[bufferIndex];
        if((NSNull*)vertexBuffer != [NSNull null])
        {
            renderEncoder.SetVertexBuffer(vertexBuffer.buffer
                                          ,uint32_t(vertexBuffer.offset)
                                          ,uint32_t(bufferIndex));
        }
    }
    
    renderEncoder.SetFragmentTexture(colorMap_, uint32_t(TextureIndexColor));
    
    for(MTKSubmesh *submesh in mesh_.submeshes)
    {
        renderEncoder.DrawIndexed(mtlpp::PrimitiveType(submesh.primitiveType)
                                  ,uint32_t(submesh.indexCount)
                                  ,mtlpp::IndexType(submesh.indexType)
                                  ,submesh.indexBuffer.buffer
                                  ,uint32_t(submesh.indexBuffer.offset));
    }
    
    renderEncoder.PopDebugGroup();
    
    renderEncoder.PushDebugGroup("Stencil");
    
    renderEncoder.SetRenderPipelineState(pipelineState2_);
    renderEncoder.SetDepthStencilState(depthState2_);
    
    renderEncoder.SetVertexBuffer(dynamicUniformBuffer2_[uniformBufferIndex_]
                                  ,0
                                  ,BufferIndexUniforms);
    
    
    for(MTKSubmesh *submesh in mesh_.submeshes)
    {
        renderEncoder.DrawIndexed(mtlpp::PrimitiveType(submesh.primitiveType)
                                  ,uint32_t(submesh.indexCount)
                                  ,mtlpp::IndexType(submesh.indexType)
                                  ,submesh.indexBuffer.buffer
                                  ,uint32_t(submesh.indexBuffer.offset));
    }
    
    renderEncoder.PopDebugGroup();
    
    renderEncoder.EndEncoding();
    
    commandBuffer.Present(currentDrawable_);
    
    
    commandBuffer.Commit();
    commandBuffer.WaitUntilCompleted();
}
