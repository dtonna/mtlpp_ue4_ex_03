//
//  ShaderCache.hpp
//  mtlpp_ue4_ex_03
//
//  Created by Noppadol Anuroje on 15/12/2564 BE.
//

#ifndef ShaderCache_hpp
#define ShaderCache_hpp

#include "renderer/mtlpp/mtlpp.hpp"

class ShaderCache {
public:
    ShaderCache() = default;
    ~ShaderCache();
    
    void CreateShader(mtlpp::Device& device);
    
    mtlpp::Library defaultLibrary_;
    mtlpp::Function vertexFunction_;
    mtlpp::Function fragmentFunction_;
    mtlpp::Function fragmentBorderFunction_;
};

#endif /* ShaderCache_hpp */
