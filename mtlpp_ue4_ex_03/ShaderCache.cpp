//
//  ShaderCache.cpp
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 15/12/2564 BE.
//

#include "ShaderCache.hpp"

ShaderCache::~ShaderCache() {
}

void ShaderCache::CreateShader(mtlpp::Device &device) {
    defaultLibrary_ = device.NewDefaultLibrary();
    
    ns::AutoReleased<ns::String> vertexShader_("vertexShader");
    ns::AutoReleased<ns::String> fragmentShader_("fragmentShader");
    ns::AutoReleased<ns::String> fragmentShader2_("fragmentShader2");
    
    vertexFunction_ = defaultLibrary_.NewFunction(vertexShader_);
    fragmentFunction_ = defaultLibrary_.NewFunction(fragmentShader_);
    fragmentBorderFunction_ = defaultLibrary_.NewFunction(fragmentShader2_);
}
