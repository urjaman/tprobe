#include "main.h"
#include "adc.h"

// Handled by adc_sample, but use INT to wakeup.
EMPTY_INTERRUPT(ADC_vect);

static uint16_t adc_vcc;

uint16_t adc_sample(void) {
	// sleepy sampling
	cli();
	ADCSRA |= _BV(ADSC);
	do {
		sys_sleep(SLEEP_MODE_ADC);
	} while (ADCSRA&_BV(ADSC)); // We might get woken up due to other reasons, thus dont assume sample done
	return ADC;
}

void adc_enable(void) {
	power_adc_enable();
	ADCSRA = _BV(ADEN) | _BV(ADIF) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1);
	ADCSRB = 0;
}

void adc_select_vcc(void) {
	ADMUX = 0b1100; // 1.1V ref according to Vcc (=measure VCC)
	/* 1ms delay with ADC enabled; better just use the ADC for it and sleep... */
	for (uint8_t n=0;n<10;n++) adc_sample();
}

void adc_select_probe(void) {
	ADMUX = 0b0010;
	adc_sample(); // flush one sample anyways ...
}

void adc_disable(void) {
	ADMUX = 0b0010; // To turn off the 1.1V ref
	ADCSRA = 0; // To turn off other ADC stuff
	power_adc_disable(); // To turn off ADC clocks
}

uint16_t adc_sample_probe_mV(void) {
	adc_enable();
	adc_select_probe();
	uint16_t prb = adc_sample();
	adc_select_vcc();
	uint16_t ivcc = adc_sample();
	adc_disable();
	uint16_t vcc = 1126000UL / (uint32_t)ivcc;
	adc_vcc = vcc;
	uint16_t mV = ((uint32_t)prb * (uint32_t)vcc) / 1024;
	return mV;
}

uint16_t adc_get_vcc_mV(void) {
	return adc_vcc;
}

