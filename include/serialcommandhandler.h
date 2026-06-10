#pragma once
#include <Arduino.h>

// 1. Defining a Function Pointer Type
// This creates a custom type called 'CommandCallback'.
// It represents a pointer to a function that takes no arguments '()' and returns nothing 'void'.
typedef void (*CommandCallback)();

class SerialCommandHandler
{
private:
    static const int MAX_COMMANDS = 10; // Maximum number of commands we can store

    // 2. Struct to pair a character with its function pointer
    struct CommandMapping
    {
        char command;
        CommandCallback callback;
    };

    // Array to hold our registered commands
    CommandMapping commands[MAX_COMMANDS];
    int commandCount = 0; // Keep track of how many commands are currently registered

public:
    // 3. Method to register a new command binding
    bool createCommand(char commandByte, CommandCallback callbackFunction)
    {
        if (commandCount < MAX_COMMANDS)
        {
            // Store the character and the function pointer in our array
            commands[commandCount].command = commandByte;
            commands[commandCount].callback = callbackFunction;
            commandCount++; // Increment our total count
            return true;    // Successfully added
        }
        return false; // Array is full
    }

    // 4. Method to process incoming serial data
    // You'll need to call this repeatedly in your main loop()
    void process()
    {
        if (Serial.available() > 0)
        {
            // Read a single character/byte from the serial buffer
            char incomingByte = Serial.read();

            // Ignore standard newline characters we might get when hitting "enter" in the monitor
            if (incomingByte == '\n' || incomingByte == '\r')
            {
                return;
            }

            // Loop through registered commands
            for (int i = 0; i < commandCount; i++)
            {
                if (commands[i].command == incomingByte)
                {
                    // Match found! Execute the bound function.
                    // This is where the function pointer is used.
                    commands[i].callback();
                    return; // Stop searching once we found a match
                }
            }

            // Optional: Print a message if the command is completely unknown
            Serial.print("Unknown command: ");
            Serial.println(incomingByte);
        }
    }
};
