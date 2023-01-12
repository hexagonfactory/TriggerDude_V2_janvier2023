// 256  512  1024 2048
// 16   32   64   128
// 1    2    4    8

void setPadsActions_normalMode() {
  static unsigned long button_micro = 0;

  if ((current_pads_value != last_pads_value) && ((millis() - last_btn_DebounceTime_millis) > DEBOUNCE_BTNS_MILLIS)) {
    last_btn_DebounceTime_millis = millis();

    if (current_pads_value == 0)
      timeFlag_pads = false;

    else /*if (current_pads_value < 4096)*/ {
      button_micro = micros();
      timeFlag_pads = true;
      update_shiftRegisters[1] = true;
      update_screen_PADS = true;
    }

    update_padLeds = true;
  }

  if ((current_pads_value > 0)) {
    for (uint8_t track = 0; track < TRACKS; ++track) {
      if (trigsIn_BitVal[track] && (millis() - last_pads_DebounceTime_millis[track] >= DEBOUNCE_PADS_MILLIS)) {
        last_pads_DebounceTime_millis[track] = millis();

        if (current_pads_value != last_pads_value) {
          selected_Track = track;
          read_Seq_dont_play_this[track] = false;
        }

        if (bank[currentBank].pattern[currentPattern].track_isGate[track])
          timeFlag_pads = true;

        if (!SHIFT_pushed && recMode && PLAY_pushed) {
          uint16_t stepToWrite = get_track_step_TRIGS(track, 0);
          uint8_t bitPos_trigs = stepToWrite & 7;

          if (!pad_long_pressed(track)) {
            if (bank[currentBank].pattern[currentPattern].quantizeBeat) {
              if (bitPos_trigs == 1)
                bitPos_trigs = 0;
              else if (bitPos_trigs == 2 || bitPos_trigs == 3) {
                bitPos_trigs = 4;
                read_Seq_dont_play_this[track] = true;
              }
              else if (bitPos_trigs == 5)
                bitPos_trigs = 4;
              else if (bitPos_trigs == 6 || bitPos_trigs == 7) {
                bitPos_trigs = 0;
                stepToWrite++;
                read_Seq_dont_play_this[track] = true;
              }
            }
          }

          uint8_t realTrack = track + bank[currentBankDisplay].pattern[currentPatternDisplay].AB_State[track];

          if (!pad_long_pressed(track)) {
            bank[currentBankDisplay].pattern[currentPatternDisplay].track_has_Trigs[realTrack / 8] |= masks[realTrack & 7];
            bank[currentBankDisplay].notEmpty_Pattern[currentPatternDisplay] = 1;
          }

          bank[currentBankDisplay].pattern[currentPatternDisplay].Trigs[realTrack][stepToWrite / 8] |= masks[bitPos_trigs];
        }

      }
    }
  }

  if (!SHIFT_pushed)
    make595Play_Trigs_from_Pads(decimalValue_from_pads(), decimalValue_from_pads2(), button_micro);

}




void setPadsActions_rollMode() { // ROLL MODE
  unsigned long button_micro = 0;
  if ((current_pads_value > 0) && (current_pads_value < 4096)) {

    if (current_pads_value != last_pads_value) {
      button_micro = micros();
      delayBeforeTrig = false;

      for (uint8_t track = 0; track < TRACKS; ++track) {
        if (trigsIn_BitVal[track] == 1) {
          selected_Track = track;
          reset_ChromaMode_UI();
          break;
        }
      }
    }

    for (uint8_t track = 0; track < TRACKS; ++track) {
      if (trigsIn_BitVal[track] && ( (millis() - last_pads_DebounceTime_millis[track]) >= DEBOUNCE_PADS_MILLIS)) {
        last_pads_DebounceTime_millis[track] = millis();
        timeFlag_pads_roll = true;
        update_padLeds = true;
      }
    }


    make595Play_Trigs_from_Roll(decimalValue_from_pads(), button_micro);

  }

  //update_padLeds = true;
  //update_screen = true;   TESTER REMETTRE ?
}


void setPadsActions_ChromaMode_KBoard() { // CHROMA MODE
  static unsigned long button_micro = 0;

  if ((current_pads_value != last_pads_value) && (current_pads_value < 4096)) {
    padPushed[selected_Track] = 0;

    if (current_pads_value > 0) {
      //if (!bank[currentBank].pattern[currentPattern].track_isGate[selected_Track])
      button_micro = micros();
      update_shiftRegisters[2] = true;
      timeFlag_pads = true;
      timeFlag_glide = true;
      padPushed[selected_Track] = 1;
      read_Seq_dont_play_this[selected_Track] = false;
      keep_note = true;
    }
  }

  if (current_pads_value > 0 && (current_pads_value < 4096)) {

    if (bank[currentBank].pattern[currentPattern].track_isGate[selected_Track])
      timeFlag_pads = true;

    for (uint8_t pad = 0; pad < PADS; ++pad) {
      if (trigsIn_BitVal[pad]) {
        pitchPointer_Pad = pad + octaveValue;
        break;
      }
    }

    if (recMode && PLAY_pushed) {
      uint16_t stepToWrite_TRIGS = get_track_step_TRIGS(selected_Track, 0);
      uint8_t bitPos_trigs = stepToWrite_TRIGS & 7;
      uint8_t stepAdvance = 0;

      if (current_pads_value != last_pads_value) {
        if (bank[currentBank].pattern[currentPattern].quantizeBeat) {
          if (bitPos_trigs == 1) {
            bitPos_trigs = 0;
          }
          else if (bitPos_trigs == 2 || bitPos_trigs == 3) {
            bitPos_trigs = 4;
            stepAdvance = 1;
            read_Seq_dont_play_this[selected_Track] = true;
          }
          else if (bitPos_trigs == 5)
            bitPos_trigs = 4;
          else if (bitPos_trigs == 6 || bitPos_trigs == 7) {
            bitPos_trigs = 0;
            stepAdvance = 1;
            stepToWrite_TRIGS++;
            read_Seq_dont_play_this[selected_Track] = true;
          }
        }
      }

      uint8_t realTrack = selected_Track + bank[currentBankDisplay].pattern[currentPatternDisplay].AB_State[selected_Track];
      uint8_t byteToWrite_trigs = stepToWrite_TRIGS / 8;
      //uint16_t stepToWrite_PITCH = (bank[currentBank].pattern[currentPattern].tracksRetrig) ? get_global_step_PITCH(stepAdvance) : get_track_step_PITCH(selected_Track, stepAdvance);
      uint16_t stepToWrite_PITCH = get_track_step_PITCH(selected_Track, stepAdvance);

      uint8_t byteToWrite_notEmpty = realTrack / 8;
      uint8_t bitPos_notEmpty = realTrack & 7;

      bank[currentBankDisplay].pattern[currentPatternDisplay].Trigs[realTrack][byteToWrite_trigs] |= masks[bitPos_trigs];

      bank[currentBankDisplay].pattern[currentPatternDisplay].Pitch[realTrack][stepToWrite_PITCH] = pitchPointer_Pad;
      bank[currentBankDisplay].pattern[currentPatternDisplay].track_has_Trigs[byteToWrite_notEmpty] |= masks[bitPos_notEmpty];
      bank[currentBankDisplay].notEmpty_Pattern[currentPatternDisplay] = 1;


      for (uint16_t x = stepToWrite_TRIGS + 1; x < get_pattern_Length(16) - 1; ++x) {
        if (!((bank[currentBankDisplay].pattern[currentPatternDisplay].Trigs[realTrack][x / 8] & masks[x & 7]) > 0))
          bank[currentBankDisplay].pattern[currentPatternDisplay].Pitch[realTrack][x / 4] = pitchPointer_Pad;
        else
          break;
      }
    }

  }

  make595Play_Trigs_from_ChromaMode(decimalValue_from_chromaMode(), decimalValue_from_chromaMode2(), button_micro, bank[currentBank].pattern[currentPattern].track_isGate[selected_Track]);
}



void setPadsActions_ChromaMode_Arp() {
  if (((current_pads_value != last_pads_value) && (current_pads_value < 4096)) && ((millis() - last_pad_push) >= 30)) {
    last_pad_push = millis();
    timeFlag_arp = true;

    scan_pads_Arp = 77;

    if (current_pads_value > last_pads_value) {
      needToCheck_Arp = 0;
      allowToCheck_Arp = 0;
      scan_Pads_Arp();
    }

    else {
      if (!arp_latch) {
        needToCheck_Arp = 0;
        allowToCheck_Arp = 0;
        scan_Pads_Arp();
      }

      else {
        datime = 0;
        needToCheck_Arp = 1;
      }
    }

    padPushed[selected_Track] = 1;

    newArp = (last_pads_value == 0) ? true : false;

    if (newArp) {
      arpCounter = 0;
    }

    if (arp_latch) {
      arp_latch_active = 1;
    }

    update_padLeds = true;
  }


  if (arp_latch && needToCheck_Arp && allowToCheck_Arp) {
    datime = millis();
    scan_pads_Arp = 99;
    if (datime - last_datime >= 600) {
      last_datime = datime;

      if (current_pads_value > 0) {
        scan_Pads_Arp();
        needToCheck_Arp = 0;
        allowToCheck_Arp = 0;
      }

      /* else {
         padPushed[selected_Track] = 0;
         newArp = true;
         arpSteps = 0;
        }*/
    }
  }

  if (needToCheck_Arp)
    allowToCheck_Arp = 1;


  if (((current_pads_value > 0) && (current_pads_value < 4096)) || arp_latch_active) {
    padPushed[selected_Track] = 1;
    if (recMode && /*PLAY_pushed*/PLAYING) {
      uint16_t stepToWrite_TRIGS = get_track_step_TRIGS(selected_Track, 0);
      //uint16_t stepToWrite_TRIGS = (bank[currentBank].pattern[currentPattern].tracksRetrig) ? get_global_step_TRIGS(0) : get_track_step_TRIGS(selected_Track, 0);
      uint8_t bitPos_trigs = stepToWrite_TRIGS & 7;
      uint8_t stepAdvance = 0;

      if (bitPos_trigs == 1) {
        bitPos_trigs = 0;
      }
      else if (bitPos_trigs == 2 || bitPos_trigs == 3) {
        bitPos_trigs = 4;
        stepAdvance = 1;
        read_Seq_dont_play_this[selected_Track] = true;
      }
      else if (bitPos_trigs == 5)
        bitPos_trigs = 4;
      else if (bitPos_trigs == 6 || bitPos_trigs == 7) {
        bitPos_trigs = 0;
        stepAdvance = 1;
        stepToWrite_TRIGS++;
        read_Seq_dont_play_this[selected_Track] = true;
      }


      uint8_t realTrack = selected_Track + bank[currentBank].pattern[currentPattern].AB_State[selected_Track];
      uint8_t byteToWrite_trigs = stepToWrite_TRIGS / 8;
      uint16_t stepToWrite_PITCH = get_track_step_PITCH(selected_Track, stepAdvance);
      //uint16_t stepToWrite_PITCH = (bank[currentBank].pattern[currentPattern].tracksRetrig) ? get_global_step_PITCH(stepAdvance) : get_track_step_PITCH(selected_Track, stepAdvance);

      uint8_t byteToWrite_notEmpty = realTrack / 8;
      uint8_t bitPos_notEmpty = realTrack & 7;

      bank[currentBank].pattern[currentPattern].Trigs[realTrack][byteToWrite_trigs] |= masks[bitPos_trigs];

      bank[currentBank].pattern[currentPattern].Pitch[realTrack][stepToWrite_PITCH] = arpNotes[arpCounter];
      bank[currentBank].pattern[currentPattern].track_has_Trigs[byteToWrite_notEmpty] |= masks[bitPos_notEmpty];
      bank[currentBank].notEmpty_Pattern[currentPattern] = 1;
    }


    make595Play_Trigs_from_ChromaArp(decimalValue_from_chromaMode(), arp_Time);
  }

}


void setPadsActions_ChromaMode_Transpose() {
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 129)) {  // que les 8 premiers pads

      for (uint8_t pad = 0; pad < 8; ++pad) { // // que les 8 premiers pads car 8 étages de transposition
        if (trigsIn_BitVal[pad] && ( (millis() - last_pads_DebounceTime_millis[pad]) >= DEBOUNCE_PADS_MILLIS)) {
          last_pads_DebounceTime_millis[pad] = micros();

          uint8_t realTrack = selected_Track + bank[currentBank].pattern[currentPattern].AB_State[selected_Track];
          bank[currentBank].pattern[currentPattern].transposeValue[realTrack] = pad - 2;

        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
  }
}


void setPadsActions_ChromaMode_CVoutAssign() {
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 9)) { // que les 4 premiers pads
      for (uint8_t pad = 0; pad < 4; ++pad) {
        if (trigsIn_BitVal[pad] && ( (millis() - last_pads_DebounceTime_millis[pad]) >= DEBOUNCE_PADS_MILLIS)) {
          last_pads_DebounceTime_millis[pad] = micros();
          cvOutputPad = pad;
          assign_CVout_track(cvOutputPad);
        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
  }
}


void setPadsActions_stepSeq() { // STEP SEQUENCER MODE
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 4096)) {
      //unsigned long btnMicros = micros();

      uint8_t realTrack = selected_Track + step_seq_ab[selected_Track];
      uint8_t byteToRead = 0;
      uint8_t byteToWrite_notEmpty = realTrack / 8;
      uint8_t bitPos_notEmpty = realTrack & 7;
      uint16_t stepSeq_page_16 = stepSeq_page * 4;

      for (uint8_t pad = 0; pad < TRACKS; ++pad) {
        if (trigsIn_BitVal[pad] /*&& ((btnMicros - last_pads_DebounceTime_millis[pad]) >= DEBOUNCE_PADS_MILLIS)*/) {
          //last_pads_DebounceTime_millis[pad] = btnMicros;

          bool padHadTrigs = false;
          static uint8_t index = 0;
          static uint8_t limit = 4;

          if (pad > 3) {
            if (pad == 4 || pad == 6 || pad == 8 || pad == 10) {
              index = 0;
              limit = 4;
            }
            else {
              index = 4;
              limit = 8;
            }
            /*if (pad == 5 || pad == 7 || pad == 9 || pad == 11) {
              index = 4;
              limit = 8;
              }*/

            uint16_t stepSeq_global_Step_16PPQN = (pad < 8) ? (pad * 4) + stepSeq_page_16 : ((pad - 8) * 4) + stepSeq_page_16;
            byteToRead = stepSeq_global_Step_16PPQN / 8;


            for (uint8_t i = index; i < limit; ++i) {
              /*if (!((bank[currentBank].pattern[currentPattern].Trigs[realTrack][byteToRead] & masks[i]) > 0)) { // si trig est = 0
                padHadTrigs = false;
                }
                else { // si trig = 1
                padHadTrigs = true;
                break;
                }*/

              if ((bank[currentBank].pattern[currentPattern].Trigs[realTrack][byteToRead] & masks[i]) > 0) { // si trig est = 1
                padHadTrigs = true;
                break;
              }
            }

            if (!MICROTIMING) {
              if (padHadTrigs) {
                for (uint8_t i = index; i < limit; ++i) {
                  bank[currentBank].pattern[currentPattern].Trigs[realTrack][byteToRead] &= ~masks[i]; // on efface les trigs
                }
              }
              else {
                bank[currentBank].pattern[currentPattern].Trigs[realTrack][byteToRead] |= masks[index]; // on ajoute 1 à bitPos
                bank[currentBank].pattern[currentPattern].track_has_Trigs[byteToWrite_notEmpty] |= masks[bitPos_notEmpty];
                bank[currentBank].notEmpty_Pattern[currentPattern] = 1;
              }
            }
            else {
              if (stepSeq_clickedStep != pad) {
                stepSeq_clickedStep = pad;
                screenConfig = 11;
              }
              else {
                stepSeq_clickedStep = 255;
                screenConfig = 10;
              }
            }

            selectedSeqStep_byte = byteToRead;
            selectedSeqStep_bitPos = index;
          }


          else { // si pad < 4
            if (MICROTIMING) {
              uint8_t selectedSeqStep_bitPos_pad = pad + index;

              if (!((bank[currentBank].pattern[currentPattern].Trigs[realTrack][selectedSeqStep_byte] & masks[selectedSeqStep_bitPos_pad]) > 0)) { // si trig est = 0
                bank[currentBank].pattern[currentPattern].Trigs[realTrack][selectedSeqStep_byte] |= masks[selectedSeqStep_bitPos_pad];
                bank[currentBank].pattern[currentPattern].track_has_Trigs[byteToWrite_notEmpty] |= masks[bitPos_notEmpty];
                bank[currentBank].notEmpty_Pattern[currentPattern] = 1;
              }
              else { // si trig = 1
                padHadTrigs = true;
                bank[currentBank].pattern[currentPattern].Trigs[realTrack][selectedSeqStep_byte] &= ~masks[selectedSeqStep_bitPos_pad];
              }
            }
          }

          if (padHadTrigs) {
            for (uint16_t i = 0; i < MAX_STEPS_16PPQN; ++i) {
              uint8_t byteToRead = i / 8;
              uint8_t bitPos = i & 7;
              if ((bank[currentBank].pattern[currentPattern].Trigs[realTrack][byteToRead] & masks[bitPos]) > 0) {
                bank[currentBank].pattern[currentPattern].track_has_Trigs[byteToWrite_notEmpty] |= masks[bitPos_notEmpty];
                break;
              }
              else {
                bank[currentBank].pattern[currentPattern].track_has_Trigs[byteToWrite_notEmpty] &= ~masks[bitPos_notEmpty];
                checkIf_patternEmpty(currentBank, currentPattern);
              }
            }
          }
          /*else {
            bank[currentBank].pattern[currentPattern].track_has_Trigs[byteToWrite_notEmpty] |= masks[bitPos_notEmpty];
            bank[currentBank].notEmpty_Pattern[currentPattern] = 1;
            }*/
        }
      }

      update_padLeds = true;
    }
  }
}


void setPadsActions_pitchSeq() { // PITCH SEQUENCER MODE
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 4096)) {

      for (uint8_t pad = 0; pad < TRACKS; ++pad) {
        if (trigsIn_BitVal[pad]) {

          if (pad < 4) {
            uint8_t cvTrack = bank[currentBank].pattern[currentPattern].cvOut_Tracks[pad];
            if (cvTrack == 12)
              bank[currentBank].pattern[currentPattern].cvOut_Tracks[pad] = selected_Track;
            else if (cvTrack == selected_Track) {
              bank[currentBank].pattern[currentPattern].cvOut_Tracks[pad] = 12;
            }
          }

          else {
            uint8_t realTrack = selected_Track + step_seq_ab[selected_Track];

            if (stepSeq_clickedStep != pad) {
              stepSeq_clickedStep = pad;
              stepSeq_global_Step_4PPQN = (pad < 8) ? pad + stepSeq_page : (pad - 8) + stepSeq_page;
              pitchPointer_Pot = bank[currentBank].pattern[currentPattern].Pitch[realTrack] [stepSeq_global_Step_4PPQN];

            }
            else {
              stepSeq_clickedStep = 255;
            }
            break;
          }

        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
  }
}


void setPadsActions_bankPatternSel() { // BANK - PATTERN SELECTION
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 4096)) {

      for (uint8_t pad = 0; pad < PATTERNS; ++pad) {
        if (trigsIn_BitVal[pad]) {
          if (screenConfig == 2) { //------------- PATTERN
            if (PLAYING && patternChange_Quantized) {
              nextPattern = pad;
              patternChange_triggered = true;
            }

            else {
              currentPattern = pad;
              loadPattern_params();
            }
            break;
          }
          else if (screenConfig == 22) { //------------- BANK
            //currentBank = track;  A DEBLOQUER QUAND ASSEZ DE STOCKAGE POUR GERER BANKS
            currentBankLED = pad;
            break;
          }
        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
      reset_ChromaMode_UI();
    }
  }
}


void setPadsActions_patternEditMode() { // PATTERN EDIT
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 4096) ) {

      for (uint8_t track = 0; track < PATTERNS; ++track) {
        if (trigsIn_BitVal[track]) {
          if (!patternToEdit_readyToAction) {
            patternSelected = track;
            break;
          }
          else {
            patternDestination = track;
            break;
          }
        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
  }
}


void setPadsActions_bankEditMode() { // BANK EDIT
  if (current_pads_value != last_pads_value) {
    if ( (current_pads_value > 0) && (current_pads_value < 4096) ) {

      for (uint8_t pad = 0; pad < BANKS; ++pad) {
        if (trigsIn_BitVal[pad]) {
          if (!bankToEdit_readyToAction) {
            bankSelected = pad;
            break;
          }
          else {
            bankDestination = pad;
            break;
          }
        }
      }

      update_padLeds = true;
      //update_screen = true;
    }
  }
}


void setPadsActions_songMode() { // PATTERN SELECTION on SONG MODE STEP
  if (current_pads_value != last_pads_value) {
    if ( (current_pads_value > 0) && (current_pads_value < 4096) ) {

      for (uint8_t pad = 0; pad < PATTERNS; ++pad) {
        if (trigsIn_BitVal[pad]) {
          bank[currentBank].songPatterns[songMode_current_Step_pot] = pad;
          break;
        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
  }
}


void setPadsActions_ABState() { // AB - LENGTH
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 4096)) {
      for (uint8_t track = 0; track < TRACKS; ++track) {
        if (trigsIn_BitVal[track]) {
          uint8_t x = bank[currentBank].pattern[currentPattern].AB_State[track];
          bank[currentBank].pattern[currentPattern].AB_State[track] = (x == 0) ? 12 : 0;
        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
  }
}


void setPadsActions_trackLength() {
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 4096)) {

      for (uint8_t track = 0; track < TRACKS; ++track) {
        if (trigsIn_BitVal[track]) {
          selected_Track = track;
          //track_length = bank[currentBank].pattern[currentPattern].track_Length[selected_Track];
          break;
        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
  }
}


void setPadsActions_muteSolo() { // MUTE - SOLO
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 4096)) {

      for ( uint8_t track = 0; track < TRACKS; ++track) {
        if (trigsIn_BitVal[track] && ( (millis() - last_pads_DebounceTime_millis[track]) >= DEBOUNCE_PADS_MILLIS)) {
          last_pads_DebounceTime_millis[track] = millis();

          if (screenConfig == 4) { //------------- MUTE
            uint8_t byteToWrite = track / 8;
            uint8_t bitPos = track & 7;

            if ((bank[currentBank].pattern[currentPattern].mute_solo_States[byteToWrite] & masks[bitPos]) > 0) {
              bank[currentBank].pattern[currentPattern].mute_solo_States[byteToWrite] &= ~masks[bitPos];
              bank[currentBank].pattern[currentPattern].allMute_active = 0;
              for (uint8_t i = 0; i < TRACKS; ++i) {
                uint8_t byteToRead = i / 8;
                uint8_t bitPos = i & 7;
                if ((bank[currentBank].pattern[currentPattern].mute_solo_States[byteToRead] & masks[bitPos]) > 0) {
                  bank[currentBank].pattern[currentPattern].mute_active = 1;
                  break;
                }
                else {
                  bank[currentBank].pattern[currentPattern].mute_active = 0;
                }
              }
            }
            else {
              bank[currentBank].pattern[currentPattern].mute_solo_States[byteToWrite] |= masks[bitPos];
              bank[currentBank].pattern[currentPattern].mute_active = 1;
            }
          }

          else if (screenConfig == 44) { //------------- SOLO
            uint8_t byteToWrite = (track + 12) / 8;
            uint8_t bitPos = (track + 12) & 7;

            if ((bank[currentBank].pattern[currentPattern].mute_solo_States[byteToWrite] & masks[bitPos]) > 0) {
              bank[currentBank].pattern[currentPattern].mute_solo_States[byteToWrite] &= ~masks[bitPos];
              for (uint8_t i = 0; i < TRACKS; ++i) {
                uint8_t byteToRead = (i + 12) / 8;
                uint8_t bitPos = (i + 12) & 7;
                if ((bank[currentBank].pattern[currentPattern].mute_solo_States[byteToRead] & masks[bitPos]) > 0) {
                  bank[currentBank].pattern[currentPattern].solo_active = 1;
                  break;
                }
                else {
                  bank[currentBank].pattern[currentPattern].solo_active = 0;
                }
              }
            }

            else {
              bank[currentBank].pattern[currentPattern].mute_solo_States[byteToWrite] |= masks[bitPos];
              bank[currentBank].pattern[currentPattern].solo_active = 1;
            }
          }
        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
  }
}


void setPadsActions_Fill() {
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 4096)) {
      update_padLeds = true;
      update_screen_PADS = true;
    }

    for (uint8_t track = 0; track < TRACKS; ++track) {
      if (trigsIn_BitVal[track]) {
        if (!SHIFT_pushed) {
          uint8_t byteToRead = track / 8;
          uint8_t bitPos = track & 7;
          if ((bank[currentBankDisplay].pattern[currentPatternDisplay].fillState[byteToRead] & masks[bitPos]) > 0) {
            bank[currentBankDisplay].pattern[currentPatternDisplay].fillState[byteToRead] &= ~masks[bitPos];

            for (uint8_t i = 0; i < TRACKS; ++i) {
              uint8_t byteToRead = i / 8;
              uint8_t bitPos = i & 7;
              if ((bank[currentBankDisplay].pattern[currentPatternDisplay].fillState[byteToRead] & masks[bitPos]) > 0) {
                bank[currentBankDisplay].pattern[currentPatternDisplay].fill_active = 1;
                break;
              }
              else {
                bank[currentBankDisplay].pattern[currentPatternDisplay].fill_active = 0;
              }
            }
          }
          else {
            bank[currentBankDisplay].pattern[currentPatternDisplay].fillState[byteToRead] |= masks[bitPos];
            bank[currentBankDisplay].pattern[currentPatternDisplay].fill_active = 1;
          }
        }

        uint8_t selTrack = selected_Track;
        selected_Track = track;
        if (selTrack != selected_Track) {
          fillDataRecording_startStep = 0;
          recMode = 0;
        }

      }
    }

    //update_padLeds = true;
    //update_screen_PADS = true;
  }
}


void setPadsActions_Shift() { // FILL - SHIFT
  if (current_pads_value != last_pads_value) {
    if ((current_pads_value > 0) && (current_pads_value < 4096)) {

      for (uint8_t track = 0; track < TRACKS; ++track) {
        if (trigsIn_BitVal[track] && ( (millis() - last_pads_DebounceTime_millis[track]) >= DEBOUNCE_PADS_MILLIS)) {
          last_pads_DebounceTime_millis[track] = millis();
          selected_Track = track;
          break;
        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
  }
}


void setPadsActions_deleteSteps() {
  if ((current_pads_value > 0) && (current_pads_value < 4096)) {

    for (uint8_t track = 0; track < TRACKS; ++track) {
      if (trigsIn_BitVal[track] && ( (millis() - last_pads_DebounceTime_millis[track]) >= DEBOUNCE_PADS_MILLIS)) {
        last_pads_DebounceTime_millis[track] = millis();
        pads_micros[track] = last_pads_DebounceTime_millis[track];

        //bool stepAdvance = ((pads_micros[track] - clock_time_16PPQN) > (step_micros / 2)) ? 1 : 0;
        uint16_t trackStep = get_track_step_TRIGS(track, ((pads_micros[track] - clock_time_16PPQN) > (step_micros / 2)));

        uint8_t realTrack = track + bank[currentBank].pattern[currentPattern].AB_State[track];

        switch (clear_trigsOrPitch_cursor) {
          case 0: // BOTH
            clearSequence(currentBankDisplay, currentPatternDisplay, realTrack, trackStep, 1, 1);
            break;

          case 1: // TRIGS
            clearSequence(currentBankDisplay, currentPatternDisplay, realTrack, trackStep, 1, 0);
            break;

          case 2:
            clearSequence(currentBankDisplay, currentPatternDisplay, realTrack, trackStep, 0, 1);
            break;
        }

        bool track_has_Trigs_changed = 0;
        for (uint16_t i = 0; i < MAX_STEPS_16PPQN; ++i) {
          uint8_t byteToRead_trigs = i / 8;
          uint8_t bitPos_trigs = i & 7;
          uint8_t byteToWrite_notEmpty = realTrack / 8;
          uint8_t bitPos_notEmpty = realTrack & 7;
          if ((bank[currentBankDisplay].pattern[currentPatternDisplay].Trigs[realTrack][byteToRead_trigs] & masks[bitPos_trigs]) > 0) {
            bank[currentBankDisplay].pattern[currentPatternDisplay].track_has_Trigs[byteToWrite_notEmpty] |= masks[bitPos_notEmpty];
            break;
          }
          else {
            if ((i == (MAX_STEPS_16PPQN - 1)) && !((bank[currentBankDisplay].pattern[currentPatternDisplay].Trigs[realTrack][byteToRead_trigs] & masks[bitPos_trigs]) > 0)) {
              bank[currentBankDisplay].pattern[currentPatternDisplay].track_has_Trigs[byteToWrite_notEmpty] &= ~masks[bitPos_notEmpty];
              track_has_Trigs_changed = 1;
            }
          }
        }
        if (track_has_Trigs_changed) {
          checkIf_patternEmpty(currentBankDisplay, currentPatternDisplay);
        }
      }

    }
    update_screen_PADS = true;
  }
}


void setPadsActions_clearTrack() {
  if (current_pads_value != last_pads_value) {
    if ( (current_pads_value > 0) && (current_pads_value < 4096) ) {
      for (uint8_t track = 0; track < TRACKS; ++track) {
        if (trigsIn_BitVal[track] && ((millis() - last_pads_DebounceTime_millis[track]) >= DEBOUNCE_PADS_MILLIS)) {
          last_pads_DebounceTime_millis[track] = millis();
          for (uint16_t i = 0; i < MAX_STEPS_16PPQN; ++i) {
            switch (clear_trigsOrPitch_cursor) {
              case 0: // BOTH
                if (clear_SelectTrack_Cursor == 0 || clear_SelectTrack_Cursor == 1) {
                  clearSequence(currentBankDisplay, currentPatternDisplay, track, i, 1, 1);
                  if (clear_SelectTrack_Cursor == 0) {
                    clearSequence(currentBankDisplay, currentPatternDisplay, track + 12, i, 1, 1);
                  }
                }
                else {
                  clearSequence(currentBankDisplay, currentPatternDisplay, track + 12, i, 1, 1);
                }
                break;

              case 1: // TRIGS
                if (clear_SelectTrack_Cursor == 0 || clear_SelectTrack_Cursor == 1) {
                  clearSequence(currentBankDisplay, currentPatternDisplay, track, i, 1, 0);
                  if (clear_SelectTrack_Cursor == 0) {
                    clearSequence(currentBankDisplay, currentPatternDisplay, track + 12, i, 1, 0);
                  }
                }
                else {
                  clearSequence(currentBankDisplay, currentPatternDisplay, track + 12, i, 1, 0);
                }
                break;

              case 2: // PITCH
                if (clear_SelectTrack_Cursor == 0 || clear_SelectTrack_Cursor == 1) {
                  clearSequence(currentBankDisplay, currentPatternDisplay, track, i, 0, 1);
                  if (clear_SelectTrack_Cursor == 0) {
                    clearSequence(currentBankDisplay, currentPatternDisplay, track + 12, i, 0, 1);
                  }
                }
                else {
                  clearSequence(currentBankDisplay, currentPatternDisplay, track + 12, i, 0, 1);
                }
                break;
            }
          }

          if (clear_SelectTrack_Cursor == 0 || clear_SelectTrack_Cursor == 1) { // BOTH OU A
            uint8_t byteToWrite_notEmpty = track / 8;
            uint8_t bitPos_notEmpty = track & 7;
            bank[currentBankDisplay].pattern[currentPatternDisplay].track_has_Trigs[byteToWrite_notEmpty] &= ~masks[bitPos_notEmpty];
            if (clear_SelectTrack_Cursor == 0) {
              uint8_t byteToWrite_notEmpty = (track + 12) / 8;
              uint8_t bitPos_notEmpty = (track + 12) & 7;
              bank[currentBankDisplay].pattern[currentPatternDisplay].track_has_Trigs[byteToWrite_notEmpty] &= ~masks[bitPos_notEmpty];
            }
          }
          else { // B
            uint8_t byteToWrite_notEmpty = (track + 12) / 8;
            uint8_t bitPos_notEmpty = (track + 12) & 7;
            //bank[currentBankDisplay].pattern[currentPatternDisplay].track_has_Trigs[track + 12] = 0;
            bank[currentBankDisplay].pattern[currentPatternDisplay].track_has_Trigs[byteToWrite_notEmpty] &= ~masks[bitPos_notEmpty];
          }

          checkIf_patternEmpty(currentBankDisplay, currentPatternDisplay);
          selected_Track = track;
        }
      }

      update_padLeds = true;
      update_screen_PADS = true;
    }
    reset_ChromaMode_UI();
  }
}
