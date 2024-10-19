#include "GraphicsAPI.hpp"
#include "Math.hpp"

// OpenGLTexture2D

UInt OpenGLTexture2D::getID() const
{
	return textureID;
}

void OpenGLTexture2D::destroy(OpenGlContext& openGlContext) const
{
	OpenGL::Textures::apiDeleteTexture(textureID);
	openGlContext.removeTexture(textureID);
}

void OpenGLTexture2D::generateMipMap() const
{
	OpenGL::Textures::apiGenerateTextureMipmap(textureID);
}

void OpenGLTexture2D::create(OpenGlContext& openGlContext, const String& name)
{
	textureID = OpenGL::Textures::apiCreateTexture(TextureTarget::Texture2D);
	OpenGL::apiObjectLabel(ObjectLabelType::Texture, textureID, name);
	openGlContext.addTexture(textureID);
}

void OpenGLTexture2D::setBorderColor(Color color) const
{
	OpenGL::Textures::apiTextureParameterfv(textureID, TextureParameterSet::BorderColor, value_ptr(color));
}

void OpenGLTexture2D::setWrapping(Wrapping wrapping) const
{
	OpenGL::Textures::apiTextureParameteri(textureID, TextureParameterSet::WrapS, static_cast<Int>(wrapping));
	OpenGL::Textures::apiTextureParameteri(textureID, TextureParameterSet::WrapT, static_cast<Int>(wrapping));
}

void OpenGLTexture2D::useTextureInSlot(const UInt textureUnitSlot) const
{
	OpenGL::Textures::apiBindTextureUnit(textureUnitSlot, textureID);
}

void OpenGLTexture2D::emptyTextureFromSlot(const UInt textureUnitSlot) const
{
	OpenGL::Textures::apiBindTextureUnit(textureUnitSlot, 0);
}

void OpenGLTexture2D::setFilters(Filter nearFilter, Filter farFilter) const
{
	OpenGL::Textures::apiTextureParameteri(textureID, TextureParameterSet::MagFilter, static_cast<Int>(nearFilter));
	OpenGL::Textures::apiTextureParameteri(textureID, TextureParameterSet::MinFilter, static_cast<Int>(farFilter));
}

void OpenGLTexture2D::setDataToDepth(const Int width, const Int height, const void* data) const
{
	constexpr Float clearData = 1.0f;
	OpenGL::Textures::apiTextureStorage2D(textureID, 1, SizedInternalFormat::Depth32F, width, height);
	if (data)
	{
		OpenGL::Textures::apiClearTexImage(textureID, 0, WritePixelsFormat::Depth, DataType::Float, data);
	}
	else
	{
		OpenGL::Textures::apiClearTexImage(textureID, 0, WritePixelsFormat::Depth, DataType::Float, &clearData);
	}
}

void OpenGLTexture2D::setData(const SizedInternalFormat internalFormat, const Int width, const Int height, const WritePixelsFormat colorFormat, const DataType dataType, const void* data) const
{
	OpenGL::Textures::apiTextureStorage2D(textureID, 1, internalFormat, width, height);
	if (data) OpenGL::Textures::apiTextureSubImage2D(textureID, 0, 0, 0, width, height, colorFormat, dataType, data);
}

void OpenGL::apiInit(OpenGlContext& openGlContext)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glewInit();");
	#endif // TRACE_OPENGL

	External::glewExperimental = GL_TRUE;
	if (const auto glewInitResult = External::glewInit(); External::glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW: " << External::glewGetErrorString(glewInitResult) << '\n';
		return;
	}

	if (External::glewIsSupported("GL_ARB_direct_state_access"))
	{
		logDebug("DSA is supported!");
	}
	else
	{
		logError("DSA is not supported!");
	}

	openGlContext.maxVertexAttributes = OpenGL::apiGetIntegerv(GetParameters::MaxVertexAttributes);
}

void OpenGL::apiFinish()
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glFinish();");
	#endif // TRACE_OPENGL
	External::glFinish();
}

UInt OpenGL::apiGetError()
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetError();");
	#endif // TRACE_OPENGL
	return External::glGetError();
}

void OpenGL::apiClearColor()
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glClear(...);");
	#endif // TRACE_OPENGL
	External::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGL::apiClearDepth()
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glClear(...);");
	#endif // TRACE_OPENGL
	External::glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLBuffer::bind() const
{
	OpenGL::Buffers::apiBindBuffer(target, bufferObjectID);
}

void OpenGLBuffer::unbind() const
{
	OpenGL::Buffers::apiBindBuffer(target, 0);
}

void OpenGLBuffer::destroy() const
{
	OpenGL::Buffers::apiDeleteBuffer(bufferObjectID);
}

void OpenGLBuffer::update(const Vec3Vector& data) const
{
	OpenGL::Buffers::apiNamedBufferData(bufferObjectID, data.size() * sizeof(Vec3), data.data(), usage);
}

void OpenGLBuffer::update(const Vec2Vector& data) const
{
	OpenGL::Buffers::apiNamedBufferData(bufferObjectID, data.size() * sizeof(Vec2), data.data(), usage);
}

void OpenGLBuffer::update(const Vector<Vec3>& data) const
{
	OpenGL::Buffers::apiNamedBufferData(bufferObjectID, data.size() * sizeof(Vec3), data.data(), usage);
}

void OpenGLBuffer::update(const Vector<Vec2>& data) const
{
	OpenGL::Buffers::apiNamedBufferData(bufferObjectID, data.size() * sizeof(Vec2), data.data(), usage);
}

void OpenGLBuffer::update(const Vector<Int>& data) const
{
	OpenGL::Buffers::apiNamedBufferData(bufferObjectID, data.size() * sizeof(Int), data.data(), usage);
}

void OpenGLBuffer::update(const Vector<UInt>& data) const
{
	OpenGL::Buffers::apiNamedBufferData(bufferObjectID, data.size() * sizeof(UInt), data.data(), usage);
}

void OpenGLBuffer::update(const Matrix& data) const
{
	OpenGL::Buffers::apiNamedBufferData(bufferObjectID, sizeof(Matrix), glm::value_ptr(data), usage);
}

void OpenGLBuffer::update(const Vector<Matrix>& data) const
{
	OpenGL::Buffers::apiNamedBufferData(bufferObjectID, data.size() * sizeof(Matrix), data.data(), usage);
}

void OpenGLBuffer::update(const GlobalUniformData& data) const
{
	OpenGL::Buffers::apiNamedBufferData(bufferObjectID, sizeof(GlobalUniformData), &data, usage);
}

OpenGlContext::~OpenGlContext()
{
	if (!textures.empty()) logError("Not all textures have been unloaded!");
}

void OpenGLShaderProgram::destroy()
{
	OpenGL::Shaders::apiDeleteProgram(programId);
	programId = 0;
}

void OpenGlContext::print() const
{
	logDebug("Culling: " + toString(culling));
	logDebug("Blending: " + toString(blending));
	logDebug("Depth Test: " + toString(depthTest));
	logDebug("Background Color: " + toString(clearColor));
	logDebug("Current Draw: " + toString(currentDrawFramebuffer));
	logDebug("Current Read: " + toString(currentReadFramebuffer));
	logDebug("Current Viewport: " + toString(viewportX) + " " + toString(viewportY) + " " + toString(viewportW) + " " + toString(viewportH));
	logDebug("Scissor Test: " + toString(scissorTest));
	logDebug("Current Scissor: " + toString(scissorX) + " " + toString(scissorY) + " " + toString(scissorW) + " " + toString(scissorH));
}

BufferID OpenGLBuffer::getID() const
{
	return bufferObjectID;
}

UInt OpenGLFramebuffer::getID() const
{
	return framebufferId;
}

void OpenGlContext::enableLogging()
{
	loggingEnabled = true;
}

void OpenGLShaderProgram::use() const
{
	OpenGL::Shaders::apiUseProgram(programId);
}

void OpenGlContext::disableLogging()
{
	loggingEnabled = false;
}

void OpenGLVertexArrayObject::create(OpenGlContext& openGlContext)
{
	vertexArrayID = OpenGL::VertexArray::apiCreateVertexArray();
	openGlContext.addVAO(vertexArrayID);
}

void OpenGLFramebuffer::destroy() const
{
	OpenGL::Framebuffer::apiDeleteFramebuffer(framebufferId);
}

void OpenGLFramebuffer::bindDraw() const
{
	OpenGL::apiBindDrawFramebuffer(framebufferId);
}

void OpenGLFramebuffer::bindRead() const
{
	OpenGL::apiBindReadFramebuffer(framebufferId);
}

void OpenGLVertexArrayObject::bind() const
{
	OpenGL::VertexArray::apiBindVertexArray(vertexArrayID);
}

Bool OpenGLFramebuffer::isComplete() const
{
	return OpenGL::Framebuffer::apiCheckNamedFramebufferStatus(framebufferId, FramebufferTarget::Draw) == FramebufferStatus::Complete;
}

BufferID OpenGL::Buffers::apiCreateBuffer()
{
	BufferID bufferID;
	External::__glewCreateBuffers(1, &bufferID);

	#ifdef TRACE_OPENGL
		openGlContext.write("glCreateBuffers(1, " + toString(bufferID) + ");");
	#endif // TRACE_OPENGL

	return bufferID;
}

void OpenGLVertexArrayObject::unbind() const
{
	OpenGL::VertexArray::apiBindVertexArray(0);
}

void OpenGL::apiSetCulling(OpenGlContext& openGlContext, const Bool value)
{
	if (value && !openGlContext.culling) apiEnable(EnableParameters::CullFace);
	if (!value && openGlContext.culling) apiDisable(EnableParameters::CullFace);
	openGlContext.culling = value;
}

void OpenGlContext::printOpenGLInfo() const
{
	const Byte* vendor = External::glGetString(GL_VENDOR);
	logDebug("Vendor: " + toString(vendor));

	const Byte* renderer = External::glGetString(GL_RENDERER);
	logDebug("Renderer: " + toString(renderer));

	const Byte* version = External::glGetString(GL_VERSION);
	logDebug("OpenGL Version: " + toString(version));

	Int maxTextureSize;
	External::glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	logDebug("Max Texture Size: " + toString(maxTextureSize));

	Int max3DTextureSize;
	External::glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max3DTextureSize);
	logDebug("Max 3D Texture Size: " + toString(max3DTextureSize));

	Int maxTextureUnits;
	External::glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	logDebug("Max Texture Units: " + toString(maxTextureUnits));

	Int maxVertexAttributes;
	External::glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);
	logDebug("Max Vertex Attributes: " + toString(maxVertexAttributes));

	Int maxUniformBufferBindings;
	External::glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
	logDebug("Max Uniform Buffer Binding Points: " + toString(maxUniformBufferBindings));

	Int maxTextureAnisotropy;
	External::glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxTextureAnisotropy);
	logDebug("Max Texture Anisotropy Level: " + toString(maxTextureAnisotropy));

	Int maxSamples;
	External::glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
	logDebug("Max Samples (MSAA): " + toString(maxSamples));

	Int maxViewports;
	External::glGetIntegerv(GL_MAX_VIEWPORTS, &maxViewports);
	logDebug("Max Viewports: " + toString(maxViewports));

	Int maxVertexUniformComponents;
	External::glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents);
	logDebug("Max Vertex Uniform Components: " + toString(maxVertexUniformComponents));

	Int maxFragmentUniformComponents;
	External::glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniformComponents);
	logDebug("Max Fragment Uniform Components: " + toString(maxFragmentUniformComponents));

	Int maxGeometryUniformComponents;
	External::glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &maxGeometryUniformComponents);
	logDebug("Max Geometry Uniform Components: " + toString(maxGeometryUniformComponents));

	Int maxVertexTextureImageUnits;
	External::glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextureImageUnits);
	logDebug("Max Vertex Texture Image Units: " + toString(maxVertexTextureImageUnits));

	Int maxGeometryTextureImageUnits;
	External::glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &maxGeometryTextureImageUnits);
	logDebug("Max Geometry Texture Image Units: " + toString(maxGeometryTextureImageUnits));

	Int maxComputeTextureImageUnits;
	External::glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &maxComputeTextureImageUnits);
	logDebug("Max Compute Texture Image Units: " + toString(maxComputeTextureImageUnits));

	Int maxComputeUniformComponents;
	External::glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &maxComputeUniformComponents);
	logDebug("Max Compute Uniform Components: " + toString(maxComputeUniformComponents));

	Int maxComputeWorkGroupCount[3];
	External::glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxComputeWorkGroupCount[0]);
	External::glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxComputeWorkGroupCount[1]);
	External::glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxComputeWorkGroupCount[2]);
	logDebug("Max Compute Work Group Count: (" + toString(maxComputeWorkGroupCount[0]) + ", " + toString(maxComputeWorkGroupCount[1]) + ", " + toString(maxComputeWorkGroupCount[2]) + ")");

	Int maxComputeWorkGroupSize[3];
	External::glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxComputeWorkGroupSize[0]);
	External::glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxComputeWorkGroupSize[1]);
	External::glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxComputeWorkGroupSize[2]);
	logDebug("Max Compute Work Group Size: (" + toString(maxComputeWorkGroupSize[0]) + ", " + toString(maxComputeWorkGroupSize[1]) + ", " + toString(maxComputeWorkGroupSize[2]) + ")");

	Int maxComputeSharedMemorySize;
	External::glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &maxComputeSharedMemorySize);
	logDebug("Max Compute Shared Memory Size: " + toString(maxComputeSharedMemorySize) + " bytes");

	Int maxComputeImageUniforms;
	External::glGetIntegerv(GL_MAX_COMPUTE_IMAGE_UNIFORMS, &maxComputeImageUniforms);
	logDebug("Max Compute Image Uniforms: " + toString(maxComputeImageUniforms));

	Int maxComputeAtomicCounters;
	External::glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &maxComputeAtomicCounters);
	logDebug("Max Compute Atomic Counters: " + toString(maxComputeAtomicCounters));

	Int maxComputeAtomicCounterBuffers;
	External::glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &maxComputeAtomicCounterBuffers);
	logDebug("Max Compute Atomic Counter Buffers: " + toString(maxComputeAtomicCounterBuffers));

	Int maxCombinedComputeUniformComponents;
	External::glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &maxCombinedComputeUniformComponents);
	logDebug("Max Combined Compute Uniform Components: " + toString(maxCombinedComputeUniformComponents));
}

ProgramID OpenGL::Shaders::apiCreateProgram()
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glCreateProgram(...);");
	#endif // TRACE_OPENGL
	return External::glCreateProgram();
}

void OpenGL::apiSetBlending(OpenGlContext& openGlContext, const Bool value)
{
	if (value && !openGlContext.blending) apiEnable(EnableParameters::Blend);
	if (!value && openGlContext.blending) apiDisable(EnableParameters::Blend);
	openGlContext.blending = value;
}

void OpenGLVertexArrayObject::destroy(OpenGlContext& openGlContext) const
{
	OpenGL::VertexArray::apiDeleteVertexArray(vertexArrayID);
	openGlContext.removeVAO(vertexArrayID);
}

void OpenGL::apiSetDepthTest(OpenGlContext& openGlContext, const Bool value)
{
	if (value && !openGlContext.depthTest) apiEnable(EnableParameters::DepthTest);
	if (!value && openGlContext.depthTest) apiDisable(EnableParameters::DepthTest);
	openGlContext.depthTest = value;
}

void OpenGL::apiSetDebugging(OpenGlContext& openGlContext, const Bool value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glDebugMessageCallback(nullptr,nullptr);");
	#endif // TRACE_OPENGL
	if (value)
	{
		External::glDebugMessageCallback(debugOutputCallback, nullptr);
		apiEnable(EnableParameters::DebugOutput);
		apiEnable(EnableParameters::DebugOutputSyncronous);
	}
	else
	{
		External::glDebugMessageCallback(nullptr, nullptr);
		apiDisable(EnableParameters::DebugOutput);
		apiDisable(EnableParameters::DebugOutputSyncronous);
	}
	openGlContext.debugging = value;
}

void OpenGL::apiSetScissorTest(OpenGlContext& openGlContext, const Bool value)
{
	if (value && !openGlContext.scissorTest) apiEnable(EnableParameters::ScissorTest);
	if (!value && openGlContext.scissorTest) apiDisable(EnableParameters::ScissorTest);
	openGlContext.scissorTest = value;
}

void OpenGL::apiSetClearColor(OpenGlContext& openGlContext, const Color color)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glClearColor(...);");
	#endif // TRACE_OPENGL
	if (openGlContext.clearColor != color)
	{
		openGlContext.clearColor = color;
		External::glClearColor(color.r, color.g, color.b, color.a);
	}
}

void OpenGLFramebuffer::create(const String& name)
{
	framebufferId = OpenGL::Framebuffer::apiCreateFramebuffer();
	OpenGL::apiObjectLabel(ObjectLabelType::Framebuffer, framebufferId, name);
}

void OpenGL::apiEnable(EnableParameters parameter)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glEnable(" + toString(static_cast<UInt>(parameter)) + ");");
	#endif // TRACE_OPENGL
	External::glEnable(static_cast<UInt>(parameter));
}

void OpenGL::apiDisable(EnableParameters parameter)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glDisable(" + toString(static_cast<UInt>(parameter)) + ");");
	#endif // TRACE_OPENGL

	External::glDisable(static_cast<UInt>(parameter));
}

Int OpenGL::apiGetIntegerv(GetParameters parameter)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetIntegerv(...);");
	#endif // TRACE_OPENGL
	Int result;
	External::glGetIntegerv(static_cast<UInt>(parameter), &result);
	return result;
}

void OpenGLVertexArrayObject::unbindIndexBuffer() const
{
	OpenGL::VertexArray::apiVertexArrayElementBuffer(vertexArrayID, 0);
}

void OpenGlContext::write(const String& message) const
{
	#ifdef TRACE_OPENGL
		if(loggingEnabled) log.write(message);
	#endif
}

void OpenGL::Buffers::apiDeleteBuffer(const UInt buffer)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glDeleteBuffers(...);");
	#endif // TRACE_OPENGL
	External::glDeleteBuffers(1, &buffer);
}

VertexArrayID OpenGL::VertexArray::apiCreateVertexArray()
{
	VertexArrayID vertexArrayID;
	External::glCreateVertexArrays(1, &vertexArrayID);

	#ifdef TRACE_OPENGL
		openGlContext.write("glCreateVertexArrays(1," + toString(vertexArrayID) + ");");
	#endif // TRACE_OPENGL

	return vertexArrayID;
}

FramebufferID OpenGL::Framebuffer::apiCreateFramebuffer()
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glCreateFramebuffers(...);");
	#endif // TRACE_OPENGL
	FramebufferID framebufferID;
	External::glCreateFramebuffers(1, &framebufferID);
	return framebufferID;
}

void OpenGlContext::addTexture(const TextureID textureID)
{
	if (openglStateTracking)
	{
		const auto it = std::ranges::find(textures, textureID);
		if (it != textures.end() && errorCheckingEnabled) logError("Texture with same ID has already been created!");
		textures.push_back(textureID);
	}
}

void OpenGL::apiBlendEquation(const BlendEquation equation)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glBlendEquation(" + toString(static_cast<UInt>(equation)) + ");");
	#endif // TRACE_OPENGL
	External::glBlendEquation(static_cast<UInt>(equation));
}

void OpenGL::apiPolygonMode(const UInt face, const UInt mode)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glPolygonMode(" + toString(face) + ", " + toString(mode) + ");");
	#endif // TRACE_OPENGL
	External::glPolygonMode(face, mode);
}

void OpenGL::Shaders::apiUseProgram(const ProgramID programId)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glUseProgram(...);");
	#endif // TRACE_OPENGL
	External::glUseProgram(programId);
}

void OpenGlContext::removeTexture(const TextureID textureID)
{
	if (openglStateTracking)
	{
		if (std::ranges::find(textures, textureID) != textures.end())
		{
			textures.remove(textureID);
		}
		else if (errorCheckingEnabled) logError("Deleting texture ID that does not exist!");
	}
}

void OpenGL::Shaders::apiDeleteShader(const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glDeleteShader(...);");
	#endif // TRACE_OPENGL
	External::glDeleteShader(shaderId);
}

void OpenGlContext::addVAO(const VertexArrayID vertexArrayID)
{
	if (openglStateTracking)
	{
		const auto it = std::ranges::find(vaos, vertexArrayID);
		if (it != vaos.end() && errorCheckingEnabled) logError("VAO with same ID has already been created!");
		vaos.push_back(vertexArrayID);
	}
}

void OpenGL::Shaders::apiCompileShader(const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glCompileShader(...);");
	#endif // TRACE_OPENGL
	External::glCompileShader(shaderId);
}

void OpenGL::Shaders::apiLinkProgram(const ProgramID programId)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glLinkProgram(...);");
	#endif // TRACE_OPENGL
	External::glLinkProgram(programId);
}

ShaderID OpenGL::Shaders::apiCreateShader(const ShaderType type)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glCreateShader(...);");
	#endif // TRACE_OPENGL
	return External::glCreateShader(static_cast<UInt>(type));
}

void OpenGLFramebuffer::clearDepth(const Float clearDepth) const
{
	OpenGL::Framebuffer::apiClearNamedFramebufferfv(framebufferId, ClearBufferType::Depth, FramebufferAttachment::Depth, &clearDepth);
}

void OpenGL::apiClipControl(const UInt origin, const UInt depth)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glClipControl(" + toString(origin) + ", " + toString(depth) + ");");
	#endif // TRACE_OPENGL
	External::glClipControl(origin, depth);
}

void OpenGL::Shaders::apiDeleteProgram(const ProgramID programId)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glDeleteProgram(...);");
	#endif // TRACE_OPENGL
	External::glDeleteProgram(programId);
}

void OpenGlContext::removeVAO(const VertexArrayID vertexArrayID)
{
	if (openglStateTracking)
	{
		if (std::ranges::find(vaos, vertexArrayID) != vaos.end())
		{
			vaos.remove(vertexArrayID);
		}
		else if (errorCheckingEnabled) logError("Deleting VAO ID that does not exist!");
	}
}

TextureID OpenGL::Textures::apiCreateTexture(TextureTarget target)
{
	TextureID textureID;
	#ifdef TRACE_OPENGL
		openGlContext.write("glCreateTextures(...);");
	#endif // TRACE_OPENGL
	External::glCreateTextures(static_cast<UInt>(target), 1, &textureID);
	#ifdef DEBUG_OPENGL
		openGlContext.addTexture(textureID);
	#endif // DEBUG_OPENGL
	return textureID;
}

void OpenGL::Textures::apiDeleteTexture(const TextureID textureID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glDeleteTextures(...);");
	#endif // TRACE_OPENGL
	External::glDeleteTextures(1, &textureID);
	#ifdef DEBUG_OPENGL
		openGlContext.removeTexture(textureID);
	#endif // DEBUG_OPENGL
}

String OpenGL::Shaders::apiGetShaderInfoLog(const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetShaderInfoLog(...);");
	#endif // TRACE_OPENGL
	const Int    logLength = apiGetShaderIntegerValue(shaderId, ShaderParameters::InfoLogLength);
	Vector<Char> log(logLength);
	External::glGetShaderInfoLog(shaderId, logLength, nullptr, log.data());
	return {log.begin(), log.end()};
}

void OpenGL::apiBindDrawFramebuffer(const FramebufferID framebufferID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glBindFramebuffer(" + toString(static_cast<UInt>(FramebufferTarget::Draw)) + "," + toString(framebufferID) + ");");
	#endif // TRACE_OPENGL
	External::glBindFramebuffer(static_cast<UInt>(FramebufferTarget::Draw), framebufferID);
}

void OpenGL::apiBindReadFramebuffer(const FramebufferID framebufferID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glBindFramebuffer(" + toString(static_cast<UInt>(FramebufferTarget::Read)) + "," + toString(framebufferID) + ");");
	#endif // TRACE_OPENGL
	External::glBindFramebuffer(static_cast<UInt>(FramebufferTarget::Read), framebufferID);
}

String OpenGL::Shaders::apiGetProgramInfoLog(const ProgramID programId)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetProgramInfoLog(...);");
	#endif // TRACE_OPENGL
	const Int    logLength = apiGetProgramIntegerValue(programId, ShaderProgramParameters::InfoLogLength);
	Vector<Char> log(logLength);
	External::glGetProgramInfoLog(programId, logLength, nullptr, log.data());
	return {log.begin(), log.end()};
}

void OpenGLFramebuffer::drawBuffers(const Vector<UInt>& attachments) const
{
	OpenGL::Framebuffer::apiNamedFramebufferDrawBuffers(framebufferId, attachments);
}

void OpenGL::Textures::apiGenerateTextureMipmap(const TextureID textureID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGenerateTextureMipmap(...);");
	#endif // TRACE_OPENGL
	External::glGenerateTextureMipmap(textureID);
}

void OpenGL::apiScissor(OpenGlContext& openGlContext, const Int x, const Int y, const Int w, const Int h)
{
	if (openGlContext.scissorX != x || openGlContext.scissorY != y || openGlContext.scissorW != w || openGlContext.scissorH != h)
	{
		#ifdef TRACE_OPENGL
			openGlContext.write("glScissor(" + toString(x) + ", " + toString(y) + ", " + toString(w) + ", " + toString(h) + ");");
		#endif // TRACE_OPENGL
		External::glScissor(x, y, w, h);
		openGlContext.scissorX = x;
		openGlContext.scissorY = y;
		openGlContext.scissorW = w;
		openGlContext.scissorH = h;
	}
}

Int OpenGL::Framebuffer::attachmentToIndex(FramebufferAttachment attachment)
{
	if (attachment == FramebufferAttachment::Depth) return 0;
	return static_cast<Int>(attachment) - GL_COLOR_ATTACHMENT0;
}

void OpenGL::apiViewport(OpenGlContext& openGlContext, const Int x, const Int y, const Int w, const Int h)
{
	if (openGlContext.viewportX != x || openGlContext.viewportY != y || openGlContext.viewportW != w || openGlContext.viewportH != h)
	{
		#ifdef TRACE_OPENGL
			openGlContext.write("glViewport(" + toString(x) + ", " + toString(y) + ", " + toString(w) + ", " + toString(h) + ");");
		#endif // TRACE_OPENGL
		External::glViewport(x, y, w, h);
		openGlContext.viewportX = x;
		openGlContext.viewportY = y;
		openGlContext.viewportW = w;
		openGlContext.viewportH = h;
	}
}

void OpenGL::VertexArray::apiBindVertexArray(const VertexArrayID vertexArrayID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glBindVertexArray(...);");
	#endif // TRACE_OPENGL
	External::glBindVertexArray(vertexArrayID);
}

void OpenGLVertexArrayObject::bindIndexBuffer(const BufferID indexBufferID) const
{
	OpenGL::VertexArray::apiVertexArrayElementBuffer(vertexArrayID, indexBufferID);
}

void OpenGL::Framebuffer::apiDeleteFramebuffer(const FramebufferID framebufferID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glDeleteFramebuffers(...);");
	#endif // TRACE_OPENGL
	External::glDeleteFramebuffers(1, &framebufferID);
}

void OpenGL::VertexArray::apiDeleteVertexArray(const VertexArrayID& vertexArrayID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glDeleteVertexArrays(...);");
	#endif // TRACE_OPENGL
	External::glDeleteVertexArrays(1, &vertexArrayID);
}

void OpenGL::Buffers::apiBindBuffer(const BufferTarget target, const BufferID bufferID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glBindBuffer(...);");
	#endif // TRACE_OPENGL
	External::glBindBuffer(static_cast<UInt>(target), bufferID);
}

void OpenGL::Shaders::apiAttachShader(const ProgramID programId, const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glAttachShader(...);");
	#endif // TRACE_OPENGL
	External::glAttachShader(programId, shaderId);
}

void OpenGL::Shaders::apiDetachShader(const ProgramID programId, const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glDetachShader(...);");
	#endif // TRACE_OPENGL
	External::glDetachShader(programId, shaderId);
}

void OpenGLVertexArrayObject::unbindVertexBuffer(const AttributeIndex attributeIndex) const
{
	OpenGL::VertexArray::apiVertexArrayVertexBuffer(vertexArrayID, attributeIndex, 0, 0, 0);
	OpenGL::VertexArray::apiDisableVertexArrayAttrib(vertexArrayID, attributeIndex);
}

void OpenGL::apiObjectLabel(ObjectLabelType type, const UInt objectID, const String& label)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glObjectLabel(" + toString(static_cast<UInt>(type)) + "," + toString(objectID) + ",\"" + label + "\");");
	#endif // TRACE_OPENGL
	External::glObjectLabel(static_cast<UInt>(type), objectID, static_cast<Int>(label.size()), label.c_str());
}

Vec4 OpenGLFramebuffer::readPixelVec4(const Int x, const Int y, const FramebufferMode mode) const
{
	auto           data   = Vec4(0);
	constexpr Int  width  = 1;
	constexpr Int  height = 1;
	constexpr auto type   = DataType::Float;
	constexpr auto format = ReadPixelsFormat::RGBA;
	OpenGL::Framebuffer::apiNamedFramebufferReadBuffer(framebufferId, mode);
	OpenGL::Framebuffer::apiReadPixels(x, y, width, height, format, type, glm::value_ptr(data));
	return data;
}

void OpenGL::apiBlendFunc(const BlendFunction sourceFactor, const BlendFunction destinationFactor)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glBlendFunc(" + toString(static_cast<UInt>(sourceFactor)) + ", " + toString(static_cast<UInt>(destinationFactor)) + ");");
	#endif // TRACE_OPENGL
	External::glBlendFunc(static_cast<UInt>(sourceFactor), static_cast<UInt>(destinationFactor));
}

IVec4 OpenGLFramebuffer::readPixelIVec4(const Int x, const Int y, const FramebufferMode mode) const
{
	auto           data   = IVec4(0);
	constexpr Int  width  = 1;
	constexpr Int  height = 1;
	constexpr auto type   = DataType::UInt;
	constexpr auto format = ReadPixelsFormat::RGBAInteger;
	OpenGL::Framebuffer::apiNamedFramebufferReadBuffer(framebufferId, mode);
	OpenGL::Framebuffer::apiReadPixels(x, y, width, height, format, type, glm::value_ptr(data));
	return data;
}

void OpenGLFramebuffer::clearColor(const FramebufferAttachment attachmentSlot, const Vec4 color) const
{
	OpenGL::Framebuffer::apiClearNamedFramebufferfv(framebufferId, ClearBufferType::Color, attachmentSlot, glm::value_ptr(color));
}

void OpenGLFramebuffer::clearColor(const FramebufferAttachment attachmentSlot, const IVec4 color) const
{
	OpenGL::Framebuffer::apiClearNamedFramebufferiv(framebufferId, ClearBufferType::Color, attachmentSlot, glm::value_ptr(color));
}

void OpenGL::Textures::apiBindTextureUnit(const TextureSlot textureUnitSlot, const TextureID textureID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glBindTextureUnit(...);");
	#endif // TRACE_OPENGL
	External::glBindTextureUnit(textureUnitSlot, textureID);
}

Int OpenGL::Shaders::apiGetShaderIntegerValue(const ShaderID shaderId, const ShaderParameters parameter)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetShaderiv(...);");
	#endif // TRACE_OPENGL
	Int result;
	External::glGetShaderiv(shaderId, static_cast<UInt>(parameter), &result);
	return result;
}

void OpenGLIndexBuffer::create(const Vector<UInt>& indices, const BufferUsage usage, const String& name)
{
	OpenGLBuffer::create(BufferTarget::ElementArray, indices, usage, name, 0);
}

void OpenGL::VertexArray::apiDrawElements(const PrimitiveMode mode, const Int indexCount, const void* indices)
{
	// mode:		the primitives opengl can construct and draw e.g. triangle, triangle strip, line, etc.
	// indexCount:	the number of indices that will be used for rendering, equivalent to the total number of indices unless partial or batch rendering is used
	// indices:		would read from an actual array of indices on cpu side, however when a VAO is used, it represents an offset instead, usually 0
	#ifdef TRACE_OPENGL
		openGlContext.write("glDrawElements(" + toString(static_cast<UInt>(mode)) + ", " + toString(indexCount) + ", " + toString(GL_UNSIGNED_INT) + ", [...]);");
	#endif // TRACE_OPENGL
	External::glDrawElements(static_cast<UInt>(mode), indexCount, GL_UNSIGNED_INT, indices);
}

void OpenGLFramebuffer::attachTexture(const FramebufferAttachment attachment, const TextureID textureId) const
{
	constexpr UInt mipMapLevel = 0;
	OpenGL::Framebuffer::apiNamedFramebufferTexture(framebufferId, attachment, textureId, mipMapLevel);
}

void OpenGL::Framebuffer::apiNamedFramebufferReadBuffer(const FramebufferID framebufferID, FramebufferMode mode)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glNamedFramebufferReadBuffer(...);");
	#endif // TRACE_OPENGL
	External::glNamedFramebufferReadBuffer(framebufferID, static_cast<UInt>(mode));
}

Int OpenGL::Shaders::apiGetProgramIntegerValue(const ProgramID programId, const ShaderProgramParameters parameter)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetProgramiv(...);");
	#endif // TRACE_OPENGL
	Int value;
	External::glGetProgramiv(programId, static_cast<UInt>(parameter), &value);
	return value;
}

void OpenGL::VertexArray::apiVertexArrayElementBuffer(const VertexArrayID vertexArrayID, const BufferID indexBufferID)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glVertexArrayElementBuffer(" + toString(vertexArrayID) + ", " + toString(indexBufferID) + ");");
	#endif // TRACE_OPENGL
	External::__glewVertexArrayElementBuffer(vertexArrayID, indexBufferID);
}

void OpenGL::Textures::apiTextureParameteri(const TextureID textureId, TextureParameterSet parameter, const Int value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glTextureParameteri(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameteri(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::Framebuffer::apiNamedFramebufferDrawBuffers(const FramebufferID framebuffer, const Vector<UInt>& buffers)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glNamedFramebufferDrawBuffers(...);");
	#endif // TRACE_OPENGL
	External::glNamedFramebufferDrawBuffers(framebuffer, static_cast<Int>(buffers.size()), buffers.data());
}

void OpenGL::Textures::apiTextureParameterf(const TextureID textureId, TextureParameterSet parameter, const Float value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glTextureParameterf(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameterf(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::Textures::apiTextureParameteriv(const TextureID textureId, TextureParameterSet parameter, const Int* value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glTextureParameteriv(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameteriv(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::VertexArray::apiEnableVertexArrayAttrib(const VertexArrayID vertexArrayID, const AttributeIndex attributeID)
{
	#ifdef CHECK_OPENGL
		if(attributeID > openGlContext.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
	#endif // CHECK_OPENGL

	#ifdef TRACE_OPENGL
		openGlContext.write("glEnableVertexArrayAttrib(" + toString(vertexArrayID) + ", " + toString(attributeID) + ");");
	#endif // TRACE_OPENGL

	External::glEnableVertexArrayAttrib(vertexArrayID, attributeID);
}

void OpenGL::Textures::apiTextureParameterIiv(const TextureID textureId, TextureParameterSet parameter, const Int* value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glTextureParameterIiv(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameterIiv(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::VertexArray::apiDisableVertexArrayAttrib(const VertexArrayID vertexArrayID, const AttributeIndex attributeID)
{
	#ifdef CHECK_OPENGL
		if(attributeID > openGlContext.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
	#endif // CHECK_OPENGL

	#ifdef TRACE_OPENGL
		openGlContext.write("glDisableVertexArrayAttrib(...);");
	#endif // TRACE_OPENGL
	External::glDisableVertexArrayAttrib(vertexArrayID, attributeID);
}

void OpenGL::Textures::apiGetTextureParameteriv(const TextureID textureID, TextureParameterGet parameterName, Int& output)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetTextureParameteriv(...);");
	#endif // TRACE_OPENGL
	External::glGetTextureParameteriv(textureID, static_cast<UInt>(parameterName), &output);
}

Bool OpenGLShaderProgram::create(const String& name, const String& vertexShaderSource, const String& fragmentShaderSource)
{
	const UInt vertexShaderId   = OpenGL::Shaders::apiCreateShader(ShaderType::Vertex);
	const UInt fragmentShaderId = OpenGL::Shaders::apiCreateShader(ShaderType::Fragment);

	const char* vertexShaderSourcePtr   = vertexShaderSource.c_str();
	const char* fragmentShaderSourcePtr = fragmentShaderSource.c_str();

	OpenGL::Shaders::apiShaderSource(vertexShaderId, 1, &vertexShaderSourcePtr, nullptr);
	OpenGL::Shaders::apiShaderSource(fragmentShaderId, 1, &fragmentShaderSourcePtr, nullptr);

	OpenGL::Shaders::apiCompileShader(vertexShaderId);
	OpenGL::Shaders::apiCompileShader(fragmentShaderId);

	const Int vertexCompileStatus   = OpenGL::Shaders::apiGetShaderIntegerValue(vertexShaderId, ShaderParameters::CompileStatus);
	const Int fragmentCompileStatus = OpenGL::Shaders::apiGetShaderIntegerValue(fragmentShaderId, ShaderParameters::CompileStatus);

	if (vertexCompileStatus != 1)
	{
		const String vertexInfoLog = OpenGL::Shaders::apiGetShaderInfoLog(vertexShaderId);
		logError("Linking shader program (" + name + ")failed. Error: " + vertexInfoLog);
	}

	if (fragmentCompileStatus != 1)
	{
		const String fragmentInfoLog = OpenGL::Shaders::apiGetShaderInfoLog(fragmentShaderId);
		logError("Linking shader program (" + name + ")failed. Error: " + fragmentInfoLog);
	}

	programId = OpenGL::Shaders::apiCreateProgram();

	OpenGL::Shaders::apiAttachShader(programId, vertexShaderId);
	OpenGL::Shaders::apiAttachShader(programId, fragmentShaderId);

	OpenGL::Shaders::apiLinkProgram(programId);

	const Int linkStatus = OpenGL::Shaders::apiGetProgramIntegerValue(programId, ShaderProgramParameters::LinkStatus);

	if (linkStatus != 1)
	{
		const String log = OpenGL::Shaders::apiGetProgramInfoLog(programId);
		logError("Linking shader program (" + name + ")failed. Error: " + log);
	}

	if (!linkStatus || !vertexCompileStatus || !fragmentCompileStatus)
	{
		OpenGL::Shaders::apiDeleteShader(vertexShaderId);
		OpenGL::Shaders::apiDeleteShader(fragmentShaderId);
		OpenGL::Shaders::apiDeleteProgram(programId);
		programId = -1;
		return false;
	}

	OpenGL::Shaders::apiUniformBlockBinding(programId, 0, 0);

	OpenGL::Shaders::apiDetachShader(programId, vertexShaderId);
	OpenGL::Shaders::apiDetachShader(programId, fragmentShaderId);

	OpenGL::Shaders::apiDeleteShader(vertexShaderId);
	OpenGL::Shaders::apiDeleteShader(fragmentShaderId);

	return true;
}

void OpenGL::Textures::apiTextureParameterfv(const TextureID textureId, TextureParameterSet parameter, const Float* value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glTextureParameterfv(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameterfv(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::Buffers::apiNamedBufferData(const BufferID bufferID, const ULL byteCount, const void* data, BufferUsage usage)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glNamedBufferData(" + toString(bufferID) + ", " + toString(byteCount) + ", [...] , " + toString(static_cast<UInt>(usage)) + ");");
	#endif // TRACE_OPENGL
	// use for dynamic buffer data, like uniform buffer
	External::glNamedBufferData(bufferID, static_cast<LL>(byteCount), data, static_cast<UInt>(usage));
}

void OpenGL::Textures::apiGetTextureParameterIiv(const TextureID textureID, TextureParameterGet parameterName, Int* output)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetTextureParameterIiv(...);");
	#endif // TRACE_OPENGL
	External::glGetTextureParameterIiv(textureID, static_cast<UInt>(parameterName), output);
}

void OpenGL::Textures::apiTextureParameterIuiv(const TextureID textureId, TextureParameterSet parameter, const UInt* value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glTextureParameterIuiv(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameterIuiv(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::Textures::apiGetTextureParameterfv(const TextureID textureID, TextureParameterGet parameterName, Float& output)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetTextureParameterfv(...);");
	#endif // TRACE_OPENGL
	External::glGetTextureParameterfv(textureID, static_cast<UInt>(parameterName), &output);
}

void OpenGL::Textures::apiGetTextureParameterIuiv(const TextureID textureID, TextureParameterGet parameterName, UInt* output)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glGetTextureParameterIuiv(...);");
	#endif // TRACE_OPENGL
	External::glGetTextureParameterIuiv(textureID, static_cast<UInt>(parameterName), output);
}

void OpenGL::Buffers::apiBindBufferBase(const BufferTarget target, const BindingPoint bindingLocation, const BufferID buffer)
{
	// binds a buffer to a binding point for that particular target
	// e.g. you can bind a uniform buffer using its bufferID to BufferTarget::Uniform at binding point 0

	#ifdef TRACE_OPENGL
		openGlContext.write("glBindBufferBase(...);");
	#endif // TRACE_OPENGL
	External::glBindBufferBase(static_cast<UInt>(target), bindingLocation, buffer);
}

void OpenGL::Shaders::apiShaderSource(const ShaderID shaderId, const Int count, const Char* const* string, const Int* length)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glShaderSource(...);");
	#endif // TRACE_OPENGL
	External::glShaderSource(shaderId, count, string, length);
}

void OpenGLVertexArrayObject::render(const PrimitiveMode primitiveMode, const Int indexCount, const void* indicesOrOffset) const
{
	OpenGL::VertexArray::apiDrawElements(primitiveMode, indexCount, indicesOrOffset);
}

FramebufferStatus OpenGL::Framebuffer::apiCheckNamedFramebufferStatus(const FramebufferID framebufferID, FramebufferTarget target)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glCheckNamedFramebufferStatus(...);");
	#endif // TRACE_OPENGL
	return static_cast<FramebufferStatus>(External::glCheckNamedFramebufferStatus(framebufferID, static_cast<UInt>(target)));
}

void OpenGL::VertexArray::apiVertexArrayBindingDivisor(const VertexArrayID vertexArrayID, const Index layoutBinding, const UInt divisor)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glVertexArrayBindingDivisor(" + toString(vertexArrayID) + ", " + toString(layoutBinding) + ", " + toString(divisor) + ");");
	#endif // TRACE_OPENGL
	External::glVertexArrayBindingDivisor(vertexArrayID, layoutBinding, divisor);
}

void OpenGL::VertexArray::apiDrawElementsInstanced(const PrimitiveMode mode, const Int indexCount, const void* indices, const Int primitiveCount)
{
	// mode:			the primitives opengl can construct and draw e.g. triangle, triangle strip, line, etc.
	// indexCount:		the number of indices that will be used for rendering, equivalent to the total number of indices unless partial or batch rendering is used
	// indices:			would read from an actual array of indices on cpu side, however when a VBO is used, it represents an offset instead, usually nullptr
	// primitiveCount:	the number of instances, usually the number of total instances unless instances are partially rendered
	#ifdef TRACE_OPENGL
		openGlContext.write("glDrawElementsInstanced(...);");
	#endif // TRACE_OPENGL
	External::glDrawElementsInstanced(static_cast<UInt>(mode), indexCount, GL_UNSIGNED_INT, indices, primitiveCount);
}

void OpenGL::Textures::apiClearTexImage(const TextureID texture, const Int mipmapLevel, WritePixelsFormat format, DataType type, const void* data)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glClearTexImage(...);");
	#endif // TRACE_OPENGL
	External::glClearTexImage(texture, mipmapLevel, static_cast<UInt>(format), static_cast<UInt>(type), data);
}

void OpenGL::Textures::apiTextureStorage2D(const TextureID texture, const Int mipmapLevel, SizedInternalFormat internalFormat, Int width, Int height)
{
	width  = max(1, width);
	height = max(1, height);

	#ifdef TRACE_OPENGL
		openGlContext.write("glTextureStorage2D(...);");
	#endif // TRACE_OPENGL
	External::glTextureStorage2D(texture, mipmapLevel, static_cast<UInt>(internalFormat), width, height);
}

void OpenGL::VertexArray::apiVertexArrayAttribBinding(const VertexArrayID vertexArrayID, const AttributeIndex attributeID, const Index layoutBinding)
{
	#ifdef CHECK_OPENGL
		if(attributeID > openGlContext.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
	#endif // CHECK_OPENGL

	#ifdef TRACE_OPENGL
		openGlContext.write("glVertexArrayAttribBinding(" + toString(vertexArrayID) + ", " + toString(attributeID) + ", " + toString(layoutBinding) + ");");
	#endif // TRACE_OPENGL
	External::glVertexArrayAttribBinding(vertexArrayID, attributeID, layoutBinding);
}

void OpenGL::Shaders::apiUniformBlockBinding(const ProgramID programId, const UniformBlockID uniformBlockIndex, const BindingPoint uniformBlockBinding)
{
	// binds the uniform block id of each shader to a predefined binding point that shaders attach to
	#ifdef TRACE_OPENGL
		openGlContext.write("glUniformBlockBinding(...);");
	#endif // TRACE_OPENGL
	External::glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding);
}

void OpenGL::Framebuffer::apiReadPixels(const Int x, const Int y, const Int width, const Int height, ReadPixelsFormat format, DataType type, void* data)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glReadPixels(...);");
	#endif // TRACE_OPENGL
	External::glReadPixels(x, y, width, height, static_cast<UInt>(format), static_cast<UInt>(type), data);
}

void OpenGLVertexArrayObject::renderInstanced(const PrimitiveMode primitiveMode, const Int indexCount, const void* indicesOrOffset, const Int instanceCount) const
{
	OpenGL::VertexArray::apiDrawElementsInstanced(primitiveMode, indexCount, indicesOrOffset, instanceCount);
}

void OpenGL::Framebuffer::apiClearNamedFramebufferiv(const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const Int* value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glClearNamedFramebufferiv(...);");
	#endif // TRACE_OPENGL
	External::glClearNamedFramebufferiv(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), value);
}

void OpenGL::Framebuffer::apiClearNamedFramebufferfv(const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const Float* value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glClearNamedFramebufferfv(...);");
	#endif // TRACE_OPENGL
	External::glClearNamedFramebufferfv(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), value);
}

void OpenGL::Framebuffer::apiClearNamedFramebufferuiv(const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const UInt* value)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glClearNamedFramebufferuiv(...);");
	#endif // TRACE_OPENGL
	External::glClearNamedFramebufferuiv(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), value);
}

void OpenGL::Framebuffer::apiNamedFramebufferTexture(const FramebufferID framebufferID, const FramebufferAttachment attachmentID, const TextureID texture, const Int mipmapLevel)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glNamedFramebufferTexture(...);");
	#endif // TRACE_OPENGL
	External::glNamedFramebufferTexture(framebufferID, static_cast<UInt>(attachmentID), texture, mipmapLevel);
}

void OpenGL::VertexArray::apiVertexArrayVertexBuffer(const VertexArrayID vertexArrayID, const UInt layoutBinding, const VertexBufferID buffer, const LL offset, const Int stride)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glVertexArrayVertexBuffer(" + toString(vertexArrayID) + ", " + toString(layoutBinding) + ", " + toString(buffer) + ", " + toString(offset) + ", " + toString(stride) + ");");
	#endif // TRACE_OPENGL
	External::glVertexArrayVertexBuffer(vertexArrayID, layoutBinding, buffer, offset, stride);
}

void OpenGL::Framebuffer::apiClearNamedFramebufferfi(const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const Float depth, const Int stencil)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glClearNamedFramebufferfi(...);");
	#endif // TRACE_OPENGL
	External::glClearNamedFramebufferfi(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), depth, stencil);
}

void OpenGL::VertexArray::apiVertexArrayAttribFormat(const VertexArrayID vertexArrayID, const AttributeIndex attributeID, const Int size, DataType type, const Bool normalized, const UInt relativeOffset)
{
	#ifdef CHECK_OPENGL
		if(attributeID > openGlContext.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
	#endif // CHECK_OPENGL

	#ifdef TRACE_OPENGL
		openGlContext.write("glVertexArrayAttribFormat(" + toString(vertexArrayID) + ", " + toString(attributeID) + ", " + toString(size) + ", " + toString(static_cast<UInt>(type)) + ", " + toString(normalized) + ", " + toString(relativeOffset) + ");");
	#endif // TRACE_OPENGL
	External::glVertexArrayAttribFormat(vertexArrayID, attributeID, size, static_cast<UInt>(type), normalized, relativeOffset);
}

void OpenGL::Textures::apiTextureSubImage2D(const TextureID texture, const Int mipmapLevel, const Int xOffset, const Int yOffset, const Int width, const Int height, WritePixelsFormat format, DataType type, const void* pixels)
{
	#ifdef TRACE_OPENGL
		openGlContext.write("glTextureSubImage2D(...);");
	#endif // TRACE_OPENGL
	External::glPixelStorei(GL_PACK_ALIGNMENT, 1);
	External::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	External::glTextureSubImage2D(texture, mipmapLevel, xOffset, yOffset, width, height, static_cast<UInt>(format), static_cast<UInt>(type), pixels);
}

void OpenGLVertexArrayObject::bindVertexBuffer(const AttributeIndex attributeIndex, const BufferID bufferID, const LL offset, const Int stride, const UInt divisor, const Int numComponents, const DataType type, const Bool normalized, const UInt relativeOffset) const
{
	// attribute index:	the hardcoded index that represents the vertex data e.g. pos = 0, uv = 1, norm = 2 etc.
	// buffer id:		the id of the vertex buffer you are attaching to the array
	// offset:			the offset at which the first byte is read (usually 0)
	// stride:			the number of bytes moved forward to read the next vertex data (also the full size of each packed value)
	// divisor:			0 if non instanced 1 or greater if instanced
	// numComponents	assuming Float or Int how many are there in the current field e.g. Vec4 -> 4 Float
	// type				the data type stored in the buffer usually Float
	// normalized:		normalizes the value when accessed
	// relativeOffset:	the relative offset of the desired attribute within each stride/vertex usually 0 for non packed data

	OpenGL::VertexArray::apiEnableVertexArrayAttrib(vertexArrayID, attributeIndex);
	OpenGL::VertexArray::apiVertexArrayAttribBinding(vertexArrayID, attributeIndex, attributeIndex);
	OpenGL::VertexArray::apiVertexArrayVertexBuffer(vertexArrayID, attributeIndex, bufferID, offset, stride);
	OpenGL::VertexArray::apiVertexArrayBindingDivisor(vertexArrayID, attributeIndex, divisor);
	OpenGL::VertexArray::apiVertexArrayAttribFormat(vertexArrayID, attributeIndex, numComponents, type, normalized, relativeOffset);
}
