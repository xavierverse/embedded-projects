#include <stdio.h>
#include <string.h>
#include <pigpio.h>
#include <unistd.h>
#include <stdlib.h>

#define LED_PIN 2 // GPIO pin connected to the LED, adjust as needed
#define DOT 300000

void blinkMorseCode(char character);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <repetitions> <message>\n", argv[0]);
        return 1;
    }

    int repetitions = atoi(argv[1]);
    if (repetitions <= 0) {
        printf("Invalid number of repetitions. It should be a positive integer.\n");
        return 1;
    }

    //gpioInitialise();
    if (gpioInitialise() < 0) {
       fprintf(stderr, "Unable to initialize pigpio\n");
        return 1;
    }

    // Configure LED pin
    gpioSetMode(LED_PIN, PI_OUTPUT);

    printf("Transmitting Morse code for: %s\n", argv[2]);

    for (int rep = 0; rep < repetitions; rep++) {
        for (int i = 0; i < strlen(argv[2]); i++) {
            char character = argv[2][i];
            blinkMorseCode(character);
        }
        usleep(1000000); // Gap between repetitions in microseconds
    }

    gpioTerminate(); // Clean up pigpio resources
    return 0;
}

void blinkMorseCode(char character) {
    // Define Morse code for letters A-Z and digits 0-9
    const char *morseCode[] = {
        ".-", "-...", "-.-.", "-..", ".",   // A-E
        "..-.", "--.", "....", "..", ".---", // F-J
        "-.-", ".-..", "--", "-.", "---",    // K-O
        ".--.", "--.-", ".-.", "...", "-",   // P-T
        "..-", "...-", ".--", "-..-", "-.--", // U-Y
        "--..", "-----", ".----", "..---",   // Z, 0-2
        "...--", "....-", ".....", "-....",   // 3-6
        "--...", "---..", "----."            // 7-9
    };

    if ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z'))
    {
        int index = (character >= 'a') ? (character - 'a') : (character - 'A');
        const char *morse = morseCode[index];

        for (int i = 0; morse[i] != '\0'; i++)
        {
            if (morse[i] == '.')
            {
                gpioWrite(LED_PIN, 1);
                usleep(DOT); // Dot duration in microseconds
            }
            else if (morse[i] == '-')
            {
                gpioWrite(LED_PIN, 1);
                usleep(3 * DOT); // Dash duration in microseconds
            }

            gpioWrite(LED_PIN, 0);
            usleep(DOT); // Gap between symbols in microseconds
        }

        usleep(2 * DOT); // Gap between letters in microseconds
    }
}

