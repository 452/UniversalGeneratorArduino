#include <CommandLine.h>

CommandLine commandLine(Serial, "$ ");

#define CHANNELS_COUNT 13
//9945 50Hz
//8316 60Hz

typedef struct
{
  volatile unsigned long previousMillis = 0;
  volatile unsigned long onTime = 0;
  volatile unsigned long offTime = 0;
  volatile int state = LOW;
} Generator;

volatile Generator generator[CHANNELS_COUNT];

void setup() {
  Serial.begin(115200);
  Serial.println(F("Universal Generator Started"));
  initInfo();
  commandLine.add("ch", handleCh);
  //commandLine.add("get", handleGetPulsePeriod);
  commandLine.add("clear", handleClear);
  commandLine.add("help", handleHelp);

  for (int channel = 0; channel <= CHANNELS_COUNT; channel++) {
    generator[channel].previousMillis = 0;
    generator[channel].onTime = 8316;
    generator[channel].offTime = 8316;
    generator[channel].state = LOW;
    pinMode(channel, OUTPUT);
  }
}

void handleGetPulsePeriod(char* tokens) {
  //Serial.println(pulseIn(IN, HIGH));
}

void handleCh(char* tokens) {
  char* channelName = strtok(NULL, " ");
  int channelPin;
  if (channelName != NULL) {
    channelPin = atoi(channelName);
    if (channelPin < 0 || channelPin > CHANNELS_COUNT) {
      Serial.println("Please enter correct channel from 0 to 13");
      channelPin = 9;
    }
  }

  char* pulseLength = strtok(NULL, " ");
  if (pulseLength != NULL) {
    generator[channelPin].onTime = atoi(pulseLength);
  }

  char* pulseDelay = strtok(NULL, " ");
  if (pulseDelay != NULL) {
    generator[channelPin].offTime = atoi(pulseDelay);
  }
  Serial.println("OK");
}

void generate(unsigned long currentMillis) {
  for (int channel = 0; channel <= CHANNELS_COUNT; channel++) {
    if ((generator[channel].state == HIGH) && (currentMillis - generator[channel].previousMillis >=  generator[channel].onTime)) {
      generator[channel].state = LOW;
      generator[channel].previousMillis = currentMillis;
      digitalWrite(channel, generator[channel].state);
    } else if ((generator[channel].state == LOW) && (currentMillis - generator[channel].previousMillis >= generator[channel].offTime)) {
      generator[channel].state = HIGH;
      generator[channel].previousMillis = currentMillis;
      digitalWrite(channel, generator[channel].state);
    }
  }
}

void loop() {
  generate(micros());
  commandLine.update();
}

/**
 * Print some help.
 *
 * @param tokens The rest of the input command.
 */
void handleHelp(char* tokens) {
  Serial.println("ch - enable impulse generator on port N with HIGH length and LOW length");
  //Serial.println("get - show length of the pulse (in microseconds) or 0 if no pulse started");
  Serial.println("clear - clean console screen");
}

/**
 * Handle the clear screen command.
 *
 * @param tokens The rest of the input command.
 */
void handleClear(char* tokens)
{
  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command
  initInfo();
}

void initInfo() {
  Serial.println(F("List of commands & examples: help"));
  Serial.println(F(": ch 13 9945 9945, choff n, clear"));
}
