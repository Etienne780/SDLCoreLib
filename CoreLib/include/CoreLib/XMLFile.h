#pragma once
#include <string>
#include <variant>
#include <optional>
#include <vector>
#include <filesystem>

namespace XML {

	/*
	* @brief Filesystem path used to locate XML files.
	*/
	using XMLFilePath = std::filesystem::path;

	/*
	* @brief Classifies a single token produced by the tokenizer.
	*        Used internally by XMLParser and XMLTokenizer.
	*/
	enum class XMLTokenType : uint8_t {
		TAG_OPEN = 0,                 // '<'
		TAG_END_OPEN,                 // '</'
		TAG_CLOSE,                    // '>'
		SELF_CLOSE,                   // '/>'
		PROCESSING_INSTRUCTION_OPEN,  // '<?'
		PROCESSING_INSTRUCTION_CLOSE, // '?>'
		NAME,
		STRING,
		TEXT,
		EQUALS,
		END_OF_FILE
	};

	/*
	* @brief A single token with its type and a non-owning view into the source text.
	*        The view is only valid as long as the source string that was passed to
	*        XMLDocument::LoadFile or XMLDocument::LoadString is alive.
	*/
	class XMLToken {
	public:
		XMLTokenType type = XMLTokenType::TAG_OPEN;
		std::string_view value;

		XMLToken() = default;

		/*
		* @brief Constructs a token pointing into an existing character buffer.
		* @param t    Token type.
		* @param pos  Pointer to the first character of the token inside the source buffer.
		* @param size Number of characters this token spans.
		*/
		XMLToken(XMLTokenType t, const char* pos, size_t size = 1);
	};

	/*
	* @brief A typed value stored inside an XMLAttribute.
	*        Internally holds one of: int, float, bool, or string_view.
	*        When the underlying type is int it can be implicitly retrieved as float or bool.
	*        string_view values are non-owning and only valid as long as the owning
	*        XMLDocument is alive.
	*/
	class XMLValue {
	public:
		using XMLVariant = std::variant<int, float, bool, std::string_view>;

		XMLValue() = default;

		/*
		* @brief Constructs an XMLValue from one of the supported variant types.
		* @param data The value to store.
		*/
		explicit XMLValue(XMLVariant data);

		/*
		* @brief Tries to retrieve the stored value as type T.
		*        int can be retrieved as float or bool without an explicit cast in the source data.
		*        Returns nullopt if the stored type cannot be converted to T.
		* @return The value as T, or nullopt if the conversion is not possible.
		*/
		template<typename T>
		std::optional<T> GetValue() const {
			if (std::holds_alternative<T>(m_data))
				return std::get<T>(m_data);

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

	private:
		XMLVariant m_data;
	};

	class XMLParser;
	class XMLDocument;

	/*
	* @brief A single XML attribute consisting of a name and one or more typed values.
	*        Multiple values occur when the attribute string contains spaces, e.g.
	*        state="hover active" is stored as two separate XMLValue entries.
	*        All string_view data is non-owning and only valid as long as the owning
	*        XMLDocument is alive.
	*/
	class XMLAttribute {
		friend class XMLParser;
	public:
		XMLAttribute() = default;

		/*
		* @brief Returns the attribute name as a non-owning view into the source text.
		* @return The attribute name.
		*/
		std::string_view GetName() const;

		/*
		* @brief Returns the value at the given index.
		*        Attributes with a single value always use index 0.
		*        Attributes with space-separated values (e.g. state="hover active") have
		*        one entry per token starting at index 0.
		*        Returns nullopt when the index is out of range.
		* @param index Zero-based index into the value list.
		* @return The value at that index, or nullopt if the index is out of range.
		*/
		std::optional<XMLValue> GetValue(size_t index) const;

		/*
		* @brief Returns all values of this attribute.
		* @return Reference to the internal value list.
		*/
		const std::vector<XMLValue>& GetValues() const;

		/*
		* @brief Returns how many values this attribute holds.
		*        Single-value attributes return 1.
		* @return Number of values.
		*/
		size_t GetValueCount() const;

	private:
		std::string_view m_name;
		std::vector<XMLValue> m_values;
	};

	/*
	* @brief A single parsed XML element with its name, attributes, inner text and children.
	*        Elements are owned by the XMLDocument tree. Do not store raw pointers to
	*        XMLElement outside the lifetime of the owning XMLDocument.
	*        All string_view data (name, inner text, attribute names and string values) is
	*        non-owning and only valid as long as the owning XMLDocument is alive.
	*/
	class XMLElement {
		friend class XMLParser;
		friend class XMLDocument;
	public:
		XMLElement() = default;

		/*
		* @brief Returns true when this element has inner text content.
		*        Inner text is the raw text between the opening and closing tag,
		*        e.g. <Label>Hello</Label> has inner text "Hello".
		*        Returns false when the element has no text or only child elements.
		* @return True if inner text is present.
		*/
		bool HasInnerText() const;

		/*
		* @brief Returns the tag name as a non-owning view into the source text.
		* @return The element name.
		*/
		std::string_view GetName() const;

		/*
		* @brief Returns the inner text as a non-owning view into the source text.
		*        Check HasInnerText() first. Returns an empty view when no inner text exists.
		* @return The inner text, or an empty view if none is present.
		*/
		std::string_view GetInnerText() const;

		/*
		* @brief Finds the first attribute with the given name.
		*        The search is case-sensitive.
		*        Returns nullopt when no attribute with that name exists.
		* @param name The attribute name to search for.
		* @return The matching attribute, or nullopt if not found.
		*/
		std::optional<XMLAttribute> GetAttribute(std::string_view name) const;

		/*
		* @brief Returns all attributes of this element.
		* @return Reference to the internal attribute list.
		*/
		const std::vector<XMLAttribute>& GetAttributes() const;

		/*
		* @brief Returns a pointer to the parent element.
		*        Returns nullptr for the root element.
		* @return Pointer to the parent, or nullptr if this is the root.
		*/
		XMLElement* GetParent() const;

		/*
		* @brief Returns all direct child elements.
		* @return Reference to the internal child list.
		*/
		const std::vector<XMLElement>& GetChildren() const;

		/*
		* @brief Shortcut to read a typed value from a named attribute in one call.
		*        Combines GetAttribute, GetValue and XMLValue::GetValue<T>.
		*        Returns nullopt when the attribute does not exist, the index is out of
		*        range, or the stored type cannot be converted to T.
		* @param name  The attribute name to look up.
		* @param index Zero-based index into the attribute value list. Defaults to 0.
		* @return The value as T, or nullopt if any step fails.
		*/
		template<typename T>
		std::optional<T> GetAttributeValue(std::string_view name, size_t index = 0) const {
			auto attr = GetAttribute(name);
			if (!attr)
				return std::nullopt;

			auto val = attr->GetValue(index);
			if (!val)
				return std::nullopt;

			return val->GetValue<T>();
		}

	private:
		std::string_view m_name;
		std::string_view m_innerText;
		std::vector<XMLAttribute> m_attributes;
		XMLElement* m_parent = nullptr;
		std::vector<XMLElement> m_children;
	};

	/*
	* @brief Entry point for loading and accessing a parsed XML file.
	*        Owns the source text and the entire element tree. All string_view data
	*        inside XMLElement, XMLAttribute and XMLValue instances points directly
	*        into the internal source buffer of this document. This means the
	*        XMLDocument must outlive any data read from it. Accessing attribute names,
	*        element names, inner text or string values after the document is destroyed
	*        is undefined behaviour.
	*        XMLDocument is non-copyable and non-movable for this reason.
	*/
	class XMLDocument {
	public:
		XMLDocument() = default;

		XMLDocument(const XMLDocument&) = delete;
		XMLDocument(XMLDocument&&) = delete;
		XMLDocument& operator=(const XMLDocument&) = delete;
		XMLDocument& operator=(XMLDocument&&) = delete;

		/*
		* @brief Loads and parses an XML file from disk.
		*        Replaces any previously loaded content.
		*        On failure the error can be retrieved with GetError or TryGetError.
		* @param path Path to the XML file.
		* @return True if the file was loaded and parsed without errors.
		*/
		bool LoadFile(const XMLFilePath& path);

		/*
		* @brief Parses XML from an in-memory string.
		*        Replaces any previously loaded content.
		*        On failure the error can be retrieved with GetError or TryGetError.
		* @param fileContent The raw XML text to parse.
		* @return True if the string was parsed without errors.
		*/
		bool LoadString(const std::string& fileContent);

		/*
		* @brief Returns true when the document was successfully loaded and parsed.
		* @return True if the document is ready to use.
		*/
		bool IsLoaded() const;

		/*
		* @brief Returns true when any error occurred during loading or parsing.
		* @return True if an error is present.
		*/
		bool HasError() const;

		/*
		* @brief Writes the error message into outErr and returns true when an error exists.
		*        Leaves outErr unchanged and returns false when there is no error.
		* @param outErr Receives the error message if one is present.
		* @return True if an error was written into outErr.
		*/
		bool TryGetError(std::string& outErr) const;

		/*
		* @brief Returns the XML version declared in the processing instruction.
		*        Returns 0.0 when no version was found.
		* @return The version number, e.g. 1.0.
		*/
		float GetVersion() const;

		/*
		* @brief Returns a pointer to the root element of the parsed tree.
		*        Returns nullptr when the document is not loaded or has no root element.
		*        The pointer is valid only as long as this XMLDocument is alive.
		* @return Pointer to the root element, or nullptr if none exists.
		*/
		const XMLElement* GetRootElement() const;

		/*
		* @brief Returns the full accumulated error message.
		*        Returns an empty string when no error occurred.
		* @return The error message.
		*/
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
		void AddError(const std::string& msg);
	};

}