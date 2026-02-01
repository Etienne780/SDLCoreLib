#include <CoreLib/Random.h>
#include <CoreLib/Log.h>

#include "Wordle.h"
#include "StaticWordList.h"

WordData::WordData(const std::string & w)
	: m_word(w) {
	m_statusList = std::vector<Wordle_LetterStatus>(w.size(), Wordle_LetterStatus::UNKNOWN);
}

WordData::WordData(const std::string& w, const std::string& RefWord)
	: WordData(w){
	CalculateWordData(RefWord);
}

void WordData::CalculateWordData(const std::string& RefWord) {
	auto& w = this->m_word;
	auto& slist = this->m_statusList;

	for (size_t i = 0; i < slist.size(); i++) {
		char currentChar = w[i];
		Wordle_LetterStatus currentStatus = Wordle_LetterStatus::UNKNOWN;

		for (size_t j = 0; j < RefWord.size(); j++) {
			char checkChar = RefWord[j];

			if (currentChar != checkChar)
				continue;

			if (i == j) {
				currentStatus = Wordle_LetterStatus::CORRECT;
				break;
			}

			currentStatus = Wordle_LetterStatus::PRESENT;
		}

		if (currentStatus == Wordle_LetterStatus::UNKNOWN) {
			currentStatus = Wordle_LetterStatus::ABSENT;
		}

		slist[i] = currentStatus;
	}
}

std::string WordData::GetWord() {
	return m_word;
}

const std::string& WordData::GetWord() const {
	return m_word;
}

std::vector<Wordle_LetterStatus> WordData::GetWordStatus() {
	return m_statusList;
}

const std::vector<Wordle_LetterStatus>& WordData::GetWordStatus() const {
	return m_statusList;
}

Wordle::Wordle() {
}

void Wordle::StartGame() {
	if (m_isGameStart) {
		Log::Warn("Wordle::StartGame: Faild to start game, game is already running!");
		return;
	}

	m_isGameStart = true;
	if (!m_wordManualSet)
		m_currentWord = GetRandomWord();
	m_currentGuessAttampt = 0;
	m_guessedWordData.clear();
}

void Wordle::StopGame() {
	m_isGameStart = false;
	m_wordManualSet = false;
}

Wordle_Result Wordle::GuessWord(const std::string & word) {
	Wordle_Result result = IsValidWord(word);
	if (result != Wordle_Result::VALID) {
		return result;
	}

	if (m_maxGuessAttempts != 0 && m_currentGuessAttampt >= m_maxGuessAttempts)
		return Wordle_Result::MAX_GUESSES_REACHED;

	m_guessedWordData.emplace_back(word, m_currentWord);
	m_currentGuessAttampt++;
	return Wordle_Result::VALID;
}

bool Wordle::IsGameEnd(bool& outWon) {
	outWon = false;
	
	for (const WordData& words : m_guessedWordData) {
		if (words.GetWord() == m_currentWord) {
			outWon = true;
			return true;
		}
	}

	if (m_maxGuessAttempts != 0 && m_currentGuessAttampt >= m_maxGuessAttempts) {
		outWon = false;
		return true;
	}

	return false;
}

Wordle& Wordle::SetWord(const std::string& word) {
	if (m_isGameStart) {
		Log::Error("Wordle::SetWord: Faild to set word, game is already running!");
		return *this;
	}
	Wordle_Result result = IsValidWord(word);
	if (result != Wordle_Result::VALID) {
		Log::Error("Wordle::SetWord: Faild to set word, {}!", result);
		return *this;
	}

	m_currentWord = word;
	m_wordManualSet = true;
	return *this;
}

Wordle& Wordle::SetMaxAttampts(size_t attampts) {
	if (m_isGameStart) {
		Log::Error("Wordle::SetMaxAttampts: Faild to set max attempts, game is already running!");
		return *this;
	}

	m_maxGuessAttempts = attampts;
	return *this;
}

bool Wordle::IsGameStart() const {
	return m_isGameStart;
}

const std::string& Wordle::GetCurrentWord() const {
	return m_currentWord;
}

size_t Wordle::GetMaxGuessAttempts() const {
	return m_maxGuessAttempts;
}

size_t Wordle::GetCurrentGuessAttempt() const {
	return m_currentGuessAttampt;
}

const std::vector<WordData>& Wordle::GetCurrentWordData() const {
	return m_guessedWordData;
}

Wordle_Result Wordle::IsValidWord(const std::string& word) {
	if (word.size() != 5)
		return Wordle_Result::INVALID_LENGTH;

	for (char c : word) {
		if (!std::isalpha(static_cast<unsigned char>(c)))
			return Wordle_Result::INVALID_CHARS;
	}

	if (WORD_LIST_SET.find(word) == WORD_LIST_SET.end())
		return Wordle_Result::NOT_IN_WORD_LIST;

	return Wordle_Result::VALID;
}

std::string Wordle::GetRandomWord() {
	size_t randomIndex = Random::GetRangeNumber<size_t>(0, WORD_LIST_COUNT - 1);
	return WORD_LIST[randomIndex];
}