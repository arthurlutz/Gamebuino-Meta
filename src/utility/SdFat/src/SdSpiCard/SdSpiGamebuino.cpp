/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2017

This is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL)
as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License (LGPL) for more details.

You should have received a copy of the GNU Lesser General Public
License (LGPL) along with the library.
If not, see <http://www.gnu.org/licenses/>.

Authors:
 - Sorunome
*/

#include "SdSpi.h"

// #include "../../../Adafruit_ZeroDMA.h"
// #include "../../../Adafruit_ZeroDMA/utility/dmac.h"
// #include "../../../Adafruit_ZeroDMA/utility/dma.h"

// Gamebuino_Meta::Adafruit_ZeroDMA myDMA;
// Gamebuino_Meta::Adafruit_ZeroDMA myDMA2;
// are we done yet?
volatile bool transfer_rx_done = false;
volatile bool transfer_tx_done = false;
// If you like, a callback can be used
void dma_callback_rx(struct dma_resource* const resource) {
	transfer_rx_done = true;
}
void dma_callback_tx(struct dma_resource* const resource) {
	transfer_tx_done = true;
}

// static SPISettings mySPISettings;

uint8_t chipSelectPin;

void SdSpi::begin(uint8_t _chipSelectPin) {
	chipSelectPin = _chipSelectPin;
	// pinMode(chipSelectPin, OUTPUT);
	// digitalWrite(chipSelectPin, HIGH);
	// SPI.begin();
	// mySPISettings = SPISettings(12000000, MSBFIRST, SPI_MODE0);
}

void SdSpi::beginTransaction(uint8_t divisor) {
#if ENABLE_SPI_TRANSACTIONS
	// SPI.beginTransaction(mySPISettings);
#endif  // ENABLE_SPI_TRANSACTIONS
#ifndef SPI_CLOCK_DIV128
	// SPI.setClockDivider(divisor);
#else  // SPI_CLOCK_DIV128
	int v;
	if (divisor <= 2) {
		v = SPI_CLOCK_DIV2;
	} else  if (divisor <= 4) {
		v = SPI_CLOCK_DIV4;
	} else  if (divisor <= 8) {
		v = SPI_CLOCK_DIV8;
	} else  if (divisor <= 16) {
		v = SPI_CLOCK_DIV16;
	} else  if (divisor <= 32) {
		v = SPI_CLOCK_DIV32;
	} else  if (divisor <= 64) {
		v = SPI_CLOCK_DIV64;
	} else {
		v = SPI_CLOCK_DIV128;
	}
	// SPI.setClockDivider(v);
#endif  // SPI_CLOCK_DIV128
}

void SdSpi::endTransaction() {
#if ENABLE_SPI_TRANSACTIONS
	// SPI.endTransaction();
#endif  // ENABLE_SPI_TRANSACTIONS
}

// no DMA needed here
uint8_t SdSpi::receive() {
	// return SPI.transfer(0XFF);
}

uint8_t SdSpi::receive(uint8_t* buf, size_t n) {
	// myDMA.configure_peripheraltrigger(SERCOM4_DMAC_ID_TX);
	// myDMA.configure_triggeraction(DMA_TRIGGER_ACTON_BEAT);
	// myDMA.allocate();
	// uint8_t b = 0xFF;
	// myDMA.setup_transfer_descriptor(
	// 	&b, // from here
	// 	(void *)(&SERCOM4->SPI.DATA.reg), // to here
	// 	n, // this many
	// 	DMA_BEAT_SIZE_BYTE, // 8 bits
	// 	false, // increment source addr?
	// 	false // increment dest addr?
	// );
	// myDMA.add_descriptor();
	// myDMA.register_callback(dma_callback_tx);
	// myDMA.enable_callback();
  //
	// myDMA2.configure_peripheraltrigger(SERCOM4_DMAC_ID_RX);
	// myDMA2.configure_triggeraction(DMA_TRIGGER_ACTON_BEAT);
	// myDMA2.allocate();
	// myDMA2.setup_transfer_descriptor(
	// 	(void *)(&SERCOM4->SPI.DATA.reg), // from here
	// 	buf, // to here
	// 	n, // this many
	// 	DMA_BEAT_SIZE_BYTE, // 8 bits
	// 	false, // increment source addr?
	// 	true // increment dest addr?
	// );
	// myDMA2.add_descriptor();
	// myDMA2.register_callback(dma_callback_rx);
	// myDMA2.enable_callback();
  //
  //
	// transfer_tx_done = transfer_rx_done = false;
	// myDMA2.start_transfer_job();
	// myDMA.start_transfer_job();
	// while (!(transfer_tx_done && transfer_rx_done)); // chill
	// myDMA.free();
	// myDMA2.free();
	return 0; // we just assume this worked.... (probably not too good an idea)
	// TODO: check for stuff
}

void SdSpi::send(uint8_t b) {
	// SPI.transfer(b);
}

void SdSpi::send(const uint8_t* buf, size_t n) {
	// myDMA.configure_peripheraltrigger(SERCOM4_DMAC_ID_TX);
	// myDMA.configure_triggeraction(DMA_TRIGGER_ACTON_BEAT);
	// myDMA.allocate();
	// myDMA.setup_transfer_descriptor(
	// 	(void *)buf, // from here
	// 	(void *)(&SERCOM4->SPI.DATA.reg), // to here
	// 	n, // this many
	// 	DMA_BEAT_SIZE_BYTE, // 8 bits
	// 	true, // increment source addr?
	// 	false // increment dest addr?
	// );
	// myDMA.add_descriptor();
	// transfer_tx_done = false;
	// myDMA.register_callback(dma_callback_tx);
	// myDMA.enable_callback();
	// myDMA.start_transfer_job();
	// while (!transfer_tx_done); // chill
	// myDMA.free();
}
