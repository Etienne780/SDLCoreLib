#pragma once
#include <string>
#include <memory>
#include <variant>
#include <optional>
#include <filesystem>

namespace XML {

	using XMLFilePath = std::filesystem::path;

	enum class XMLTokenType : uint8_t {
		TAG_OPEN = 0, /* '<' */ 
		TAG_END_OPEN, /* '</' */ 
		TAG_CLOSE,	  /* '>' */ 
		SELF_CLOSE,   /* '/>' */ 
		PROCESSING_INSTRUCTION_OPEN,  /* '<?' */ 
		PROCESSING_INSTRUCTION_CLOSE, /* '?>' */ 
		NAME,		  /* '<Name otherName="String">Text</Name>' */ 
		STRING,
		TEXT,
		EQUALS,
		END_OF_FILE
	};

	class XMLToken {
	public:
		XMLTokenType type = XMLTokenType::TAG_OPEN;
		std::string_view value;

		XMLToken() = default;
		~XMLToken() = default;
		XMLToken(XMLTokenType t, const char* pos, size_t size = 1);
	};

	class XMLValue {
	public:
		using XMLVariant = std::variant<int, float, bool, std::string_view>;

		XMLVariant m_data;

		XMLValue() = default;
		~XMLValue() = default;

		XMLValue(XMLVariant data);

		template<typename T>
		std::optional<T> GetValue() const {
			if (std::holds_alternative<T>(m_data)) {
				return std::get<T>(m_data);
			}

			if constexpr (std::is_same_v<T, float>) {
				if (std::holds_alternative<int>(m_data))
					return static_cast<float>(std::get<int>(m_data));
			}

			if constexpr (std::is_same_v<T, bool>) {
				if (std::holds_alternative<int>(m_data))
					return static_cast<bool>(std::get<int>(m_data));
			}

			return std::nullopt;
		}
	};

	namespace {
		class XMLParser;
	}

	class XMLAttribute {
	friend class XMLParser;
	public:
		explicit XMLAttribute() = default;
		~XMLAttribute() = default;

		std::string_view GetName() const;
		const XMLValue& GetValue(size_t index) const;
		const std::vector<XMLValue>& GetValues() const;

	private:
		std::string_view m_name;
		std::vector<XMLValue> m_values;
	};

	class XMLDocument;

	class XMLElement {
	friend class XMLParser;
	friend class XMLDocument;
	public:
		explicit XMLElement() = default;
		~XMLElement() = default;

		bool HasInnerText() const;

		std::string_view GetName() const;
		std::string_view GetInnerText() const;
		std::optional<XMLAttribute> GetAttribute(const std::string& name) const;
		const std::vector<XMLAttribute>& GetAttributes() const;

		XMLElement* GetParent() const;
		const std::vector<XMLElement>& GetChilds() const;

	private:
		std::vector<XMLAttribute> m_attributes;

		std::string_view m_name;
		std::string_view m_innerText;

		XMLElement* m_parent = nullptr;
		std::vector<XMLElement> m_childs;
	};

	namespace {

		class XMLReader {
		public:
			explicit XMLReader(const std::string& data);

			char Peek() const;
			char PeekNext() const;
			char Get();

			void Unget();
			bool End() const;
			bool Match(char c);
			bool Match(const char* str);
			int GetLine() const;
			const char* GetPos();

		private:
			const char* m_pos = nullptr;
			const char* m_begin = nullptr;
			const char* m_end = nullptr;
			int m_line = 0;
		};

		class XMLTokenizer {
		public:
			explicit XMLTokenizer(XMLReader& reader);

			XMLToken Next();
			XMLToken Peek();
			bool Match(XMLTokenType type);

			int GetLine();

		private:
			XMLReader& m_reader;
			XMLToken m_current;
			bool m_hasCurrent = false;
			bool m_insideElement = false;

			XMLToken ReadToken();
			XMLToken ReadString();
			XMLToken ReadName();
			XMLToken ReadText();
			void SkipComment();
		};

		class XMLParser {
		public:
			explicit XMLParser(XMLTokenizer& tokenizer);

			std::optional<XMLElement> Parse();

			bool HasError() const;
			const std::string& GetError() const;
			float GetVersion() const;

		private:
			float m_version = 0.0f;
			XMLTokenizer& m_tokenizer;
			std::string m_error;

			std::optional<XMLElement> ParseElement();
			void ParseAttributes(XMLElement& element);

			XMLToken Expect(XMLTokenType type);

			void AddError(const std::string& error);
		};

	}

	class XMLDocument {
	public:
		explicit XMLDocument() = default;
		~XMLDocument() = default;

		bool LoadFile(const XMLFilePath& path);
		bool LoadString(const std::string& fileContent);

		bool IsLoaded() const;
		bool HasError() const;
		bool TryGetError(std::string& outErr) const;

		float GetVersion() const;

		const XMLElement* GetRootElement() const;

		const std::string& GetError() const;

	private:
		bool m_loaded = false;
		float m_version = 0.0f;
		std::string m_fileContent;

		bool m_hasRootElement = false;
		XMLElement m_rootElement;

		std::string m_error;

		bool ParseContent();
		static void FixParentPointers(XMLElement& element);
		void AddError(const std::string& error);
	};

}