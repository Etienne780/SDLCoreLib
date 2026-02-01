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
	m_guessedWords.clear();
}

void Wordle::StopGame() {
	m_isGameStart = false;
	m_wordManualSet = false;
}

GuessResult Wordle::GuessWord(const std::string & word) {
	GuessResult result = IsValidWord(word);
	if (result != GuessResult::VALID) {
		return result;
	}

	if (m_currentGuessCount >= m_maxGuessTrys)
		return GuessResult::MAX_GUESSES_REACHED;

	m_guessedWords.emplace_back(word, m_currentWord);
	m_currentGuessCount++;
	return GuessResult::VALID;
}

bool Wordle::IsGameEnd(bool& outWon) {
	outWon = false;
	
	for (const WordData& words : m_guessedWords) {
		if (words.GetWord() == m_currentWord) {
			outWon = true;
			return true;
		}
	}

	if (m_currentGuessCount >= m_maxGuessTrys) {
		outWon = false;
		return true;
	}

	return false;
}

Wordle& Wordle::SetWord(const std::string& word) {
	if (m_isGameStart) {
		Log::Warn("Wordle::SetWord: Faild to set word, game is already running!");
		return *this;
	}

	m_currentWord = word;
	m_wordManualSet = true;
	return *this;
}

Wordle& Wordle::SetMaxTrys(size_t trys) {
	if (m_isGameStart) {
		Log::Warn("Wordle::SetMaxTrys: Faild to set max trys, game is already running!");
		return *this;
	}

	m_maxGuessTrys = trys;
	return *this;
}

GuessResult Wordle::IsValidWord(const std::string& word) {
	if (word.size() != 5)
		return GuessResult::INVALID_LENGTH;

	for (char c : word) {
		if (!std::isalpha(static_cast<unsigned char>(c)))
			return GuessResult::INVALID_CHARS;
	}

	if (WORD_LIST_SET.find(word) != WORD_LIST_SET.end())
		return GuessResult::NOT_IN_WORD_LIST;

	return GuessResult::VALID;
}

std::string Wordle::GetRandomWord() {
	size_t randomIndex = Random::GetRangeNumber<size_t>(0, WORD_LIST_COUNT);
	return WORD_LIST[randomIndex];
}