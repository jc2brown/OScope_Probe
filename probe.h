
#ifndef Probe_H_
#define Probe_H_

#include "Arduino.h"


#ifdef __cplusplus
	extern "C" {
#endif
	void loop();
	void setup();
#ifdef __cplusplus
	}
#endif


#define DEBUG 0

#define HEADER_MAGIC 3


// Pin1
#define CH1     2
#define CH1DDR  DDRD
#define CH1PORT PORTD
#define CH1PIN  PIND
#define CH1MASK 0b00000100

// Pin2
#define CH3     A0
#define CH3DDR  DDRC
#define CH3PORT PORTC
#define CH3MASK 0b00000001


enum PacketType {
	DIGITAL_SEQUENCE = 1,
	DIGITAL_VAL_DUR = 2,
	DIGITAL_VAL_DUR_FREQ = 3,
	ANALOG_SEQUENCE = 4,
	ANALOG_VAL_DUR = 5,
	ANALOG_VAL_DUR_FREQ = 6
};



// ********************* Packets ******************************

// Manual math required
#define PACKET_SIZE    128
#define HEADER_SIZE     16
#define PAYLOAD_SIZE   112

struct header_struct {
	uint8_t magic;
	uint8_t pin;
	uint8_t type;
	uint8_t numBits;
	uint16_t numDataBytes;
	uint16_t samples;
	uint32_t startMicros;
	uint32_t endMicros;
};

struct packet_struct {
	struct header_struct header;
	uint8_t payload[PAYLOAD_SIZE];
};

union packet_union {
	uint8_t raw[PACKET_SIZE];
	struct packet_struct data;
};


// ********************* Fast ADC ******************************

#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif




#define ANALOG 0
#define DIGITAL 1

union packet_union ch1packet;


void printPacket(union packet_union packet);
void handleCommand(int cmd, int value);


#endif	// End Guard
