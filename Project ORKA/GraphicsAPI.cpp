#include "GraphicsAPI.hpp"
#include "Math.hpp"

OpenGlStateCopy openGlState;

void OpenGL::apiInit()
{
#ifdef TRACE_OPENGL
		openGlState.write("glewInit();");
#endif // TRACE_OPENGL

	glewExperimental = GL_TRUE;
	if(const auto glewInitResult = glewInit(); glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewInitResult) << std::endl;
		return;
	}

	if(glewIsSupported("GL_ARB_direct_state_access")) logDebug("DSA is supported!");
	else logError("DSA is not supported!");

	openGlState.maxVertexAttributes = OpenGL::apiGetIntegerv(GetParameters::MaxVertexAttribs);
}

void OpenGL::apiFinish()
{
#ifdef TRACE_OPENGL
		openGlState.write("glFinish();");
#endif // TRACE_OPENGL
	glFinish();
}

UInt OpenGL::apiGetError()
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetError();");
#endif // TRACE_OPENGL
	return glGetError();
}

void OpenGL::apiClearColor()
{
#ifdef TRACE_OPENGL
		openGlState.write("glClear(...);");
#endif // TRACE_OPENGL
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGL::apiClearDepth()
{
#ifdef TRACE_OPENGL
		openGlState.write("glClear(...);");
#endif // TRACE_OPENGL
	glClear(GL_DEPTH_BUFFER_BIT);
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

OpenGlStateCopy::~OpenGlStateCopy()
{
	if(!textures.empty()) logError("Not all textures have been unloaded!");
}

UInt OpenGLTexture2D::getID() const
{
	return textureID;
}

void OpenGLShaderProgram::destroy()
{
	OpenGL::Shaders::apiDeleteProgram(programId);
	programId = 0;
}

void OpenGlStateCopy::print() const
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
	return framebufferID;
}

void OpenGlStateCopy::enableLogging()
{
	loggingEnabled = true;
}

void OpenGLShaderProgram::use() const
{
	OpenGL::Shaders::apiUseProgram(programId);
}

void OpenGLTexture2D::destroy() const
{
	OpenGL::Textures::apiDeleteTexture(textureID);
	openGlState.removeTexture(textureID);
}

void OpenGlStateCopy::disableLogging()
{
	loggingEnabled = false;
}

void OpenGLVertexArrayObject::create()
{
	vertexArrayID = OpenGL::VertexArray::apiCreateVertexArray();
	openGlState.addVAO(vertexArrayID);
}

void OpenGLFramebuffer::destroy() const
{
	OpenGL::Framebuffer::apiDeleteFramebuffer(framebufferID);
}

void OpenGLFramebuffer::bindDraw() const
{
	OpenGL::apiBindDrawFramebuffer(framebufferID);
}

void OpenGLFramebuffer::bindRead() const
{
	OpenGL::apiBindReadFramebuffer(framebufferID);
}

void OpenGLVertexArrayObject::bind() const
{
	OpenGL::VertexArray::apiBindVertexArray(vertexArrayID);
}

Bool OpenGLFramebuffer::isComplete() const
{
	return OpenGL::Framebuffer::apiCheckNamedFramebufferStatus(framebufferID, FramebufferTarget::Draw) == FramebufferStatus::Complete;
}

BufferID OpenGL::Buffers::apiCreateBuffer()
{
	BufferID bufferID;
	glCreateBuffers(1, &bufferID);

#ifdef TRACE_OPENGL
		openGlState.write("glCreateBuffers(1, " + toString(bufferID) + ");");
#endif // TRACE_OPENGL

	return bufferID;
}

void OpenGLTexture2D::generateMipMap() const
{
	OpenGL::Textures::apiGenerateTextureMipmap(textureID);
}

void OpenGLVertexArrayObject::unbind() const
{
	OpenGL::VertexArray::apiBindVertexArray(0);
}

void OpenGL::apiSetCulling(const Bool value)
{
	if(value && !openGlState.culling) apiEnable(EnableParameters::CullFace);
	if(!value && openGlState.culling) apiDisable(EnableParameters::CullFace);
	openGlState.culling = value;
}

ProgramID OpenGL::Shaders::apiCreateProgram()
{
#ifdef TRACE_OPENGL
		openGlState.write("glCreateProgram(...);");
#endif // TRACE_OPENGL
	return glCreateProgram();
}

void OpenGL::apiSetBlending(const Bool value)
{
	if(value && !openGlState.blending) apiEnable(EnableParameters::Blend);
	if(!value && openGlState.blending) apiDisable(EnableParameters::Blend);
	openGlState.blending = value;
}

void OpenGLVertexArrayObject::destroy() const
{
	OpenGL::VertexArray::apiDeleteVertexArray(vertexArrayID);
	openGlState.removeVAO(vertexArrayID);
}

void OpenGL::apiSetDepthTest(const Bool value)
{
	if(value && !openGlState.depthTest) apiEnable(EnableParameters::DepthTest);
	if(!value && openGlState.depthTest) apiDisable(EnableParameters::DepthTest);
	openGlState.depthTest = value;
}

void OpenGL::apiSetDebugging(const Bool value)
{
#ifdef TRACE_OPENGL
		openGlState.write("glDebugMessageCallback(nullptr,nullptr);");
#endif // TRACE_OPENGL
	if(value)
	{
		glDebugMessageCallback(debugOutputCallback, nullptr);
		apiEnable(EnableParameters::DebugOutput);
		apiEnable(EnableParameters::DebugOutputSyncronous);
	}
	else
	{
		glDebugMessageCallback(nullptr, nullptr);
		apiDisable(EnableParameters::DebugOutput);
		apiDisable(EnableParameters::DebugOutputSyncronous);
	}
	openGlState.debugging = value;
}

void OpenGLTexture2D::create(const String& name)
{
	textureID = OpenGL::Textures::apiCreateTexture(TextureTarget::Texture2D);
	OpenGL::apiObjectLabel(ObjectLabelType::Texture, textureID, name);
	openGlState.addTexture(textureID);
}

void OpenGL::apiSetScissorTest(const Bool value)
{
	if(value && !openGlState.scissorTest) apiEnable(EnableParameters::ScissorTest);
	if(!value && openGlState.scissorTest) apiDisable(EnableParameters::ScissorTest);
	openGlState.scissorTest = value;
}

void OpenGL::apiSetClearColor(const Color color)
{
#ifdef TRACE_OPENGL
		openGlState.write("glClearColor(...);");
#endif // TRACE_OPENGL
	if(openGlState.clearColor != color)
	{
		openGlState.clearColor = color;
		glClearColor(color.r, color.g, color.b, color.a);
	}
}

void OpenGLFramebuffer::create(const String& name)
{
	framebufferID = OpenGL::Framebuffer::apiCreateFramebuffer();
	OpenGL::apiObjectLabel(ObjectLabelType::Framebuffer, framebufferID, name);
}

void OpenGL::apiEnable(EnableParameters parameter)
{
#ifdef TRACE_OPENGL
		openGlState.write("glEnable(" + toString(static_cast<UInt>(parameter)) + ");");
#endif // TRACE_OPENGL
	glEnable(static_cast<UInt>(parameter));
}

void OpenGL::apiDisable(EnableParameters parameter)
{
#ifdef TRACE_OPENGL
		openGlState.write("glDisable(" + toString(static_cast<UInt>(parameter)) + ");");
#endif // TRACE_OPENGL

	glDisable(static_cast<UInt>(parameter));
}

Int OpenGL::apiGetIntegerv(GetParameters parameter)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetIntegerv(...);");
#endif // TRACE_OPENGL
	Int result;
	glGetIntegerv(static_cast<UInt>(parameter), &result);
	return result;
}

void OpenGLTexture2D::setBorderColor(Color color) const
{
	OpenGL::Textures::apiTextureParameterfv(textureID, TextureParameterSet::BorderColor, value_ptr(color));
}

void OpenGLVertexArrayObject::unbindIndexBuffer() const
{
	OpenGL::VertexArray::apiVertexArrayElementBuffer(vertexArrayID, 0);
}

void OpenGlStateCopy::write(const String& message) const
{
#ifdef TRACE_OPENGL
		if(loggingEnabled) log.write(message);
#endif
}

void OpenGL::Buffers::apiDeleteBuffer(const UInt buffer)
{
#ifdef TRACE_OPENGL
		openGlState.write("glDeleteBuffers(...);");
#endif // TRACE_OPENGL
	glDeleteBuffers(1, &buffer);
}

VertexArrayID OpenGL::VertexArray::apiCreateVertexArray()
{
	VertexArrayID vertexArrayID;
	glCreateVertexArrays(1, &vertexArrayID);

#ifdef TRACE_OPENGL
		openGlState.write("glCreateVertexArrays(1," + toString(vertexArrayID) + ");");
#endif // TRACE_OPENGL

	return vertexArrayID;
}

FramebufferID OpenGL::Framebuffer::apiCreateFramebuffer()
{
#ifdef TRACE_OPENGL
		openGlState.write("glCreateFramebuffers(...);");
#endif // TRACE_OPENGL
	FramebufferID framebufferID;
	glCreateFramebuffers(1, &framebufferID);
	return framebufferID;
}

void OpenGLTexture2D::setWrapping(Wrapping wrapping) const
{
	OpenGL::Textures::apiTextureParameteri(textureID, TextureParameterSet::WrapS, static_cast<Int>(wrapping));
	OpenGL::Textures::apiTextureParameteri(textureID, TextureParameterSet::WrapT, static_cast<Int>(wrapping));
}

void OpenGlStateCopy::addTexture(const TextureID textureID)
{
	if(openglStateTracking)
	{
		const auto it = std::ranges::find(textures, textureID);
		if(it != textures.end() && errorCheckingEnabled) logError("Texture with same ID has already been created!");
		textures.push_back(textureID);
	}
}

void OpenGL::apiBlendEquation(const BlendEquation equation)
{
#ifdef TRACE_OPENGL
		openGlState.write("glBlendEquation(" + toString(static_cast<UInt>(equation)) + ");");
#endif // TRACE_OPENGL
	glBlendEquation(static_cast<UInt>(equation));
}

void OpenGL::apiPolygonMode(const UInt face, const UInt mode)
{
#ifdef TRACE_OPENGL
		openGlState.write("glPolygonMode(" + toString(face) + ", " + toString(mode) + ");");
#endif // TRACE_OPENGL
	glPolygonMode(face, mode);
}

void OpenGL::Shaders::apiUseProgram(const ProgramID programId)
{
#ifdef TRACE_OPENGL
		openGlState.write("glUseProgram(...);");
#endif // TRACE_OPENGL
	glUseProgram(programId);
}

void OpenGlStateCopy::removeTexture(const TextureID textureID)
{
	if(openglStateTracking)
	{
		if(std::ranges::find(textures, textureID) != textures.end()) textures.remove(textureID);
		else if(errorCheckingEnabled) logError("Deleting texture ID that does not exist!");
	}
}

void OpenGL::Shaders::apiDeleteShader(const ShaderID shaderId)
{
#ifdef TRACE_OPENGL
		openGlState.write("glDeleteShader(...);");
#endif // TRACE_OPENGL
	glDeleteShader(shaderId);
}

void OpenGlStateCopy::addVAO(const VertexArrayID vertexArrayID)
{
	if(openglStateTracking)
	{
		const auto it = std::ranges::find(vaos, vertexArrayID);
		if(it != vaos.end() && errorCheckingEnabled) logError("VAO with same ID has already been created!");
		vaos.push_back(vertexArrayID);
	}
}

void OpenGL::Shaders::apiCompileShader(const ShaderID shaderId)
{
#ifdef TRACE_OPENGL
		openGlState.write("glCompileShader(...);");
#endif // TRACE_OPENGL
	glCompileShader(shaderId);
}

void OpenGL::Shaders::apiLinkProgram(const ProgramID programId)
{
#ifdef TRACE_OPENGL
		openGlState.write("glLinkProgram(...);");
#endif // TRACE_OPENGL
	glLinkProgram(programId);
}

ShaderID OpenGL::Shaders::apiCreateShader(const ShaderType type)
{
#ifdef TRACE_OPENGL
		openGlState.write("glCreateShader(...);");
#endif // TRACE_OPENGL
	return glCreateShader(static_cast<UInt>(type));
}

void OpenGLFramebuffer::clearDepth(const Float clearDepth) const
{
	OpenGL::Framebuffer::apiClearNamedFramebufferfv(framebufferID, ClearBufferType::Depth, FramebufferAttachment::Depth, &clearDepth);
}

void OpenGL::apiClipControl(const UInt origin, const UInt depth)
{
#ifdef TRACE_OPENGL
		openGlState.write("glClipControl(" + toString(origin) + ", " + toString(depth) + ");");
#endif // TRACE_OPENGL
	glClipControl(origin, depth);
}

void OpenGL::Shaders::apiDeleteProgram(const ProgramID programId)
{
#ifdef TRACE_OPENGL
		openGlState.write("glDeleteProgram(...);");
#endif // TRACE_OPENGL
	glDeleteProgram(programId);
}

void OpenGlStateCopy::removeVAO(const VertexArrayID vertexArrayID)
{
	if(openglStateTracking)
	{
		if(std::ranges::find(vaos, vertexArrayID) != vaos.end()) vaos.remove(vertexArrayID);
		else if(errorCheckingEnabled) logError("Deleting VAO ID that does not exist!");
	}
}

TextureID OpenGL::Textures::apiCreateTexture(TextureTarget target)
{
	TextureID textureID;
#ifdef TRACE_OPENGL
		openGlState.write("glCreateTextures(...);");
#endif // TRACE_OPENGL
	glCreateTextures(static_cast<UInt>(target), 1, &textureID);
#ifdef DEBUG_OPENGL
		openGlState.addTexture(textureID);
#endif // DEBUG_OPENGL
	return textureID;
}

void OpenGL::Textures::apiDeleteTexture(const TextureID textureID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glDeleteTextures(...);");
#endif // TRACE_OPENGL
	glDeleteTextures(1, &textureID);
#ifdef DEBUG_OPENGL
		openGlState.removeTexture(textureID);
#endif // DEBUG_OPENGL
}

String OpenGL::Shaders::apiGetShaderInfoLog(const ShaderID shaderId)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetShaderInfoLog(...);");
#endif // TRACE_OPENGL
	const Int    logLength = apiGetShaderIntegerValue(shaderId, ShaderParameters::InfoLogLength);
	Vector<Char> log(logLength);
	glGetShaderInfoLog(shaderId, logLength, nullptr, log.data());
	return {log.begin(), log.end()};
}

void OpenGL::apiBindDrawFramebuffer(const FramebufferID framebufferID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glBindFramebuffer(" + toString(static_cast<UInt>(FramebufferTarget::Draw)) + "," + toString(framebufferID) + ");");
#endif // TRACE_OPENGL
	glBindFramebuffer(static_cast<UInt>(FramebufferTarget::Draw), framebufferID);
}

void OpenGL::apiBindReadFramebuffer(const FramebufferID framebufferID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glBindFramebuffer(" + toString(static_cast<UInt>(FramebufferTarget::Read)) + "," + toString(framebufferID) + ");");
#endif // TRACE_OPENGL
	glBindFramebuffer(static_cast<UInt>(FramebufferTarget::Read), framebufferID);
}

String OpenGL::Shaders::apiGetProgramInfoLog(const ProgramID programId)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetProgramInfoLog(...);");
#endif // TRACE_OPENGL
	const Int    logLength = apiGetProgramIntegerValue(programId, ShaderProgramParameters::InfoLogLength);
	Vector<Char> log(logLength);
	glGetProgramInfoLog(programId, logLength, nullptr, log.data());
	return {log.begin(), log.end()};
}

void OpenGLTexture2D::useTextureInSlot(const UInt textureUnitSlot) const
{
	OpenGL::Textures::apiBindTextureUnit(textureUnitSlot, textureID);
}

void OpenGLFramebuffer::drawBuffers(const Vector<UInt>& attachments) const
{
	OpenGL::Framebuffer::apiNamedFramebufferDrawBuffers(framebufferID, attachments);
}

void OpenGL::Textures::apiGenerateTextureMipmap(const TextureID textureID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGenerateTextureMipmap(...);");
#endif // TRACE_OPENGL
	glGenerateTextureMipmap(textureID);
}

void OpenGLTexture2D::setFilters(Filter nearFilter, Filter farFilter) const
{
	OpenGL::Textures::apiTextureParameteri(textureID, TextureParameterSet::MagFilter, static_cast<Int>(nearFilter));
	OpenGL::Textures::apiTextureParameteri(textureID, TextureParameterSet::MinFilter, static_cast<Int>(farFilter));
}

void OpenGL::apiScissor(const Int x, const Int y, const Int w, const Int h)
{
	if(openGlState.scissorX != x || openGlState.scissorY != y || openGlState.scissorW != w || openGlState.scissorH != h)
	{
#ifdef TRACE_OPENGL
			openGlState.write("glScissor(" + toString(x) + ", " + toString(y) + ", " + toString(w) + ", " + toString(h) + ");");
#endif // TRACE_OPENGL
		glScissor(x, y, w, h);
		openGlState.scissorX = x;
		openGlState.scissorY = y;
		openGlState.scissorW = w;
		openGlState.scissorH = h;
	}
}

Int OpenGL::Framebuffer::attachmentToIndex(FramebufferAttachment attachment)
{
	if(attachment == FramebufferAttachment::Depth) return 0;
	return static_cast<Int>(attachment) - GL_COLOR_ATTACHMENT0;
}

void OpenGL::apiViewport(const Int x, const Int y, const Int w, const Int h)
{
	if(openGlState.viewportX != x || openGlState.viewportY != y || openGlState.viewportW != w || openGlState.viewportH != h)
	{
#ifdef TRACE_OPENGL
			openGlState.write("glViewport(" + toString(x) + ", " + toString(y) + ", " + toString(w) + ", " + toString(h) + ");");
#endif // TRACE_OPENGL
		glViewport(x, y, w, h);
		openGlState.viewportX = x;
		openGlState.viewportY = y;
		openGlState.viewportW = w;
		openGlState.viewportH = h;
	}
}

void OpenGL::VertexArray::apiBindVertexArray(const VertexArrayID vertexArrayID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glBindVertexArray(...);");
#endif // TRACE_OPENGL
	glBindVertexArray(vertexArrayID);
}

void OpenGLVertexArrayObject::bindIndexBuffer(const BufferID indexBufferID) const
{
	OpenGL::VertexArray::apiVertexArrayElementBuffer(vertexArrayID, indexBufferID);
}

void OpenGL::Framebuffer::apiDeleteFramebuffer(const FramebufferID framebufferID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glDeleteFramebuffers(...);");
#endif // TRACE_OPENGL
	glDeleteFramebuffers(1, &framebufferID);
}

void OpenGL::VertexArray::apiDeleteVertexArray(const VertexArrayID& vertexArrayID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glDeleteVertexArrays(...);");
#endif // TRACE_OPENGL
	glDeleteVertexArrays(1, &vertexArrayID);
}

void OpenGL::Buffers::apiBindBuffer(const BufferTarget target, const BufferID bufferID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glBindBuffer(...);");
#endif // TRACE_OPENGL
	glBindBuffer(static_cast<UInt>(target), bufferID);
}

void OpenGL::Shaders::apiAttachShader(const ProgramID programId, const ShaderID shaderId)
{
#ifdef TRACE_OPENGL
		openGlState.write("glAttachShader(...);");
#endif // TRACE_OPENGL
	glAttachShader(programId, shaderId);
}

void OpenGL::Shaders::apiDetachShader(const ProgramID programId, const ShaderID shaderId)
{
#ifdef TRACE_OPENGL
		openGlState.write("glDetachShader(...);");
#endif // TRACE_OPENGL
	glDetachShader(programId, shaderId);
}

void OpenGLVertexArrayObject::unbindVertexBuffer(const AttributeIndex attributeIndex) const
{
	OpenGL::VertexArray::apiVertexArrayVertexBuffer(vertexArrayID, attributeIndex, 0, 0, 0);
	OpenGL::VertexArray::apiDisableVertexArrayAttrib(vertexArrayID, attributeIndex);
}

void OpenGL::apiObjectLabel(ObjectLabelType type, const UInt objectID, const String& label)
{
#ifdef TRACE_OPENGL
		openGlState.write("glObjectLabel(" + toString(static_cast<UInt>(type)) + "," + toString(objectID) + ",\"" + label + "\");");
#endif // TRACE_OPENGL
	glObjectLabel(static_cast<UInt>(type), objectID, static_cast<Int>(label.size()), label.c_str());
}

void OpenGLTexture2D::setDataToDepth(const Int width, const Int height, const void* data) const
{
	constexpr Float clearData = 1.0f;
	OpenGL::Textures::apiTextureStorage2D(textureID, 1, SizedInternalFormat::Depth32F, width, height);
	if(data) OpenGL::Textures::apiClearTexImage(textureID, 0, WritePixelsFormat::Depth, DataType::Float, data);
	else OpenGL::Textures::apiClearTexImage(textureID, 0, WritePixelsFormat::Depth, DataType::Float, &clearData);
}

Vec4 OpenGLFramebuffer::readPixelVec4(const Int x, const Int y, const FramebufferMode mode) const
{
	auto           data   = Vec4(0);
	constexpr Int  width  = 1;
	constexpr Int  height = 1;
	constexpr auto type   = DataType::Float;
	constexpr auto format = ReadPixelsFormat::RGBA;
	OpenGL::Framebuffer::apiNamedFramebufferReadBuffer(framebufferID, mode);
	OpenGL::Framebuffer::apiReadPixels(x, y, width, height, format, type, glm::value_ptr(data));
	return data;
}

void OpenGL::apiBlendFunc(const BlendFunction sourceFactor, const BlendFunction destinationFactor)
{
#ifdef TRACE_OPENGL
		openGlState.write("glBlendFunc(" + toString(static_cast<UInt>(sourceFactor)) + ", " + toString(static_cast<UInt>(destinationFactor)) + ");");
#endif // TRACE_OPENGL
	glBlendFunc(static_cast<UInt>(sourceFactor), static_cast<UInt>(destinationFactor));
}

IVec4 OpenGLFramebuffer::readPixelIVec4(const Int x, const Int y, const FramebufferMode mode) const
{
	auto           data   = IVec4(0);
	constexpr Int  width  = 1;
	constexpr Int  height = 1;
	constexpr auto type   = DataType::UInt;
	constexpr auto format = ReadPixelsFormat::RGBAInteger;
	OpenGL::Framebuffer::apiNamedFramebufferReadBuffer(framebufferID, mode);
	OpenGL::Framebuffer::apiReadPixels(x, y, width, height, format, type, glm::value_ptr(data));
	return data;
}

void OpenGLFramebuffer::clearColor(const FramebufferAttachment attachmentSlot, const Vec4 color) const
{
	OpenGL::Framebuffer::apiClearNamedFramebufferfv(framebufferID, ClearBufferType::Color, attachmentSlot, glm::value_ptr(color));
}

void OpenGLFramebuffer::clearColor(const FramebufferAttachment attachmentSlot, const IVec4 color) const
{
	OpenGL::Framebuffer::apiClearNamedFramebufferiv(framebufferID, ClearBufferType::Color, attachmentSlot, glm::value_ptr(color));
}

void OpenGL::Textures::apiBindTextureUnit(const TextureSlot textureUnitSlot, const TextureID textureID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glBindTextureUnit(...);");
#endif // TRACE_OPENGL
	glBindTextureUnit(textureUnitSlot, textureID);
}

Int OpenGL::Shaders::apiGetShaderIntegerValue(const ShaderID shaderId, const ShaderParameters parameter)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetShaderiv(...);");
#endif // TRACE_OPENGL
	Int result;
	glGetShaderiv(shaderId, static_cast<UInt>(parameter), &result);
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
		openGlState.write("glDrawElements(" + toString(static_cast<UInt>(mode)) + ", " + toString(indexCount) + ", " + toString(GL_UNSIGNED_INT) + ", [...]);");
#endif // TRACE_OPENGL
	glDrawElements(static_cast<UInt>(mode), indexCount, GL_UNSIGNED_INT, indices);
}

void OpenGLFramebuffer::attachTexture(const FramebufferAttachment attachment, const TextureID textureId) const
{
	constexpr UInt mipMapLevel = 0;
	OpenGL::Framebuffer::apiNamedFramebufferTexture(framebufferID, attachment, textureId, mipMapLevel);
}

void OpenGL::Framebuffer::apiNamedFramebufferReadBuffer(const FramebufferID framebufferID, FramebufferMode mode)
{
#ifdef TRACE_OPENGL
		openGlState.write("glNamedFramebufferReadBuffer(...);");
#endif // TRACE_OPENGL
	glNamedFramebufferReadBuffer(framebufferID, static_cast<UInt>(mode));
}

Int OpenGL::Shaders::apiGetProgramIntegerValue(const ProgramID programId, const ShaderProgramParameters parameter)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetProgramiv(...);");
#endif // TRACE_OPENGL
	Int value;
	glGetProgramiv(programId, static_cast<UInt>(parameter), &value);
	return value;
}

void OpenGL::VertexArray::apiVertexArrayElementBuffer(const VertexArrayID vertexArrayID, const BufferID indexBufferID)
{
#ifdef TRACE_OPENGL
		openGlState.write("glVertexArrayElementBuffer(" + toString(vertexArrayID) + ", " + toString(indexBufferID) + ");");
#endif // TRACE_OPENGL
	glVertexArrayElementBuffer(vertexArrayID, indexBufferID);
}

void OpenGL::Textures::apiTextureParameteri(const TextureID textureId, TextureParameterSet parameter, const Int value)
{
#ifdef TRACE_OPENGL
		openGlState.write("glTextureParameteri(...);");
#endif // TRACE_OPENGL
	glTextureParameteri(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::Framebuffer::apiNamedFramebufferDrawBuffers(const FramebufferID framebuffer, const Vector<UInt>& buffers)
{
#ifdef TRACE_OPENGL
		openGlState.write("glNamedFramebufferDrawBuffers(...);");
#endif // TRACE_OPENGL
	glNamedFramebufferDrawBuffers(framebuffer, static_cast<Int>(buffers.size()), buffers.data());
}

void OpenGL::Textures::apiTextureParameterf(const TextureID textureId, TextureParameterSet parameter, const Float value)
{
#ifdef TRACE_OPENGL
		openGlState.write("glTextureParameterf(...);");
#endif // TRACE_OPENGL
	glTextureParameterf(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::Textures::apiTextureParameteriv(const TextureID textureId, TextureParameterSet parameter, const Int* value)
{
#ifdef TRACE_OPENGL
		openGlState.write("glTextureParameteriv(...);");
#endif // TRACE_OPENGL
	glTextureParameteriv(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::VertexArray::apiEnableVertexArrayAttrib(const VertexArrayID vertexArrayID, const AttributeIndex attributeID)
{
#ifdef CHECK_OPENGL
		if(attributeID > openGlState.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
#endif // CHECK_OPENGL

#ifdef TRACE_OPENGL
		openGlState.write("glEnableVertexArrayAttrib(" + toString(vertexArrayID) + ", " + toString(attributeID) + ");");
#endif // TRACE_OPENGL

	glEnableVertexArrayAttrib(vertexArrayID, attributeID);
}

void OpenGL::Textures::apiTextureParameterIiv(const TextureID textureId, TextureParameterSet parameter, const Int* value)
{
#ifdef TRACE_OPENGL
		openGlState.write("glTextureParameterIiv(...);");
#endif // TRACE_OPENGL
	glTextureParameterIiv(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::VertexArray::apiDisableVertexArrayAttrib(const VertexArrayID vertexArrayID, const AttributeIndex attributeID)
{
#ifdef CHECK_OPENGL
		if(attributeID > openGlState.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
#endif // CHECK_OPENGL

#ifdef TRACE_OPENGL
		openGlState.write("glDisableVertexArrayAttrib(...);");
#endif // TRACE_OPENGL
	glDisableVertexArrayAttrib(vertexArrayID, attributeID);
}

void OpenGL::Textures::apiGetTextureParameteriv(const TextureID textureID, TextureParameterGet parameterName, Int& output)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetTextureParameteriv(...);");
#endif // TRACE_OPENGL
	glGetTextureParameteriv(textureID, static_cast<UInt>(parameterName), &output);
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

	if(vertexCompileStatus != 1)
	{
		const String vertexInfoLog = OpenGL::Shaders::apiGetShaderInfoLog(vertexShaderId);
		logError("Linking shader program (" + name + ")failed. Error: " + vertexInfoLog);
	}

	if(fragmentCompileStatus != 1)
	{
		const String fragmentInfoLog = OpenGL::Shaders::apiGetShaderInfoLog(fragmentShaderId);
		logError("Linking shader program (" + name + ")failed. Error: " + fragmentInfoLog);
	}

	programId = OpenGL::Shaders::apiCreateProgram();

	OpenGL::Shaders::apiAttachShader(programId, vertexShaderId);
	OpenGL::Shaders::apiAttachShader(programId, fragmentShaderId);

	OpenGL::Shaders::apiLinkProgram(programId);

	const Int linkStatus = OpenGL::Shaders::apiGetProgramIntegerValue(programId, ShaderProgramParameters::LinkStatus);

	if(linkStatus != 1)
	{
		const String log = OpenGL::Shaders::apiGetProgramInfoLog(programId);
		logError("Linking shader program (" + name + ")failed. Error: " + log);
	}

	if(!linkStatus || !vertexCompileStatus || !fragmentCompileStatus)
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
		openGlState.write("glTextureParameterfv(...);");
#endif // TRACE_OPENGL
	glTextureParameterfv(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::Buffers::apiNamedBufferData(const BufferID bufferID, const ULL byteCount, const void* data, BufferUsage usage)
{
#ifdef TRACE_OPENGL
		openGlState.write("glNamedBufferData(" + toString(bufferID) + ", " + toString(byteCount) + ", [...] , " + toString(static_cast<UInt>(usage)) + ");");
#endif // TRACE_OPENGL
	// use for dynamic buffer data, like uniform buffer
	glNamedBufferData(bufferID, static_cast<LL>(byteCount), data, static_cast<UInt>(usage));
}

void OpenGL::Textures::apiGetTextureParameterIiv(const TextureID textureID, TextureParameterGet parameterName, Int* output)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetTextureParameterIiv(...);");
#endif // TRACE_OPENGL
	glGetTextureParameterIiv(textureID, static_cast<UInt>(parameterName), output);
}

void OpenGL::Textures::apiTextureParameterIuiv(const TextureID textureId, TextureParameterSet parameter, const UInt* value)
{
#ifdef TRACE_OPENGL
		openGlState.write("glTextureParameterIuiv(...);");
#endif // TRACE_OPENGL
	glTextureParameterIuiv(textureId, static_cast<UInt>(parameter), value);
}

void OpenGL::Textures::apiGetTextureParameterfv(const TextureID textureID, TextureParameterGet parameterName, Float& output)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetTextureParameterfv(...);");
#endif // TRACE_OPENGL
	glGetTextureParameterfv(textureID, static_cast<UInt>(parameterName), &output);
}

void OpenGL::Textures::apiGetTextureParameterIuiv(const TextureID textureID, TextureParameterGet parameterName, UInt* output)
{
#ifdef TRACE_OPENGL
		openGlState.write("glGetTextureParameterIuiv(...);");
#endif // TRACE_OPENGL
	glGetTextureParameterIuiv(textureID, static_cast<UInt>(parameterName), output);
}

void OpenGL::Buffers::apiBindBufferBase(const BufferTarget target, const BindingPoint bindingLocation, const BufferID buffer)
{
	// binds a buffer to a binding point for that particular target
	// e.g. you can bind a uniform buffer using its bufferID to BufferTarget::Uniform at binding point 0

#ifdef TRACE_OPENGL
		openGlState.write("glBindBufferBase(...);");
#endif // TRACE_OPENGL
	glBindBufferBase(static_cast<UInt>(target), bindingLocation, buffer);
}

void OpenGL::Shaders::apiShaderSource(const ShaderID shaderId, const Int count, const Char* const* string, const Int* length)
{
#ifdef TRACE_OPENGL
		openGlState.write("glShaderSource(...);");
#endif // TRACE_OPENGL
	glShaderSource(shaderId, count, string, length);
}

void OpenGLVertexArrayObject::render(const PrimitiveMode primitiveMode, const Int indexCount, const void* indicesOrOffset) const
{
	OpenGL::VertexArray::apiDrawElements(primitiveMode, indexCount, indicesOrOffset);
}

FramebufferStatus OpenGL::Framebuffer::apiCheckNamedFramebufferStatus(const FramebufferID framebufferID, FramebufferTarget target)
{
#ifdef TRACE_OPENGL
		openGlState.write("glCheckNamedFramebufferStatus(...);");
#endif // TRACE_OPENGL
	return static_cast<FramebufferStatus>(glCheckNamedFramebufferStatus(framebufferID, static_cast<UInt>(target)));
}

void OpenGL::VertexArray::apiVertexArrayBindingDivisor(const VertexArrayID vertexArrayID, const Index layoutBinding, const UInt divisor)
{
#ifdef TRACE_OPENGL
		openGlState.write("glVertexArrayBindingDivisor(" + toString(vertexArrayID) + ", " + toString(layoutBinding) + ", " + toString(divisor) + ");");
#endif // TRACE_OPENGL
	glVertexArrayBindingDivisor(vertexArrayID, layoutBinding, divisor);
}

void OpenGL::VertexArray::apiDrawElementsInstanced(const PrimitiveMode mode, const Int indexCount, const void* indices, const Int primitiveCount)
{
	// mode:			the primitives opengl can construct and draw e.g. triangle, triangle strip, line, etc.
	// indexCount:		the number of indices that will be used for rendering, equivalent to the total number of indices unless partial or batch rendering is used
	// indices:			would read from an actual array of indices on cpu side, however when a VBO is used, it represents an offset instead, usually nullptr
	// primitiveCount:	the number of instances, usually the number of total instances unless instances are partially rendered
#ifdef TRACE_OPENGL
		openGlState.write("glDrawElementsInstanced(...);");
#endif // TRACE_OPENGL
	glDrawElementsInstanced(static_cast<UInt>(mode), indexCount, GL_UNSIGNED_INT, indices, primitiveCount);
}

void OpenGL::Textures::apiClearTexImage(const TextureID texture, const Int mipmapLevel, WritePixelsFormat format, DataType type, const void* data)
{
#ifdef TRACE_OPENGL
		openGlState.write("glClearTexImage(...);");
#endif // TRACE_OPENGL
	glClearTexImage(texture, mipmapLevel, static_cast<UInt>(format), static_cast<UInt>(type), data);
}

void OpenGL::Textures::apiTextureStorage2D(const TextureID texture, const Int mipmapLevel, SizedInternalFormat internalFormat, Int width, Int height)
{
	width  = max(1, width);
	height = max(1, height);

#ifdef TRACE_OPENGL
		openGlState.write("glTextureStorage2D(...);");
#endif // TRACE_OPENGL
	glTextureStorage2D(texture, mipmapLevel, static_cast<UInt>(internalFormat), width, height);
}

void OpenGL::VertexArray::apiVertexArrayAttribBinding(const VertexArrayID vertexArrayID, const AttributeIndex attributeID, const Index layoutBinding)
{
#ifdef CHECK_OPENGL
		if(attributeID > openGlState.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
#endif // CHECK_OPENGL

#ifdef TRACE_OPENGL
		openGlState.write("glVertexArrayAttribBinding(" + toString(vertexArrayID) + ", " + toString(attributeID) + ", " + toString(layoutBinding) + ");");
#endif // TRACE_OPENGL
	glVertexArrayAttribBinding(vertexArrayID, attributeID, layoutBinding);
}

void OpenGL::Shaders::apiUniformBlockBinding(const ProgramID programId, const UniformBlockID uniformBlockIndex, const BindingPoint uniformBlockBinding)
{
	// binds the uniform block id of each shader to a predefined binding point that shaders attach to
#ifdef TRACE_OPENGL
		openGlState.write("glUniformBlockBinding(...);");
#endif // TRACE_OPENGL
	glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding);
}

void OpenGL::Framebuffer::apiReadPixels(const Int x, const Int y, const Int width, const Int height, ReadPixelsFormat format, DataType type, void* data)
{
#ifdef TRACE_OPENGL
		openGlState.write("glReadPixels(...);");
#endif // TRACE_OPENGL
	glReadPixels(x, y, width, height, static_cast<UInt>(format), static_cast<UInt>(type), data);
}

void OpenGLVertexArrayObject::renderInstanced(const PrimitiveMode primitiveMode, const Int indexCount, const void* indicesOrOffset, const Int instanceCount) const
{
	OpenGL::VertexArray::apiDrawElementsInstanced(primitiveMode, indexCount, indicesOrOffset, instanceCount);
}

void OpenGL::Framebuffer::apiClearNamedFramebufferiv(const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const Int* value)
{
#ifdef TRACE_OPENGL
		openGlState.write("glClearNamedFramebufferiv(...);");
#endif // TRACE_OPENGL
	glClearNamedFramebufferiv(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), value);
}

void OpenGL::Framebuffer::apiClearNamedFramebufferfv(const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const Float* value)
{
#ifdef TRACE_OPENGL
		openGlState.write("glClearNamedFramebufferfv(...);");
#endif // TRACE_OPENGL
	glClearNamedFramebufferfv(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), value);
}

void OpenGL::Framebuffer::apiClearNamedFramebufferuiv(const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const UInt* value)
{
#ifdef TRACE_OPENGL
		openGlState.write("glClearNamedFramebufferuiv(...);");
#endif // TRACE_OPENGL
	glClearNamedFramebufferuiv(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), value);
}

void OpenGL::Framebuffer::apiNamedFramebufferTexture(const FramebufferID framebufferID, const FramebufferAttachment attachmentID, const TextureID texture, const Int mipmapLevel)
{
#ifdef TRACE_OPENGL
		openGlState.write("glNamedFramebufferTexture(...);");
#endif // TRACE_OPENGL
	glNamedFramebufferTexture(framebufferID, static_cast<UInt>(attachmentID), texture, mipmapLevel);
}

void OpenGL::VertexArray::apiVertexArrayVertexBuffer(const VertexArrayID vertexArrayID, const UInt layoutBinding, const VertexBufferID buffer, const LL offset, const Int stride)
{
#ifdef TRACE_OPENGL
		openGlState.write("glVertexArrayVertexBuffer(" + toString(vertexArrayID) + ", " + toString(layoutBinding) + ", " + toString(buffer) + ", " + toString(offset) + ", " + toString(stride) + ");");
#endif // TRACE_OPENGL
	glVertexArrayVertexBuffer(vertexArrayID, layoutBinding, buffer, offset, stride);
}

void OpenGLTexture2D::setData(const SizedInternalFormat internalFormat, const Int width, const Int height, const WritePixelsFormat colorFormat, const DataType dataType, const void* data) const
{
	OpenGL::Textures::apiTextureStorage2D(textureID, 1, internalFormat, width, height);
	if(data) OpenGL::Textures::apiTextureSubImage2D(textureID, 0, 0, 0, width, height, colorFormat, dataType, data);
}

void OpenGL::Framebuffer::apiClearNamedFramebufferfi(const FramebufferID framebufferID, ClearBufferType buffer, const FramebufferAttachment attachmentSlot, const Float depth, const Int stencil)
{
#ifdef TRACE_OPENGL
		openGlState.write("glClearNamedFramebufferfi(...);");
#endif // TRACE_OPENGL
	glClearNamedFramebufferfi(framebufferID, static_cast<UInt>(buffer), attachmentToIndex(attachmentSlot), depth, stencil);
}

void OpenGL::VertexArray::apiVertexArrayAttribFormat(const VertexArrayID vertexArrayID, const AttributeIndex attributeID, const Int size, DataType type, const Bool normalized, const UInt relativeOffset)
{
#ifdef CHECK_OPENGL
		if(attributeID > openGlState.maxVertexAttributes - 1) logError("Attribute Index out of bounds!");
#endif // CHECK_OPENGL

#ifdef TRACE_OPENGL
		openGlState.write("glVertexArrayAttribFormat(" + toString(vertexArrayID) + ", " + toString(attributeID) + ", " + toString(size) + ", " + toString(static_cast<UInt>(type)) + ", " + toString(normalized) + ", " + toString(relativeOffset) + ");");
#endif // TRACE_OPENGL
	glVertexArrayAttribFormat(vertexArrayID, attributeID, size, static_cast<UInt>(type), normalized, relativeOffset);
}

void OpenGL::Textures::apiTextureSubImage2D(const TextureID texture, const Int mipmapLevel, const Int xOffset, const Int yOffset, const Int width, const Int height, WritePixelsFormat format, DataType type, const void* pixels)
{
#ifdef TRACE_OPENGL
		openGlState.write("glTextureSubImage2D(...);");
#endif // TRACE_OPENGL
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glTextureSubImage2D(texture, mipmapLevel, xOffset, yOffset, width, height, static_cast<UInt>(format), static_cast<UInt>(type), pixels);
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
