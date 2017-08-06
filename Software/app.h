/*
 * @file app.h
 * @date May 24, 2017
 * @author hamster
 *
 */

#ifndef APP_H_
#define APP_H_

void run_app_mode(bool setup);
void badgeSetup(void);
void sawBadge(uint16_t id);
void addSeenManufacturer(uint16_t id);
bool getSeenManufacturer(uint16_t id);
void clearSeenManufacturer(void);
void clearGameData(void);
void updateGameData(char* vector, uint8_t diceRoll);
void updateCommandData(uint16_t flags);
void clearCommandData(void);
void playGame(void);
void protoReset(void);
void masterCommandHandle(void);

typedef struct {
	bool crypto;
	bool queercon;
	bool dc801;
	bool dc503;
	bool bender;
	bool b_d;
	bool dczia;
	bool dczia_shotbot;
} SEEN_MANUFACTURER;

typedef struct {
	uint8_t diceRoll;
	uint16_t waitTime;
} INFECTION_PARAMS;

typedef struct {
	char username[9];
	char vector[9];
} CONFIG_PARAMS;

typedef struct {
	char vector[9];
	uint8_t diceRoll;
	bool nearby;
} GAME_DATA;

typedef struct {
	bool partyMode;
	bool fullReconfig;
	bool sendingCommand;
} COMMAND_DATA;

extern volatile INFECTION_PARAMS infectionParams;
extern CONFIG_PARAMS configParams;


#define MIN_WAIT_TIME 500  // Infection game run every these many seconds + rand number
#define DEFAULT_USER	"801SHEEP"
#define DEFAULT_VECTOR	"_Helga_"

#define MASTER_BADGE	false

#endif /* APP_H_ */
