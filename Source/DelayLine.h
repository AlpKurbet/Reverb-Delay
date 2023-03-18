/*
  ==============================================================================

    DelayLine.h
    Created: 18 Mar 2023 5:40:56pm
    Author:  Alperen Kurbetci

  ==============================================================================
*/

#pragma once
#include <vector>

template <typename Type>
class DelayLine
{
public:
    
    ///Needed functions for delay line
    
    //Clearing the data pointer
    void delayClear() noexcept
    {
        std::fill(rawData.begin(), rawData.end(), Type(0));
    }

    //Setting the size of the data pointer
    size_t size() const noexcept
    {
        return rawData.size();
    }

    //Re-size of the data pointer
    void delayResize(size_t newValue)
    {
        rawData.resize(newValue);
        leastRecentIndex = 0;
    }

    //Return the delay index
    Type delayBack() const noexcept
    {
        return rawData[leastRecentIndex];
    }
    
    //Return the delay index setting
    Type delayGet(size_t delayInSamples) const noexcept
    {
        jassert(delayInSamples >= 0 && delayInSamples < size());

        return rawData[(leastRecentIndex + 1 + delayInSamples) % size()];
    }

    /** Set the specified sample in the delay line */
    void delaySet(size_t delayInSamples, Type newValue) noexcept
    {
        jassert(delayInSamples >= 0 && delayInSamples < size());

        rawData[(leastRecentIndex + 1 + delayInSamples) % size()] = newValue;
    }

    /** Adds a new value to the delay line, overwriting the least recently added sample */
    void delayPush(Type valueToAdd) noexcept
    {
        rawData[leastRecentIndex] = valueToAdd;
        leastRecentIndex = leastRecentIndex == 0 ? size() - 1 : leastRecentIndex - 1;
    }
    
    
private:
    
    //The pointer for delay line
    std::vector<Type> rawData;
    
    //Delay sample index
    size_t leastRecentIndex = 0;
};
