#pragma once
#include <CoreLib/Log.h>
#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector4.h>

enum class Direction {
	UP = 0,
	DOWN,
	LEFT,
	RIGHT
};

class Pong {
public:
	enum class CollisionType {
		NONE = 0,
		PADDLE_LEFT,
		PADDLE_RIGHT,
		TOP,
		BOTTOM,
		LEFT,
		RIGHT
	};

	Pong() = default;

	/*
	* @brief Starts pong
	*/
	void Start(int playFieldWidth, int playFieldHeight);

	/*
	* @brief Updates pong
	*/
	void Update(float deltaTime);

	/**
	* @brief Sets the movement direction for a specified paddle.
	*
	* Updates the internal movement direction of the given paddle based on the
	* provided vertical direction `dir`. If the paddle or direction is invalid,
	* no change is made.
	*
	* @param paddle The paddle to update (LEFT or RIGHT).
	* @param dir The desired movement direction (UP or DOWN).
	* @return Reference to the current Pong object to allow method chaining.
	*/
	Pong& SetPaddleMoveDir(Direction paddle, Direction direction);
	Pong& SetPaddleSpeed(float speed);
	Pong& SetBallSpeed(float speed);

	Vector2 GetPlayField() const;
	float GetPaddleSpeed() const;
	Vector2 GetPaddleSize() const;
	float GetBallSize() const;
	Vector2 GetBallPose() const;

	bool TryGetScore(int& outScore, Direction paddle) const;
	/**
	* @brief Tries to get the current position of a specified paddle.
	*
	* Retrieves the position of the paddle indicated by `paddle`. If the paddle
	* direction is invalid, returns false and sets `outPos` to a default Vector2.
	*
	* @param[out] outPos Reference to a Vector2 that will receive the paddle position.
	* @param paddle The paddle whose position should be retrieved (LEFT or RIGHT).
	* @return true if the paddle exists and the position was retrieved successfully, false otherwise.
	*/
	bool TryGetPaddlePos(Vector2& outPos, Direction paddle) const;

private:

	Vector2 m_playField;
	CollisionType m_lastCollisionType = CollisionType::NONE;
	
	int m_scorePaddleLeft = 0;
	int m_scorePaddleRight = 0;
	Vector2 m_paddleLeftPos;
	int m_paddleLeftDir = 0;
	Vector2 m_paddleRightPos;
	int m_paddleRightDir = 0;

	Vector2 m_ballPos;
	Vector2 m_ballDir;

	float m_paddleSpeed = 500;
	float m_ballSpeed = 400;
	Vector2 m_paddleSize;
	float m_ballSize = 20;

	void SpawnBall();
	void MovePaddles(float deltaTime);
	void CollisionInteraction(CollisionType type);

	bool IsBallColliding(CollisionType& outType);
	bool IsCollidingRect(Vector4 Rect, Vector4 otherRect);
	bool TryGetHorizontalDir(int& outDir, Direction dir);
	bool TryGetVerticalDir(int& outDir, Direction dir);
};

template<>
static inline std::string FormatUtils::toString<Direction>(Direction dir) {
	switch (dir)
	{
	case Direction::UP:		return "UP";
	case Direction::DOWN:	return "DOWN";
	case Direction::LEFT:	return "LEFT";
	case Direction::RIGHT:	return "RIGHT";
	default:				return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<Pong::CollisionType>(Pong::CollisionType type) {
	switch (type)
	{
	case Pong::CollisionType::NONE:         return "NONE";
	case Pong::CollisionType::PADDLE_LEFT:  return "PADDLE_LEFT";
	case Pong::CollisionType::PADDLE_RIGHT: return "PADDLE_RIGHT";
	case Pong::CollisionType::TOP:          return "TOP";
	case Pong::CollisionType::BOTTOM:       return "BOTTOM";
	case Pong::CollisionType::LEFT:         return "LEFT";
	case Pong::CollisionType::RIGHT:        return "RIGHT";
	default:                                return "UNKNOWN";
	}
}