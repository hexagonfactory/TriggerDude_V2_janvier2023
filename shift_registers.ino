void read_shift_registers() {
  last_pads_value = current_pads_value;

  digitalWrite(SR_165_LATCH, LOW);
  digitalWrite(SR_165_LATCH, HIGH);

  uint32_t input_Value = 0;

  for (uint8_t btn = 0; btn < DATA_WIDTH; ++btn) {
    input_Value |= (digitalRead(SR_165_DATA) << ((DATA_WIDTH - 1) - btn));
    digitalWrite(SR_165_CLOCK, HIGH);
    digitalWrite(SR_165_CLOCK, LOW);
  }

  current_pads_value = input_Value;

  for (uint8_t btn = 0; btn < DATA_WIDTH; ++btn) {
    if (btn < TRACKS) {
      if ((screenConfig == 1 && bank[currentBank].pattern[currentPattern].track_isGate[btn]) ||              // NORMAL MODE - TRACK IS GATE
          screenConfig == 6 ||                                                                               // DELETE STEPS MODE
          screenConfig == 8 ||                                                                               // ROLL MODE
          (screenConfig == 33 && bank[currentBank].pattern[currentPattern].track_isGate[selected_Track]) ||  // KBOARD MODE - TRACK IS GATE
          screenConfig == 34) {                                                                              // ARP MODE
        trigsIn_BitVal[btn] = bitRead(current_pads_value, btn);
      }

      else {
        trigsIn_BitVal[btn] = bitRead(current_pads_value, btn) & !bitRead(last_pads_value, btn);
      }
    }

    else if (btn < PADS) {
      //trigsIn_BitVal[btn] = bitRead(current_pads_value, btn); ?
      trigsIn_BitVal[btn] = bitRead(current_pads_value, btn) & !bitRead(last_pads_value, btn);
    }

    else {
      btnsIn_BitVal[btn - PADS] = bitRead(current_pads_value, btn) & !bitRead(last_pads_value, btn);
    }
  }
}


/*void read74HC165() {
  last_pads_1_8_value = current_pads_1_8_value;
  last_pads_9_16_value = current_pads_9_16_value;
  last_btns_value = current_btns_value;

  digitalWrite(SR_165_LATCH, LOW);
  digitalWrite(SR_165_LATCH, HIGH);

  SPI.beginTransaction(Settings165);
  //digitalWrite(SR_165_CLOCK, LOW);

  current_pads_1_8_value = SPI.transfer(0);
  current_pads_9_16_value = SPI.transfer(0);
  current_btns_value = SPI.transfer(0);

  //digitalWrite(SR_165_CLOCK, HIGH);
  SPI.endTransaction();

  byte SR_mask = 1;

  for (uint8_t btn = 0; btn < 8; btn++) {
    if ((current_pads_1_8_value & SR_mask) != (last_pads_1_8_value & SR_mask)) {
      if ((screenConfig == 1 && bank[currentBank].pattern[currentPattern].track_isGate[btn]) ||              // NORMAL MODE - TRACK IS GATE
          screenConfig == 6 ||                                                                               // DELETE STEPS MODE
          screenConfig == 8 ||                                                                               // ROLL MODE
          (screenConfig == 33 && bank[currentBank].pattern[currentPattern].track_isGate[selected_Track]) ||  // KBOARD MODE - TRACK IS GATE
          screenConfig == 34) {                                                                              // ARP MODE
        trigsIn_BitVal[btn] = (current_pads_1_8_value & SR_mask);
      }

      else
        trigsIn_BitVal[btn] = (current_pads_1_8_value & SR_mask) & !(last_pads_1_8_value & SR_mask);
    }


    SR_mask <<= 1;
  }

  SR_mask = 0;

  for (uint8_t btn = 8; btn < 16; btn++) {
    if ((current_pads_9_16_value & SR_mask) != (last_pads_9_16_value & SR_mask)) {
      if ((screenConfig == 1 && bank[currentBank].pattern[currentPattern].track_isGate[btn]) ||              // NORMAL MODE - TRACK IS GATE
          screenConfig == 6 ||                                                                               // DELETE STEPS MODE
          screenConfig == 8 ||                                                                               // ROLL MODE
          (screenConfig == 33 && bank[currentBank].pattern[currentPattern].track_isGate[selected_Track]) ||  // KBOARD MODE - TRACK IS GATE
          screenConfig == 34) {                                                                              // ARP MODE
        trigsIn_BitVal[btn] = (current_pads_9_16_value & SR_mask);
      }
    }


    SR_mask <<= 1;
  }

  SR_mask = 0;

  for (uint8_t btn = 0; btn < 8; btn++) {
    if ((current_btns_value & SR_mask) != (current_btns_value & SR_mask)) {
      btnsIn_BitVal[btn] = (current_btns_value & SR_mask) & !(last_btns_value & SR_mask);
    }


    SR_mask <<= 1;
  }


  // btnsIn_BitVal[btn - PADS] = bitRead(current_btns_value, btn) & !bitRead(last_btns_value, btn);

}*/


bool pad_long_pressed(uint8_t id) {
  return bitRead(last_pads_value, id) & bitRead(current_pads_value, id);
}


unsigned int decimalValue_from_pads() {
  uint16_t decimalValue_pads = 0;
  for (uint8_t i = TRACKS - 1, j = 0; i > 3; i--) {
    decimalValue_pads += trigsIn_BitVal[j++] << i;
  }

  return decimalValue_pads;
}


unsigned int decimalValue_from_pads2() {
  unsigned int decimalValue_pads = 0;
  for (uint8_t i = TRACKS - 1, j = 0; i > 3; i--) {
    decimalValue_pads += (trigsIn_BitVal[j] && bank[currentBank].pattern[currentPattern].track_isGate[j]) << i;
    j++;
  }
  return decimalValue_pads;
}


/*unsigned int decimalValue_from_chromaMode() {
  unsigned int decimalValue_chroma = 0;

  for (uint8_t i = DATA_WIDTH - 1, j = 0; i > 3; i--) {
    decimalValue_chroma += ((j == selected_Track) && padPushed[j]) << i;   // on force à sortir 1 uniquement sur selected_Track ET si trigsIn
    j++;
  }

  //decimalValue_chroma |= seqDecVal;

  return decimalValue_chroma;
  }*/

unsigned int decimalValue_from_chromaMode() {
  bool trigsIn_Chroma_BitVal[TRACKS];

  for (byte track = 0; track < TRACKS; ++track) {
    trigsIn_Chroma_BitVal[track] = 0;
  }

  trigsIn_Chroma_BitVal[selected_Track] = padPushed[selected_Track]; // on force le fait de ne sortir des trigs QUE sur selected_track

  unsigned int decimalValue_chroma = 0;

  for (byte i = PADS - 1, j = 0; i > 3; i--) {  //   int i = 15,   j = 0;    i > 3;    i--
    decimalValue_chroma += trigsIn_Chroma_BitVal[j++] << i;
  }

  return decimalValue_chroma;
}


unsigned int decimalValue_from_chromaMode2() {
  bool trigsIn_Chroma_BitVal[TRACKS];

  for (byte track = 0; track < TRACKS; ++track) {
    trigsIn_Chroma_BitVal[track] = 0;
  }

  trigsIn_Chroma_BitVal[selected_Track] = (padPushed[selected_Track] && bank[currentBank].pattern[currentPattern].track_isGate[selected_Track]); // on force le fait de ne sortir des trigs QUE sur selected_track

  unsigned int decimalValue_chroma = 0;

  for (byte i = PADS - 1, j = 0; i > 3; i--) {
    decimalValue_chroma += trigsIn_Chroma_BitVal[j++] << i;
  }

  return decimalValue_chroma;
}


void shiftOutTrigs(unsigned long value) {
  // value = 1011.0110 1101
  value *= 16;
  // value = 1011.0110 1101.0000
  digitalWrite(SR_Output_latch, LOW);
  shiftOut(SR_Output_data, SR_Output_clock, LSBFIRST, lowByte(value));
  shiftOut(SR_Output_data, SR_Output_clock, LSBFIRST, highByte(value));
  digitalWrite(SR_Output_latch, HIGH);
}


/*void make595Play_Trigs_from_Sequence(uint16_t decimVal_curStep, unsigned long decimVal_dur_curStep, unsigned long duration) {
  if (update_shiftRegisters) {
    shiftOutTrigs(decimVal_curStep);

    if ((micros() - duration) >= trig_DURATION) {
      update_shiftRegisters = false;
      shiftOutTrigs(decimVal_dur_curStep);
    }
  }
  }*/

/*void make595Play_Trigs_from_Sequence(uint16_t decimVal_curStep) {
  if (update_shiftRegisters) {
    shiftOutTrigs(decimVal_curStep);
    update_shiftRegisters = false;
  }
  }*/


void make595Play_Trigs_from_Sequence(uint16_t decimVal_curStep) {
  uint16_t seqDecVal = decimVal_curStep;

  if ((micros() - clock_time_16PPQN) >= trig_DURATION) {
    update_shiftRegisters[0] = true;
    seqDecVal = decimalValue_seq2;
  }

  if (update_shiftRegisters[0]) {
    shiftOutTrigs(seqDecVal);
    update_shiftRegisters[0] = false;
  }
}


void make595Play_Trigs_from_Pads(unsigned long playVal, unsigned long stopVal, unsigned long butMicro) {
  if (timeFlag_pads) {
    if (update_shiftRegisters[1]) {
      update_shiftRegisters[1] = false;
      shiftOutTrigs(playVal);
    }

    else if ((micros() - butMicro) >= trig_DURATION) {
      timeFlag_pads = false;
      shiftOutTrigs(stopVal);
    }
  }
}


void make595Play_Trigs_from_ChromaMode(unsigned long playVal, unsigned long stopVal, unsigned long butMicro, bool isGate) {
  if (timeFlag_pads) {
    if (update_shiftRegisters[2]) {
      update_shiftRegisters[2] = false;
      shiftOutTrigs(playVal);
    }

    else if (!isGate && ((micros() - butMicro) >= trig_DURATION)) {
      timeFlag_pads = false;
      shiftOutTrigs(stopVal);
    }
  }
}


void make595Play_Trigs_from_ChromaArp(unsigned long x, unsigned long butMicro) {
  if (timeFlag_arp) {
    shiftOutTrigs(x);

    if ((micros() - butMicro) >= trig_DURATION ) {
      shiftOutTrigs(0);
      timeFlag_arp = false;
    }
  }
}


void make595Play_Trigs_from_Roll(unsigned long x, unsigned long butMicro) {
  if (timeFlag_pads_roll) {
    //unsigned int trigDuration = (rollRate_micros <= trig_DURATION) ? 2000 : trig_DURATION;
    if (delayBeforeTrig == false) {
      shiftOutTrigs(x);
      canPass1 = 0;
      canPass2 = 0;


      unsigned long t1 = micros();
      if ((t1 - butMicro) >= trig_DURATION) {
        last_sr_roll_time1 = t1;
        shiftOutTrigs(0);
        delayBeforeTrig = true;
        canPass1 = 1;
        canPass2 = 0;
      }
    }

    else {
      if (canPass1) {
        unsigned long t2 = micros();
        if ((t2 - last_sr_roll_time1) >= rollRate_micros) {
          last_sr_roll_time2 = t2;
          shiftOutTrigs(x);
          canPass1 = 0;
          canPass2 = 1;
        }
      }

      if (canPass2) {
        unsigned long t3 = micros();
        if ((t3 - last_sr_roll_time2) >= trig_DURATION) {
          last_sr_roll_time1 = t3;
          shiftOutTrigs(0);
          canPass1 = 1;
          canPass2 = 0;
          timeFlag_pads_roll = false;
        }
      }

    }
  }
}



/*
  //Generate LFO waves
  //SINE :
  // Example program
  #include <iostream>
  #include <string>
  #include <math.h>

  int sine[256];
  int s;

  int main() {

  for (int i = 0; i < 256; i++ ) {

    float rads = 3.14159265359/180;
    s = (float)(32767+32767*sin(1.40625*i*rads));

    //s = 32768 + 32768 * sin( i * (3.14159265359 / 180.0));

    sine[i] = s;
  }


  for (const auto& e : sine) {
    std::cout << e << ", ";
  }
  }


  // RAMP UP :
  #include <iostream>
  #include <string>
  #include <math.h>

  int rampUp[256];
  int s;

  int main() {



  for (int i = 0; i < 256; i++)  //256
  {
    rampUp[i] = i*256;
  }




  for (const auto& e : rampUp) {
    std::cout << e << ", ";
  }
  }

*/
