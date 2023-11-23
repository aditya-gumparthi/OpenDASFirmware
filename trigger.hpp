#ifndef D330EBD1_5DF3_4C99_8C64_DF71A81CD623
#define D330EBD1_5DF3_4C99_8C64_DF71A81CD623
#include <iostream>
#include "ppBuf.hpp" // Include your ppBuf class

enum SignalType
{
    RISING,
    FALLING,
    EITHER_RISING_FALLING
};

template <typename T, const size_t N>
float getAverage(const ppBuf<T, N> &data, size_t start, size_t end)
{
    float sum = 0.0;
    for (size_t i = start; i < end; ++i)
    {
        sum += data.read_active_buf(i);
    }
    return sum / (end - start);
}

template <typename T, const size_t N>
int findTriggerIndex(const ppBuf<T, N> &data, float trigger_level, SignalType signal, size_t averaging = 5)
{
    for (size_t i = averaging; i < N; ++i)
    {
        float prevAvg = getAverage(data, i - averaging, i);
        float currAvg = getAverage(data, i - averaging + 1, i + 1);

        switch (signal)
        {
        case RISING:
            if (prevAvg < trigger_level && currAvg >= trigger_level)
            {
                return i;
            }
            break;
        case FALLING:
            if (prevAvg >= trigger_level && currAvg < trigger_level)
            {
                return i;
            }
            break;
        case EITHER_RISING_FALLING:
            if ((prevAvg < trigger_level && currAvg >= trigger_level) ||
                (prevAvg >= trigger_level && currAvg < trigger_level))
            {
                return i;
            }
            break;
        }
    }
    return -1; // Return -1 if no trigger is found
}

template <typename T, const size_t N>
void printTriggerData(ppBuf<T, N> &buffer, float trigger_level, SignalType signal, size_t before_n, size_t after_n)
{
    static size_t lastProcessedIteration = 0;

    // Check if the buffer is fresh
    if (buffer.iteration_counter == lastProcessedIteration)
    {
        return; // Buffer already processed
    }

    // Update the last processed iteration
    lastProcessedIteration = buffer.iteration_counter;

    int triggerIndex = findTriggerIndex(buffer, trigger_level, signal);
    if (triggerIndex != -1)
    {
        // Ensure there is enough space after the trigger
        if (triggerIndex + after_n >= N)
        {
            printf("Trigger ignored due to insufficient space after trigger index.\n");
            return;
        }

        // Calculate start and end indices for printing
        size_t start = triggerIndex > before_n ? triggerIndex - before_n : 0;
        size_t end = triggerIndex + after_n;

        // Print values before and after the trigger
        printf("Trigger found at index: %d\n", triggerIndex);
        printf("Values around trigger:\n");
        for (size_t i = start; i <= end; ++i)
        {
            printf("%f ", buffer.read_active_buf(i));
        }
        printf("\n");
    }
    else
    {
        printf("No trigger found.\n");
    }
}

// int main() {
//     const size_t bufferSize = 10;
//     ppBuf<float, bufferSize> dataBuffer; // Create ppBuf instance
//     // Populate the data buffer
//     for (size_t i = 0; i < bufferSize; ++i) {
//         dataBuffer.append(/* some float value */);
//     }

//     float trigger_level = 3.0;
//     SignalType signal = EITHER_RISING_FALLING;
//     size_t averaging = 5; // Default averaging

//     int triggerIndex = findTriggerIndex(dataBuffer, trigger_level, signal, averaging);
//     if (triggerIndex != -1) {
//         std::cout << "Trigger found at index: " << triggerIndex << std::endl;
//     } else {
//         std::cout << "No trigger found." << std::endl;
//     }

//     return 0;
// }

#endif /* D330EBD1_5DF3_4C99_8C64_DF71A81CD623 */
