void reset_modes_on_screenChange(/*uint8_t currentScreen*/) {
  SHIFT_pushed = false;
  recMode = 0;
  pitchMode = 0;
  MICROTIMING = 0;
  glideMode = 0;
  Trigs_Undo_enable = 0;
  Pitch_Undo_enable = 0;
  reset_ChromaMode_UI();
  update_padLeds = true;

  //if (currentScreen != screenConfig)
  update_screen_INIT = true;
}


void detect_remoteBtnsPush() {
  // SHIFT BUTTON
  bool SHIFT_Btn_State = digitalRead(SHIFT_Button);
  //if (SHIFT_Btn_State != last_SHIFT_Btn_State && ((millis() - last_btn_DebounceTime_millis) > DEBOUNCE_BTNS_MILLIS)) {
  //last_btn_DebounceTime_millis = millis();

  if (SHIFT_Btn_State == 0) {
    SHIFT_pushed = true;
    update_screen_SHIFT_BTN = true;
  }

  else {
    SHIFT_pushed = false;
    update_screen_SHIFT_BTN = false;
  }

  //last_SHIFT_Btn_State = SHIFT_Btn_State;
  //}

  // BACK BUTTON
  bool remoteBtn_Back_State = digitalRead(BACK_Button);
  if (remoteBtn_Back_State != last_remoteBtn_Back_State && ((millis() - last_btn_DebounceTime_millis) > DEBOUNCE_BTNS_MILLIS)) {
    last_btn_DebounceTime_millis = millis();

    if (remoteBtn_Back_State == 0) {
      digitalWrite(BACK_led, HIGH);

      global_Step_4PPQN = get_pattern_Length(4) - 1;
      global_Step_16PPQN = get_pattern_Length(16) - 1;

      for (uint8_t track = 0; track < TRACKS; ++track) {
        switch (bank[currentBank].pattern[currentPattern].loop_dir[track]) {
          case 0 : // FORWARD
            track_Step_16PPQN[track] = get_track_Length(track, 16) - 1;
            break;

          case 1 : // BACKWARD
            track_Step_16PPQN[track] = get_firstStep(16);
            break;

          case 2 : // PING PONG
            track_Step_16PPQN[track] = get_track_Length(track, 16) - 1;
            loop_dir_right[track] = true;
            break;
        }
      }

      //tick = 31;
      global_Step_Counter16 = 15; //global_Step_Counter16 = 0;

      if (!PLAYING) {
        displayFirstStep_1 = 1;
        digitalWrite(resetOutPin, HIGH); // produit un trig sur la sortie RESET
        digitalWrite(resetOutPin, LOW);
        // voir si devoir mettre aussi digitalWrite(clockOutPin, HIGH);
      }
      else
        displayFirstStep_1 = 0;


      update_screen_TOP = true;
      update_padLeds = true;
    }

    else
      digitalWrite(BACK_led, LOW);

    last_remoteBtn_Back_State = remoteBtn_Back_State;
  }


  // PLAY BUTTON
  bool remoteBtn_Play_State = digitalRead(PLAY_Button);
  if (remoteBtn_Play_State != last_remoteBtn_Play_State && ((millis() - last_btn_DebounceTime_millis) > DEBOUNCE_BTNS_MILLIS)) {
    last_btn_DebounceTime_millis = millis();

    if (remoteBtn_Play_State == 0) {
      //digitalWrite(PLAY_led, HIGH);

      if (SYNC_mode_internal) {
        PLAY_pushed = !PLAY_pushed;
        PLAYING = !PLAYING;
      }

      else {
        PLAY_pushed = !PLAY_pushed;
        if (PLAYING)
          PLAYING = 0;
      }

      if (!PLAYING)
        shiftOutTrigs(0);

    }

    /*else
      digitalWrite(PLAY_led, LOW);*/

    last_remoteBtn_Play_State = remoteBtn_Play_State;
  }


  // REC BUTTON
  bool remoteBtn_Rec_State = digitalRead(REC_Button);
  if (remoteBtn_Rec_State != last_remoteBtn_Rec_State && ((millis() - last_btn_DebounceTime_millis) > DEBOUNCE_BTNS_MILLIS)) {
    last_btn_DebounceTime_millis = millis();

    if (remoteBtn_Rec_State == 0) {

      if (!SHIFT_pushed && screenConfig == 10) {
        MICROTIMING = !MICROTIMING;
        stepSeq_clickedStep = 255;
      }

      else if ((!SHIFT_pushed && screenConfig == 1) || screenConfig == 33 || screenConfig == 5) {
        recMode = !recMode;

        if (recMode) {
          digitalWrite(REC_led, HIGH);
          if (screenConfig == 5) {
            fillDataRecording_startStep = get_track_step_TRIGS(selected_Track, 0);
          }
        }

        else
          digitalWrite(REC_led, LOW);

      }
    }

    last_remoteBtn_Rec_State = remoteBtn_Rec_State;
  }


  // LIVE BUTTON
  bool remoteBtn_LIVE_State = digitalRead(LIVE_Button);
  if (remoteBtn_LIVE_State != last_remoteBtn_LIVE_State && ((millis() - last_btn_DebounceTime_millis) > DEBOUNCE_BTNS_MILLIS)) {
    last_btn_DebounceTime_millis = millis();

    if (remoteBtn_LIVE_State == 0) {
      if (!SHIFT_pushed) {
        if (screenConfig != 1)
          screenConfig = 1;
      }

      else {
        if (screenConfig != 33)
          screenConfig = 33;
      }

      reset_modes_on_screenChange();
    }

    last_remoteBtn_Rec_State = remoteBtn_Rec_State;
  }




  // SETTINGS BUTTON
  bool SETTINGS_Btn_State = digitalRead(SETTINGS_Button);
  if (SETTINGS_Btn_State != last_SETTINGS_Btn_State && ((millis() - last_btn_DebounceTime_millis) > DEBOUNCE_BTNS_MILLIS)) {
    last_btn_DebounceTime_millis = millis();

    if (SETTINGS_Btn_State == 0) {
      update_screen_INIT = true;

      if (screenConfig != 7)
        screenConfig = 7;

      else
        screenConfig = 1;

      update_screen_INIT = true;
    }

    last_SETTINGS_Btn_State = SETTINGS_Btn_State;
  }


  // ENC ALT BUTTON
  bool encoders_ALT_Btn_State = digitalRead(encoders_ALT_Button);
  if (encoders_ALT_Btn_State != last_encoders_ALT_Btn_State && ((millis() - last_btn_DebounceTime_millis) > DEBOUNCE_BTNS_MILLIS)) {
    last_btn_DebounceTime_millis = millis();

    if (encoders_ALT_Btn_State == 0) {
      if (enc_ALT_enabled == 0) {
        enc_ALT_enabled = 1;
        digitalWrite(enc_ALT_led, HIGH);
      }

      else {
        enc_ALT_enabled = 0;
        digitalWrite(enc_ALT_led, LOW);
      }

      update_screen_ENC_ALT_BTN = true;
    }

    last_encoders_ALT_Btn_State = encoders_ALT_Btn_State;
  }

}




void catch_PatternBank_OnOff() {
  if (btnsIn_BitVal[4]) {
    if (SHIFT_pushed) {
      if (screenConfig != 22)
        screenConfig = 22;
    }

    else {
      if (screenConfig != 2)
        screenConfig = 2;
    }

    reset_modes_on_screenChange();
  }
}


void catch_AbLength_OnOff() {
  if (btnsIn_BitVal[3]) {
    if (SHIFT_pushed) {
      if (screenConfig != 9) {
        screenConfig = 9;
      }
    }

    else {
      if (screenConfig != 3) {
        screenConfig = 3;
      }
    }

    reset_modes_on_screenChange(/*2*/);
  }
}


void catch_MuteSolo_OnOff() {
  if (btnsIn_BitVal[2]) {
    if (SHIFT_pushed) {
      if (screenConfig != 44) {
        screenConfig = 44;
      }
    }

    else {
      if (screenConfig != 4) {
        screenConfig = 4;
      }

      reset_modes_on_screenChange();
    }
  }
}


void catch_FillShift_OnOff() {
  if (btnsIn_BitVal[1]) {
    if (SHIFT_pushed) {
      if (screenConfig != 55) {
        screenConfig = 55;
      }
    }

    else {
      if (screenConfig != 5) {
        screenConfig = 5;
      }
    }

    reset_modes_on_screenChange(/*2*/);
  }
}


void catch_DeleteClear_OnOff() {
  if (btnsIn_BitVal[0]) {

    if (SHIFT_pushed) {
      if (screenConfig != 66) {
        screenConfig = 66;
      }
    }

    else {
      if (screenConfig != 6) {
        screenConfig = 6;
      }
    }

    reset_modes_on_screenChange(/*2*/);
  }
}


void catch_LFO_OnOff() {
  if (btnsIn_BitVal[5]) {

    if (!SHIFT_pushed) {
      if (screenConfig != 17)
        screenConfig = 17;
    }

    else {
      if (screenConfig != 18)
        screenConfig = 18;
    }

    LFO_Settings_titles_Cursor = 0;
    reset_modes_on_screenChange();
  }
}


void catch_CV_OnOff() {
  if (btnsIn_BitVal[6]) {

    if (SHIFT_pushed) {
      if (screenConfig != 66) {
        screenConfig = 66;
      }
    }

    else {
      if (screenConfig != 6) {
        screenConfig = 6;
      }
    }

    reset_modes_on_screenChange();
  }
}


void catch_GRID_OnOff() {
  if (btnsIn_BitVal[7]) {

    if (SHIFT_pushed) {
      if (screenConfig != 12) {
        screenConfig = 12;
      }
    }

    else {
      if (screenConfig != 10) {
        screenConfig = 10;
      }
    }

    reset_modes_on_screenChange();
  }
}


void catch_Mode_Selection_Change() {
  catch_PatternBank_OnOff();
  catch_AbLength_OnOff();
  catch_MuteSolo_OnOff();
  catch_FillShift_OnOff();
  catch_DeleteClear_OnOff();
  catch_LFO_OnOff();
  catch_CV_OnOff();
  catch_GRID_OnOff();
}


/*
  //launch SETTINGS SCREEN
  if (screenConfig != 7) {
  reset_modes_on_screenChange();
  screenConfig = 7;
  }

  //launch GRID STEPS (step seq)
  if (screenConfig != 10) {
  screenConfig = 10;
  recMode = 0;
  canResetScreen = false;
  update_screen_INIT = true;
  }

  DATA_WIDTH = 24
  PADS = 16
  TRACKS = 12

  bool  trigsIn_BitVal[PADS];
  bool  btnsIn_BitVal[8];

  bool trigsOut_BitVal[TRACKS];
  bool trigsOut_BitVal2[TRACKS];

  trigsOut_BitVal2
*/
