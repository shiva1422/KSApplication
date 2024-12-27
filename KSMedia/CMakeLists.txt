cmake_minimum_required(VERSION 3.31.0)
add_library(ksimage KSImageLoader.cpp  ImageCodec/STBImageLoader.cpp ImageCodec/stb_image.h)
