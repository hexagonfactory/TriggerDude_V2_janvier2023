void print_GLOBAL_Setting_title(uint8_t param_Pos, uint8_t param_Title, bool paramHighlight) {
  uint8_t y = (param_Pos == 1) ? 21 : (param_Pos == 2) ? 32 : 43;
  if (paramHighlight)
    setFontMode_BG(0);
  u8g2.setFont(triggerDude_big_font);
  u8g2.setCursor(6, y);
  u8g2.print(GLOBAL_Settings_titles[param_Title]);
  u8g2.setCursor(85, y);
}


void print_LFO_Setting_title(uint8_t param_Pos, uint8_t param_Title, bool paramHighlight) {
  uint8_t y = (param_Pos == 1) ? 21 : (param_Pos == 2) ? 32 : 43;
  if (paramHighlight)
    setFontMode_BG(0);
  u8g2.setFont(triggerDude_big_font);
  u8g2.setCursor(6, y);
  u8g2.print(LFO_Settings_titles[param_Title]);
  u8g2.setCursor(85, y);
}


void print_GLOBAL_Setting_scrollBar() {
  u8g2.drawFrame(0, 12, 3, 32);
  u8g2.drawBox(1, GLOBAL_Settings_titles_Cursor + 13, 1, 7);
}

void print_LFO_Setting_scrollBar() {
  u8g2.drawFrame(0, 12, 3, 32);
  u8g2.drawBox(1, LFO_Settings_titles_Cursor + 13, 1, 7);
}


void draw_GLOBAL_Settings_list() {
  switch (GLOBAL_Settings_titles_Cursor) {
    case 0: // LOOP MODE
      u8g2.drawBox(4, 12, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor, 1);
      (bank[currentBank].pattern[currentPattern].tracksRetrig) ? u8g2.print("RETRIG") : u8g2.print("FREE");
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor + 1, 0);
      if (bank[currentBank].pattern[currentPattern].tracksRetrig)
        u8g2.print(bank[currentBank].pattern[currentPattern].firstStep + 1);
      else
        u8g2.print("-");

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 2, 0);
      u8g2.print(cvIn_modulation_Target[CVin[0].dest_Cursor]);
      break;

    case 1: // FirstStep
      u8g2.drawBox(4, 23, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      (bank[currentBank].pattern[currentPattern].tracksRetrig) ? u8g2.print("RETRIG") : u8g2.print("FREE");

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      if (bank[currentBank].pattern[currentPattern].tracksRetrig)
        u8g2.print(bank[currentBank].pattern[currentPattern].firstStep + 1);
      else
        u8g2.print("-");
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      u8g2.print(cvIn_modulation_Target[CVin[0].dest_Cursor]);

      display_enc2_click(88, (enc2_Range_10)  ? "+/- 1" : "+/- 10");
      break;

    case 2: // CVin_1
      u8g2.drawBox(4, 23, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      u8g2.print(bank[currentBank].pattern[currentPattern].firstStep + 1);

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      u8g2.print(cvIn_modulation_Target[CVin[0].dest_Cursor]);
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      u8g2.print(cvIn_modulation_Target[CVin[1].dest_Cursor]);

      display_enc2_click(70, (CVin[0].enable && (CVin[0].dest_Cursor == CVin[0].dest_Selected)) ? "DEACTIVATE" : "  ACTIVATE");
      break;

    case 3: // CVin_2
      u8g2.drawBox(4, 23, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      u8g2.print(cvIn_modulation_Target[CVin[0].dest_Cursor]);

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      u8g2.print(cvIn_modulation_Target[CVin[1].dest_Cursor]);
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      (replicateBars) ? u8g2.print("ON") : u8g2.print("OFF");

      display_enc2_click(70, (CVin[1].enable && (CVin[1].dest_Cursor == CVin[1].dest_Selected))  ? "DEACTIVATE" : "  ACTIVATE");
      break;

    case 4: // replicateBars
      u8g2.drawBox(4, 23, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      u8g2.print(cvIn_modulation_Target[CVin[1].dest_Cursor]);

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      (replicateBars) ? u8g2.print("ON") : u8g2.print("OFF");
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      (SYNC_mode_internal) ? u8g2.print("INT") : u8g2.print("EXT");
      break;

    case 5: // SYNC Mode
      u8g2.drawBox(4, 23, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      (replicateBars) ? u8g2.print("ON") : u8g2.print("OFF");

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      (SYNC_mode_internal) ? u8g2.print("INT") : u8g2.print("EXT");
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      u8g2.setCursor(85, 44);
      (SYNC_mode_internal) ? u8g2.print(bank[currentBank].pattern[currentPattern].internal_BPM) : u8g2.print("--");
      break;

    case 6: // set BPM
      u8g2.drawBox(4, 23, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      //u8g2.setCursor(85, 22);
      (SYNC_mode_internal) ? u8g2.print("INT") : u8g2.print("EXT");

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      (SYNC_mode_internal) ? u8g2.print(bank[currentBank].pattern[currentPattern].internal_BPM) : u8g2.print("--");
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      if (bank[currentBank].pattern[currentPattern].clkIn_Div == 1)
        u8g2.print("=");
      else {
        u8g2.print("/");
        u8g2.print(bank[currentBank].pattern[currentPattern].clkIn_Div);
      }

      if (SYNC_mode_internal) {
        display_enc2_click(88, (enc2_Range_10)  ? "+/- 1" : "+/- 10");
      }
      break;

    case 7: // set ClkIn DIV
      u8g2.drawBox(4, 23, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      (SYNC_mode_internal) ? u8g2.print(bank[currentBank].pattern[currentPattern].internal_BPM) : u8g2.print("--");

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      if (bank[currentBank].pattern[currentPattern].clkIn_Div == 1)
        u8g2.print("=");
      else {
        u8g2.print("/");
        u8g2.print(bank[currentBank].pattern[currentPattern].clkIn_Div);
      }
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      if (bank[currentBank].pattern[currentPattern].clkOut_Div == 1)
        u8g2.print("=");
      else {
        u8g2.print("/");
        u8g2.print(bank[currentBank].pattern[currentPattern].clkOut_Div);
      }
      break;

    case 8: // set ClkOut DIV
      u8g2.drawBox(4, 23, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      if (bank[currentBank].pattern[currentPattern].clkIn_Div == 1)
        u8g2.print("=");
      else {
        u8g2.print("/");
        u8g2.print(bank[currentBank].pattern[currentPattern].clkIn_Div);
      }

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      if (bank[currentBank].pattern[currentPattern].clkOut_Div == 1)
        u8g2.print("=");
      else {
        u8g2.print("/");
        u8g2.print(bank[currentBank].pattern[currentPattern].clkOut_Div);
      }
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      (bank[currentBank].pattern[currentPattern].quantizeBeat == 1) ? u8g2.print("ON") : u8g2.print("OFF");
      break;

    case 9: // set QUANTIZE
      u8g2.drawBox(4, 23, 128, 11);
      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      if (bank[currentBank].pattern[currentPattern].clkOut_Div == 1)
        u8g2.print("=");
      else {
        u8g2.print("/");
        u8g2.print(bank[currentBank].pattern[currentPattern].clkOut_Div);
      }

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      (bank[currentBank].pattern[currentPattern].quantizeBeat == 1) ? u8g2.print("ON") : u8g2.print("OFF");
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      u8g2.print(bank[currentBank].pattern[currentPattern].shuffle);

      display_LFO_CvOut_Text(0);
      break;

    case 10: // set SHUFFLE
      u8g2.drawBox(4, 23, 128, 11);

      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      (bank[currentBank].pattern[currentPattern].quantizeBeat == 1) ? u8g2.print("ON") : u8g2.print("OFF");
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      u8g2.print(bank[currentBank].pattern[currentPattern].shuffle);
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      (patternChange_Quantized) ? u8g2.print("ON") : u8g2.print("OFF");
      break;

    case 11: // PATTERN CHANGE QUANTIZED
      u8g2.drawBox(4, 23, 128, 11);
      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      u8g2.print(bank[currentBank].pattern[currentPattern].shuffle);

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      (patternChange_Quantized) ? u8g2.print("ON") : u8g2.print("OFF");
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      print_trig_dur();
      break;

    case 12: // TRIG DURATION
      u8g2.drawBox(4, 23, 128, 11);
      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 1, 0);
      (patternChange_Quantized) ? u8g2.print("ON") : u8g2.print("OFF");

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor, 1);
      print_trig_dur();
      setFontMode_BG(1);

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor + 1, 0);
      u8g2.print(pads_brightness);
      break;

    case 13: // LED BRIGHTNESS
      u8g2.drawBox(4, 34, 128, 11);
      print_GLOBAL_Setting_title(1, GLOBAL_Settings_titles_Cursor - 2, 0);
      (patternChange_Quantized) ? u8g2.print("ON") : u8g2.print("OFF");

      print_GLOBAL_Setting_title(2, GLOBAL_Settings_titles_Cursor - 1, 0);
      print_trig_dur();

      print_GLOBAL_Setting_title(3, GLOBAL_Settings_titles_Cursor, 1);
      u8g2.print(pads_brightness);
      setFontMode_BG(1);
      break;
  }
}


void draw_LFO_Settings_list(bool lfoNum) {
  switch (LFO_Settings_titles_Cursor) {
    case 0: // set LFO N DEST
      u8g2.drawBox(4, 12, 128, 11);
      print_LFO_Setting_title(1, LFO_Settings_titles_Cursor, 1);
      u8g2.print(LFOs_output[bank[currentBank].pattern[currentPattern].lfo_dest[lfoNum]]);
      setFontMode_BG(1);

      print_LFO_Setting_title(2, LFO_Settings_titles_Cursor + 1, 0);
      u8g2.print(LFOs_waves[bank[currentBank].pattern[currentPattern].lfo_wave[lfoNum]]);

      print_LFO_Setting_title(1, LFO_Settings_titles_Cursor - 1, 0);
      u8g2.print(LFOs_output[bank[currentBank].pattern[currentPattern].lfo_rate[lfoNum]]);

      break;

    case 1: // set LFO N WAVE
      u8g2.drawBox(4, 23, 128, 11);
      print_LFO_Setting_title(1, LFO_Settings_titles_Cursor, 0);
      u8g2.print(LFOs_output[bank[currentBank].pattern[currentPattern].lfo_dest[lfoNum]]);
      
      print_LFO_Setting_title(2, LFO_Settings_titles_Cursor + 1, 1);
      u8g2.print(LFOs_waves[bank[currentBank].pattern[currentPattern].lfo_wave[lfoNum]]);
      setFontMode_BG(1);

      print_LFO_Setting_title(1, LFO_Settings_titles_Cursor - 1, 0);
      u8g2.print(LFOs_output[bank[currentBank].pattern[currentPattern].lfo_rate[lfoNum]]);
      break;

    case 2: // set LFO N RATE
      u8g2.drawBox(4, 23, 128, 11);
      print_LFO_Setting_title(1, LFO_Settings_titles_Cursor - 1, 0);
      u8g2.print(LFOs_waves[bank[currentBank].pattern[currentPattern].lfo_wave[lfoNum]]);

      print_LFO_Setting_title(2, LFO_Settings_titles_Cursor, 1);
      u8g2.print(LFOs_rates[bank[currentBank].pattern[currentPattern].lfo_rate[lfoNum]]);
      setFontMode_BG(1);

      print_LFO_Setting_title(3, LFO_Settings_titles_Cursor + 1, 0);
      u8g2.print(bank[currentBank].pattern[currentPattern].lfo_gain[lfoNum]);
      break;

    case 3: // set LFO N GAIN
      u8g2.drawBox(4, 23, 128, 11);
      print_LFO_Setting_title(1, LFO_Settings_titles_Cursor - 1, 0);
      u8g2.print(LFOs_rates[bank[currentBank].pattern[currentPattern].lfo_rate[lfoNum]]);

      print_LFO_Setting_title(2, LFO_Settings_titles_Cursor, 1);
      u8g2.print(bank[currentBank].pattern[currentPattern].lfo_gain[lfoNum]);
      setFontMode_BG(1);

      print_LFO_Setting_title(3, LFO_Settings_titles_Cursor + 1, 0);
      u8g2.print(bank[currentBank].pattern[currentPattern].lfo_phase[lfoNum]);

      display_enc2_click(88, (enc2_Range_10)  ? "+/- 1" : "+/- 10");
      break;

    case 4: // set LFO N PHASE
      u8g2.drawBox(4, 34, 128, 11);
      
      print_LFO_Setting_title(1, LFO_Settings_titles_Cursor - 1, 0);
      u8g2.print(LFOs_rates[bank[currentBank].pattern[currentPattern].lfo_rate[lfoNum]]);
      
      print_LFO_Setting_title(1, LFO_Settings_titles_Cursor - 1, 0);
      u8g2.print(bank[currentBank].pattern[currentPattern].lfo_gain[lfoNum]);

      print_LFO_Setting_title(2, LFO_Settings_titles_Cursor, 1);
      u8g2.print(bank[currentBank].pattern[currentPattern].lfo_phase[lfoNum]);
      setFontMode_BG(1);

      display_enc2_click(88, "RESET");
      break;
  }
}


void display_LFO_CvOut_Text(uint8_t lfoNum) {
  uint8_t x = bank[currentBank].pattern[currentPattern].cvOut_Tracks[bank[currentBank].pattern[currentPattern].lfo_dest[lfoNum]];
  uint8_t y = (lfoNum == 0) ? 13 : 14;

  if (x == y)
    display_enc2_click(70, "DEACTIVATE");
  else if  (x == 12)
    display_enc2_click(80, "ACTIVATE");
  else {
    u8g2.setCursor(77, 63);
    u8g2.setFont(triggerDude_small_font);
    u8g2.print("Used by ");
    if (lfoNum == 0)
      (x == 14) ? u8g2.print("L2") : u8g2.print(x + 1);
    else
      (x == 13) ? u8g2.print("L1") : u8g2.print(x + 1);
    //u8g2.print(" !");
    u8g2.setFont(u8g2_font_unifont_t_symbols);
    //u8g2.drawGlyph(120, 65, 0x25be);
  }

  update_screen_POTS_CLK_R = true;
}


void print_trig_dur() {
  /*if (trig_durations_Cursor <= 1) {
    u8g2.print(trig_DURATION);
    u8g2.print("us");
    }
    else {*/
  u8g2.print(trig_DURATION / 1000);
  u8g2.print("ms");
  //}
}


void display_debug_data() {
  u8g2.setFont(triggerDude_small_font);

  u8g2.setCursor(0, 8); // shiftValue
  u8g2.print("shift[");
  for (uint8_t track = 0; track < TRACKS; ++track) {
    u8g2.print(bank[currentBank].pattern[currentPattern].shiftValue[track]);
    if (track < TRACKS - 1) u8g2.print(",");
    else u8g2.print("]");
  }

  u8g2.setCursor(0, 16);
  u8g2.print("patLgth : ");
  u8g2.print(get_pattern_Length(4)); //pattern_Length

  u8g2.setCursor(0, 24);
  u8g2.print("bankSt[");
  for (uint8_t track = 0; track < TRACKS; ++track) {
    u8g2.print(bank[currentBank].pattern[currentPattern].AB_State[track]);
    if (track < TRACKS - 1) u8g2.print(",");
    else u8g2.print("]");
  }

  u8g2.setCursor(0, 32);
  u8g2.print("cvOut[");
  for (uint8_t i = 0; i < 4; i++) {
    u8g2.print(bank[currentBank].pattern[currentPattern].cvOut_Tracks[i]);
    if (i < 3) u8g2.print(",");
    else u8g2.print("]");
  }

  u8g2.setCursor(0, 40);
  u8g2.print(step_micros);
  u8g2.setCursor(0, 48);
  u8g2.print(LFO[0].rate_micros);
  /*
    u8g2.setCursor(0, 30);
    u8g2.print("[");
    for (uint8_t i = 0; i < TRACKS; i++) {
      u8g2.print(bank[currentBank].pattern[currentPattern].Pitch[0][i]);
      if (i < TRACKS-1) u8g2.print(",");
      else u8g2.print("]");
    }

    u8g2.setCursor(0, 36);
    u8g2.print("[");
    for (uint8_t i = 0; i < TRACKS; i++) {
      u8g2.print(bank[currentBank].pattern[currentPattern].Trigs[0][i]);
      if (i < TRACKS-1) u8g2.print(",");
      else u8g2.print("]");
    }

    u8g2.setCursor(0, 42);
    u8g2.print("[");
    for (uint8_t i = 0; i < TRACKS; i++) {
      u8g2.print(bank[currentBank].pattern[currentPattern].muteState[i]); //dec
      if (i < TRACKS-1) u8g2.print(",");
      else u8g2.print("]");
    }

    u8g2.setCursor(0, 48);
    u8g2.print("[");
    for (uint8_t i = 0; i < TRACKS; i++) {
      u8g2.print(bank[currentBank].pattern[currentPattern].soloState[i]); //dec
      if (i < TRACKS-1) u8g2.print(",");
      else u8g2.print("]");
    }

    u8g2.setCursor(0, 54);
    u8g2.print("[");
    for (uint8_t i = 0; i < TRACKS; i++) {
      u8g2.print(bank[currentBank].pattern[currentPattern].fillState[i]); //dec (12)
      if (i < TRACKS-1) u8g2.print(",");
      else u8g2.print("]");
    }

    u8g2.setCursor(0, 60);
    u8g2.print(bank[currentBank].pattern[currentPattern].mute_active);
    u8g2.print("/");
    u8g2.print(bank[currentBank].pattern[currentPattern].allMute_active);
    u8g2.print("/");
    u8g2.print(bank[currentBank].pattern[currentPattern].solo_active);
    u8g2.print("/");
    u8g2.print(bank[currentBank].pattern[currentPattern].allMute_active);
    u8g2.print("/");
    u8g2.print(bank[currentBank].pattern[currentPattern].fill_active);
    u8g2.print("/");
    u8g2.print(bank[currentBank].pattern[currentPattern].allAorB);
    u8g2.print("/");
    u8g2.print(bank[currentBank].pattern[currentPattern].shiftingTrigs);
    u8g2.print("/");
    u8g2.print(bank[currentBank].pattern[currentPattern].shiftingPitch);
  */
  u8g2.setCursor(0, 54);
  u8g2.print("/");
  u8g2.print(bank[currentBank].pattern[currentPattern].lfo_enable[0]);
  u8g2.print("-");
  u8g2.print(bank[currentBank].pattern[currentPattern].lfo_enable[1]);
  u8g2.print("/");
  u8g2.print(bank[currentBank].pattern[currentPattern].lfo_dest[0]);
  u8g2.print("-");
  u8g2.print(bank[currentBank].pattern[currentPattern].lfo_dest[1]);
  u8g2.print("/");
  u8g2.print(bank[currentBank].pattern[currentPattern].lfo_wave[0]);
  u8g2.print("-");
  u8g2.print(bank[currentBank].pattern[currentPattern].lfo_wave[1]);
  u8g2.print("/");
  u8g2.print(bank[currentBank].pattern[currentPattern].lfo_rate[0]);
  u8g2.print("-");
  u8g2.print(bank[currentBank].pattern[currentPattern].lfo_rate[1]);
}
