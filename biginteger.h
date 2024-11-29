#include <string>
#include <iostream>

#ifndef BIGINTEGER_
#define BIGINTEGER_

class BigInteger
{
	size_t size_; // количество разрядов.
	unsigned int* data_; // массив цифр числа.
	signed char sign_; // знак.

	void swap (BigInteger& other)
	{
		std::swap(size_, other.size_);
		std::swap(data_, other.data_);
		std::swap(sign_, other.sign_);
	}

	void normalize ()
	{
		if (size_ == 1 && data_[0] == 0)
		{
			sign_ = 1;
		}
	}

public:
	BigInteger () : size_ (1), sign_ (1), data_ (new unsigned int[1] { 0 }) {}
	BigInteger (const BigInteger& other) : size_ (other.size_), sign_ (other.sign_), data_ (new unsigned int[size_])
	{
		for (size_t i = 0; i < size_; ++i)
		{
			data_[i] = other.data_[i];
		}
	}
	BigInteger (BigInteger&& other) noexcept : size_ (0), data_ (nullptr), sign_ (1)
	{
		swap (other);
	}
	BigInteger (std::string str);

	BigInteger (unsigned short num);
	BigInteger (signed short num);
	BigInteger (unsigned int num);
	BigInteger (signed int num);
	BigInteger (unsigned long num);
	BigInteger (signed long num);
	BigInteger (unsigned long long num);
	BigInteger (signed long long num);

	BigInteger& operator=(const BigInteger& other)
	{
		if (this != &other)
		{
			BigInteger tmp (other);
			swap (tmp);
		}

		return *this;
	}
	BigInteger& operator=(BigInteger&& other) noexcept
	{
		if (this != &other)
		{
			swap (other);
		}

		return *this;
	}
	BigInteger operator-() const
	{
		BigInteger result (*this);
		result.sign_ = -result.sign_;

		result.normalize ();

		return result;
	}
	BigInteger operator+() const
	{
		return *this;
	}

	bool is_equal (const BigInteger& other) const;
	bool is_less (const BigInteger& other) const;

	BigInteger& plus_min_abs (const BigInteger& other);
	BigInteger& minus_min_abs (const BigInteger& other);
	BigInteger abs () const
	{
		BigInteger tmp (*this);
		tmp.sign_ = 1;

		return tmp;
	}

	BigInteger& long_div_min_abs (const BigInteger& other, bool is_quot = true);

	BigInteger& operator+= (const BigInteger& other);
	BigInteger& operator-= (const BigInteger& other)
	{
		*this += -other;
		return *this;
	}
	BigInteger& operator*= (const BigInteger& other);
	BigInteger& operator/= (const BigInteger& other);
	BigInteger& operator%= (const BigInteger& other);

	BigInteger& operator++()
	{
		return (*this += BigInteger (1));
	}
	BigInteger operator++(int)
	{
		BigInteger tmp (*this);
		*this += BigInteger (1);

		return tmp;
	}
	BigInteger& operator--()
	{
		return (*this += BigInteger (-1));
	}
	BigInteger operator--(int)
	{
		BigInteger tmp (*this);
		*this += BigInteger (-1);

		return tmp;
	}

	~BigInteger ()
	{
		delete[] data_;
	}

	friend std::string to_string (const BigInteger& A);
};

bool operator==(const BigInteger& left, const BigInteger& right);
bool operator!=(const BigInteger& left, const BigInteger& right);
bool operator<(const BigInteger& left, const BigInteger& right);
bool operator<=(const BigInteger& left, const BigInteger& right);
bool operator>(const BigInteger& left, const BigInteger& right);
bool operator>=(const BigInteger& left, const BigInteger& right);

BigInteger operator+ (const BigInteger& left, const BigInteger& right);
BigInteger operator- (const BigInteger& left, const BigInteger& right);
BigInteger operator* (const BigInteger& left, const BigInteger& right);
BigInteger operator/ (const BigInteger& left, const BigInteger& right);
BigInteger operator% (const BigInteger& left, const BigInteger& right);

#endif // BIGINTEGER_
