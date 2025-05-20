#include "Encoder.h"

// Global encoder instances
Encoder* encoder1 = nullptr;
Encoder* encoder2 = nullptr;

// Interrupt handlers
void IRAM_ATTR encoder1ISR() {
    if (encoder1) {
        encoder1->incrementCount();
    }
}

void IRAM_ATTR encoder2ISR() {
    if (encoder2) {
        encoder2->incrementCount();
    }
} 