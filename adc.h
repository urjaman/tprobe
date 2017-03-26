#pragma once

void adc_enable(void);
void adc_select_vcc(void);
void adc_select_probe(void);
uint16_t adc_sample(void);
void adc_disable(void);

uint16_t adc_sample_probe_mV(void);
uint16_t adc_get_vcc_mV(void);


