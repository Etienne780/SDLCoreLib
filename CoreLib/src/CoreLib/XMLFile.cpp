#include "XMLFile.h"
#include <fstream>
#include <cstring>
#include <cctype>

namespace {

	class XMLReader {
	public:
		explicit XMLReader(const std::string& data)
			: m_pos(data.data())
			, m_begin(data.data())
			, m_end(data.data() + data.size())
			, m_line(1) {
		}

		char Peek() const {
			if (m_pos >= m_end)
				return '\0';
			return *m_pos;
		}

		char PeekNext() const {
			if (m_pos + 1 >= m_end)
				return '\0';
			return *(m_pos + 1);
		}

		char Get() {
			if (m_pos >= m_end)
				return '\0';
			char c = *(m_pos++);
			if (c == '\n')
				m_line++;
			return c;
		}

		void Unget() {
			if (m_pos == m_begin)
				return;
			m_pos--;
			if (*m_pos == '\n')
				m_line--;
		}

		bool End() const {
			return m_pos >= m_end;
		}

		bool Match(char c) {
			if (Peek() != c)
				return false;
			Get();
			return true;
		}

		bool Match(const char* str) {
			size_t len = strlen(str);
			if (m_pos + len > m_end)
				return false;
			if (memcmp(m_pos, str, len) != 0)
				return false;
			for (size_t i = 0; i < len; ++i)
				Get();
			return true;
		}

		int GetLine() const {
			return m_line;
		}

		const char* GetPos() const {
			return m_pos;
		}

	private:
		const char* m_pos = nullptr;
		const char* m_begin = nullptr;
		const char* m_end = nullptr;
		int m_line = 0;
	};

	class XMLTokenizer {
	public:
		explicit XMLTokenizer(XMLReader& reader)
			: m_reader(reader) {
		}

		XML::XMLToken Next() {
			if (m_hasCurrent) {
				m_hasCurrent = false;
				return m_current;
			}
			return ReadToken();
		}

		XML::XMLToken Peek() {
			if (!m_hasCurrent) {
				m_current = ReadToken();
				m_hasCurrent = true;
			}
			return m_current;
		}

		bool Match(XML::XMLTokenType type) {
			if (Peek().type != type)
				return false;
			Next();
			return true;
		}

		int GetLine() {
			return m_reader.GetLine();
		}

	private:
		XMLReader& m_reader;
		XML::XMLToken m_current;
		bool m_hasCurrent = false;
		bool m_insideTag = false;

		XML::XMLToken ReadToken() {
			using TT = XML::XMLTokenType;

			while (!m_reader.End()) {
				const char* start = m_reader.GetPos();
				char c = m_reader.Get();

				switch (c) {
				case '<':
					m_insideTag = true;
					if (m_reader.Match('/'))
						return { TT::TAG_END_OPEN, start };
					if (m_reader.Match('?'))
						return { TT::PROCESSING_INSTRUCTION_OPEN, start };
					if (m_reader.Match('!') && m_reader.Match("--")) {
						SkipComment();
						continue;
					}
					return { TT::TAG_OPEN, start };

				case '>':
					m_insideTag = false;
					return { TT::TAG_CLOSE, start };

				case '/':
					if (m_reader.Match('>')) {
						m_insideTag = false;
						return { TT::SELF_CLOSE, start };
					}
					break;

				case '?':
					if (m_reader.Match('>')) {
						m_insideTag = false;
						return { TT::PROCESSING_INSTRUCTION_CLOSE, start };
					}
					break;

				case '=':
					return { TT::EQUALS, start };

				case '"':
					return ReadString();

				default:
					if (!isspace(static_cast<unsigned char>(c))) {
						m_reader.Unget();
						if (m_insideTag)
							return ReadName();
						return ReadText();
					}
				}
			}

			return { XML::XMLTokenType::END_OF_FILE, nullptr };
		}

		XML::XMLToken ReadString() {
			const char* start = m_reader.GetPos();
			while (!m_reader.End()) {
				char c = m_reader.Get();
				if (c == '"') {
					const char* end = m_reader.GetPos() - 1;
					return { XML::XMLTokenType::STRING, start, static_cast<size_t>(end - start) };
				}
			}
			return { XML::XMLTokenType::END_OF_FILE, nullptr };
		}

		XML::XMLToken ReadName() {
			const char* start = m_reader.GetPos();
			while (!m_reader.End()) {
				char c = m_reader.Peek();
				if (isspace(static_cast<unsigned char>(c)) || c == '=' || c == '>' || c == '/')
					break;
				m_reader.Get();
			}
			return { XML::XMLTokenType::NAME, start, static_cast<size_t>(m_reader.GetPos() - start) };
		}

		XML::XMLToken ReadText() {
			const char* start = m_reader.GetPos();
			while (!m_reader.End() && m_reader.Peek() != '<')
				m_reader.Get();
			return { XML::XMLTokenType::TEXT, start, static_cast<size_t>(m_reader.GetPos() - start) };
		}

		void SkipComment() {
			while (!m_reader.End()) {
				if (m_reader.Match("-->")) {
					m_insideTag = false;
					return;
				}
				m_reader.Get();
			}
		}
	};

	XML::XMLValue ParseAttributeValue(std::string_view sv) {
		if (sv == "true")
			return XML::XMLValue(true);
		if (sv == "false")
			return XML::XMLValue(false);

		std::string s(sv);
		size_t idx = 0;

		try {
			int i = std::stoi(s, &idx);
			if (idx == s.size())
				return XML::XMLValue(i);
		}
		catch (...) {}

		try {
			idx = 0;
			float f = std::stof(s, &idx);
			if (idx == s.size())
				return XML::XMLValue(f);
		}
		catch (...) {}

		return XML::XMLValue(sv);
	}

	std::vector<XML::XMLValue> ParseAttributeValueList(std::string_view sv) {
		std::vector<XML::XMLValue> result;

		const char* p = sv.data();
		const char* end = p + sv.size();
		const char* tokenStart = p;

		for (; p < end; ++p) {
			if (isspace(static_cast<unsigned char>(*p))) {
				if (p > tokenStart)
					result.push_back(ParseAttributeValue({ tokenStart, static_cast<size_t>(p - tokenStart) }));
				tokenStart = p + 1;
			}
		}

		if (tokenStart < end)
			result.push_back(ParseAttributeValue({ tokenStart, static_cast<size_t>(end - tokenStart) }));

		return result;
	}

}

namespace XML {

	class XMLParser {
	public:
		explicit XMLParser(XMLTokenizer& tokenizer)
			: m_tokenizer(tokenizer) {
		}

		std::optional<XMLElement> Parse() {
			while (m_tokenizer.Peek().type == XMLTokenType::TEXT)
				m_tokenizer.Next();

			if (m_tokenizer.Peek().type != XMLTokenType::PROCESSING_INSTRUCTION_OPEN) {
				AddError("Expected processing instruction at start of document (e.g. <?xml version=\"1.0\"?>)");
				return std::nullopt;
			}

			while (m_tokenizer.Peek().type == XMLTokenType::PROCESSING_INSTRUCTION_OPEN) {
				m_tokenizer.Next();

				while (m_tokenizer.Peek().type != XMLTokenType::PROCESSING_INSTRUCTION_CLOSE &&
					m_tokenizer.Peek().type != XMLTokenType::END_OF_FILE) {
					XMLToken tok = m_tokenizer.Next();
					if (tok.type == XMLTokenType::NAME && tok.value == "version") {
						Expect(XMLTokenType::EQUALS);
						XMLToken ver = Expect(XMLTokenType::STRING);
						if (!HasError()) {
							try {
								m_version = std::stof(std::string(ver.value));
							}
							catch (...) {
								AddError("Invalid version value in processing instruction");
							}
						}
					}
				}

				if (m_tokenizer.Peek().type == XMLTokenType::END_OF_FILE) {
					AddError("Unterminated processing instruction at line " +
						std::to_string(m_tokenizer.GetLine()));
					return std::nullopt;
				}

				m_tokenizer.Next();

				while (m_tokenizer.Peek().type == XMLTokenType::TEXT)
					m_tokenizer.Next();
			}

			return ParseElement();
		}

		bool HasError() const {
			return !m_error.empty();
		}

		const std::string& GetError() const {
			return m_error;
		}

		float GetVersion() const {
			return m_version;
		}

	private:
		float m_version = 0.0f;
		XMLTokenizer& m_tokenizer;
		std::string m_error;

		std::optional<XMLElement> ParseElement() {
			XMLElement element;

			Expect(XMLTokenType::TAG_OPEN);
			XMLToken nameToken = Expect(XMLTokenType::NAME);
			element.m_name = nameToken.value;

			if (HasError())
				return std::nullopt;

			ParseAttributes(element);

			if (HasError())
				return std::nullopt;

			XMLToken next = m_tokenizer.Next();
			if (next.type == XMLTokenType::SELF_CLOSE)
				return element;

			if (next.type != XMLTokenType::TAG_CLOSE) {
				AddError("Expected '>' or '/>' at line " + std::to_string(m_tokenizer.GetLine()));
				return std::nullopt;
			}

			while (true) {
				XMLToken peek = m_tokenizer.Peek();

				if (peek.type == XMLTokenType::TEXT) {
					element.m_innerText = m_tokenizer.Next().value;
				}
				else if (peek.type == XMLTokenType::TAG_OPEN) {
					auto child = ParseElement();
					if (child)
						element.m_children.push_back(std::move(*child));
				}
				else if (peek.type == XMLTokenType::TAG_END_OPEN) {
					m_tokenizer.Next();
					XMLToken closingName = Expect(XMLTokenType::NAME);
					if (closingName.value != element.m_name) {
						AddError("Closing tag </" + std::string(closingName.value) +
							"> does not match opening tag <" + std::string(element.m_name) +
							"> at line " + std::to_string(m_tokenizer.GetLine()));
					}
					Expect(XMLTokenType::TAG_CLOSE);
					if (HasError())
						return std::nullopt;
					break;
				}
				else {
					break;
				}
			}

			return element;
		}

		void ParseAttributes(XMLElement& element) {
			while (m_tokenizer.Peek().type == XMLTokenType::NAME) {
				XMLToken name = m_tokenizer.Next();
				Expect(XMLTokenType::EQUALS);
				XMLToken value = Expect(XMLTokenType::STRING);

				if (HasError())
					return;

				XMLAttribute attr;
				attr.m_name = name.value;
				attr.m_values = ParseAttributeValueList(value.value);
				element.m_attributes.push_back(std::move(attr));
			}
		}

		XMLToken Expect(XMLTokenType type) {
			XMLToken tok = m_tokenizer.Next();
			if (tok.type != type)
				AddError("Unexpected token at line " + std::to_string(m_tokenizer.GetLine()));
			return tok;
		}

		void AddError(const std::string& msg) {
			if (!msg.empty()) {
				m_error += msg;
				m_error += '\n';
			}
		}
	};


	XMLToken::XMLToken(XMLTokenType t, const char* pos, size_t size)
		: type(t), value(pos, size) {
	}

	XMLValue::XMLValue(XMLVariant data)
		: m_data(data) {
	}

	std::string_view XMLAttribute::GetName() const {
		return m_name;
	}

	std::optional<XMLValue> XMLAttribute::GetValue(size_t index) const {
		if (index >= m_values.size())
			return std::nullopt;
		return m_values[index];
	}

	const std::vector<XMLValue>& XMLAttribute::GetValues() const {
		return m_values;
	}

	size_t XMLAttribute::GetValueCount() const {
		return m_values.size();
	}

	bool XMLElement::HasInnerText() const {
		return m_innerText.data() != nullptr;
	}

	std::string_view XMLElement::GetName() const {
		return m_name;
	}

	std::string_view XMLElement::GetInnerText() const {
		return m_innerText;
	}

	std::optional<XMLAttribute> XMLElement::GetAttribute(std::string_view name) const {
		for (const auto& attr : m_attributes) {
			if (attr.GetName() == name)
				return attr;
		}
		return std::nullopt;
	}

	const std::vector<XMLAttribute>& XMLElement::GetAttributes() const {
		return m_attributes;
	}

	XMLElement* XMLElement::GetParent() const {
		return m_parent;
	}

	const std::vector<XMLElement>& XMLElement::GetChildren() const {
		return m_children;
	}

	void XMLDocument::FixParentPointers(XMLElement& element) {
		for (auto& child : element.m_children) {
			child.m_parent = &element;
			FixParentPointers(child);
		}
	}

	bool XMLDocument::ParseContent() {
		XMLReader reader(m_fileContent);
		XMLTokenizer tokenizer(reader);
		XMLParser parser(tokenizer);

		auto root = parser.Parse();

		if (parser.HasError())
			AddError(parser.GetError());

		m_version = parser.GetVersion();
		m_hasRootElement = root.has_value();

		if (m_hasRootElement) {
			m_rootElement = std::move(*root);
			FixParentPointers(m_rootElement);
		}

		return m_hasRootElement && !parser.HasError();
	}

	bool XMLDocument::LoadFile(const XMLFilePath& path) {
		namespace fs = std::filesystem;

		if (path.filename().empty()) {
			AddError("File path has no file name!");
			return false;
		}

		fs::path parent = path.parent_path();
		if (!parent.empty() && !fs::exists(parent)) {
			AddError("Directory does not exist: " + parent.string());
			return false;
		}

		std::ifstream f(path, std::ios::binary | std::ios::ate);
		if (!f.is_open()) {
			AddError("Could not open file: " + path.string());
			return false;
		}

		m_fileContent.resize(static_cast<size_t>(f.tellg()));
		f.seekg(0);
		f.read(m_fileContent.data(), static_cast<std::streamsize>(m_fileContent.size()));

		m_loaded = ParseContent();
		return m_loaded;
	}

	bool XMLDocument::LoadString(const std::string& fileContent) {
		m_fileContent = fileContent;
		m_loaded = ParseContent();
		return m_loaded;
	}

	bool XMLDocument::IsLoaded() const {
		return m_loaded;
	}

	bool XMLDocument::HasError() const {
		return !m_error.empty();
	}

	bool XMLDocument::TryGetError(std::string& outErr) const {
		if (!HasError())
			return false;
		outErr = m_error;
		return true;
	}

	float XMLDocument::GetVersion() const {
		return m_version;
	}

	const std::string& XMLDocument::GetError() const {
		return m_error;
	}

	const XMLElement* XMLDocument::GetRootElement() const {
		if (m_hasRootElement)
			return &m_rootElement;
		return nullptr;
	}

	void XMLDocument::AddError(const std::string& msg) {
		if (!msg.empty()) {
			m_error += msg;
			m_error += '\n';
		}
	}

}