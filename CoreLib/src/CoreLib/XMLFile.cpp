#include <fstream>
#include "XMLFile.h"

namespace XML {

	static bool ValidateFilePath(const XMLFilePath& path, std::string& errorOut) {
		namespace fs = std::filesystem;

		if (path.filename().empty()) {
			errorOut = "File path has no file name!";
			return false;
		}

		fs::path parentDir = path.parent_path();
		if (!parentDir.empty() && !fs::exists(parentDir)) {
			errorOut = "File path does not exist!";
			return false;
		}

		return true;
	}

	static XMLValue ParseAttributeValue(std::string_view sv) {
		if (sv == "true")
			return XMLValue(true);
		if (sv == "false")
			return XMLValue(false);

		std::string s(sv);
		size_t idx;
		try {
			idx = 0;
			int i = std::stoi(s, &idx);
			if (idx == s.size())
				return XMLValue(i);
		}
		catch (...) {}

		try {
			idx = 0;
			float f = std::stof(s, &idx);
			if (idx == s.size())
				return XMLValue(f);
		}
		catch (...) {}

		return XMLValue(sv);
	}

	static std::vector<XMLValue> ParseAttributeValueList(std::string_view sv) {
		std::vector<XMLValue> result;

		const char* begin = sv.data();
		const char* end = begin + sv.size();
		const char* tokenStart = begin;

		for (const char* p = begin; p < end; p++) {
			if (std::isspace(static_cast<unsigned char>(*p))) {
				if (p > tokenStart) {
					result.push_back(ParseAttributeValue(std::string_view(tokenStart, p - tokenStart)));
				}
				tokenStart = p + 1;
			}
		}

		if (tokenStart < end) {
			result.push_back(ParseAttributeValue(std::string_view(tokenStart, end - tokenStart)));
		}

		return result;
	}

	XMLToken::XMLToken(XMLTokenType t, const char* pos, size_t size)
		: type(t), value(pos, size) {
	}

	XMLValue::XMLValue(XMLVariant data)
		: m_data(data) {
	}

	std::string_view XMLAttribute::GetName() const {
		return m_name;
	}

	const XMLValue& XMLAttribute::GetValue(size_t index) const {
		return m_values.at(index);
	}

	const std::vector<XMLValue>& XMLAttribute::GetValues() const {
		return m_values;
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

	std::optional<XMLAttribute> XMLElement::GetAttribute(const std::string& name) const {
		for (auto& att : m_attributes) {
			if (att.GetName() == name) {
				return att;
			}
		}

		return std::nullopt;
	}

	const std::vector<XMLAttribute>& XMLElement::GetAttributes() const {
		return m_attributes;
	}

	XMLElement* XMLElement::GetParent() const {
		return m_parent;
	}

	const std::vector<XMLElement>& XMLElement::GetChilds() const {
		return m_childs;
	}

	XMLReader::XMLReader(const std::string& data)
		: m_pos(data.data()), m_begin(data.data()), m_end(data.data() + data.size()), m_line(1) {
	}

	char XMLReader::Peek() const {
		if (m_pos >= m_end) return '\0';
		return *m_pos;
	}

	char XMLReader::PeekNext() const {
		if (m_pos + 1 >= m_end) return '\0';
		return *(m_pos + 1);
	}

	char XMLReader::Get() {
		if (m_pos >= m_end) return '\0';
		char c = *(m_pos++);
		if (c == '\n') m_line++;
		return c;
	}

	void XMLReader::Unget() {
		if (m_pos == m_begin) return;
		m_pos--;
		if (*m_pos == '\n') m_line--;
	}

	bool XMLReader::End() const {
		return m_pos >= m_end;
	}

	bool XMLReader::Match(char c) {
		if (Peek() != c) return false;
		Get();
		return true;
	}

	bool XMLReader::Match(const char* str) {
		size_t len = strlen(str);

		if (m_pos + len > m_end) return false;
		if (memcmp(m_pos, str, len) != 0) return false;

		for (size_t i = 0; i < len; ++i) Get();
		return true;
	}

	int XMLReader::GetLine() const {
		return m_line;
	}

	const char* XMLReader::GetPos() {
		return m_pos;
	}

	XMLTokenizer::XMLTokenizer(XMLReader& reader)
		: m_reader(reader) {
	}

	XMLToken XMLTokenizer::Next() {
		if (m_hasCurrent) {
			m_hasCurrent = false;
			return m_current;
		}

		return ReadToken();
	}

	XMLToken XMLTokenizer::Peek() {
		if (!m_hasCurrent) {
			m_current = ReadToken();
			m_hasCurrent = true;
		}

		return m_current;
	}

	bool XMLTokenizer::Match(XMLTokenType type) {
		if (Peek().type != type) return false;
		Next();
		return true;
	}

	int XMLTokenizer::GetLine() {
		return m_reader.GetLine();
	}

	XMLToken XMLTokenizer::ReadToken() {
		while (!m_reader.End()) {
			const char* start = m_reader.GetPos();
			char c = m_reader.Get();

			switch (c) {

			case '<':
				m_insideElement = true;
				if (m_reader.Match('/')) {
					return XMLToken{ XMLTokenType::TAG_END_OPEN, start };
				}
				if (m_reader.Match('?')) {
					return XMLToken{ XMLTokenType::PROCESSING_INSTRUCTION_OPEN, start };
				}
				if (m_reader.Match('!')) {
					if (m_reader.Match("--")) {
						SkipComment();
						continue;
					}
				}
				return XMLToken{ XMLTokenType::TAG_OPEN, start };

			case '>':
				m_insideElement = false;
				return XMLToken{ XMLTokenType::TAG_CLOSE, start };

			case '/':
				if (m_reader.Match('>')) {
					m_insideElement = false;
					return XMLToken{ XMLTokenType::SELF_CLOSE, start };
				}
				break;

			case '?':
				if (m_reader.Match('>')) {
					m_insideElement = false;
					return XMLToken{ XMLTokenType::PROCESSING_INSTRUCTION_CLOSE, start };
				}
				break;

			case '=':
				return XMLToken{ XMLTokenType::EQUALS, start };

			case '"':
				return ReadString();

			default:
				if (!isspace(c)) {
					m_reader.Unget();
					return (m_insideElement) ? 
						ReadName() : 
						ReadText();
				}
			}
		}

		return XMLToken{ XMLTokenType::END_OF_FILE, nullptr };
	}

	XMLToken XMLTokenizer::ReadString() {
		const char* start = m_reader.GetPos();

		while (!m_reader.End()) {
			char c = m_reader.Get();

			if (c == '"') {
				const char* end = m_reader.GetPos() - 1;
				return XMLToken{ XMLTokenType::STRING, start, static_cast<size_t>(end - start) };
			}
		}

		return XMLToken{ XMLTokenType::END_OF_FILE, nullptr };
	}

	XMLToken XMLTokenizer::ReadName() {
		const char* start = m_reader.GetPos();

		while (!m_reader.End()) {
			char c = m_reader.Peek();
			if (isspace(c) || c == '=' || c == '>' || c == '/') break;
			m_reader.Get();
		}

		const char* end = m_reader.GetPos();
		return XMLToken{ XMLTokenType::NAME, start, static_cast<size_t>(end - start) };
	}

	XMLToken XMLTokenizer::ReadText() {
		const char* start = m_reader.GetPos();

		while (!m_reader.End()) {
			char c = m_reader.Peek();
			if (c == '<') break;
			m_reader.Get();
		}

		const char* end = m_reader.GetPos();
		return XMLToken{ XMLTokenType::TEXT, start, static_cast<size_t>(end - start) };
	}

	void XMLTokenizer::SkipComment() {
		while (!m_reader.End()) {

			if (m_reader.Match("-->")) {
				m_insideElement = false;
				return;
			}

			m_reader.Get();
		}
	}

	XMLParser::XMLParser(XMLTokenizer& tokenizer)
		: m_tokenizer(tokenizer) {
	}

	std::optional<XMLElement> XMLParser::Parse() {
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
					XMLToken versionValue = Expect(XMLTokenType::STRING);
					if (!HasError()) {
						try {
							m_version = std::stof(std::string(versionValue.value));
						}
						catch (...) {
							AddError("Invalid version value in processing instruction");
						}
					}
				}
			}

			if (m_tokenizer.Peek().type == XMLTokenType::END_OF_FILE) {
				AddError("Unterminated processing instruction at line " + std::to_string(m_tokenizer.GetLine()));
				return std::nullopt;
			}

			m_tokenizer.Next();

			while (m_tokenizer.Peek().type == XMLTokenType::TEXT)
				m_tokenizer.Next();
		}

		return ParseElement();
	}

	bool XMLParser::HasError() const {
		return !m_error.empty();
	}

	const std::string& XMLParser::GetError() const {
		return m_error;
	}

	float XMLParser::GetVersion() const {
		return m_version;
	}

	std::optional<XMLElement> XMLParser::ParseElement() {
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
		if (next.type == XMLTokenType::SELF_CLOSE) {
			return element;
		}
		else if (next.type != XMLTokenType::TAG_CLOSE) {
			AddError("Expected '>' or '/>'");
			return std::nullopt;
		}

		while (true) {
			XMLToken peek = m_tokenizer.Peek();
			if (peek.type == XMLTokenType::TEXT) {
				XMLToken textToken = m_tokenizer.Next();
				element.m_innerText = textToken.value;
			}
			else if (peek.type == XMLTokenType::TAG_OPEN) {
				auto child = ParseElement();
				if (child.has_value()) {
					element.m_childs.push_back(std::move(*child));
				}
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

	void XMLParser::ParseAttributes(XMLElement& element) {
		while (true) {
			XMLToken peek = m_tokenizer.Peek();
			if (peek.type != XMLTokenType::NAME)
				break;

			XMLToken name = m_tokenizer.Next();
			Expect(XMLTokenType::EQUALS);
			XMLToken value = Expect(XMLTokenType::STRING);

			if (HasError())
				return;

			XMLAttribute attr;
			attr.m_name = name.value;
			attr.m_values = ParseAttributeValueList(value.value);
			element.m_attributes.push_back(attr);
		}
	}

	XMLToken XMLParser::Expect(XMLTokenType type) {
		XMLToken tok = m_tokenizer.Next();
		if (tok.type != type) {
			AddError("Unexpected token at line " + std::to_string(m_tokenizer.GetLine()));
		}
		return tok;
	}

	void XMLParser::AddError(const std::string& error) {
		if (!error.empty()) {
			m_error += error;
			m_error.push_back('\n');
		}
	}

	void XMLDocument::FixParentPointers(XMLElement& element) {
		for (auto& child : element.m_childs) {
			child.m_parent = &element;
			FixParentPointers(child);
		}
	}

	bool XMLDocument::ParseContent() {
		XMLReader reader(m_fileContent);
		XMLTokenizer tokenizer(reader);
		XMLParser parser(tokenizer);

		auto rootElement = parser.Parse();

		if (parser.HasError()) {
			AddError(parser.GetError());
		}

		m_version = parser.GetVersion();
		m_hasRootElement = rootElement.has_value();
		if (m_hasRootElement) {
			m_rootElement = std::move(*rootElement);
			FixParentPointers(m_rootElement);
		}

		return m_hasRootElement && !parser.HasError();
	}

	bool XMLDocument::LoadFile(const XMLFilePath& path) {
		std::string error;
		if (!ValidateFilePath(path, error)) {
			AddError("File path was invalid!");
			AddError(error);
			return false;
		}

		std::ifstream f(path, std::ios::binary | std::ios::ate);
		if (!f.is_open()) {
			AddError("Could not open file!");
			return false;
		}
		m_fileContent = std::string(static_cast<size_t>(f.tellg()), '\0');
		f.seekg(0);
		f.read(m_fileContent.data(), static_cast<std::streamsize>(m_fileContent.size()));
		f.close();

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
		bool hasError = HasError();
		if (hasError) outErr = m_error;
		return hasError;
	}

	float XMLDocument::GetVersion() const {
		return m_version;
	}

	const XMLElement* XMLDocument::GetRootElement() const {
		return (m_hasRootElement) ? &m_rootElement : nullptr;
	}

	const std::string& XMLDocument::GetError() const {
		return m_error;
	}

	void XMLDocument::AddError(const std::string& error) {
		if (!error.empty()) {
			m_error += error;
			m_error.push_back('\n');
		}
	}

}