unsigned int scale_transpose_value(int8_t pitchPtr) { // scale_transpose_value(pitchPointer_Transposed);
  if (pitchPtr < 0) {
    for (uint8_t x = 0; x < 2; ++x) {
      pitchPtr += 12;
      if (pitchPtr > -1)
        break;
    }
  }

  else {
    uint8_t arrSize = sizeof(notesValues) / sizeof(notesValues[0]);
    if (pitchPtr > arrSize - 1) {
      for (uint8_t x = 0; x < 5; ++x) {
        pitchPtr -= 12;
        if (pitchPtr < arrSize)
          break;
      }
    }
  }

  return pitchPtr;
}


void makeDACplayNotes() {
  for (uint8_t dac = 0; dac < 4; ++dac) {
    uint8_t track = bank[currentBank].pattern[currentPattern].cvOut_Tracks[dac];

    if (track < TRACKS) {
      uint8_t trackStep = get_track_step_PITCH(track, 0);
      uint8_t realTrack = track + bank[currentBankDisplay].pattern[currentPatternDisplay].AB_State[track];
      int8_t transpose_coeff = bank[currentBankDisplay].pattern[currentPatternDisplay].transposeValue[realTrack];

      int8_t pitchPointer_Transposed = bank[currentBankDisplay].pattern[currentPatternDisplay].Pitch[realTrack] [trackStep] + (12 * transpose_coeff);

      uint16_t bytePlaying_trigs = get_track_step_TRIGS(track, 0) / 8;
      uint8_t bytePlaying_bitPos = bytePlaying_trigs & 7;

      if (bank[currentBankDisplay].pattern[currentPatternDisplay].glide_steps[track] == 0) {
        if (!padPushed[track]) {
          if (keep_note) {
            pitchOut = notesValues[pitchPointer_Pad];

            if ((bank[currentBank].pattern[currentPattern].Trigs[realTrack][bytePlaying_trigs] & masks[bytePlaying_bitPos]) > 0)
              keep_note = false;
          }

          else
            pitchOut = notesValues[scale_transpose_value(pitchPointer_Transposed)];

          if (arp_latch && arp_latch_active)
            pitchOut = notesValues[arpNotes[arpCounter]];
        }

        else {
          if (chromaMode_KBoard)
            pitchOut = notesValues[pitchPointer_Pad];
          else if (chromaMode_Arp)
            pitchOut = notesValues[arpNotes[arpCounter]];
        }
      }

      else {
        if (timeFlag_glide && (DaC[dac].gliding == false)) {
          timeFlag_glide = false;
          uint8_t prevStep = (trackStep == bank[currentBankDisplay].pattern[currentPatternDisplay].firstStep) ? get_pattern_Length(4) - 1 : trackStep - 1;
          int8_t prev_pitchPointer_Transposed = bank[currentBankDisplay].pattern[currentPatternDisplay].Pitch[realTrack][prevStep] + (12 * transpose_coeff);
          int8_t current_pitchPointer_Transposed = pitchPointer_Transposed;


          if (prev_pitchPointer_Transposed != current_pitchPointer_Transposed) {
            DaC[dac].gliding = true;
            DaC[dac].startPitch = notesValues[scale_transpose_value(prev_pitchPointer_Transposed)];
            DaC[dac].endPitch = notesValues[scale_transpose_value(current_pitchPointer_Transposed)];

            DaC[dac].currentPitch = DaC[dac].startPitch;

            DaC[dac].glide_dir = (DaC[dac].startPitch < DaC[dac].endPitch) ? 0 : 1;  // 0 -> vers note + haute / 1 -> vers note + basse

            if (DaC[dac].glide_dir == 0)
              DaC[dac].incrementValue = round((DaC[dac].endPitch - DaC[dac].startPitch) / DaC[dac].glide_duration_micros);
            else
              DaC[dac].incrementValue = round((DaC[dac].startPitch - DaC[dac].endPitch) / DaC[dac].glide_duration_micros);
          }

          else {
            DaC[dac].gliding = false;
            DaC[dac].currentPitch = notesValues[scale_transpose_value(pitchPointer_Transposed)];
          }
        }

        if (DaC[dac].gliding) {
          if (glidePulse == LOW) {
            glidePulse = HIGH;

            if (DaC[dac].glide_dir == 0) {
              if (DaC[dac].currentPitch <= (DaC[dac].endPitch - DaC[dac].incrementValue)) {
                //if (micros() - DaC[dac].glide_trig_micros < DaC[dac].glide_duration_micros) {
                DaC[dac].currentPitch += DaC[dac].incrementValue;

              }
              else {
                DaC[dac].currentPitch = DaC[dac].endPitch;
                DaC[dac].gliding = false;
              }
            }

            else {
              if (DaC[dac].currentPitch >= (DaC[dac].endPitch + DaC[dac].incrementValue)) {
                //if (micros() - DaC[dac].glide_trig_micros < DaC[dac].glide_duration_micros) {
                DaC[dac].currentPitch -= DaC[dac].incrementValue;
              }
              else {
                DaC[dac].currentPitch = DaC[dac].endPitch;
                DaC[dac].gliding = false;
              }
            }
          }
        }

        pitchOut = DaC[dac].currentPitch;
      }
    }


    else if (track == 12) {    // desactivé
      pitchOut = notesValues[24];
    }


    else { // 13 = LFO1 ou 14 = LFO2
      uint8_t lfoNum = (track == 14);  // lfNum = 1 si 14, 0 si 13;
      int lfoPointer = LFO[lfoNum].counter + bank[currentBank].pattern[currentPattern].lfo_phase[lfoNum];

      if (lfoPointer >= LFO_WAVE_RESOLUTION)
        lfoPointer = bank[currentBank].pattern[currentPattern].lfo_phase[lfoNum] - (LFO_WAVE_RESOLUTION - LFO[lfoNum].counter);

      else if (lfoPointer < 0)
        lfoPointer += LFO_WAVE_RESOLUTION; // lfoPointer = LFO_WAVE_RESOLUTION - (-lfoPointer);

      switch (get_LFO_wave_Display(lfoNum)) {
        case 0 : // Ramp Up
          pitchOut = rampUp[lfoPointer];
          break;

        case 1 : // Ramp Down
          pitchOut = rampUp[(LFO_WAVE_RESOLUTION - 1) - lfoPointer];  // [0, 1, 2, 3, 4, ..., 251, 252, 253, 254, 255]
          break;

        case 2 : // Sine
          pitchOut = sine[lfoPointer];
          break;

        case 3 : // Triangle
          pitchOut = triangle[lfoPointer];
          break;

        case 4 : // Square
          pitchOut = LFO[lfoNum].squareVal;
          break;

        case 5 : // Random
          pitchOut = LFO[lfoNum].randomVal;
          break;
      }

      if (bank[currentBankDisplay].pattern[currentPatternDisplay].lfo_gain[lfoNum] != 100) {
        pitchOut -= (pitchOut - 26200.0) * (1 - (bank[currentBankDisplay].pattern[currentPatternDisplay].lfo_gain[lfoNum] / 100.0));
      }
    }

    Dac.setValue(dac, -(pitchOut));
  }
}



/*
  void makeDACplayNotes_from_Chroma_Arp(uint8_t pitchVal, uint8_t track) { //pitch_pointer_pads
  uint8_t dacPLAYING = 0;
  for (uint8_t dac = 0; dac < 4; ++dac) {
    if (bank[currentBank].pattern[currentPattern].cvOut_Tracks[dac] == track) {
      dacPLAYING = dac;
      break;
    }
  }

  int pitchOut = notesValues[pitchVal];
  Dac.setValue(dacPLAYING, -(pitchOut));

  }
*/
