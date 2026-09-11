#include "GraphicsAPI.hpp"
#include "Math.hpp"

// OpenGLTexture2D

UInt OpenGLTexture2D::getID() const
{
	return textureID;
}
void OpenGLTexture2D::destroy(OpenGlContext& openGlContext) const
{
	OpenGL::Textures::apiDeleteTexture(openGlContext, textureID);
	#ifdef DEBUG_OPENGL
	openGlContext.removeTexture(textureID);
	#endif // DEBUG_OPENGL
}
void OpenGLTexture2D::generateMipMap(OpenGlContext& openGlContext) const
{
	OpenGL::Textures::apiGenerateTextureMipmap(openGlContext, textureID);
}
void OpenGLTexture2D::create(OpenGlContext& openGlContext, const String& name)
{
	textureID = OpenGL::Textures::apiCreateTexture(openGlContext, TextureTarget::Texture2D);
	OpenGL::apiObjectLabel(openGlContext, ObjectLabelType::Texture, textureID, name);
	#ifdef DEBUG_OPENGL
	openGlContext.addTexture(textureID);
	#endif // DEBUG_OPENGL
}
void OpenGLTexture2D::setBorderColor(OpenGlContext& openGlContext, Color color) const
{
	OpenGL::Textures::apiTextureParameterfv(openGlContext, textureID, TextureParameterSet::BorderColor, value_ptr(color));
}
void OpenGLTexture2D::setWrapping(OpenGlContext& openGlContext, Wrapping wrapping) const
{
	OpenGL::Textures::apiTextureParameteri(openGlContext, textureID, TextureParameterSet::WrapS, static_cast<Int>(wrapping));
	OpenGL::Textures::apiTextureParameteri(openGlContext, textureID, TextureParameterSet::WrapT, static_cast<Int>(wrapping));
}
void OpenGLTexture2D::useTextureInSlot(OpenGlContext& openGlContext, const UInt textureUnitSlot) const
{
	OpenGL::Textures::apiBindTextureUnit(openGlContext, textureUnitSlot, textureID);
}
void OpenGLTexture2D::emptyTextureFromSlot(OpenGlContext& openGlContext, const UInt textureUnitSlot) const
{
	OpenGL::Textures::apiBindTextureUnit(openGlContext, textureUnitSlot, 0);
}
void OpenGLTexture2D::setFilters(OpenGlContext& openGlContext, Filter nearFilter, Filter farFilter) const
{
	OpenGL::Textures::apiTextureParameteri(openGlContext, textureID, TextureParameterSet::MagFilter, static_cast<Int>(nearFilter));
	OpenGL::Textures::apiTextureParameteri(openGlContext, textureID, TextureParameterSet::MinFilter, static_cast<Int>(farFilter));
}
void OpenGLTexture2D::setDataToDepth(OpenGlContext& openGlContext, const Int width, const Int height, const void* data) const
{
	constexpr Float clearData = 1.0f;
	OpenGL::Textures::apiTextureStorage2D(openGlContext, textureID, 1, SizedInternalFormat::Depth32F, width, height);
	if (data) OpenGL::Textures::apiClearTexImage(openGlContext, textureID, 0, WritePixelsFormat::Depth, DataType::Float, data);
	else OpenGL::Textures::apiClearTexImage(openGlContext, textureID, 0, WritePixelsFormat::Depth, DataType::Float, &clearData);
}
void OpenGLTexture2D::setData(OpenGlContext& openGlContext, const SizedInternalFormat internalFormat, const Int width, const Int height, const WritePixelsFormat colorFormat, const DataType dataType, const void* data) const
{
	OpenGL::Textures::apiTextureStorage2D(openGlContext, textureID, 1, internalFormat, width, height);
	if (data) OpenGL::Textures::apiTextureSubImage2D(openGlContext, textureID, 0, 0, 0, width, height, colorFormat, dataType, data);
}

BufferID OpenGLBuffer::getID() const
{
	return bufferObjectID;
}
void OpenGLBuffer::bind(OpenGlContext& openGlContext) const
{
	OpenGL::Buffers::apiBindBuffer(openGlContext, target, bufferObjectID);
}
void OpenGLBuffer::unbind(OpenGlContext& openGlContext) const
{
	OpenGL::Buffers::apiBindBuffer(openGlContext, target, 0);
}
void OpenGLBuffer::bindBase(OpenGlContext& openGlContext, const BindingPoint bindingLocation) const
{
	OpenGL::Buffers::apiBindBufferBase(openGlContext, target, bindingLocation, bufferObjectID);
}
void OpenGLBuffer::allocate(OpenGlContext& openGlContext, const BufferTarget targetValue, const ULL byteCount, const BufferUsage usageValue, const String& name, const BindingPoint bindingLocation)
{
	this->target = targetValue;
	this->usage  = usageValue;
	if (bufferObjectID != static_cast<VertexBufferID>(-1)) OpenGL::Buffers::apiDeleteBuffer(openGlContext, bufferObjectID);
	bufferObjectID = OpenGL::Buffers::apiCreateBuffer(openGlContext);
	OpenGL::apiObjectLabel(openGlContext, ObjectLabelType::Buffer, bufferObjectID, name);
	if (target == BufferTarget::TransformFeedback || target == BufferTarget::Uniform || target == BufferTarget::ShaderStorage || target == BufferTarget::AtomicCounter)
		OpenGL::Buffers::apiBindBufferBase(openGlContext, target, bindingLocation, bufferObjectID);
	OpenGL::Buffers::apiNamedBufferData(openGlContext, bufferObjectID, byteCount, nullptr, usage);
}
void OpenGLBuffer::destroy(OpenGlContext& openGlContext)
{
	OpenGL::Buffers::apiDeleteBuffer(openGlContext, bufferObjectID);
	bufferObjectID = -1;
}
void OpenGLBuffer::update(OpenGlContext& openGlContext, const Matrix& data)
{
	OpenGL::Buffers::apiNamedBufferData(openGlContext, bufferObjectID, sizeof(Matrix), glm::value_ptr(data), usage);
}
void OpenGLBuffer::update(OpenGlContext& openGlContext, const Vector<Int>& data)
{
	OpenGL::Buffers::apiNamedBufferData(openGlContext, bufferObjectID, data.size() * sizeof(Int), data.data(), usage);
}
void OpenGLBuffer::update(OpenGlContext& openGlContext, const Vec2Vector& data)
{
	update(openGlContext, static_cast<const Vector<Vec2>&>(data));
}
void OpenGLBuffer::update(OpenGlContext& openGlContext, const Vector<Vec3>& data)
{
	OpenGL::Buffers::apiNamedBufferData(openGlContext, bufferObjectID, data.size() * sizeof(Vec3), data.data(), usage);
}
void OpenGLBuffer::update(OpenGlContext& openGlContext, const Vec3Vector& data)
{
	update(openGlContext, static_cast<const Vector<Vec3>&>(data));
}
void OpenGLBuffer::update(OpenGlContext& openGlContext, const Vector<Vec2>& data)
{
	OpenGL::Buffers::apiNamedBufferData(openGlContext, bufferObjectID, data.size() * sizeof(Vec2), data.data(), usage);
}
void OpenGLBuffer::update(OpenGlContext& openGlContext, const Vector<UInt>& data)
{
	OpenGL::Buffers::apiNamedBufferData(openGlContext, bufferObjectID, data.size() * sizeof(UInt), data.data(), usage);
}
void OpenGLBuffer::update(OpenGlContext& openGlContext, const Vector<Matrix>& data)
{
	OpenGL::Buffers::apiNamedBufferData(openGlContext, bufferObjectID, data.size() * sizeof(Matrix), data.data(), usage);
}
void OpenGLBuffer::update(OpenGlContext& openGlContext, const GlobalUniformData& data)
{
	OpenGL::Buffers::apiNamedBufferData(openGlContext, bufferObjectID, sizeof(GlobalUniformData), &data, usage);
}

OpenGlContext::~OpenGlContext()
{
	if (!textures.empty()) logError("Not all textures have been unloaded!");
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

OpenGlContext::OpenGlContext() {
	maxVertexAttributes = OpenGL::apiGetIntegerv(*this, GetParameters::MaxVertexAttributes);
}

void OpenGlContext::enableLogging()
{
	loggingEnabled = true;
#ifdef TRACE_OPENGL
	log.openNewFile();
	logDebug("OpenGL trace log file: " + log.getPath().string());
#endif
}
void OpenGlContext::disableLogging()
{
	loggingEnabled = false;
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
void OpenGlContext::write(const String& message)
{
	#ifdef TRACE_OPENGL
	if (loggingEnabled) log.write(message);
	#endif
}
void OpenGlContext::addTexture(const TextureID textureID)
{
	if (openglStateTracking)
	{
		if (const auto it = std::ranges::find(textures, textureID); it != textures.end() && errorCheckingEnabled) logError("Texture with same ID has already been created! ID: " + toString(textureID));
		textures.push_back(textureID);
	}
}
void OpenGlContext::removeTexture(const TextureID textureID)
{
	if (openglStateTracking)
	{
		if (std::ranges::find(textures, textureID) != textures.end()) textures.remove(textureID);
		else if (errorCheckingEnabled) logError("Deleting texture ID that does not exist!");
	}
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
void OpenGlContext::removeVAO(const VertexArrayID vertexArrayID)
{
	if (openglStateTracking)
	{
		if (std::ranges::find(vaos, vertexArrayID) != vaos.end()) vaos.remove(vertexArrayID);
		else if (errorCheckingEnabled) logError("Deleting VAO ID that does not exist!");
	}
}

Bool OpenGlContext::getCulling() const
{
	return culling;
}

Bool OpenGlContext::getBlending() const
{
	return blending;
}

Bool OpenGlContext::getDepthTest() const
{
	return depthTest;
}

Bool OpenGlContext::getWireframeMode() const
{
	return wireframeMode;
}

void OpenGLShaderProgram::destroy(OpenGlContext& openGlContext)
{
	OpenGL::Shaders::apiDeleteProgram(openGlContext, programId);
	programId = 0;
}
void OpenGLShaderProgram::use(OpenGlContext& openGlContext) const
{
	OpenGL::Shaders::apiUseProgram(openGlContext, programId);
}
Bool OpenGLShaderProgram::create(OpenGlContext& openGlContext, const String& name, const String& vertexShaderSource, const String& fragmentShaderSource)
{
	const UInt vertexShaderId   = OpenGL::Shaders::apiCreateShader(openGlContext, ShaderType::Vertex);
	const UInt fragmentShaderId = OpenGL::Shaders::apiCreateShader(openGlContext, ShaderType::Fragment);

	const char* vertexShaderSourcePtr   = vertexShaderSource.c_str();
	const char* fragmentShaderSourcePtr = fragmentShaderSource.c_str();

	OpenGL::Shaders::apiShaderSource(openGlContext, vertexShaderId, 1, &vertexShaderSourcePtr, nullptr);
	OpenGL::Shaders::apiShaderSource(openGlContext, fragmentShaderId, 1, &fragmentShaderSourcePtr, nullptr);

	OpenGL::Shaders::apiCompileShader(openGlContext, vertexShaderId);
	OpenGL::Shaders::apiCompileShader(openGlContext, fragmentShaderId);

	const Int vertexCompileStatus   = OpenGL::Shaders::apiGetShaderIntegerValue(openGlContext, vertexShaderId, ShaderParameters::CompileStatus);
	const Int fragmentCompileStatus = OpenGL::Shaders::apiGetShaderIntegerValue(openGlContext, fragmentShaderId, ShaderParameters::CompileStatus);

	if (vertexCompileStatus != 1)
	{
		const String vertexInfoLog = OpenGL::Shaders::apiGetShaderInfoLog(openGlContext, vertexShaderId);
		logError("Linking shader program (" + name + ")failed. Error: " + vertexInfoLog);
	}

	if (fragmentCompileStatus != 1)
	{
		const String fragmentInfoLog = OpenGL::Shaders::apiGetShaderInfoLog(openGlContext, fragmentShaderId);
		logError("Linking shader program (" + name + ")failed. Error: " + fragmentInfoLog);
	}

	programId = OpenGL::Shaders::apiCreateProgram(openGlContext);

	OpenGL::Shaders::apiAttachShader(openGlContext, programId, vertexShaderId);
	OpenGL::Shaders::apiAttachShader(openGlContext, programId, fragmentShaderId);

	OpenGL::Shaders::apiLinkProgram(openGlContext, programId);

	const Int linkStatus = OpenGL::Shaders::apiGetProgramIntegerValue(openGlContext, programId, ShaderProgramParameters::LinkStatus);

	if (linkStatus != 1)
	{
		const String log = OpenGL::Shaders::apiGetProgramInfoLog(openGlContext, programId);
		logError("Linking shader program (" + name + ")failed. Error: " + log);
	}

	if (!linkStatus || !vertexCompileStatus || !fragmentCompileStatus)
	{
		OpenGL::Shaders::apiDeleteShader(openGlContext, vertexShaderId);
		OpenGL::Shaders::apiDeleteShader(openGlContext, fragmentShaderId);
		OpenGL::Shaders::apiDeleteProgram(openGlContext, programId);
		programId = -1;
		return false;
	}

	OpenGL::Shaders::apiUniformBlockBinding(openGlContext, programId, 0, 0);

	OpenGL::Shaders::apiDetachShader(openGlContext, programId, vertexShaderId);
	OpenGL::Shaders::apiDetachShader(openGlContext, programId, fragmentShaderId);

	OpenGL::Shaders::apiDeleteShader(openGlContext, vertexShaderId);
	OpenGL::Shaders::apiDeleteShader(openGlContext, fragmentShaderId);

	return true;
}

Bool OpenGLShaderProgram::createCompute(OpenGlContext& openGlContext, const String& name, const String& computeShaderSource)
{
	const UInt computeShaderId = OpenGL::Shaders::apiCreateShader(openGlContext, ShaderType::Compute);

	const char* computeShaderSourcePtr = computeShaderSource.c_str();
	OpenGL::Shaders::apiShaderSource(openGlContext, computeShaderId, 1, &computeShaderSourcePtr, nullptr);
	OpenGL::Shaders::apiCompileShader(openGlContext, computeShaderId);

	const Int computeCompileStatus = OpenGL::Shaders::apiGetShaderIntegerValue(openGlContext, computeShaderId, ShaderParameters::CompileStatus);
	if (computeCompileStatus != 1)
	{
		const String computeInfoLog = OpenGL::Shaders::apiGetShaderInfoLog(openGlContext, computeShaderId);
		logError("Compiling compute shader (" + name + ") failed. Error: " + computeInfoLog);
	}

	programId = OpenGL::Shaders::apiCreateProgram(openGlContext);
	OpenGL::Shaders::apiAttachShader(openGlContext, programId, computeShaderId);
	OpenGL::Shaders::apiLinkProgram(openGlContext, programId);

	const Int linkStatus = OpenGL::Shaders::apiGetProgramIntegerValue(openGlContext, programId, ShaderProgramParameters::LinkStatus);
	if (linkStatus != 1)
	{
		const String log = OpenGL::Shaders::apiGetProgramInfoLog(openGlContext, programId);
		logError("Linking compute shader program (" + name + ") failed. Error: " + log);
	}

	if (!linkStatus || !computeCompileStatus)
	{
		OpenGL::Shaders::apiDeleteShader(openGlContext, computeShaderId);
		OpenGL::Shaders::apiDeleteProgram(openGlContext, programId);
		programId = -1;
		return false;
	}

	OpenGL::Shaders::apiUniformBlockBinding(openGlContext, programId, 0, 0);
	OpenGL::Shaders::apiDetachShader(openGlContext, programId, computeShaderId);
	OpenGL::Shaders::apiDeleteShader(openGlContext, computeShaderId);
	return true;
}

UInt OpenGLFramebuffer::getID() const
{
	return framebufferId;
}
void OpenGLFramebuffer::destroy(OpenGlContext& openGlContext) const
{
	OpenGL::Framebuffer::apiDeleteFramebuffer(openGlContext, framebufferId);
	// The framebuffer object is gone; mark the id as invalid to avoid
	// accidental use-after-free when debugging is disabled.
	const_cast<UInt&>(framebufferId) = static_cast<UInt>(-1);
}
void OpenGLFramebuffer::bindDraw(OpenGlContext& openGlContext) const
{
	OpenGL::apiBindDrawFramebuffer(openGlContext, framebufferId);
}
void OpenGLFramebuffer::bindRead(OpenGlContext& openGlContext) const
{
	OpenGL::apiBindReadFramebuffer(openGlContext, framebufferId);
}
Bool OpenGLFramebuffer::isComplete(OpenGlContext& openGlContext) const
{
	return OpenGL::Framebuffer::apiCheckNamedFramebufferStatus(openGlContext, framebufferId, FramebufferTarget::Draw) == FramebufferStatus::Complete;
}
void OpenGLFramebuffer::create(OpenGlContext& openGlContext, const String& name)
{
	framebufferId = OpenGL::Framebuffer::apiCreateFramebuffer(openGlContext);
	OpenGL::apiObjectLabel(openGlContext, ObjectLabelType::Framebuffer, framebufferId, name);
}
void OpenGLFramebuffer::clearDepth(OpenGlContext& openGlContext, const Float clearDepth) const
{
	if (framebufferId == static_cast<UInt>(-1)) return;
	OpenGL::Framebuffer::apiClearNamedFramebufferfv(openGlContext, framebufferId, ClearBufferType::Depth, FramebufferAttachment::Depth, &clearDepth);
}
void OpenGLFramebuffer::drawBuffers(OpenGlContext& openGlContext, const Vector<UInt>& attachments) const
{
	OpenGL::Framebuffer::apiNamedFramebufferDrawBuffers(openGlContext, framebufferId, attachments);
}
Vec4 OpenGLFramebuffer::readPixelVec4(OpenGlContext& openGlContext, const Int x, const Int y, const FramebufferMode mode) const
{
	auto           data   = Vec4(0);
	constexpr Int  width  = 1;
	constexpr Int  height = 1;
	constexpr auto type   = DataType::Float;
	constexpr auto format = ReadPixelsFormat::RGBA;
	OpenGL::Framebuffer::apiNamedFramebufferReadBuffer(openGlContext, framebufferId, mode);
	OpenGL::Framebuffer::apiReadPixels(openGlContext, x, y, width, height, format, type, glm::value_ptr(data));
	return data;
}
IVec4 OpenGLFramebuffer::readPixelIVec4(OpenGlContext& openGlContext, const Int x, const Int y, const FramebufferMode mode) const
{
	auto           data   = IVec4(0);
	constexpr Int  width  = 1;
	constexpr Int  height = 1;
	constexpr auto type   = DataType::UInt;
	constexpr auto format = ReadPixelsFormat::RGBAInteger;
	OpenGL::Framebuffer::apiNamedFramebufferReadBuffer(openGlContext, framebufferId, mode);
	OpenGL::Framebuffer::apiReadPixels(openGlContext, x, y, width, height, format, type, glm::value_ptr(data));
	return data;
}
void OpenGLFramebuffer::clearColor(OpenGlContext& openGlContext, const FramebufferAttachment attachmentSlot, const Vec4 color) const
{
	if (framebufferId == static_cast<UInt>(-1)) return;
	OpenGL::Framebuffer::apiClearNamedFramebufferfv(openGlContext, framebufferId, ClearBufferType::Color, attachmentSlot, glm::value_ptr(color));
}
void OpenGLFramebuffer::clearColor(OpenGlContext& openGlContext, const FramebufferAttachment attachmentSlot, const IVec4 color) const
{
	if (framebufferId == static_cast<UInt>(-1)) return;
	OpenGL::Framebuffer::apiClearNamedFramebufferiv(openGlContext, framebufferId, ClearBufferType::Color, attachmentSlot, glm::value_ptr(color));
}
void OpenGLFramebuffer::attachTexture(OpenGlContext& openGlContext, const FramebufferAttachment attachment, const TextureID textureId) const
{
	constexpr UInt mipMapLevel = 0;
	OpenGL::Framebuffer::apiNamedFramebufferTexture(openGlContext, framebufferId, attachment, textureId, mipMapLevel);
}

void OpenGLVertexArrayObject::create(OpenGlContext& openGlContext)
{
	vertexArrayID = OpenGL::VertexArray::apiCreateVertexArray(openGlContext);
	openGlContext.addVAO(vertexArrayID);
}
void OpenGLVertexArrayObject::bind(OpenGlContext& openGlContext) const
{
	OpenGL::VertexArray::apiBindVertexArray(openGlContext, vertexArrayID);
}
void OpenGLVertexArrayObject::unbind(OpenGlContext& openGlContext) const
{
	OpenGL::VertexArray::apiBindVertexArray(openGlContext, 0);
}
void OpenGLVertexArrayObject::destroy(OpenGlContext& openGlContext) const
{
	OpenGL::VertexArray::apiDeleteVertexArray(openGlContext, vertexArrayID);
	openGlContext.removeVAO(vertexArrayID);
}
void OpenGLVertexArrayObject::unbindIndexBuffer(OpenGlContext& openGlContext) const
{
	OpenGL::VertexArray::apiVertexArrayElementBuffer(openGlContext, vertexArrayID, 0);
}
void OpenGLVertexArrayObject::bindIndexBuffer(OpenGlContext& openGlContext, const BufferID indexBufferID) const
{
	OpenGL::VertexArray::apiVertexArrayElementBuffer(openGlContext, vertexArrayID, indexBufferID);
}
void OpenGLVertexArrayObject::unbindVertexBuffer(OpenGlContext& openGlContext, const AttributeIndex attributeIndex) const
{
	OpenGL::VertexArray::apiVertexArrayVertexBuffer(openGlContext, vertexArrayID, attributeIndex, 0, 0, 0);
	OpenGL::VertexArray::apiDisableVertexArrayAttrib(openGlContext, vertexArrayID, attributeIndex);
}
void OpenGLVertexArrayObject::render(OpenGlContext& openGlContext, const PrimitiveMode primitiveMode, const Int indexCount, const void* indicesOrOffset) const
{
	OpenGL::VertexArray::apiDrawElements(openGlContext, primitiveMode, indexCount, indicesOrOffset);
}
void OpenGLVertexArrayObject::renderArrays(OpenGlContext& openGlContext, const PrimitiveMode primitiveMode, const Int first, const Int count) const
{
	OpenGL::VertexArray::apiDrawArrays(openGlContext, primitiveMode, first, count);
}
void OpenGLVertexArrayObject::renderInstanced(OpenGlContext& openGlContext, const PrimitiveMode primitiveMode, const Int indexCount, const void* indicesOrOffset, const Int instanceCount) const
{
	OpenGL::VertexArray::apiDrawElementsInstanced(openGlContext, primitiveMode, indexCount, indicesOrOffset, instanceCount);
}
void OpenGLVertexArrayObject::bindVertexBuffer(OpenGlContext& openGlContext, const AttributeIndex attributeIndex, const BufferID bufferID, const LL offset, const Int stride, const UInt divisor, const Int numComponents, const DataType type, const Bool normalized, const UInt relativeOffset) const
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

	OpenGL::VertexArray::apiEnableVertexArrayAttrib(openGlContext, vertexArrayID, attributeIndex);
	OpenGL::VertexArray::apiVertexArrayAttribBinding(openGlContext, vertexArrayID, attributeIndex, attributeIndex);
	OpenGL::VertexArray::apiVertexArrayVertexBuffer(openGlContext, vertexArrayID, attributeIndex, bufferID, offset, stride);
	OpenGL::VertexArray::apiVertexArrayBindingDivisor(openGlContext, vertexArrayID, attributeIndex, divisor);
	OpenGL::VertexArray::apiVertexArrayAttribFormat(openGlContext, vertexArrayID, attributeIndex, numComponents, type, normalized, relativeOffset);
}

void OpenGLIndexBuffer::create(OpenGlContext& openGlContext, const Vector<UInt>& indices, const BufferUsage usage, const String& name)
{
	OpenGLBuffer::create(openGlContext, BufferTarget::ElementArray, indices, usage, name, 0);
}

void OpenGL::apiInit(OpenGlContext& openGlContext)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glewInit();");
	#endif // TRACE_OPENGL

	External::glewExperimental = GL_TRUE;
	const auto glewInitResult = External::glewInit();
	if (glewInitResult != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW: " << External::glewGetErrorString(glewInitResult) << '\n';
		return;
	}
	// GLEW can leave a benign GL_INVALID_ENUM on some drivers.
	while (External::glGetError() != GL_NO_ERROR) {}

	if (External::glewIsSupported("GL_ARB_direct_state_access")) logDebug("DSA is supported!");
	else logError("DSA is not supported!");
}
void OpenGL::apiFinish(OpenGlContext& openGlContext)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glFinish();");
	#endif // TRACE_OPENGL
	External::glFinish();
}
UInt OpenGL::apiGetError(OpenGlContext& openGlContext)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetError();");
	#endif // TRACE_OPENGL
	return External::glGetError();
}
void OpenGL::apiClearColor(OpenGlContext& openGlContext)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glClear(...);");
	#endif // TRACE_OPENGL
	External::glClear(GL_COLOR_BUFFER_BIT);
}
void OpenGL::apiClearDepth(OpenGlContext& openGlContext)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glClear(...);");
	#endif // TRACE_OPENGL
	External::glClear(GL_DEPTH_BUFFER_BIT);
}
BufferID OpenGL::Buffers::apiCreateBuffer(OpenGlContext& openGlContext)
{
	BufferID bufferID;
	External::__glewCreateBuffers(1, &bufferID);

	#ifdef TRACE_OPENGL
	openGlContext.write("glCreateBuffers(1, " + toString(bufferID) + ");");
	#endif // TRACE_OPENGL

	return bufferID;
}
ProgramID OpenGL::Shaders::apiCreateProgram(OpenGlContext& openGlContext)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glCreateProgram(...);");
	#endif // TRACE_OPENGL
	return External::glCreateProgram();
}
void OpenGL::apiSetCulling(OpenGlContext& openGlContext, const Bool value)
{
	if (value && !openGlContext.culling) apiEnable(openGlContext, EnableParameters::CullFace);
	if (!value && openGlContext.culling) apiDisable(openGlContext, EnableParameters::CullFace);
	openGlContext.culling = value;
}
void OpenGL::apiSetBlending(OpenGlContext& openGlContext, const Bool value)
{
	if (value && !openGlContext.blending) apiEnable(openGlContext, EnableParameters::Blend);
	if (!value && openGlContext.blending) apiDisable(openGlContext, EnableParameters::Blend);
	openGlContext.blending = value;
}
Int OpenGL::Framebuffer::attachmentToIndex(FramebufferAttachment attachment)
{
	if (attachment == FramebufferAttachment::Depth) return 0;
	return static_cast<Int>(attachment) - GL_COLOR_ATTACHMENT0;
}
void OpenGL::apiSetDepthTest(OpenGlContext& openGlContext, const Bool value)
{
	if (value && !openGlContext.depthTest) apiEnable(openGlContext, EnableParameters::DepthTest);
	if (!value && openGlContext.depthTest) apiDisable(openGlContext, EnableParameters::DepthTest);
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
		apiEnable(openGlContext, EnableParameters::DebugOutput);
		apiEnable(openGlContext, EnableParameters::DebugOutputSyncronous);
	}
	else
	{
		External::glDebugMessageCallback(nullptr, nullptr);
		apiDisable(openGlContext, EnableParameters::DebugOutput);
		apiDisable(openGlContext, EnableParameters::DebugOutputSyncronous);
	}
	openGlContext.debugging = value;
}
void OpenGL::apiSetScissorTest(OpenGlContext& openGlContext, const Bool value)
{
	if (value && !openGlContext.scissorTest) apiEnable(openGlContext, EnableParameters::ScissorTest);
	if (!value && openGlContext.scissorTest) apiDisable(openGlContext, EnableParameters::ScissorTest);
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
void OpenGL::apiEnable(OpenGlContext& openGlContext, EnableParameters parameter)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glEnable(" + toString(static_cast<UInt>(parameter)) + ");");
	#endif // TRACE_OPENGL
	External::glEnable(static_cast<UInt>(parameter));
}
void OpenGL::apiScissor(OpenGlContext& openGlContext, Int x, Int y, Int w, Int h)
{
	if (x < 0 || y < 0 || w < 0 || h < 0) logWarning("Scissor parameters must be positive!");
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
void OpenGL::apiDisable(OpenGlContext& openGlContext, EnableParameters parameter)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDisable(" + toString(static_cast<UInt>(parameter)) + ");");
	#endif // TRACE_OPENGL

	External::glDisable(static_cast<UInt>(parameter));
}
Int OpenGL::apiGetIntegerv(OpenGlContext& openGlContext, GetParameters parameter)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetIntegerv(...);");
	#endif // TRACE_OPENGL
	Int result;
	External::glGetIntegerv(static_cast<UInt>(parameter), &result);
	return result;
}
VertexArrayID OpenGL::VertexArray::apiCreateVertexArray(OpenGlContext& openGlContext)
{
	VertexArrayID vertexArrayID;
	External::glCreateVertexArrays(1, &vertexArrayID);

	#ifdef TRACE_OPENGL
	openGlContext.write("glCreateVertexArrays(1," + toString(vertexArrayID) + ");");
	#endif // TRACE_OPENGL

	return vertexArrayID;
}
FramebufferID OpenGL::Framebuffer::apiCreateFramebuffer(OpenGlContext& openGlContext)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glCreateFramebuffers(...);");
	#endif // TRACE_OPENGL
	FramebufferID framebufferID;
	External::glCreateFramebuffers(1, &framebufferID);
	return framebufferID;
}
void OpenGL::Buffers::apiDeleteBuffer(OpenGlContext& openGlContext, const UInt buffer)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDeleteBuffers(...);");
	#endif // TRACE_OPENGL
	External::glDeleteBuffers(1, &buffer);
}
void OpenGL::apiBlendEquation(OpenGlContext& openGlContext, const BlendEquation equation)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glBlendEquation(" + toString(static_cast<UInt>(equation)) + ");");
	#endif // TRACE_OPENGL
	External::glBlendEquation(static_cast<UInt>(equation));
}
void OpenGL::apiPolygonMode(OpenGlContext& openGlContext, const UInt face, const UInt mode)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glPolygonMode(" + toString(face) + ", " + toString(mode) + ");");
	#endif // TRACE_OPENGL
	External::glPolygonMode(face, mode);
	openGlContext.wireframeMode = (mode == GL_LINE);
}
void OpenGL::Shaders::apiUseProgram(OpenGlContext& openGlContext, const ProgramID programId)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glUseProgram(...);");
	#endif // TRACE_OPENGL
	External::glUseProgram(programId);
}
void OpenGL::Shaders::apiDeleteShader(OpenGlContext& openGlContext, const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDeleteShader(...);");
	#endif // TRACE_OPENGL
	External::glDeleteShader(shaderId);
}
void OpenGL::Shaders::apiCompileShader(OpenGlContext& openGlContext, const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glCompileShader(...);");
	#endif // TRACE_OPENGL
	External::glCompileShader(shaderId);
}
void OpenGL::Shaders::apiLinkProgram(OpenGlContext& openGlContext, const ProgramID programId)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glLinkProgram(...);");
	#endif // TRACE_OPENGL
	External::glLinkProgram(programId);
}
ShaderID OpenGL::Shaders::apiCreateShader(OpenGlContext& openGlContext, const ShaderType type)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glCreateShader(...);");
	#endif // TRACE_OPENGL
	return External::glCreateShader(static_cast<UInt>(type));
}
void OpenGL::apiClipControl(OpenGlContext& openGlContext, const UInt origin, const UInt depth)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glClipControl(" + toString(origin) + ", " + toString(depth) + ");");
	#endif // TRACE_OPENGL
	External::glClipControl(origin, depth);
}
void OpenGL::Shaders::apiDeleteProgram(OpenGlContext& openGlContext, const ProgramID programId)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDeleteProgram(...);");
	#endif // TRACE_OPENGL
	External::glDeleteProgram(programId);
}
TextureID OpenGL::Textures::apiCreateTexture(OpenGlContext& openGlContext, TextureTarget target)
{
	TextureID textureID;
	#ifdef TRACE_OPENGL
	openGlContext.write("glCreateTextures(...);");
	#endif // TRACE_OPENGL
	External::glCreateTextures(static_cast<UInt>(target), 1, &textureID);
	return textureID;
}
void OpenGL::Textures::apiDeleteTexture(OpenGlContext& openGlContext, const TextureID textureID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDeleteTextures(...);");
	#endif // TRACE_OPENGL
	External::glDeleteTextures(1, &textureID);
}
String OpenGL::Shaders::apiGetShaderInfoLog(OpenGlContext& openGlContext, const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetShaderInfoLog(...);");
	#endif // TRACE_OPENGL
	const Int    logLength = apiGetShaderIntegerValue(openGlContext, shaderId, ShaderParameters::InfoLogLength);
	Vector<Char> log(logLength);
	External::glGetShaderInfoLog(shaderId, logLength, nullptr, log.data());
	return {log.begin(), log.end()};
}
void OpenGL::apiBindDrawFramebuffer(OpenGlContext& openGlContext, const FramebufferID framebufferID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glBindFramebuffer(" + toString(static_cast<UInt>(FramebufferTarget::Draw)) + "," + toString(framebufferID) + ");");
	#endif // TRACE_OPENGL
	External::glBindFramebuffer(static_cast<UInt>(FramebufferTarget::Draw), framebufferID);
}
void OpenGL::apiBindReadFramebuffer(OpenGlContext& openGlContext, const FramebufferID framebufferID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glBindFramebuffer(" + toString(static_cast<UInt>(FramebufferTarget::Read)) + "," + toString(framebufferID) + ");");
	#endif // TRACE_OPENGL
	External::glBindFramebuffer(static_cast<UInt>(FramebufferTarget::Read), framebufferID);
}
String OpenGL::Shaders::apiGetProgramInfoLog(OpenGlContext& openGlContext, const ProgramID programId)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetProgramInfoLog(...);");
	#endif // TRACE_OPENGL
	const Int    logLength = apiGetProgramIntegerValue(openGlContext, programId, ShaderProgramParameters::InfoLogLength);
	Vector<Char> log(logLength);
	External::glGetProgramInfoLog(programId, logLength, nullptr, log.data());
	return {log.begin(), log.end()};
}
void OpenGL::Textures::apiGenerateTextureMipmap(OpenGlContext& openGlContext, const TextureID textureID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGenerateTextureMipmap(...);");
	#endif // TRACE_OPENGL
	External::glGenerateTextureMipmap(textureID);
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
void OpenGL::VertexArray::apiBindVertexArray(OpenGlContext& openGlContext, const VertexArrayID vertexArrayID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glBindVertexArray(...);");
	#endif // TRACE_OPENGL
	External::glBindVertexArray(vertexArrayID);
}
void OpenGL::Framebuffer::apiDeleteFramebuffer(OpenGlContext& openGlContext, const FramebufferID framebufferID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDeleteFramebuffers(...);");
	#endif // TRACE_OPENGL
	External::glDeleteFramebuffers(1, &framebufferID);
}
void OpenGL::VertexArray::apiDeleteVertexArray(OpenGlContext& openGlContext, const VertexArrayID& vertexArrayID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDeleteVertexArrays(...);");
	#endif // TRACE_OPENGL
	External::glDeleteVertexArrays(1, &vertexArrayID);
}
void OpenGL::Buffers::apiBindBuffer(OpenGlContext& openGlContext, const BufferTarget target, const BufferID bufferID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glBindBuffer(...);");
	#endif // TRACE_OPENGL
	External::glBindBuffer(static_cast<UInt>(target), bufferID);
}
void OpenGL::Shaders::apiAttachShader(OpenGlContext& openGlContext, const ProgramID programId, const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glAttachShader(...);");
	#endif // TRACE_OPENGL
	External::glAttachShader(programId, shaderId);
}
void OpenGL::Shaders::apiDetachShader(OpenGlContext& openGlContext, const ProgramID programId, const ShaderID shaderId)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDetachShader(...);");
	#endif // TRACE_OPENGL
	External::glDetachShader(programId, shaderId);
}
void OpenGL::apiObjectLabel(OpenGlContext& openGlContext, ObjectLabelType type, const UInt objectID, const String& label)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glObjectLabel(" + toString(static_cast<UInt>(type)) + "," + toString(objectID) + ",\"" + label + "\");");
	#endif // TRACE_OPENGL
	External::glObjectLabel(static_cast<UInt>(type), objectID, static_cast<Int>(label.size()), label.c_str());
}
void OpenGL::apiBlendFunc(OpenGlContext& openGlContext, const BlendFunction sourceFactor, const BlendFunction destinationFactor)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glBlendFunc(" + toString(static_cast<UInt>(sourceFactor)) + ", " + toString(static_cast<UInt>(destinationFactor)) + ");");
	#endif // TRACE_OPENGL
	External::glBlendFunc(static_cast<UInt>(sourceFactor), static_cast<UInt>(destinationFactor));
}
void OpenGL::apiDispatchCompute(OpenGlContext& openGlContext, const UInt numGroupsX, const UInt numGroupsY, const UInt numGroupsZ)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDispatchCompute(" + toString(numGroupsX) + ", " + toString(numGroupsY) + ", " + toString(numGroupsZ) + ");");
	#endif // TRACE_OPENGL
	External::glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}
void OpenGL::apiMemoryBarrier(OpenGlContext& openGlContext, const UInt barriers)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glMemoryBarrier(" + toString(barriers) + ");");
	#endif // TRACE_OPENGL
	External::glMemoryBarrier(barriers);
}
void OpenGL::apiBindImageTexture(OpenGlContext& openGlContext, const UInt unit, const TextureID texture, const Int level, const Bool layered, const Int layer, const ImageAccess access, const SizedInternalFormat format)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glBindImageTexture(...);");
	#endif // TRACE_OPENGL
	External::glBindImageTexture(unit, texture, level, layered ? GL_TRUE : GL_FALSE, layer, static_cast<UInt>(access), static_cast<UInt>(format));
}
void OpenGL::Textures::apiBindTextureUnit(OpenGlContext& openGlContext, const TextureSlot textureUnitSlot, const TextureID textureID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glBindTextureUnit(...);");
	#endif // TRACE_OPENGL
	External::glBindTextureUnit(textureUnitSlot, textureID);
}
Int OpenGL::Shaders::apiGetShaderIntegerValue(OpenGlContext& openGlContext, const ShaderID shaderId, const ShaderParameters parameter)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetShaderiv(...);");
	#endif // TRACE_OPENGL
	Int result;
	External::glGetShaderiv(shaderId, static_cast<UInt>(parameter), &result);
	return result;
}
void OpenGL::VertexArray::apiDrawArrays(OpenGlContext& openGlContext, const PrimitiveMode mode, const Int first, const Int count)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glDrawArrays(" + toString(static_cast<UInt>(mode)) + ", " + toString(first) + ", " + toString(count) + ");");
	#endif // TRACE_OPENGL
	External::glDrawArrays(static_cast<UInt>(mode), first, count);
}
void OpenGL::VertexArray::apiDrawElements(OpenGlContext& openGlContext, const PrimitiveMode mode, const Int indexCount, const void* indices)
{
	// mode:		the primitives opengl can construct and draw e.g. triangle, triangle strip, line, etc.
	// indexCount:	the number of indices that will be used for rendering, equivalent to the total number of indices unless partial or batch rendering is used
	// indices:		would read from an actual array of indices on cpu side, however when a VAO is used, it represents an offset instead, usually 0
	#ifdef TRACE_OPENGL
	openGlContext.write("glDrawElements(" + toString(static_cast<UInt>(mode)) + ", " + toString(indexCount) + ", " + toString(GL_UNSIGNED_INT) + ", [...]);");
	#endif // TRACE_OPENGL
	External::glDrawElements(static_cast<UInt>(mode), indexCount, GL_UNSIGNED_INT, indices);
}
void OpenGL::Framebuffer::apiNamedFramebufferReadBuffer(OpenGlContext& openGlContext, const FramebufferID framebufferID, FramebufferMode mode)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glNamedFramebufferReadBuffer(...);");
	#endif // TRACE_OPENGL
	External::glNamedFramebufferReadBuffer(framebufferID, static_cast<UInt>(mode));
}
Int OpenGL::Shaders::apiGetProgramIntegerValue(OpenGlContext& openGlContext, const ProgramID programId, const ShaderProgramParameters parameter)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetProgramiv(...);");
	#endif // TRACE_OPENGL
	Int value;
	External::glGetProgramiv(programId, static_cast<UInt>(parameter), &value);
	return value;
}
void OpenGL::VertexArray::apiVertexArrayElementBuffer(OpenGlContext& openGlContext, const VertexArrayID vertexArrayID, const BufferID indexBufferID)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glVertexArrayElementBuffer(" + toString(vertexArrayID) + ", " + toString(indexBufferID) + ");");
	#endif // TRACE_OPENGL
	External::__glewVertexArrayElementBuffer(vertexArrayID, indexBufferID);
}
void OpenGL::Textures::apiTextureParameteri(OpenGlContext& openGlContext, const TextureID textureId, TextureParameterSet parameter, const Int value)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glTextureParameteri(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameteri(textureId, static_cast<UInt>(parameter), value);
}
void OpenGL::Framebuffer::apiNamedFramebufferDrawBuffers(OpenGlContext& openGlContext, const FramebufferID framebuffer, const Vector<UInt>& buffers)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glNamedFramebufferDrawBuffers(...);");
	#endif // TRACE_OPENGL
	External::glNamedFramebufferDrawBuffers(framebuffer, static_cast<Int>(buffers.size()), buffers.data());
}
void OpenGL::Textures::apiTextureParameterf(OpenGlContext& openGlContext, const TextureID textureId, TextureParameterSet parameter, const Float value)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glTextureParameterf(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameterf(textureId, static_cast<UInt>(parameter), value);
}
void OpenGL::Textures::apiTextureParameteriv(OpenGlContext& openGlContext, const TextureID textureId, TextureParameterSet parameter, const Int* value)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glTextureParameteriv(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameteriv(textureId, static_cast<UInt>(parameter), value);
}
void OpenGL::VertexArray::apiEnableVertexArrayAttrib(OpenGlContext& openGlContext, const VertexArrayID vertexArrayID, const AttributeIndex attributeID)
{
	#ifdef CHECK_OPENGL
	if (attributeID > openGlContext.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
	#endif // CHECK_OPENGL

	#ifdef TRACE_OPENGL
	openGlContext.write("glEnableVertexArrayAttrib(" + toString(vertexArrayID) + ", " + toString(attributeID) + ");");
	#endif // TRACE_OPENGL

	External::glEnableVertexArrayAttrib(vertexArrayID, attributeID);
}
void OpenGL::Textures::apiTextureParameterIiv(OpenGlContext& openGlContext, const TextureID textureId, TextureParameterSet parameter, const Int* value)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glTextureParameterIiv(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameterIiv(textureId, static_cast<UInt>(parameter), value);
}
void OpenGL::VertexArray::apiDisableVertexArrayAttrib(OpenGlContext& openGlContext, const VertexArrayID vertexArrayID, const AttributeIndex attributeID)
{
	#ifdef CHECK_OPENGL
	if (attributeID > openGlContext.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
	#endif // CHECK_OPENGL

	#ifdef TRACE_OPENGL
	openGlContext.write("glDisableVertexArrayAttrib(...);");
	#endif // TRACE_OPENGL
	External::glDisableVertexArrayAttrib(vertexArrayID, attributeID);
}
void OpenGL::Textures::apiGetTextureParameteriv(OpenGlContext& openGlContext, const TextureID textureID, TextureParameterGet parameterName, Int& output)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetTextureParameteriv(...);");
	#endif // TRACE_OPENGL
	External::glGetTextureParameteriv(textureID, static_cast<UInt>(parameterName), &output);
}
void OpenGL::Textures::apiTextureParameterfv(OpenGlContext& openGlContext, const TextureID textureId, TextureParameterSet parameter, const Float* value)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glTextureParameterfv(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameterfv(textureId, static_cast<UInt>(parameter), value);
}
void OpenGL::Buffers::apiNamedBufferData(OpenGlContext& openGlContext, const BufferID bufferID, const ULL byteCount, const void* data, BufferUsage usage)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glNamedBufferData(" + toString(bufferID) + ", " + toString(byteCount) + ", [...] , " + toString(static_cast<UInt>(usage)) + ");");
	#endif // TRACE_OPENGL
	// use for dynamic buffer data, like uniform buffer
	External::glNamedBufferData(bufferID, static_cast<LL>(byteCount), data, static_cast<UInt>(usage));
}
void OpenGL::Textures::apiGetTextureParameterIiv(OpenGlContext& openGlContext, const TextureID textureID, TextureParameterGet parameterName, Int* output)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetTextureParameterIiv(...);");
	#endif // TRACE_OPENGL
	External::glGetTextureParameterIiv(textureID, static_cast<UInt>(parameterName), output);
}
void OpenGL::Textures::apiTextureParameterIuiv(OpenGlContext& openGlContext, const TextureID textureId, TextureParameterSet parameter, const UInt* value)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glTextureParameterIuiv(...);");
	#endif // TRACE_OPENGL
	External::glTextureParameterIuiv(textureId, static_cast<UInt>(parameter), value);
}
void OpenGL::Textures::apiGetTextureParameterfv(OpenGlContext& openGlContext, const TextureID textureID, TextureParameterGet parameterName, Float& output)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetTextureParameterfv(...);");
	#endif // TRACE_OPENGL
	External::glGetTextureParameterfv(textureID, static_cast<UInt>(parameterName), &output);
}
void OpenGL::Textures::apiGetTextureParameterIuiv(OpenGlContext& openGlContext, const TextureID textureID, TextureParameterGet parameterName, UInt* output)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glGetTextureParameterIuiv(...);");
	#endif // TRACE_OPENGL
	External::glGetTextureParameterIuiv(textureID, static_cast<UInt>(parameterName), output);
}
void OpenGL::Buffers::apiBindBufferBase(OpenGlContext& openGlContext, const BufferTarget target, const BindingPoint bindingLocation, const BufferID buffer)
{
	// binds a buffer to a binding point for that particular target
	// e.g. you can bind a uniform buffer using its bufferID to BufferTarget::Uniform at binding point 0

	#ifdef TRACE_OPENGL
	openGlContext.write("glBindBufferBase(...);");
	#endif // TRACE_OPENGL
	External::glBindBufferBase(static_cast<UInt>(target), bindingLocation, buffer);
}
void OpenGL::Shaders::apiShaderSource(OpenGlContext& openGlContext, const ShaderID shaderId, const Int count, const Char* const* string, const Int* length)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glShaderSource(...);");
	#endif // TRACE_OPENGL
	External::glShaderSource(shaderId, count, string, length);
}
FramebufferStatus OpenGL::Framebuffer::apiCheckNamedFramebufferStatus(OpenGlContext& openGlContext, const FramebufferID framebufferID, FramebufferTarget target)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glCheckNamedFramebufferStatus(...);");
	#endif // TRACE_OPENGL
	return static_cast<FramebufferStatus>(External::glCheckNamedFramebufferStatus(framebufferID, static_cast<UInt>(target)));
}
void OpenGL::VertexArray::apiVertexArrayBindingDivisor(OpenGlContext& openGlContext, const VertexArrayID vertexArrayID, const Index layoutBinding, const UInt divisor)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glVertexArrayBindingDivisor(" + toString(vertexArrayID) + ", " + toString(layoutBinding) + ", " + toString(divisor) + ");");
	#endif // TRACE_OPENGL
	External::glVertexArrayBindingDivisor(vertexArrayID, layoutBinding, divisor);
}
void OpenGL::VertexArray::apiDrawElementsInstanced(OpenGlContext& openGlContext, const PrimitiveMode mode, const Int indexCount, const void* indices, const Int primitiveCount)
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
void OpenGL::Textures::apiClearTexImage(OpenGlContext& openGlContext, const TextureID texture, const Int mipmapLevel, WritePixelsFormat format, DataType type, const void* data)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glClearTexImage(...);");
	#endif // TRACE_OPENGL
	External::glClearTexImage(texture, mipmapLevel, static_cast<UInt>(format), static_cast<UInt>(type), data);
}
void OpenGL::Textures::apiTextureStorage2D(OpenGlContext& openGlContext, const TextureID texture, const Int mipmapLevel, SizedInternalFormat internalFormat, Int width, Int height)
{
	width  = max(1, width);
	height = max(1, height);

	#ifdef TRACE_OPENGL
	openGlContext.write("glTextureStorage2D(...);");
	#endif // TRACE_OPENGL
	External::glTextureStorage2D(texture, mipmapLevel, static_cast<UInt>(internalFormat), width, height);
}
void OpenGL::VertexArray::apiVertexArrayAttribBinding(OpenGlContext& openGlContext, const VertexArrayID vertexArrayID, const AttributeIndex attributeID, const Index layoutBinding)
{
	#ifdef CHECK_OPENGL
	if (attributeID > openGlContext.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
	#endif // CHECK_OPENGL

	#ifdef TRACE_OPENGL
	openGlContext.write("glVertexArrayAttribBinding(" + toString(vertexArrayID) + ", " + toString(attributeID) + ", " + toString(layoutBinding) + ");");
	#endif // TRACE_OPENGL
	External::glVertexArrayAttribBinding(vertexArrayID, attributeID, layoutBinding);
}
void OpenGL::Shaders::apiUniformBlockBinding(OpenGlContext& openGlContext, const ProgramID programId, const UniformBlockID uniformBlockIndex, const BindingPoint uniformBlockBinding)
{
	// binds the uniform block id of each shader to a predefined binding point that shaders attach to
	#ifdef TRACE_OPENGL
	openGlContext.write("glUniformBlockBinding(...);");
	#endif // TRACE_OPENGL
	External::glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding);
}
void OpenGL::Framebuffer::apiReadPixels(OpenGlContext& openGlContext, const Int x, const Int y, const Int width, const Int height, ReadPixelsFormat format, DataType type, void* data)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glReadPixels(...);");
	#endif // TRACE_OPENGL
	External::glReadPixels(x, y, width, height, static_cast<UInt>(format), static_cast<UInt>(type), data);
}
void OpenGL::Framebuffer::apiClearNamedFramebufferiv(OpenGlContext& openGlContext, const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const Int* value)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glClearNamedFramebufferiv(...);");
	#endif // TRACE_OPENGL
	External::glClearNamedFramebufferiv(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), value);
}
void OpenGL::Framebuffer::apiClearNamedFramebufferfv(OpenGlContext& openGlContext, const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const Float* value)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glClearNamedFramebufferfv(...);");
	#endif // TRACE_OPENGL
	External::glClearNamedFramebufferfv(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), value);
}
void OpenGL::Framebuffer::apiClearNamedFramebufferuiv(OpenGlContext& openGlContext, const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const UInt* value)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glClearNamedFramebufferuiv(...);");
	#endif // TRACE_OPENGL
	External::glClearNamedFramebufferuiv(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), value);
}
void OpenGL::Framebuffer::apiNamedFramebufferTexture(OpenGlContext& openGlContext, const FramebufferID framebufferID, const FramebufferAttachment attachmentID, const TextureID texture, const Int mipmapLevel)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glNamedFramebufferTexture(...);");
	#endif // TRACE_OPENGL
	External::glNamedFramebufferTexture(framebufferID, static_cast<UInt>(attachmentID), texture, mipmapLevel);
}
void OpenGL::VertexArray::apiVertexArrayVertexBuffer(OpenGlContext& openGlContext, const VertexArrayID vertexArrayID, const UInt layoutBinding, const VertexBufferID buffer, const LL offset, const Int stride)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glVertexArrayVertexBuffer(" + toString(vertexArrayID) + ", " + toString(layoutBinding) + ", " + toString(buffer) + ", " + toString(offset) + ", " + toString(stride) + ");");
	#endif // TRACE_OPENGL
	External::glVertexArrayVertexBuffer(vertexArrayID, layoutBinding, buffer, offset, stride);
}
void OpenGL::Framebuffer::apiClearNamedFramebufferfi(OpenGlContext& openGlContext, const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const Float depth, const Int stencil)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glClearNamedFramebufferfi(...);");
	#endif // TRACE_OPENGL
	External::glClearNamedFramebufferfi(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), depth, stencil);
}
void OpenGL::VertexArray::apiVertexArrayAttribFormat(OpenGlContext& openGlContext, const VertexArrayID vertexArrayID, const AttributeIndex attributeID, const Int size, DataType type, const Bool normalized, const UInt relativeOffset)
{
	#ifdef CHECK_OPENGL
	if (attributeID > openGlContext.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
	#endif // CHECK_OPENGL

	#ifdef TRACE_OPENGL
	openGlContext.write("glVertexArrayAttribFormat(" + toString(vertexArrayID) + ", " + toString(attributeID) + ", " + toString(size) + ", " + toString(static_cast<UInt>(type)) + ", " + toString(normalized) + ", " + toString(relativeOffset) + ");");
	#endif // TRACE_OPENGL
	External::glVertexArrayAttribFormat(vertexArrayID, attributeID, size, static_cast<UInt>(type), normalized, relativeOffset);
}
void OpenGL::Textures::apiTextureSubImage2D(OpenGlContext& openGlContext, const TextureID texture, const Int mipmapLevel, const Int xOffset, const Int yOffset, const Int width, const Int height, WritePixelsFormat format, DataType type, const void* pixels)
{
	#ifdef TRACE_OPENGL
	openGlContext.write("glTextureSubImage2D(...);");
	#endif // TRACE_OPENGL
	External::glPixelStorei(GL_PACK_ALIGNMENT, 1);
	External::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	External::glTextureSubImage2D(texture, mipmapLevel, xOffset, yOffset, width, height, static_cast<UInt>(format), static_cast<UInt>(type), pixels);
}
