#pragma once
// versie zonder std::array

// NB: dit is een "hack-debug-logger"
// Om het volgens het boekje te doen, zou een channel gebruikt moeten worden.

// Voorbeeld van handig gebruik:

// Voeg toe in je one-en only main.cpp:
// ...
// #include "Logger.h"
// Logger* pLogger = nullptr;
//
// int main()
// {
//    // Onderstaande pin is "active low", met pull-up weerstand naar 3.3V. Dus als je de button indrukt, wordt hij laag.
//    auto dumpButton = hwlib::target::pin_in(hwlib::target::pins::d10);
//    Logger logger(dumpButton, 13);
//    pLogger = &logger;
//    .... 

// Voeg toe in header files waar je iets wilt loggen:
// #include "Logger.h"
// extern Logger* pLogger;

// Binnen de klasse kun je vervolgens razendsnel text en getallen loggen:
//
// pLogger->logText("RGC_MeldHitReceived");
// pLogger->logInt(PlayerID);
// pLogger->logInt(WeaponID);

// Op enig moment kun je de logs naar het scherm laten couten
// door het indrukken van de dumpButton.

class Logger : public rtos::task<>
{
private:
    static const int LogSize = 100; // Passing size by template parameter would be nicer..
    // Don't select the LogSize too high, or you may suffer from stack overflow troubles.
    hwlib::target::pin_in &buttonDump; // A press on this button (active low) activates the dump.

    const char *arTextLogs[LogSize] = {}; // Circular buffer.
    int32_t arIntLogs[LogSize] = {};

    unsigned long logCount = 0;         // Absolute count of logs
    unsigned long latestBatchCount = 0; // Count of logs since last dump.
    unsigned int index = 0;             // Index to write next log to.
    const int minInt = -(1 << 30);      // of zo..
private:
    void main()
    {
        unsigned int j = 0; // lokale enumeratievariabele
        bool prevPress = true;
        while (true)
        {
            bool buttonReadDump = !buttonDump.read();

            if (buttonReadDump && !prevPress)
            {
                int nFirstLogSequenceNumber = (logCount - latestBatchCount);
                hwlib::cout << "-------------------------------------" << hwlib::endl;
                hwlib::cout << "--------------Log Dump---------------" << hwlib::endl;
                hwlib::cout << "sequenceNumber of first log: " << nFirstLogSequenceNumber << hwlib::endl;
                hwlib::cout << "-------------------------------------" << hwlib::endl;
                // for (auto &log : logs)
                j = index; // start at oldest entry of the circular buffers.
                for (unsigned int i = 0; i < LogSize; i++)
                {
                    // hwlib::cout << j;
                    if (arTextLogs[j] != nullptr)
                    {
                        hwlib::cout << arTextLogs[j] << hwlib::endl;
                    }
                    else if (arIntLogs[j] != minInt)
                    {
                        hwlib::cout << arIntLogs[j] << hwlib::endl;
                    }

                    j += 1;
                    if (j > (LogSize - 1))
                    {
                        j = 0;
                    }
                }

                ClearTextLogs();
                ClearIntLogs();
                latestBatchCount = 0;

                rtos::display_statistics();
                rtos::statistics_clear();

                hwlib::wait_ms(100);
            }
            prevPress = buttonReadDump;
            hwlib::wait_ms(100);
        }
    }

public:
    Logger(hwlib::target::pin_in &buttonDump, unsigned int priority)
        : rtos::task<3000>(priority, "Logger"), buttonDump(buttonDump)
    {
        ClearTextLogs();
        ClearIntLogs();
    }

    void logText(const char *text)
    {
		// Standaard worden alleen de eerste LogSize berichten sinds de laatste dumpbutton press
		// bewaard en gecout.
		// Als je onderstaande regel uitcommentarieerd, worden juist de laatste LogSize berichten
		// sinds de laatste dumpbutton press bewaard en gecout.
		// NB: commentarieer in dat geval ook de eerste regel van logInt uit!
        if(latestBatchCount==LogSize) return;

        arTextLogs[index] = text;
        arIntLogs[index] = minInt;

        IncreaseCount();
    }

    void logInt(int32_t intNumber)
    {
		// Sync onderstaande regel met die in LogText
        if(latestBatchCount==LogSize) return;

        arTextLogs[index] = nullptr;
        arIntLogs[index] = intNumber;

        IncreaseCount();
    }

private:
    void IncreaseCount()
    {
        logCount++;
        latestBatchCount++;
        index++;

        if (latestBatchCount > LogSize)
        {
            latestBatchCount = LogSize; // LogSize is the deepest possible history.
        }

        if (index > (LogSize - 1))
        {
            index = 0;
        }
    }

    void ClearTextLogs()
    {
        int i = 0;
        for (i = 0; i < LogSize; i++)
        {
            arTextLogs[i] = nullptr;
        }
    }

    void ClearIntLogs()
    {
        int i = 0;
        for (i = 0; i < LogSize; i++)
        {
            arIntLogs[i] = minInt;
        }
    }
};
