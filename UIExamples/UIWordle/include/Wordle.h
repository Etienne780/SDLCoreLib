#pragma once
#include <string>
#include <vector>
#include <CoreLib/FormatUtils.h>

enum class Wordle_LetterStatus {
	UNKNOWN,
	PRESENT,	// Letter is in word
	CORRECT,    // Letter is in correct position
	ABSENT      // Letter is not in word
};

/**
* @brief Result codes for a word guess in Wordle.
*
* Each value indicates the outcome of a guess attempt.
*/
enum class Wordle_Result {
	VALID = 0,          /**< The guess was valid and recorded */
	INVALID_LENGTH = 1,  /**< The word length is not 5 */
	INVALID_CHARS = 2,   /**< The word contains non-alphabetic characters */
	NOT_IN_WORD_LIST = 3,  /**< The word is not in the allowed word list */
	MAX_GUESSES_REACHED = 4 /**< The maximum number of guesses has been reached */
};

class WordData {
public:
	/**
	* @brief Constructs a WordData object for a given word.
	*
	* Initializes the status list with UNKNOWN for each letter.
	*
	* @param w The word to store.
	*/
	WordData(const std::string& w);

	/**
	* @brief Constructs a WordData object and calculates letter statuses immediately.
	*
	* Calls CalculateWordData on construction.
	*
	* @param w The word to store.
	* @param RefWord The reference word to compare against.
	*/
	WordData(const std::string& w, const std::string& RefWord);
	
	/**
	* @brief Calculates letter statuses comparing this word with a reference word.
	*
	* Fills m_statusList according to Wordle rules:
	* 
	* - CORRECT: Letter is at the correct position
	* 
	* - PRESENT: Letter exists but at a different position
	* 
	* - ABSENT: Letter does not exist in the reference word
	*
	* @param RefWord The word to compare against.
	*/
	void CalculateWordData(const std::string& RefWord);

	std::string GetWord();
	const std::string& GetWord() const;
	std::vector<Wordle_LetterStatus> GetWordStatus();
	const std::vector<Wordle_LetterStatus>& GetWordStatus() const;

private:
	std::string m_word;
	std::vector<Wordle_LetterStatus> m_statusList;
};

class Wordle {
public:
	Wordle();

	void StartGame();
	void StopGame();

	/**
	* @brief Attempts to guess a word in the current Wordle game.
	*
	* This function performs validation using IsValidWord(), checks whether
	* the maximum number of guesses has been reached, and records the guess
	* if valid.
	*
	* note: Use IsGameEnd() to check if the game has ended after a valid guess.
	* 
	* @param word The word to guess.
	* @return A GuessResult value indicating the outcome of the guess
	*/
	Wordle_Result GuessWord(const std::string& word);

	bool IsGameEnd(bool& outWon);

	Wordle& SetWord(const std::string& word);
	Wordle& SetMaxAttampts(size_t attampts);

	bool IsGameStart() const;
	const std::string& GetCurrentWord() const;
	size_t GetMaxGuessAttempts() const;
	size_t GetCurrentGuessAttempt() const;
	const std::vector<WordData>& GetCurrentWordData() const;

private:
	bool m_isGameStart = false;
	bool m_wordManualSet = false;

	std::string m_currentWord;
	size_t m_maxGuessAttempts = 0;
	size_t m_currentGuessAttampt = 0;

	std::vector<WordData> m_guessedWordData;

	/**
	* @brief Checks whether a given word is valid for the Wordle game.
	*
	* Performs the following checks:
	* 
	* 1. Word length must be exactly 5 characters.
	* 
	* 2. All characters must be alphabetic (A-Z or a-z).
	* 
	* 3. Word must exist in the allowed word list (WORD_LIST_SET).
	*
	* @param word The word to validate.
	* @return A GuessResult value indicating the validation result
	*/
	Wordle_Result IsValidWord(const std::string& word);
	std::string GetRandomWord();
};

template<>
static inline std::string FormatUtils::toString<Wordle_LetterStatus>(Wordle_LetterStatus status) {
	switch (status) {
	case Wordle_LetterStatus::PRESENT:	return "Present";
	case Wordle_LetterStatus::CORRECT:	return "Correct";
	case Wordle_LetterStatus::ABSENT:	return "Absent";
	default:
	case Wordle_LetterStatus::UNKNOWN:	return "UKNOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<Wordle_Result>(Wordle_Result result) {
	switch (result) {
	case Wordle_Result::VALID:					return "Valid";
	case Wordle_Result::INVALID_LENGTH:		return "InvalidLength";
	case Wordle_Result::INVALID_CHARS:			return "InvalidChars";
	case Wordle_Result::NOT_IN_WORD_LIST:		return "NotInWordList";
	case Wordle_Result::MAX_GUESSES_REACHED:	return "MaxGuessesReached";
	default:										return "UKNOWN";
	}
}