#!/bin/bash

#run from root dir!
mkdir -p bin/assets
mkdir -p bin/assets/shaders

echo "Compiling shaders..."

echo "assets/shaders/Builtin_ObjShader.vert.glsl -> bin/assets/shaders/Builtin_ObjShader.vert.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=vert assets/shaders/Builtin_ObjShader.vert.glsl -o bin/assets/shaders/Builtin_ObjShader.vert.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "assets/shaders/Builtin_ObjShader.frag.glsl -> bin/assets/shaders/Builtin_ObjShader.frag.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=frag assets/shaders/Builtin_ObjShader.frag.glsl -o bin/assets/shaders/Builtin_ObjShader.frag.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "Copying assets..."
echo cp -R "assets" "bin"
cp -R "assets" "bin"

echo "Done."