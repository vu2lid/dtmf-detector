#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define N       256 //205
#define IX_LEN  8
#define THRESHOLD	200.0

const float cos_t[IX_LEN] = {
  0.9039892931234433, 0.881921264348355, 0.8577286100002721, 0.8175848131515837, 0.7071067811865476, 0.6531728429537769, 0.5758081914178454, 0.49289819222978415
  };
  
const float sin_t[IX_LEN] = {
  0.4275550934302821, 0.47139673682599764, 0.5141027441932217, 0.5758081914178453, 0.7071067811865475, 0.7572088465064845, 0.8175848131515837, 0.8700869911087113
  };


uint16_t samples_noise[] = {
	510, 511, 510, 511, 511, 511, 511, 511, 512, 513, 512, 512, 512, 512, 512, 512, 513, 512, 512, 513, 
	513, 513, 512, 513, 513, 512, 512, 512, 513, 513, 512, 512, 512, 512, 512, 512, 513, 512, 512, 512, 
	512, 511, 511, 511, 512, 512, 512, 512, 511, 512, 512, 512, 512, 511, 511, 511, 511, 512, 511, 511, 
	511, 511, 511, 510, 510, 510, 511, 511, 511, 510, 511, 510, 509, 511, 510, 510, 509, 510, 510, 509, 
	509, 509, 509, 511, 509, 509, 509, 509, 509, 509, 509, 509, 510, 510, 509, 509, 509, 509, 509, 509, 
	510, 508, 509, 509, 509, 508, 509, 510, 509, 509, 508, 509, 510, 510, 510, 509, 509, 510, 510, 509, 
	508, 509, 509, 509, 509, 509, 509, 509, 510, 510, 509, 510, 509, 508, 509, 509, 510, 510, 510, 511, 
	511, 511, 512, 512, 512, 511, 512, 511, 511, 512, 512, 512, 511, 511, 511, 511, 512, 512, 511, 512, 
	512, 511, 512, 513, 512, 511, 512, 511, 511, 512, 512, 512, 512, 513, 513, 512, 512, 512, 512, 512, 
	512, 513, 512, 512, 512, 512, 512, 511, 512, 512, 512, 511, 513, 512, 511, 512, 512, 511, 511, 511, 
	511, 511, 511, 511, 512};

uint16_t samples_3[] = {
	506, 501, 506, 513, 518, 514, 504, 500, 507, 520, 529, 531, 521, 509, 502, 507, 
	516, 518, 513, 504, 498, 504, 517, 528, 530, 522, 511, 505, 509, 517, 520, 513, 
	502, 495, 500, 515, 525, 528, 521, 510, 505, 509, 518, 519, 512, 499, 493, 497, 
	511, 522, 525, 518, 509, 505, 511, 519, 521, 513, 500, 491, 494, 506, 518, 521, 
	514, 507, 504, 511, 520, 523, 514, 500, 492, 495, 506, 516, 518, 513, 504, 504, 
	511, 521, 524, 515, 501, 492, 493, 503, 513, 515, 509, 502, 502, 510, 521, 525, 
	517, 503, 493, 492, 502, 511, 513, 508, 499, 500, 509, 521, 526, 519, 506, 496, 
	496, 505, 513, 513, 506, 498, 498, 508, 521, 527, 521, 508, 498, 499, 506, 513, 
	513, 505, 498, 496, 508, 521, 527, 523, 511, 501, 502, 510, 516, 515, 505, 496, 
	496, 507, 521, 529, 524, 514, 505, 505, 513, 519, 517, 507, 496, 495, 505, 519, 
	527, 524, 514, 507, 508, 515, 522, 519, 508, 497, 495, 504, 516, 526, 523, 514, 
	507, 510, 519, 524, 522, 510, 498, 494, 502, 516, 523, 521, 514, 509, 511, 519, 
	526, 523, 511, 499, 495, 503, 514, 522, 520, 512, 507, 511, 520, 528, 526, 513, 
	500, 495, 501, 513, 519, 516, 508, 504, 509, 519, 529, 527, 514, 502, 496, 501, 
	512, 518, 515, 506, 502, 506, 517, 527, 528, 516, 504, 497, 501, 512, 516, 512, 
	503, 498, 505, 517, 528, 529, 517, 504, 500, 504, 512, 515, 510, 500, 496, 501
};

const char table[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

void goertzel(uint16_t *samples, float *spectrum) {
  float v_0, v_1, v_2;
    
  for (uint8_t k = 0; k < IX_LEN; k++) {
    float a = 2. * cos_t[k];
    v_0 = v_1 = v_2 = .0;    
    for (uint16_t i = 0; i < N; i++) {
    	v_2 = v_1;
    	v_1 = v_0;
		v_0 = (float)(samples[i]) + a * v_1 - v_2;
    }
    float re = cos_t[k] * v_0 - v_1;
    float im = sin_t[k] * v_0;

    spectrum[k] = sqrt(re * re + im * im);        
  }  
}

char detect_digit(float *spectrum) {
	int8_t row = -1, col = -1;
	for (uint8_t i = 0; i < IX_LEN; i++) {
		if (spectrum[i] > THRESHOLD) {
			if (row == -1) {
				row = i;
			} else if (col == -1) {
				col = i;
			} else {
				return 0;
			}
		}
	}

	if (row != -1 && col != -1) {
		return table[row][col - 4];
	} else {
		return 0;
	}
}

int main() {
	float spectrum[IX_LEN];
	goertzel(samples_3, spectrum);

	for (int i = 0; i < IX_LEN; i++) {
		printf("%f ", spectrum[i]);
	}
	printf("\nDetected digit: %c\n", detect_digit(spectrum));

	return 0;
}