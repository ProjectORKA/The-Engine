#include "UiSnapshots.hpp"

#include <iomanip>
#include <sstream>

namespace Ui::Testing
{
	namespace
	{
		String escaped(const String& value)
		{
			String output;
			output.reserve(value.size() + 8);
			const Char hex[] = "0123456789abcdef";
			for (const unsigned char c : value)
			{
				switch (c)
				{
				case '"': output += "\\\""; break;
				case '\\': output += "\\\\"; break;
				case '\b': output += "\\b"; break;
				case '\f': output += "\\f"; break;
				case '\n': output += "\\n"; break;
				case '\r': output += "\\r"; break;
				case '\t': output += "\\t"; break;
				default:
					if (c < 0x20)
					{
						output += "\\u00";
						output.push_back(hex[c >> 4]);
						output.push_back(hex[c & 15]);
					}
					else output.push_back(static_cast<Char>(c));
				}
			}
			return output;
		}

		void stringField(std::ostringstream& stream, const String& name,
			const String& value, const Bool comma = true)
		{
			stream << '"' << name << "\":\"" << escaped(value) << '"';
			if (comma) stream << ',';
		}

		void vec2(std::ostringstream& stream, const Vec2 value)
		{
			stream << '[' << value.x << ',' << value.y << ']';
		}

		void ivec2(std::ostringstream& stream, const IVec2 value)
		{
			stream << '[' << value.x << ',' << value.y << ']';
		}
	}

	String serializeLayoutSnapshot(
		UiDocument& document, const UiLayoutSnapshotOptions& options)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(
			static_cast<Int>(std::min(options.decimalPlaces, 9u)));
		stream << "{\"schema\":\"orka.ui.layout.v1\",\"viewport\":{";
		stream << "\"logical\":"; ivec2(stream, document.viewport.logicalSize);
		stream << ",\"framebuffer\":"; ivec2(stream, document.viewport.framebufferSize);
		stream << ",\"scale\":"; vec2(stream, document.viewport.contentScale);
		stream << ",\"revision\":" << document.viewport.revision << "},\"nodes\":[";
		Bool first = true;
		UInt depth = 0;
		Vector<UInt> childEnds;
		document.traverse([&](const UiNodeHandle handle, UiNode& node)
			{
				while (!childEnds.empty() && childEnds.back() == 0)
				{
					childEnds.pop_back();
					if (depth) --depth;
					if (!childEnds.empty()) --childEnds.back();
				}
				if (!first) stream << ',';
				first = false;
				stream << '{';
				stream << "\"handle\":[" << handle.index << ',' << handle.generation << "],";
				stream << "\"depth\":" << depth << ',';
				stringField(stream, "type", node.authored.typeName);
				stringField(stream, "id", node.authored.id);
				stream << "\"classes\":[";
				for (SizeT index = 0; index < node.authored.classes.size(); ++index)
				{
					if (index) stream << ',';
					stream << '"' << escaped(node.authored.classes[index]) << '"';
				}
				stream << "],";
				if (options.includeText) stringField(stream, "text", node.authored.text);
				stream << "\"visible\":" << (node.state.visible ? "true" : "false") << ',';
				stream << "\"enabled\":" << (node.state.enabled ? "true" : "false") << ',';
				stream << "\"focused\":" << (node.state.focused ? "true" : "false") << ',';
				if (options.includeDirtyFlags)
					stream << "\"dirty\":" << node.dirtyFlags << ',';
				stream << "\"position\":"; vec2(stream, node.computed.position);
				stream << ",\"size\":"; vec2(stream, node.computed.size);
				stream << ",\"contentPosition\":"; vec2(stream, node.computed.contentPosition);
				stream << ",\"contentSize\":"; vec2(stream, node.computed.contentSize);
				stream << ",\"clipPosition\":"; vec2(stream, node.computed.clipPosition);
				stream << ",\"clipSize\":"; vec2(stream, node.computed.clipSize);
				stream << ",\"scrollExtent\":"; vec2(stream, node.computed.scrollExtent);
				stream << ",\"baseline\":" << node.computed.baseline;
				stream << ",\"styleRevision\":" << node.computed.styleRevision;
				stream << ",\"layoutRevision\":" << node.computed.layoutRevision;
				if (options.includeSemantics)
				{
					stream << ",\"semantics\":{";
					stream << "\"role\":" << static_cast<UInt>(node.state.semantics.role) << ',';
					stringField(stream, "label", node.state.semantics.label);
					stringField(stream, "value", node.state.semantics.value);
					stream << "\"selected\":" <<
						(node.state.semantics.selected ? "true" : "false") << ',';
					stream << "\"checked\":" <<
						(node.state.semantics.checked ? "true" : "false") << '}';
				}
				stream << '}';
				if (!node.childHandles().empty())
				{
					childEnds.push_back(static_cast<UInt>(node.childHandles().size()));
					++depth;
				}
				else if (!childEnds.empty()) --childEnds.back();
			});
		stream << "]}";
		return stream.str();
	}

	String serializeScreenshotGoldenMetadata(
		const UiScreenshotGoldenMetadata& metadata)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(3);
		stream << "{\"schema\":\"orka.ui.screenshot-golden.v1\",";
		stringField(stream, "goldenId", metadata.goldenId);
		stringField(stream, "bundle", metadata.bundleName);
		stringField(stream, "scene", metadata.scene);
		stream << "\"logicalSize\":"; ivec2(stream, metadata.logicalSize);
		stream << ",\"framebufferSize\":"; ivec2(stream, metadata.framebufferSize);
		stream << ",\"contentScale\":"; vec2(stream, metadata.contentScale);
		stream << ',';
		stringField(stream, "renderer", metadata.renderer);
		stringField(stream, "platform", metadata.platform);
		stringField(stream, "colorSpace", metadata.colorSpace);
		stream << "\"bundleGeneration\":" << metadata.bundleGeneration << ',';
		stream << "\"seed\":" << metadata.deterministicSeed << ',';
		stringField(stream, "expectedImageHash", metadata.expectedImageHash);
		stringField(stream, "capturedImageHash", metadata.capturedImageHash);
		stringField(stream, "captureArtifact", metadata.captureArtifact);
		stringField(stream, "note", metadata.note, false);
		stream << '}';
		return stream.str();
	}
}
