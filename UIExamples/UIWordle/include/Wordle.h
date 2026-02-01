#pragma once
#include <string>
#include <vector>

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
enum class GuessResult {
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
	GuessResult GuessWord(const std::string& word);

	bool IsGameEnd(bool& outWon);

	Wordle& SetWord(const std::string& word);
	Wordle& SetMaxTrys(size_t trys);

private:
	bool m_isGameStart = false;
	bool m_wordManualSet = false;

	std::string m_currentWord;
	size_t m_maxGuessTrys = 0;
	size_t m_currentGuessCount = 0;

	std::vector<WordData> m_guessedWords;

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
	GuessResult IsValidWord(const std::string& word);
	std::string GetRandomWord();
};