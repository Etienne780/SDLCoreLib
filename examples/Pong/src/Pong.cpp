#include <CoreLib/Math/MathUtil.h>
#include "Pong.h"

void Pong::Start(int w, int h) {
	m_scorePaddleLeft = 0;
	m_scorePaddleRight = 0;

	float fw = static_cast<float>(w);
	float fh = static_cast<float>(h);
	float halfw = fw / 2;
	float halfh = fh / 2;

	m_playField.Set(fw, fh);

	float padding = 25;
	m_paddleSize.Set(40, fh / 3);
	m_paddleLeftPos.Set(padding, halfh - m_paddleSize.y / 2);
	m_paddleRightPos.Set(fw - m_paddleSize.x - padding, halfh - m_paddleSize.y / 2);

	m_ballSize = 20;
	SpawnBall();
}

void Pong::Update(float dt) {
	MovePaddles(dt);

	// move ball
	m_ballPos += m_ballDir * m_ballSpeed * dt;
	CollisionType type = CollisionType::NONE;
	if (IsBallColliding(type)) {
		if (m_lastCollisionType != type) {
			m_lastCollisionType = type;
			CollisionInteraction(type);
		}
	}
}

Pong& Pong::SetPaddleMoveDir(Direction paddle, Direction dir) {
	int* paddleDir = nullptr;
	switch (paddle)
	{
	case Direction::LEFT:
		paddleDir = &m_paddleLeftDir;
		break;
	case Direction::RIGHT:
		paddleDir = &m_paddleRightDir;
		break;
	default:
		break;
	}

	int moveDir = 0;
	if (paddleDir && TryGetVerticalDir(moveDir, dir)) {
		(*paddleDir) = moveDir;
	}

	return *this;
}

Pong& Pong::SetPaddleSpeed(float speed) {
	m_paddleSpeed = speed;
	return *this;
}

Pong& Pong::SetBallSpeed(float speed) {
	m_ballSpeed = speed;
	return *this;
}

Vector2 Pong::GetPlayField() const {
	return m_playField;
}

float Pong::GetPaddleSpeed() const {
	return m_paddleSpeed;
}

Vector2 Pong::GetPaddleSize() const {
	return m_paddleSize;
}

float Pong::GetBallSize() const {
	return m_ballSize;
}

Vector2 Pong::GetBallPose() const {
	return m_ballPos;
}

bool Pong::TryGetScore(int& outScore, Direction paddle) const {
	bool result = true;
	switch (paddle)
	{
	case Direction::LEFT:
		outScore = m_scorePaddleLeft;
		break;
	case Direction::RIGHT:
		outScore = m_scorePaddleRight;
		break;
	default:
		Log::Warn("Pong::TryGetScore: The given paddle '{}' is not available, suppert dirs: LEFT, RIGHT!", paddle);
		outScore = -1;
		result = false;
		break;
	}

	return result;
}

bool Pong::TryGetPaddlePos(Vector2& outPos, Direction paddle) const {
	bool result = true;
	switch (paddle)
	{
	case Direction::LEFT:	
		outPos =  m_paddleLeftPos;
		break;
	case Direction::RIGHT:	
		outPos =  m_paddleRightPos;
		break;
	default:
		Log::Warn("Pong::TryGetPaddlePos: The given paddle '{}' is not available, suppert dirs: LEFT, RIGHT!", paddle);
		outPos = Vector2();
		result = false;
		break;
	}

	return result;
}

void Pong::SpawnBall() {
	m_ballPos.Set(m_playField.x/2 - m_ballSize / 2, m_playField.y/2 - m_ballSize / 2);
	
	m_ballDir.x = 1.23928f;
	m_ballDir.y = -1.2864398f;
	m_ballDir.Normalize();
}

void Pong::MovePaddles(float dt) {
	float& leftY = m_paddleLeftPos.y;
	float& rightY = m_paddleRightPos.y;
	
	leftY += m_paddleLeftDir * -1 * m_paddleSpeed * dt;
	rightY += m_paddleRightDir * -1 * m_paddleSpeed * dt;

	float maxHeight = m_playField.y - m_paddleSize.y;
	MathUtil::Clamp(leftY, 0, maxHeight);
	MathUtil::Clamp(rightY, 0, maxHeight);

	m_paddleLeftDir = 0;
	m_paddleRightDir = 0;
}

void Pong::CollisionInteraction(CollisionType type) {
	if (type == CollisionType::LEFT) {
		m_scorePaddleRight++;
		SpawnBall();
	}
	else if (type == CollisionType::RIGHT) {
		m_scorePaddleLeft++;
		SpawnBall();
	}
	else if (type == CollisionType::PADDLE_LEFT ||
		type == CollisionType::PADDLE_RIGHT) {
		m_ballDir.x *= -1;
	}
	else if (type == CollisionType::TOP ||
		type == CollisionType::BOTTOM) {
		m_ballDir.y *= -1;
	}
}

bool Pong::IsBallColliding(CollisionType& outType) {
	Vector4 ballTrans(m_ballPos, m_ballSize, m_ballSize);

	// left paddle
	if (IsCollidingRect(ballTrans,
		Vector4(m_paddleLeftPos, m_paddleSize))) {
		outType = CollisionType::PADDLE_LEFT;
		return true;
	}

	// right paddle
	if (IsCollidingRect(ballTrans,
		Vector4(m_paddleRightPos, m_paddleSize))) {
		outType = CollisionType::PADDLE_RIGHT;
		return true;
	}

	// playField
	if (m_ballPos.x <= 0) {
		outType = CollisionType::LEFT;
		return true;
	}

	if (m_ballPos.x + m_ballSize >= m_playField.x) {
		outType = CollisionType::RIGHT;
		return true;
	}

	if (m_ballPos.y <= 0) {
		outType = CollisionType::TOP;
		return true;
	}

	if (m_ballPos.y + m_ballSize >= m_playField.y) {
		outType = CollisionType::BOTTOM;
		return true;
	}

	outType = CollisionType::NONE;
	return false;
}

bool Pong::IsCollidingRect(Vector4 a, Vector4 b) {
	return (a.x < b.x + b.z &&
			a.x + a.z > b.x &&
			a.y < b.y + b.w &&
			a.y + a.w > b.y);
}

bool Pong::TryGetHorizontalDir(int& outDir, Direction dir) {
	bool result = true;
	switch (dir)
	{
	case Direction::LEFT: 
		outDir = -1;
		break;
	case Direction::RIGHT: 
		outDir = 1;
		break;
	default:
		Log::Warn("Pong::TryGetHorizontalDir: The given dir '{}' is not horizontal", dir);
		outDir = 0;
		result = false;
	}

	return result;
}

bool Pong::TryGetVerticalDir(int& outDir, Direction dir) {
	bool result = true;
	switch (dir)
	{
	case Direction::DOWN:
		outDir = -1;
		break;
	case Direction::UP:
		outDir = 1;
		break;
	default:
		Log::Warn("Pong::TryGetVerticalDir: The given dir '{}' is not vertical", dir);
		outDir = 0;
		result = false;
	}

	return result;
}