#pragma once

class NoMove
{
public:
	NoMove() = default;
	NoMove(NoMove&&) = delete;

public:
	NoMove& operator = (NoMove&&) = delete;
};

class NoCopy
{
public:
	NoCopy() = default;
	NoCopy(const NoCopy&) = delete;

public:
	NoCopy& operator = (const NoCopy&) = delete;
};

template< typename T >
class Singleton
	: public NoCopy, NoMove
{
public:
	Singleton() = default;

public:
	static T& instance()
	{
		static T object = { };
		return object;
	}
};
