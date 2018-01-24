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
 - Aurelien Rodot
 - Sorunome
*/

#include <SDL.h>
#include "Sound.h"
#include "Pattern.h"
#include "Tone.h"
#include "Raw.h"
#include "../Sound-SD.h"
#include "../../config/config.h"

#define analogWrite(x, y)
#define analogWriteResolution(x)

namespace Gamebuino_Meta {

const uint16_t playOKPattern[] = {0x0005,0x138,0x168,0x0000};
const uint16_t playCancelPattern[] = {0x0005,0x168,0x138,0x0000};
const uint16_t playTickP[] = {0x0045,0x168,0x0000};

uint8_t globalVolume = 6;
bool muted = false;

#if SOUND_CHANNELS > 0
Sound_Channel channels[SOUND_CHANNELS];
Sound_Handler* handlers[SOUND_CHANNELS];

bool tcIsSyncing() {
	// return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
	return false;
}

void tcStart() {
	// Enable TC

	// TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
	while (tcIsSyncing());
}

void tcReset() {
	// Reset TCx
	// TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
	while (tcIsSyncing());
	// while (TC5->COUNT16.CTRLA.bit.SWRST);
}

void tcDisable() {
	// Disable TC5
	// TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	while (tcIsSyncing());
}

void tcConfigure(uint32_t sampleRate) {
	// Enable GCLK for TCC2 and TC5 (timer counter input clock)
	// GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
	// while (GCLK->STATUS.bit.SYNCBUSY);

	tcReset();

	// Set Timer counter Mode to 16 bits
	// TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  //
	// // Set TC5 mode as match frequency
	// TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  //
	// TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;
  //
	// TC5->COUNT16.CC[0].reg = (uint16_t) (SystemCoreClock / sampleRate - 1);
	while (tcIsSyncing());

	// Configure interrupt request
	// NVIC_DisableIRQ(TC5_IRQn);
	// NVIC_ClearPendingIRQ(TC5_IRQn);
	// NVIC_SetPriority(TC5_IRQn, 0);
	// NVIC_EnableIRQ(TC5_IRQn);
  //
	// // Enable the TC5 interrupt request
	// TC5->COUNT16.INTENSET.bit.MC0 = 1;
	// while (tcIsSyncing());
}

void dacConfigure(void) {
	analogWriteResolution(10);
	analogWrite(A0, 0);
}

int8_t findEmptyChannel() {
	for (uint8_t i = 0; i < SOUND_CHANNELS; i++) {
		if (!channels[i].use) {
			if (handlers[i]) {
				delete handlers[i];
				handlers[i] = 0;
			}
			return i;
		}
	}
	return -1;
}
#endif

Sound_Handler::Sound_Handler(Sound_Channel* _channel) {
	channel = _channel;
}

Sound_Handler::~Sound_Handler() {

}

uint32_t Sound_Handler::getPos() {
	return 0xFFFFFFFF;
}

void Sound_Handler::setChannel(Sound_Channel* _channel) {
	channel = _channel;
}

extern "C" void fillBuffer(void*  userdata, Uint8* stream, int len);

SDL_AudioDeviceID dev;

void Sound::begin() {
#if SOUND_CHANNELS > 0
	dacConfigure();
	tcConfigure(SOUND_FREQ);
	tcStart();
#endif

	SDL_AudioSpec want, have;
	SDL_zero(want);
	want.freq = SOUND_FREQ;
	want.format = AUDIO_U8;
	want.channels = 1;
	want.samples = 2048;
	want.callback = fillBuffer;

	dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (dev == 0) {
    SDL_Log("Failed to open audio: %s", SDL_GetError());
	} else {
	  if (have.format != want.format) {
      SDL_Log("We didn't get AUDIO_U8 audio format.");
	  }
	  SDL_PauseAudioDevice(dev, 0); /* start audio playing. */
	}
}

int8_t Sound::play(const char* filename, bool loop) {
#if SOUND_CHANNELS > 0
	int8_t i = findEmptyChannel();
	if (i < 0 || i >= SOUND_CHANNELS) {
		return -1; // no free channels atm
	}
	channels[i].loop = loop;
	handlers[i] = new Sound_Handler_Wav(&(channels[i]));
	if (!((Sound_Handler_Wav*)handlers[i])->init(filename)) {
		delete handlers[i];
		handlers[i] = 0;
		return -1;
	}
	return i;
#else // SOUND_CHANNELS
	return -1;
#endif // SOUND_CHANNELS
}

int8_t Sound::play(char* filename, bool loop) {
	return play((const char*)filename, loop);
}

int8_t Sound::play(const uint16_t* buffer, bool loop) {
#if SOUND_CHANNELS > 0
	int8_t i = findEmptyChannel();
	if (i < 0 || i >= SOUND_CHANNELS) {
		return -1; // no free channels atm
	}
	channels[i].loop = loop;
	handlers[i] = new Sound_Handler_Pattern(&(channels[i]), (uint8_t*)buffer);
	return i;
#else // SOUND_CHANNELS
	return -1;
#endif // SOUND_CHANNELS
}

int8_t Sound::play(uint16_t* buffer, bool loop) {
	return play((const uint16_t*)buffer, loop);
}

int8_t Sound::play(const uint8_t* buf, uint32_t len, bool loop) {
#if SOUND_CHANNELS > 0
	int8_t i = findEmptyChannel();
	if (i < 0 || i >= SOUND_CHANNELS) {
		return -1; // no free channels atm
	}
	channels[i].loop = loop;
	handlers[i] = new Sound_Handler_Raw(&(channels[i]), (uint8_t*)buf, len);
	return i;
#else // SOUND_CHANNELS
	return -1;
#endif // SOUND_CHANNELS
}

int8_t Sound::play(uint8_t* buf, uint32_t len, bool loop) {
	return play((const uint8_t*)buf, len, loop);
}

int8_t Sound::play(Sound_Handler* handler, bool loop) {
#if SOUND_CHANNELS > 0
	int8_t i = findEmptyChannel();
	if (i < 0 || i >= SOUND_CHANNELS) {
		return -1; // no free channels atm
	}
	channels[i].loop = loop;
	handlers[i] = handler;
	handlers[i]->setChannel(&(channels[i]));
	return i;
#else // SOUND_CHANNELS
	return -1;
#endif // SOUND_CHANNELS
}

int8_t Sound::tone(uint32_t frequency, int32_t duration) {
#if SOUND_CHANNELS > 0
	int8_t i = findEmptyChannel();
	if (i < 0 || i >= SOUND_CHANNELS) {
		return -1; // no free channels atm
	}
	channels[i].loop = duration == 0;
	handlers[i] = new Sound_Handler_Tone(&(channels[i]), frequency, duration, i);
	return i;
#else // SOUND_CHANNELS
	return -1;
#endif // SOUND_CHANNELS
}

void Sound::stop(int8_t i) {
	if (i >= SOUND_CHANNELS || i < 0) {
		return;
	}
	channels[i].use = false;
	if (handlers[i]) {
		delete handlers[i];
		handlers[i] = 0;
	}
}

int8_t Sound::playOK() {
	return play(playOKPattern);
}

int8_t Sound::playCancel() {
	return play(playCancelPattern);
}

int8_t Sound::playTick() {
	return play(playTickP);
}

bool efx_only = false;

void Sound::update() {
#if SOUND_CHANNELS > 0
	for (uint8_t i = 0; i < SOUND_CHANNELS; i++) {
		if (channels[i].use) {
			if (!efx_only || (!channels[i].loop && channels[i].type == Sound_Channel_Type::square)) {
				handlers[i]->update();
			}
		} else if (handlers[i]) {
			delete handlers[i];
			handlers[i] = 0;
		}
	}
#endif // SOUND_CHANNELS
}

void Sound::mute() {
	muted = true;
}

void Sound::unmute() {
	muted = false;
}

bool Sound::isMute() {
	return muted;
}

void Sound::startEfxOnly() {
	efx_only = true;
}

void Sound::stopEfxOnly() {
	efx_only = false;
}

bool Sound::isPlaying(int8_t i) {
	if (i < 0 || i >= SOUND_CHANNELS) {
		return false;
	}
	return channels[i].use;
}

void Sound::setVolume(uint8_t volume) {
	globalVolume = constrain(volume, 0, 8);
}

uint8_t Sound::getVolume() {
	return globalVolume;
}

uint32_t Sound::getPos(int8_t i) {
	if (!isPlaying(i) || !(handlers[i])) {
		return 0xFFFFFFFF;
	}
	return handlers[i]->getPos();
}

#if SOUND_CHANNELS > 0

#ifdef __cplusplus
extern "C" {
#endif
uint16_t Audio_Handler (void) __attribute__((optimize("-O3")));

uint16_t flowdown = 0;

void fillBuffer(void*  userdata, Uint8* stream, int len) {
	uint16_t sample;
	for(uint16_t i=0; i < len; i++) {
			sample = Audio_Handler();
			sample >>= 2; // 10-bit back to 8 bit audio since we have no 10-bit DAC
			stream[i] = sample;
	}
}

uint16_t Audio_Handler (void) {
	int16_t output = 0;

	if (!globalVolume || muted) {
		// TC5->COUNT16.INTFLAG.bit.MC0 = 1;
		return output;
	}

	for (uint8_t i = 0; i < SOUND_CHANNELS; i++) {
		if (channels[i].use) {
			switch (channels[i].type) {
			case Sound_Channel_Type::raw:
				if (efx_only) {
					break;
				}
				if (channels[i].index < channels[i].total - 1) {
					output += (channels[i].buffer[channels[i].index++] - 0x80);
				} else if (!channels[i].last) {
					channels[i].index = 0;
					output += (channels[i].buffer[channels[i].index++] - 0x80);
				} else if (channels[i].loop) {
					handlers[i]->rewind();
				} else {
					channels[i].use = false;
				}
				break;
			case Sound_Channel_Type::square:
				if (efx_only && channels[i].loop) {
					break;
				}
				if (channels[i].index++ >= channels[i].total) {
					channels[i].last = !channels[i].last;
					channels[i].index = 0;
				}
				if (channels[i].last) {
					output -= channels[i].amplitude;
				} else {
					output += channels[i].amplitude;
				}
				break;
			}
		}
	}
	if (output) {
		//we multiply by 4 to use the whole 0..1024 DAC range even with 8-bit 0..255 waves
		//then we attenuate the signal. The attenuation is not linear because human ear's response isn't ;)
		//we use a >> instead of division for better performances as this interrupt runs quite often
		//RAW VALUE		VOLUME		OUTPUT
		//255			8			1024	//amplify sound to use full DAC range
											//might cause clipping if several sounds are played simultaneously
		//255			7			512
		//255			6			255		//keep sound as original
		//255			5			127		//reduced volume

		output = (output * 4) >> (8 - globalVolume);
		//offset the signed value to be centered around 512
		//as the 10-bit DAC output is between 0 and 1024

		// we need to slowly fade up our zero-level to not have any plop when starting to play sound
		if (flowdown < 512) {
			flowdown++;
		}
		output += flowdown;
		if (output < 0) {
			output = 0;
		}
		analogWrite(A0, output);
		return output;
	} else {
		// we need to output 0 when not in use to not have weird sound effects with the neoLeds as the interrupt isn't 100% constant there.
		// however, jumping down from 512 (zero-positin) to 0 would give a plop
		// so instead we gradually decrease instead
		analogWrite(A0, flowdown); // zero-position
		return flowdown;
		if (flowdown > 0) {
			flowdown--;
		}
	}
	// TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}

void TC5_Handler (void) __attribute__ ((weakalias("Audio_Handler")));

#ifdef __cplusplus
}
#endif

#endif // SOUND_CHANNELS

} // Gamebuino_Meta
