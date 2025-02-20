#ifndef UCIOPTION_H
#define UCIOPTION_H

#include <string>

class UciOption {

public:

    // Constructors
	UciOption() : myDefaultValue(""), myCurrentValue(""), myType(""), myMin(0), myMax(0)
    {
    };

	UciOption(std::string defaultValue, std::string type) :
		myDefaultValue(defaultValue),
		myCurrentValue(defaultValue),
		myType(type),
		myMin(0), myMax(0)
	{
	};

	UciOption(std::string defaultValue, std::string type, int min, int max) :
		myDefaultValue(defaultValue),
		myCurrentValue(defaultValue),
		myType(type),
		myMin(min), myMax(max)
	{
	};

    // Overloading
	UciOption& operator=(const std::string& v )
	{
		//Blindly update for now
		myCurrentValue = v;

		return *this;
	}

	UciOption(const std::string v) : myType("string"), myMin(0), myMax(0)
	{ myDefaultValue = myCurrentValue = v; }

	const std::string& getCurrentValue() const
	{
		return myCurrentValue;
	}

	void setCurrentValue(const std::string& currentValue)
	{
		myCurrentValue = currentValue;
	}

	const std::string& getDefaultValue() const
	{
		return myDefaultValue;
	}

	void setDefaultValue(const std::string& defaultValue)
	{
		myDefaultValue = defaultValue;
	}

	const std::string& getType() const
	{
		return myType;
	}

	void setType(const std::string& type)
	{
		myType = type;
	}

	int getMax() const
	{
		return myMax;
	}

	void setMax(int max)
	{
		myMax = max;
	}

	int getMin() const
	{
		return myMin;
	}

	void setMin(int min)
	{
		myMin = min;
	}

private:
	std::string myDefaultValue, myCurrentValue, myType;
	int myMin, myMax;


	//size_t idx;
};

inline std::ostream& operator<<(std::ostream &strm, const UciOption &UciOption) {

	strm << UciOption.getCurrentValue();

	return strm;
}

#endif