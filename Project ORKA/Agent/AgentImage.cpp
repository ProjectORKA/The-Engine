#include "AgentImage.hpp"

#include "../GraphicsAPI.hpp"
#include "../Renderer.hpp"
#include "../Window.hpp"

#include <atomic>

namespace
{
	std::atomic<U64> nextScreenshotId = 1;

	void appendU32(Vector<Byte>& output, const U32 value)
	{
		output.push_back(static_cast<Byte>((value >> 24) & 0xff));
		output.push_back(static_cast<Byte>((value >> 16) & 0xff));
		output.push_back(static_cast<Byte>((value >> 8) & 0xff));
		output.push_back(static_cast<Byte>(value & 0xff));
	}

	U32 crc32(const Byte* data, const SizeT size)
	{
		U32 crc = 0xffffffffu;
		for(SizeT i = 0; i < size; ++i)
		{
			crc ^= data[i];
			for(Int bit = 0; bit < 8; ++bit)
				crc = (crc >> 1) ^ (0xedb88320u & static_cast<U32>(-(static_cast<Int>(crc & 1))));
		}
		return ~crc;
	}

	U32 adler32(const Vector<Byte>& data)
	{
		constexpr U32 modulus = 65521;
		U32 a = 1;
		U32 b = 0;
		for(const Byte value : data)
		{
			a = (a + value) % modulus;
			b = (b + a) % modulus;
		}
		return (b << 16) | a;
	}

	void appendChunk(Vector<Byte>& png, const char type[4], const Vector<Byte>& data)
	{
		appendU32(png, static_cast<U32>(data.size()));
		const SizeT crcStart = png.size();
		png.insert(png.end(), type, type + 4);
		png.insert(png.end(), data.begin(), data.end());
		appendU32(png, crc32(png.data() + crcStart, png.size() - crcStart));
	}

	Vector<Byte> encodePng(const Vector<Byte>& rgba, const Int width, const Int height)
	{
		Vector<Byte> scanlines;
		scanlines.reserve((static_cast<SizeT>(width) * 4 + 1) * height);
		for(Int y = 0; y < height; ++y)
		{
			scanlines.push_back(0);
			const SizeT row = static_cast<SizeT>(y) * width * 4;
			scanlines.insert(scanlines.end(), rgba.begin() + row, rgba.begin() + row + width * 4);
		}

		Vector<Byte> zlib{0x78, 0x01};
		SizeT offset = 0;
		while(offset < scanlines.size())
		{
			const U16 blockSize = static_cast<U16>((std::min)(
				static_cast<SizeT>(65535), scanlines.size() - offset));
			const Bool finalBlock = offset + blockSize == scanlines.size();
			zlib.push_back(finalBlock ? 1 : 0);
			zlib.push_back(static_cast<Byte>(blockSize & 0xff));
			zlib.push_back(static_cast<Byte>((blockSize >> 8) & 0xff));
			const U16 inverse = static_cast<U16>(~blockSize);
			zlib.push_back(static_cast<Byte>(inverse & 0xff));
			zlib.push_back(static_cast<Byte>((inverse >> 8) & 0xff));
			zlib.insert(zlib.end(), scanlines.begin() + offset,
				scanlines.begin() + offset + blockSize);
			offset += blockSize;
		}
		appendU32(zlib, adler32(scanlines));

		Vector<Byte> png{137, 80, 78, 71, 13, 10, 26, 10};
		Vector<Byte> header;
		appendU32(header, static_cast<U32>(width));
		appendU32(header, static_cast<U32>(height));
		header.insert(header.end(), {8, 6, 0, 0, 0});
		appendChunk(png, "IHDR", header);
		appendChunk(png, "IDAT", zlib);
		appendChunk(png, "IEND", {});
		return png;
	}

	Path artifactRoot()
	{
		return std::filesystem::absolute(Path("AgentArtifacts")).lexically_normal();
	}

	Bool isInside(const Path& root, const Path& candidate)
	{
		String rootText = root.generic_string();
		String candidateText = candidate.generic_string();
		std::transform(rootText.begin(), rootText.end(), rootText.begin(),
			[](const unsigned char value) { return static_cast<Char>(std::tolower(value)); });
		std::transform(candidateText.begin(), candidateText.end(), candidateText.begin(),
			[](const unsigned char value) { return static_cast<Char>(std::tolower(value)); });
		if(candidateText == rootText) return true;
		if(!rootText.ends_with('/')) rootText.push_back('/');
		return candidateText.starts_with(rootText);
	}

	String hexHash(const Vector<Byte>& data)
	{
		U64 hash = 1469598103934665603ull;
		for(const Byte value : data)
		{
			hash ^= value;
			hash *= 1099511628211ull;
		}
		std::ostringstream output;
		output << std::hex << std::setfill('0') << std::setw(16) << hash;
		return output.str();
	}
}

AgentValue captureAgentScreenshot(Window& window, const AgentValue& params)
{
	const IVec2 size = window.getContentSize();
	if(size.x <= 0 || size.y <= 0) throw std::runtime_error("Window framebuffer is empty");
	const U64 byteCount = static_cast<U64>(size.x) * size.y * 4;
	if(byteCount > 256ull * 1024ull * 1024ull)
		throw std::runtime_error("Screenshot exceeds the 256 MiB safety limit");

	const Path root = artifactRoot();
	std::filesystem::create_directories(root);
	Path outputPath;
	if(const AgentValue* path = params.find("path"); path && path->isString() && !path->asString().empty())
	{
		outputPath = Path(path->asString());
		if(outputPath.is_relative()) outputPath = root / outputPath;
		outputPath = outputPath.lexically_normal();
		if(!isInside(root, outputPath))
			throw std::runtime_error("Screenshot path must be inside AgentArtifacts");
	}
	else
	{
		outputPath = root / ("screenshot-window-" + std::to_string(window.id) + "-"
			+ std::to_string(nextScreenshotId.fetch_add(1)) + ".png");
	}
	if(outputPath.extension() != ".png") throw std::runtime_error("Screenshot path must end in .png");
	std::filesystem::create_directories(outputPath.parent_path());

	Vector<Byte> bottomUp(static_cast<SizeT>(byteCount));
	External::glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	External::glReadBuffer(GL_BACK);
	External::glPixelStorei(GL_PACK_ALIGNMENT, 1);
	OpenGL::Framebuffer::apiReadPixels(window.renderer.openGlContext, 0, 0, size.x, size.y,
		ReadPixelsFormat::RGBA, DataType::Byte, bottomUp.data());

	Vector<Byte> topDown(bottomUp.size());
	const SizeT rowBytes = static_cast<SizeT>(size.x) * 4;
	for(Int y = 0; y < size.y; ++y)
		std::copy_n(bottomUp.data() + static_cast<SizeT>(size.y - y - 1) * rowBytes,
			rowBytes, topDown.data() + static_cast<SizeT>(y) * rowBytes);

	const Vector<Byte> png = encodePng(topDown, size.x, size.y);
	std::ofstream output(outputPath, std::ios::binary | std::ios::trunc);
	if(!output) throw std::runtime_error("Could not create screenshot artifact");
	output.write(reinterpret_cast<const Char*>(png.data()), static_cast<std::streamsize>(png.size()));
	if(!output) throw std::runtime_error("Could not write screenshot artifact");

	return AgentValue::Object{
		{"path", outputPath.string()},
		{"width", size.x},
		{"height", size.y},
		{"format", "png"},
		{"bytes", static_cast<U64>(png.size())},
		{"hashAlgorithm", "fnv1a64"},
		{"hash", hexHash(png)},
		{"frameIndex", window.agentFrameIndex.load()}
	};
}
