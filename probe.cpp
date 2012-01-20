// Do not remove the include below
#include "probe.h"



// ********************* Setup ******************************



void setup() {
/*
	// Set up packets
	ch1packet.data.header.magic = HEADER_MAGIC;
	ch1packet.data.header.pin = CH1;
	ch1packet.data.header.type = DIGITAL_SEQUENCE;
	ch1packet.data.header.numBits = 0;
	ch1packet.data.header.numDataBytes = 0;
	ch1packet.data.header.samples = 2;
*/


	// Set up packets
	ch1packet.data.header.magic = HEADER_MAGIC;
	ch1packet.data.header.pin = CH3;
	ch1packet.data.header.type = ANALOG_SEQUENCE;
	ch1packet.data.header.numBits = 0;
	ch1packet.data.header.numDataBytes = 0;
	ch1packet.data.header.samples = 1;


	// set prescale to 16
	#if FASTADC
		sbi(ADCSRA, ADPS2) ;
		cbi(ADCSRA, ADPS1) ;
		cbi(ADCSRA, ADPS0) ;
	#endif

	// Set CH1 to input mode
	CH1DDR &= ~CH1MASK;
	CH1PORT |= CH1MASK;


	// Set CH1 to input mode
	CH3DDR &= ~CH3MASK;
	CH3PORT |= CH3MASK;

	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);

	// Start serial communication
	Serial.begin(115200);
}


// ********************* Loop ******************************

#define SAMPLES 1
void handleCommand(int cmd, int value) {
	switch (cmd) {
	case SAMPLES:
		ch1packet.data.header.samples = value;
		break;
	}
}


void loop() {

	char cmd;
	int value;
	int sum;
	int avg;
	int samplesRemaining;

	// Set up initial conditions
	sum = 0;
	samplesRemaining = ch1packet.data.header.samples;
	ch1packet.data.payload[ ch1packet.data.header.numDataBytes ] = 0;

	while (1) {

		// Take reading
		switch ( ch1packet.data.header.type ) {
			case DIGITAL_SEQUENCE:
				value = ( CH1PIN & CH1MASK ) > 0;
				break;
			case ANALOG_SEQUENCE:
				value = map(analogRead(CH3), 0, 1023, 0, 255);
				break;
		}

		// Add reading to average
		sum += value;
		samplesRemaining -= 1;

		// Add average to packet
		if ( samplesRemaining == 0 ) {

			// Reset for later use
			samplesRemaining = ch1packet.data.header.samples;
			// Average
			switch ( ch1packet.data.header.type ) {
				case DIGITAL_SEQUENCE:
					ch1packet.data.payload[ ch1packet.data.header.numDataBytes ] <<= 1;
					ch1packet.data.payload[ ch1packet.data.header.numDataBytes ] |= sum / samplesRemaining;
					if ( ++ch1packet.data.header.numBits == 8 && ch1packet.data.header.numDataBytes + 1 < PAYLOAD_SIZE ) {
						ch1packet.data.payload[ ++ch1packet.data.header.numDataBytes ] = 0;
						ch1packet.data.header.numBits = 0;
					}
					break;
				case ANALOG_SEQUENCE:
					ch1packet.data.payload[ ch1packet.data.header.numDataBytes++ ] = map(analogRead(CH3), 0, 1023, 0, 255);
					break;
			}
			sum = 0;
		}

		// Write out any full data
		if ( ch1packet.data.header.numDataBytes == PAYLOAD_SIZE - 1 ) {
			ch1packet.data.header.endMicros = micros();
			#if DEBUG
				printPacket(ch1packet);
			#else
				Serial.write(ch1packet.raw, PACKET_SIZE);
			#endif

			if ( Serial.available() >= 4 ) {
				cmd = Serial.parseInt();// << 8;
				//cmd |= Serial.read();
				value = Serial.parseInt();// << 8;
				//value |= Serial.read();
				handleCommand(cmd, value);
			}
			ch1packet.data.header.numDataBytes = 0;
			ch1packet.data.payload[ ch1packet.data.header.numDataBytes ] = 0;
			ch1packet.data.header.numBits = 0;
			ch1packet.data.header.startMicros = micros();
		}
	}
}


void printPacket(union packet_union packet) {
	Serial.println(packet.data.header.endMicros - packet.data.header.startMicros);
	for ( int i = 0; i < PACKET_SIZE; i += 4 ) {
		for ( int j = 0; j < 4; j += 1 ) {
			Serial.print(packet.raw[ i + j ]);
			Serial.print(" ");
		}
		Serial.print("\n");
		delay(100);
	}
}


