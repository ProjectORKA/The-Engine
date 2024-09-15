#pragma once
#include "Game.hpp"
#include "Random.hpp"

//struct VoxelChunk
//{
//	ULL data[64][64];
//	OpenGLTexture2D
//
//	VoxelChunk()
//	{
//		for (int x = 0; x < 64; x++)
//		{
//			for (int y = 0; y < 64; y++)
//			{
//				data[x][y] = randomULLFast();
//			}
//		}
//	}
//
//	void upload()
//	{
//		GLuint uploadVoxelData(bool voxels[VOXEL_DIM][VOXEL_DIM][VOXEL_DIM]) {
//    GLuint textureID;
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_3D, textureID);
//
//    // Convert bool array to GLubyte for OpenGL
//    GLubyte voxelData[VOXEL_DIM * VOXEL_DIM * VOXEL_DIM];
//    for (int x = 0; x < VOXEL_DIM; ++x) {
//        for (int y = 0; y < VOXEL_DIM; ++y) {
//            for (int z = 0; z < VOXEL_DIM; ++z) {
//                voxelData[x + VOXEL_DIM * (y + VOXEL_DIM * z)] = voxels[x][y][z] ? 255 : 0;
//            }
//        }
//    }
//
//    // Upload the voxel data to the GPU
//    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, VOXEL_DIM, VOXEL_DIM, VOXEL_DIM, 0, GL_RED, GL_UNSIGNED_BYTE, voxelData);
//
//    // Set texture parameters
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//    return textureID;
//}
//	}
//};
//
//struct VoxelsRenderer : GameRenderer
//{
//	ShaderProgram program;
//
//	void update(Window& window) override {}
//
//	void create(Window& window) override
//	{
//		const String vertexShaderSource = R"glsl(
//		#version 330 core
//layout (location = 0) in vec3 position;
//out vec3 texCoords;
//void main() {
//    gl_Position = vec4(position, 1.0);
//    texCoords = position;
//}
//)glsl";
//
//		const String fragmentShaderSource = R"glsl(
//#version 330 core
//in vec3 texCoords;
//out vec4 FragColor;
//uniform sampler3D voxelTexture;
//void main() {
//    float alpha = texture(voxelTexture, texCoords).r;
//    if (alpha < 0.5)
//        discard;
//    FragColor = vec4(1.0, 1.0, 1.0, alpha);
//}
//)glsl";
//
//		program.create("voxels", vertexShaderSource, fragmentShaderSource);
//	}
//
//	void destroy(Window& window) override {}
//	void connect(GameSimulation& simulation) override {}
//	void inputEvent(Window& window, InputEvent input) override {}
//	void render(Window& window, const TiledRectangle renderArea) override {}
//	void renderInteractive(Window& window, const TiledRectangle renderArea) override {}
//};
//
//struct Voxels
//{
//	UserInterface  ui;
//	Window         window;
//	VoxelsRenderer renderer;
//
//	void run()
//	{
//		ui.create();
//		ui.window("Voxels", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
//		ui.run();
//	}
//};
