#include <string>
#include "biginteger.h"

BigInteger::BigInteger (std::string str) : sign_(1), size_(1), data_(new unsigned int[1]{0})
{
	size_t begin = 0;

	if (isdigit (str[0]) == 0)
	{
		if (str[0] == '-' && str[1] != '\0')
		{
			sign_ = -1;
			begin = 1;
		} else if (str[0] == '+' && str[1] != '\0')
		{
			begin = 1;
		} else
		{
			delete[] data_;
			throw (std::invalid_argument (str));
		}
	}

	while (str[begin] == '0')
	{
		++begin;
	}

	size_t str_size = str.size ();

	unsigned char rem = 0;
	bool is_zero = false;
	unsigned int deg2 = 1;

	/*
		переводим строковое число из десятичной системы в двоичную путем деления на 2
		и в зависимости от остатков прибавляем степени двойки к разряду.
	*/

	while (!is_zero)
	{
		is_zero = true;
		for (size_t i = begin; i < str_size; ++i)
		{
			if (isdigit (str[i]) == 0)
			{
				delete[] data_;
				throw (std::invalid_argument (str));
			}

			unsigned char tmp = str[i] - '0' + rem;

			str[i] = (tmp >> 1) + '0';
			rem = ((tmp & 1) == 1) ? 10 : 0;

			if (is_zero && str[i] != '0')
			{
				begin = i;
				is_zero = false;
			}
		}

		if (rem > 0)
		{
			data_[size_ - 1] += deg2;
		}
		deg2 <<= 1;
		rem = 0;

		// если степень двойки превысила 31, то добавляем новый разряд числа.
		if (deg2 == 0)
		{
			deg2 = 1;
			++size_;
			unsigned int* tmp = new unsigned int[size_];
			for (size_t i = 0; i < size_ - 1; ++i)
			{
				tmp[i] = data_[i];
			}
			tmp[size_ - 1] = 0;

			delete[] data_;
			data_ = tmp;
		}
	}

	normalize();
}

BigInteger::BigInteger (unsigned short num) : BigInteger (static_cast<unsigned long long>(num)) {}
BigInteger::BigInteger (signed short num) : BigInteger (static_cast<signed long long>(num)){}
BigInteger::BigInteger (unsigned int num) : BigInteger (static_cast<unsigned long long>(num)){}
BigInteger::BigInteger (signed int num) : BigInteger (static_cast<signed long long>(num)) {}
BigInteger::BigInteger (unsigned long num) : BigInteger (static_cast<unsigned long long>(num)) {}
BigInteger::BigInteger (signed long num) : BigInteger (static_cast<signed long long>(num)) {}
BigInteger::BigInteger (unsigned long long num) : sign_ (1), size_(2)
{
	data_ = new unsigned int[2];
	data_[0] = static_cast<unsigned int>(num);
	data_[1] = static_cast<unsigned int>(num >> 32);

	if (data_[1] == 0)
	{
		--size_;
	}
}
BigInteger::BigInteger (signed long long num) : sign_ (1), size_ (2)
{
	data_ = new unsigned int[2];
	if (num < 0)
	{
		sign_ = -1;
		num = ~num;
		unsigned long long tmp = static_cast<unsigned long long>(num);
		++tmp;
		data_[0] = static_cast<unsigned int>(tmp);
		data_[1] = static_cast<unsigned int>(tmp >> 32);
	}
	else
	{
		data_[0] = static_cast<unsigned int>(num);
		data_[1] = static_cast<unsigned int>(num >> 32);
	}

	if (data_[1] == 0)
	{
		--size_;
	}
}

bool BigInteger::is_equal (const BigInteger& other) const
{
	bool answer = (sign_ == other.sign_) && (size_ == other.size_);

	if (answer)
	{
		for (size_t i = 0; i < size_; ++i)
		{
			if (data_[i] != other.data_[i])
			{
				answer = false;
				break;
			}
		}
	}

	return answer;
}
bool BigInteger::is_less (const BigInteger& other) const
{
	if (sign_ < other.sign_)
	{
		return true;
	}
	if (other.sign_ < sign_)
	{
		return false;
	}

	if (sign_ < 0)
	{
		return (-other).is_less(-*this);
	}

	if (size_ < other.size_)
	{
		return true;
	}
	if (other.size_ < size_)
	{
		return false;
	}

	for (int i = size_ - 1; i >= 0; --i)
	{
		if (data_[i] < other.data_[i])
		{
			return true;
		}
		if (other.data_[i] < data_[i])
		{
			return false;
		}
	}

	return false;
}

BigInteger& BigInteger::minus_min_abs (const BigInteger& other)
{
	signed long long diff = 0LL;
	unsigned int rem = 0;

	size_t i = 0;
	for (; i < other.size_; ++i)
	{
		diff = static_cast<signed long long>(data_[i]) - other.data_[i] - rem;

		data_[i] = static_cast<unsigned int>(diff);
		rem = (diff < 0) ? 1 : 0;
	}

	for (; i < size_; ++i)
	{
		diff = static_cast<signed long long>(data_[i]) - rem;

		data_[i] = static_cast<unsigned int>(diff);
		rem = (diff < 0) ? 1 : 0;
	}

	// считаем новый размер, убирая незначащие нули.
	while (size_ > 1 && data_[size_ - 1] == 0)
	{
		--size_;
	}

	return *this;
}
BigInteger& BigInteger::plus_min_abs (const BigInteger& other)
{
	unsigned int* tmp = new unsigned int[size_ + 1];
	unsigned long long sum = 0LL;
	unsigned int rem = 0;

	size_t i = 0;
	for (; i < other.size_; ++i)
	{
		sum = static_cast<unsigned long long>(data_[i]) + other.data_[i] + rem;

		tmp[i] = static_cast<unsigned int>(sum);
		rem = static_cast<unsigned int>(sum >> 32);
	}

	for (; i < size_; ++i)
	{
		sum = static_cast<unsigned long long>(data_[i]) + rem;

		tmp[i] = static_cast<unsigned int>(sum);
		rem = static_cast<unsigned int>(sum >> 32);
	}

	if (rem > 0)
	{
		tmp[size_++] = rem;
	}

	delete[] data_;
	data_ = tmp;

	return *this;
}

BigInteger& BigInteger::long_div_min_abs (const BigInteger& other, bool is_quot)
{
	const unsigned long long base = 0x100000000; // 2^32.

	BigInteger all_div (*this);
	BigInteger divisor (other);

	// нормализуем оба числа.
	unsigned long long coef = base / (divisor.data_[other.size_ - 1] + 1ULL);
	all_div *= coef;
	++all_div.size_;
	divisor *= coef;
	++divisor.size_;

	// выделяем память под частное.
	size_t diff = size_ - other.size_;
	size_t last_size = other.size_ - 1;
	size_ = diff + 1;
	delete[] data_;
	data_ = new unsigned int[size_];

	unsigned long long num = 0;
	unsigned long long quot = 0;
	unsigned long long rem = 0;
	for (int j = diff; j >= 0; --j)
	{
		// выполняем делением путем подбора частного и проверки.
		num = all_div.data_[j + other.size_] * base + all_div.data_[j + last_size];

		quot = num / divisor.data_[last_size];
		rem = num % divisor.data_[last_size];

		if (quot == base || (other.size_ > 1 && (quot * divisor.data_[last_size - 1]) > (base * rem + all_div.data_[j + last_size - 1])))
		{
			--quot;
			rem += divisor.data_[last_size];
		}

		signed long long diff = 0LL;
		unsigned long long prod = 0LL;
		unsigned int diff_rem = 0;

		for (size_t i = 0; i <= other.size_; ++i)
		{
			prod = quot * divisor.data_[i];
			diff = static_cast<unsigned int>(prod);

			diff = all_div.data_[i + j] - diff;
			diff -= diff_rem;

			all_div.data_[i + j] = static_cast<unsigned int>(diff);
			diff_rem = static_cast<unsigned int>(prod >> 32);
			diff_rem += (diff < 0) ? 1 : 0;
		}

		data_[j] = static_cast<unsigned int>(quot);
	}

	if (!is_quot)
	{ 
		*this = std::move(BigInteger(rem /= coef));
	}

	while (size_ > 1 && data_[size_ - 1] == 0)
	{
		--size_;
	}

	return *this;
}

BigInteger& BigInteger::operator+= (const BigInteger& other)
{
	if (sign_ == other.sign_)
	{
		if (size_ < other.size_)
		{
			BigInteger tmp(other);
			*this = std::move(tmp.plus_min_abs(*this));
		}
		else
		{
			plus_min_abs(other);
		}
	}
	else
	{
		BigInteger this_abs = std::move(abs());
		BigInteger other_abs = std::move(other.abs());
		
		if (this_abs < other_abs)
		{
			other_abs.sign_ = other.sign_;
			*this = std::move(other_abs.minus_min_abs(*this));
		}
		else
		{
			minus_min_abs (other);
		}

		normalize ();
	}

	return *this;
}

BigInteger& BigInteger::operator*= (const BigInteger& other)
{
	sign_ = (sign_ != other.sign_) ? -1 : 1;
	
	size_t new_size = size_ + other.size_;
	unsigned int* tmp = new unsigned int[new_size];
	unsigned long long prod = 0LL;
	unsigned int rem = 0;

	for (size_t i = 0; i < new_size; ++i)
	{
		tmp[i] = 0;
	}

	for (size_t j = 0; j < other.size_; ++j)
	{
		if (other.data_[j] == 0)
		{
			tmp[j + size_] = 0;
			continue;
		}

		rem = 0;
		for (size_t i = 0; i < size_; ++i)
		{
			prod = static_cast<unsigned long long>(data_[i]) * other.data_[j] + tmp[i + j] + rem;

			tmp[i + j] = static_cast<unsigned int>(prod);
			rem = static_cast<unsigned int>(prod >> 32);
		}

		tmp[j + size_] = rem;
	}

	size_ = new_size;
	delete[] data_;
	data_ = tmp;

	while (size_ > 1 && data_[size_ - 1] == 0)
	{
		--size_;
	}

	normalize();

	return *this;
}
BigInteger& BigInteger::operator/= (const BigInteger& other)
{
	signed char sign = (sign_ != other.sign_) ? -1 : 1;
	BigInteger tmp = std::move(other.abs());
	sign_ = 1;

	if (other.size_ == 1 && other.data_[0] == 0)
	{
		throw "error";
	}

	if (*this < tmp)
	{
		*this = std::move(BigInteger());
	}
	else if (*this == tmp)
	{
		*this = std::move(BigInteger(1));
	}
	else
	{
		long_div_min_abs (tmp);
	}

	sign_ = sign;
	normalize();

	return *this;
}
BigInteger& BigInteger::operator%= (const BigInteger& other)
{
	signed char sign = sign_;
	BigInteger tmp = std::move (other.abs ());
	sign_ = 1;

	if (other.size_ == 1 && other.data_[0] == 0)
	{
		throw "error";
	}

	if (*this == tmp)
	{
		*this = std::move (BigInteger ());
	} else
	{
		long_div_min_abs (tmp, false);
	}

	sign_ = sign;
	normalize();

	return *this;
}

bool operator==(const BigInteger& left, const BigInteger& right)
{
	return left.is_equal (right);
}
bool operator!=(const BigInteger& left, const BigInteger& right)
{
	return !(left == right);
}
bool operator<(const BigInteger& left, const BigInteger& right)
{
	return left.is_less (right);
}
bool operator<=(const BigInteger& left, const BigInteger& right)
{
	return (left < right) || (left == right);
}
bool operator>(const BigInteger& left, const BigInteger& right)
{
	return right < left;
}
bool operator>=(const BigInteger& left, const BigInteger& right)
{
	return !(left < right);
}

BigInteger operator+ (const BigInteger& left, const BigInteger& right)
{
	BigInteger tmp (left);
	tmp += right;

	return tmp;
}
BigInteger operator- (const BigInteger& left, const BigInteger& right)
{
	BigInteger tmp (left);
	tmp += -right;

	return tmp;
}
BigInteger operator* (const BigInteger& left, const BigInteger& right)
{
	BigInteger tmp (left);
	tmp *= right;

	return tmp;
}
BigInteger operator/ (const BigInteger& left, const BigInteger& right)
{
	BigInteger tmp (left);
	tmp /= right;

	return tmp;
}
BigInteger operator% (const BigInteger& left, const BigInteger& right)
{
	BigInteger tmp (left);
	tmp %= right;

	return tmp;
}

std::string to_string (const BigInteger& A)
{
	std::string result = "0";
	unsigned int num = 0;
	unsigned int rem = 0;
	unsigned int prod = 0;
	unsigned int sum = 0;

	for (int i = static_cast<int>(A.size_) - 1; i >= 0; --i)
	{
		num = 0x80000000; // 2^31.
		while (num != 0)
		{
			// умножаем строковое число на 2.
			rem = 0;
			for (int j = static_cast<int>(result.size ()) - 1; j >= 0; --j)
			{
				prod = ((result[j] - '0') << 1) + rem;
				result[j] = (prod % 10) + '0';
				rem = prod / 10;
			}

			if (rem > 0)
			{
				result = std::to_string (rem) + result;
			}

			// прибавляем 1.
			if ((A.data_[i] & num) == num)
			{
				rem = 1;
				for (int j = static_cast<int>(result.size ()) - 1; j >= 0 && rem > 0; --j)
				{
					sum = (result[j] - '0') + rem;
					result[j] = (sum % 10) + '0';
					rem = sum / 10;
				}

				if (rem > 0)
				{
					result = std::to_string (rem) + result;
				}
			}

			num >>= 1;
		}
	}

	if (A.sign_ < 0 && result != "0")
	{
		result = "-" + result;
	}

	return result;
}