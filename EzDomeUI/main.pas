unit main;

interface

uses
  Winapi.Windows, System.SyncObjs, System.SysUtils, System.Types, System.UITypes,
  System.Classes, System.Variants, FMX.Types, FMX.Controls, FMX.Forms,
  FMX.Graphics, FMX.Dialogs, System.Math.Vectors, FMX.Controls3D, FMX.Objects3D,
  FMX.Objects, FMXTee.Engine, FMXTee.Series, FMXTee.Procs, FMXTee.Chart, CPort,
  FMX.Viewport3D, FMX.Controls.Presentation, FMX.StdCtrls, ComObj, FMX.Edit,
  FMX.Memo.Types, FMX.ScrollBox, FMX.Memo, System.Threading, Registry,
  System.StrUtils, OtlEventMonitor, Character, FMX.frxFMX, FMX.Menus,
  IdBaseComponent, IdComponent, IdUDPBase, IdUDPServer, IdGlobal, IdSocketHandle,
  System.Generics.Collections, System.DateUtils, CPDrv, FMX.EditBox,
  FMX.NumberBox;

type
  TMain_form = class(TForm)
    cmprt: TComPort;
    cmdtpckt1: TComDataPacket;
    btn_rot_con: TButton;
    arcdl_az: TArcDial;
    lbl_rot_des_pos: TLabel;
    btn_rot_find_home: TButton;
    btn_rot_reset: TButton;
    btn_rot_go_home: TButton;
    btn_rot_stop: TButton;
    btn_rot_settings: TButton;
    stylbk1: TStyleBook;
    btn_params: TButton;
    btn_emerg_close: TButton;
    btn_move_shut: TButton;
    btn_rot_estop: TButton;
    btn_shut_estp: TButton;
    btn_adv: TButton;
    trckbr_fan: TTrackBar;
    mm_info: TMemo;
    chk_alt_en: TCheckBox;
    btn_clear_info: TButton;
    lbl_DHT: TLabel;
    idpsrvr1: TIdUDPServer;
    pnl1: TPanel;
    mm_log: TMemo;
    edt_cmd_send: TEdit;
    lbl_udp_port: TLabel;
    nmbrbx_udp_port: TNumberBox;
    btn_send_cmd: TButton;
    btn_log_clear: TButton;
    chk_mm_as: TCheckBox;
    procedure FormCreate(Sender: TObject);
    procedure btn_rot_conClick(Sender: TObject);
    procedure btn_rot_settingsClick(Sender: TObject);
    procedure arcdl_azChange(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure cmdtpckt1Packet(Sender: TObject; const Str: string);
    procedure arcdl_azMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Single);
    procedure btn_rot_resetClick(Sender: TObject);
    procedure btn_rot_find_homeClick(Sender: TObject);
    procedure btn_rot_go_homeClick(Sender: TObject);
    procedure btn_rot_stopClick(Sender: TObject);
    procedure btn_rot_estopClick(Sender: TObject);
    procedure btn_log_clearClick(Sender: TObject);
    procedure swtch_shutClick(Sender: TObject);
    procedure btn_paramsClick(Sender: TObject);
    procedure btn_move_shutClick(Sender: TObject);
    procedure btn_advClick(Sender: TObject);
    procedure trckbr_fanChange(Sender: TObject);
    procedure btn_send_cmdClick(Sender: TObject);
    procedure edt_cmd_sendKeyDown(Sender: TObject; var Key: Word; var KeyChar: WideChar; Shift: TShiftState);
    procedure btn_clear_infoClick(Sender: TObject);
    procedure chk_alt_enChange(Sender: TObject);
    procedure idpsrvr1UDPRead(AThread: TIdUDPListenerThread; const AData: TIdBytes; ABinding: TIdSocketHandle);
    procedure idpsrvr1UDPException(AThread: TIdUDPListenerThread; ABinding: TIdSocketHandle; const AMessage: string; const AExceptionClass: TClass);
    procedure idpsrvr1Status(ASender: TObject; const AStatus: TIdStatus; const AStatusText: string);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure mm_logChangeTracking(Sender: TObject);
  private
    MountConnected: Boolean;
    RotatorConnected: Boolean;
    Shut_connected: Boolean;
    Shut_OC: boolean; // shutter opened true, closed false

    ALTorAZ: Boolean;  //switch beetwen alt or az cmd send
    prev_l_az_fs: string;

    MountName: string;

    ComPort: string;
    UDPPort: integer;

    SLcmd: TStringList;
    SLval: TStringList;

    SLUDPcmd: TStringList;
    SLUDPval: TStringList;

    SLParamLst: TStringList;
    cmdhistoryStepping: integer;
    SLcmdHistory: TStringList;

    RegINI: TRegistryIniFile;
    function ArcdialtoAZ(dial_val: integer): string;
    function DoubleToDMtoAZ(az: double): string;
    procedure UDP_transASCOM(s: string);
    procedure UDP_trns_Serial(cmd: string);

    procedure rot_send_cmd(ctype: string; cval: string);
    procedure rot_enable_control(en: boolean);
    procedure mm_info_add(S: string);
    procedure mm_add(RT: integer; S: string);
    procedure LoadReg;
    procedure SaveReg;
    procedure shut_controll(OC, val, pos: string);
    procedure rot_DHT_recive(c, h: string);

  public
    PeerIp: string;
    PeerPort: word;
  end;

const
  REGKEY = 'HKEY_CURRENT_USER\SOFTWARE\EzDome';
  RX = 0;
  TX = 1;
  LBL_MNT_CON = 'Connect mount';
  LBL_MNT_DC = 'Disconnnect mount';
  LBL_ROT_CON = 'Connect dome';
  LBL_ROT_DC = 'Disconnnect dome';
  DELIMETER = '#';
  DMSEP = ':';
  //Serial com


////ROTATOR COMMANDS
//IN
  ROT_I_REQUEST_INIT = 'I';
  ROT_I_STOP = 'P';            //Stop rotator
  ROT_I_EMERGENCY_STOP = 'Y';  //Emergency stop rotator
  ROT_I_SYNC = 'S';            //Sync to mount
  ROT_I_RESET = 'R';           //reset, reboot
  ROT_I_QUERY_POS = 'Q';       //query position
  ROT_I_FAN = 'A';             //fan controll
  ROT_I_ALT = 'L';

//IN/OUT
  ROT_IO_DMPOS = 'D';  //GOTO DM position
  ROT_IO_HOME = 'H';   // Rotator find home

//OUT
  ROT_O_PARAMS = 'P';
////ROT_O_PARAMS params type with values, example :P#2:460800
//  0 firmware version
//  1 motor_step_rev(step*micro)
//  2 rotator_full_rotation / step
//  3 steps/DM:
//  4 pos_rotator

  ROT_O_INFORMATION = 'F';
////ROT_O_INFORMATION
// 0 Rotator emergency STOP!
// 1 Rotator STOP!
// 2 Rotator finding home
// 3 Rotator reached home nothing to do
// 4 Rotator at home, zerosearch finished, reset Rotator pos to 0
  ROT_O_PING = 'G';
  ROT_O_DHT = 'T';
////SHUTTER COMMANDS
//IN
  SHUT_I_INIT = 'i';
  SHUT_I_EMERGENCY_CLOSE = 'e';  //used for fast closing like rain or etc.
  SHUT_I_EMERGENCY_STOP = 'y';
  SHUT_I_RESET = 'r';  //reboot system
  SHUT_I_QRY_ENDSTOP = 'q';

//N/OUT
  SHUT_IO_CLOSE = 'c';
  SHUT_IO_OPEN = 'o';
// SHUT_IO_VOLTAGE ='w' //not developed yet

//OUT
  SHUT_O_VER = 'v';
  SHUT_O_INFORMATION = 'f';

////SHUT_O_INFORMATION
// 0 BLE disconnected
// 1 BLE Connected
// 2 BLE is running

var
  Main_form: TMain_form;


 // iUtility: ASCOM_Utilities_TLB.IUtil;

implementation

{$R *.fmx}

procedure TMain_form.rot_DHT_recive(c, h: string);
begin
  lbl_DHT.Text := 'Temp: ' + c + ' C° Hum: ' + h;
end;

procedure TMain_form.shut_controll(OC, val, pos: string);
begin
  //OC is openin or closeing
  //val 0 = closed/opened, 1 = opening or closing, -1 error during movement
  if (OC = 'c') and (val = '0') then
  begin
    btn_move_shut.Text := 'Open';
    Shut_OC := false;
    exit;
  end;

  if (OC = 'c') and (val = '-1') then
  begin
    btn_move_shut.Text := 'Close error';
    Shut_OC := true;
    exit;
  end;

  if (OC = 'c') and (val = '1') then
  begin
    btn_move_shut.Text := 'Closing' + ' ' + pos + '%';
    exit;
  end;

  if (OC = 'o') and (val = '0') then
  begin
    btn_move_shut.Text := 'Close';
    Shut_OC := true;
    exit;
  end;

  if (OC = 'o') and (val = '1') then
  begin
    btn_move_shut.Text := 'Opening' + ' ' + pos + '%';
    exit;
  end;

  if (OC = 'o') and (val = '-1') then
  begin
    btn_move_shut.Text := 'Open error';
    Shut_OC := false;
    exit;
  end;

end;

function TMain_form.DoubleToDMtoAZ(az: double): string;
var
  mm: integer;
  mm_f: double;
begin
  mm_f := Frac(az);
  mm := round(Round(Frac(az) * 1000) * 0.06);
  result := FloatToStr(az - mm_f) + DMSEP + IntToStr(mm);
end;

procedure TMain_form.edt_cmd_sendKeyDown(Sender: TObject; var Key: Word; var KeyChar: WideChar; Shift: TShiftState);
begin

  if ord(Key) = VK_RETURN then
    btn_send_cmd.OnClick(btn_send_cmd);

  if (Ord(Key) = VK_DOWN) and (SLcmdHistory.Count > 0) then
  try
    edt_cmd_send.Text := SLcmdHistory.Strings[cmdhistoryStepping];
    if cmdhistoryStepping > 0 then
      cmdhistoryStepping := cmdhistoryStepping - 1;
  except
  end;

  if (Ord(Key) = VK_UP) and (SLcmdHistory.Count > 0) then
  try
    edt_cmd_send.Text := SLcmdHistory.Strings[cmdhistoryStepping];
    if cmdhistoryStepping < SLcmdHistory.Count - 1 then
      cmdhistoryStepping := cmdhistoryStepping + 1;
  except
  end;

end;

procedure TMain_form.SaveReg;
begin
  RegINI.WriteString('EzDome', 'MountName', MountName);
  RegINI.WriteString('EzDome', 'SerialPort', ComPort);
  RegINI.WriteInteger('EzDome', 'UDPPort', round(nmbrbx_udp_port.Value));
  RegINI.WriteBool('EzDome', 'ALTlimit', chk_alt_en.IsChecked);
  RegINI.WriteInteger('EzDome', 'PosX', Self.Left);
  RegINI.WriteInteger('EzDome', 'PosY', Self.Top);
  RegINI.WriteBool('EzDome', 'LogAutoScroll', chk_mm_as.IsChecked);

end;

procedure TMain_form.LoadReg;
begin
  MountName := RegINI.ReadString('EzDome', 'MountName', MountName);
  ComPort := RegINI.ReadString('EzDome', 'SerialPort', ComPort);
  UDPPort := RegINI.ReadInteger('EzDome', 'UDPPort', UDPPort);
  chk_alt_en.IsChecked := RegINI.ReadBool('EzDome', 'ALTlimit', chk_alt_en.IsChecked);
  Self.Left := RegINI.ReadInteger('EzDome', 'PosX', Self.Left);
  Self.Top := RegINI.ReadInteger('EzDome', 'PosY', Self.Top);
  chk_mm_as.IsChecked := RegINI.ReadBool('EzDome', 'LogAutoScroll', chk_mm_as.IsChecked);

  nmbrbx_udp_port.Text := IntToStr(UDPPort);

end;

procedure TMain_form.swtch_shutClick(Sender: TObject);
begin
  if TSwitch(Sender).IsChecked then
    rot_send_cmd(SHUT_IO_CLOSE, '0')
  else
    rot_send_cmd(SHUT_IO_OPEN, '0');

end;

procedure TMain_form.trckbr_fanChange(Sender: TObject);
begin
  rot_send_cmd(ROT_I_FAN, Round(TTrackBar(Sender).value).ToString);
end;

procedure TMain_form.mm_info_add(S: string);
begin

  mm_info.Lines.Add(DateTimeToStr(Now) + ' ' + S);

end;

procedure TMain_form.mm_logChangeTracking(Sender: TObject);
begin
  if chk_mm_as.IsChecked then
    TMemo(Sender).GoToTextEnd;
end;

procedure TMain_form.mm_add(RT: integer; S: string);
begin
  case RT of
    0:
      mm_log.Lines.Add(DateTimeToStr(Now) + ' COM RX: ' + S);
    1:
      mm_log.Lines.Add(DateTimeToStr(Now) + ' COM TX: ' + S);
  end;
end;

procedure TMain_form.rot_enable_control(en: boolean);
begin

  arcdl_az.Enabled := en;
  btn_move_shut.Enabled := en;

  btn_rot_go_home.Enabled := en;
  btn_rot_find_home.Enabled := en;
  btn_rot_stop.Enabled := en;
  btn_rot_estop.Enabled := en;
  btn_rot_reset.Enabled := en;

  btn_params.Enabled := en;
  btn_emerg_close.Enabled := en;
  btn_shut_estp.Enabled := en;
  btn_rot_settings.Enabled := not en;

  chk_alt_en.Enabled := en;
  trckbr_fan.Enabled := en;

end;

procedure TMain_form.rot_send_cmd(ctype: string; cval: string);
var
  aTask: ITask;
begin

  mm_add(TX, ctype + DELIMETER + (cval));

  aTask := TTask.Create(
    procedure
    begin
     // sleep(3000); // 3 seconds
      TThread.Synchronize(TThread.Current,
        procedure
        begin

          cmprt.WriteStr(ctype + DELIMETER + (cval));

        end);

    end);
  aTask.ExecuteWork;

end;

procedure TMain_form.arcdl_azChange(Sender: TObject);
var
  val: double;
begin
  val := TArcDial(Sender).Value;
  lbl_rot_des_pos.Text := ArcdialtoAZ(round(val)) + '°';
end;

procedure TMain_form.arcdl_azMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Single);
begin

  rot_send_cmd(ROT_IO_DMPOS, ArcdialtoAZ(round(arcdl_az.Value)) + '.0');

end;

function TMain_form.ArcdialtoAZ(dial_val: integer): string;
begin

 { if AZ < 0 then
    result := IntToStr(360 - (AZ *  - 1))
  else
    Result := IntToStr(AZ);    }
  //North
  if (dial_val < 0) and (dial_val <= 180) then
    result := IntToStr(dial_val *  - 1)
  else
    Result := IntToStr(359 - dial_val);

end;

procedure TMain_form.btn_send_cmdClick(Sender: TObject);
begin
  cmdhistoryStepping := SLcmdHistory.Count;
  cmprt.WriteStr(edt_cmd_send.Text);
  SLcmdHistory.Add(edt_cmd_send.Text);
  edt_cmd_send.Text := '';
end;

procedure TMain_form.btn_clear_infoClick(Sender: TObject);
begin
  mm_info.Lines.Clear;
end;

procedure TMain_form.btn_advClick(Sender: TObject);
begin

  if pnl1.Visible = true then
  begin

    pnl1.Visible := false;
    Width := Width - round(pnl1.Width);
  end
  else
  begin

    pnl1.Visible := true;
    Width := Width + round(pnl1.Width);
  end;

end;

procedure TMain_form.btn_log_clearClick(Sender: TObject);
begin
  mm_log.Lines.Clear;
end;

procedure TMain_form.btn_move_shutClick(Sender: TObject);
begin
// shutter opened true, closed false
  if Shut_OC = False then
    rot_send_cmd(SHUT_IO_OPEN, '0')
  else
    rot_send_cmd(SHUT_IO_CLOSE, '0');

end;

procedure TMain_form.btn_paramsClick(Sender: TObject);
begin
  ShowMessage(SLParamLst.Text);
end;

procedure TMain_form.btn_rot_conClick(Sender: TObject);
begin
  if btn_rot_con.Text = LBL_ROT_CON then
  begin
    try
   //   cmprt.BaudRate := BaudRate;
      cmprt.Port := ComPort;
      cmprt.Connected := True;
      cmprt.Open;
      cmprt.ClearBuffer(True, True);
      RotatorConnected := True;
      btn_rot_con.Text := LBL_ROT_DC;

      SLParamLst.Clear;
      RotatorConnected := True;
      cmdtpckt1.ResetBuffer;
      rot_enable_control(RotatorConnected);
      rot_send_cmd(ROT_I_REQUEST_INIT, '0');
      rot_send_cmd(SHUT_I_QRY_ENDSTOP, '0');

    except

      on E: EOleException do
      begin
        mm_info_add(E.Message);

      end;
    end;
  end
  else
  begin
    try
      cmprt.Connected := False;
      RotatorConnected := False;
      rot_enable_control(RotatorConnected);
      btn_rot_con.Text := LBL_ROT_CON;

    except

      on E: EOleException do
      begin
        mm_info_add(E.Message);

      end;
    end;
  end;
end;

procedure TMain_form.btn_rot_estopClick(Sender: TObject);
begin

  rot_send_cmd(ROT_I_EMERGENCY_STOP, '0');
end;

procedure TMain_form.btn_rot_find_homeClick(Sender: TObject);
begin

  rot_send_cmd(ROT_IO_HOME, '0');
end;

procedure TMain_form.btn_rot_go_homeClick(Sender: TObject);
begin

  rot_send_cmd(ROT_IO_DMPOS, '0.0');
end;

procedure TMain_form.btn_rot_resetClick(Sender: TObject);
begin
  if FMX.Dialogs.MessageDlg('System will be rebooted, do You want to proceed?', TMsgDlgType.mtWarning, [TMsgDlgBtn.mbYes, TMsgDlgBtn.mbNo], 0, TMsgDlgBtn.mbYes) = mrYes then
  begin
    rot_send_cmd(ROT_I_RESET, '0');
    btn_rot_con.OnClick(btn_rot_con);
  end;


  //btn_rot_con.OnClick(btn_rot_con);
end;

procedure TMain_form.btn_rot_settingsClick(Sender: TObject);
begin
  cmprt.ShowSetupDialog;
  ComPort := cmprt.Port;
  SaveReg;
end;

procedure TMain_form.btn_rot_stopClick(Sender: TObject);
begin
  rot_send_cmd(ROT_I_STOP, '0');
end;

procedure TMain_form.chk_alt_enChange(Sender: TObject);
begin
  SaveReg;
end;

procedure TMain_form.cmdtpckt1Packet(Sender: TObject; const Str: string);
var
  sp_s: string;
begin
  //Incoming packets ordered by frequently to rare

 // if chk_fulllog.IsChecked then
  mm_add(RX, Str);

  SLcmd.DelimitedText := Str;
  SLval.DelimitedText := SLcmd[1];

  //rotator postion get and sync with jog
  if SLcmd[0] = ROT_IO_DMPOS then
  begin
    UDP_transASCOM(Str);
    sp_s := SplitString(SLcmd[1], '.')[0];
    arcdl_az.Value := StrToInt(ArcdialtoAZ(StrToInt(sp_s)));
    exit;
  end;

  //shutter moving processing, splitting the incomming msg example c#1:30 -> msgtype 'c' | state '1' | value '30'
  //message type:  SLcmd[0]
  //state: SplitString(SLcmd[1], ':')[0]
  //value: SplitString(SLcmd[1], ':')[1]
  if ((SLcmd[0] = SHUT_IO_OPEN) or (SLcmd[0] = SHUT_IO_CLOSE)) and (SplitString(SLcmd[1], ':')[0] = '1') then
 //   if ((SLcmd[0] = SHUT_IO_OPEN) or (SLcmd[0] = SHUT_IO_CLOSE)) and (SLcmd[1] = '0') then

  begin

  //UDP_transASCOM(Str);
    UDP_transASCOM(SLcmd[0] + '#' + '1');
   // mm_info.Lines.Add(SplitString(SLcmd[1], ':')[1]);
   // shut_controll(SLcmd[0], SLcmd[1]);
    shut_controll(SLcmd[0], '1', SplitString(SLcmd[1], ':')[1]);
    btn_move_shut.Enabled := False;
    btn_emerg_close.Enabled := True;
    exit;
  end;

  //shutter opened/closed
  if ((SLcmd[0] = SHUT_IO_OPEN) or (SLcmd[0] = SHUT_IO_CLOSE)) and (SLcmd[1] = '0') then
  begin
    UDP_transASCOM(Str);
    shut_controll(SLcmd[0], SLcmd[1], '');
    btn_move_shut.Enabled := True;
    btn_emerg_close.Enabled := True;
    exit;
  end;

  //shutter close/open errors when endstop not reached or something hitted the endstops
  if ((SLcmd[0] = SHUT_IO_OPEN) or (SLcmd[0] = SHUT_IO_CLOSE)) and (SLcmd[1] = '-1') then
  begin
    UDP_transASCOM(Str);
    shut_controll(SLcmd[0], SLcmd[1], 'n/a');
    btn_move_shut.Enabled := True;
    btn_emerg_close.Enabled := True;
    exit;
  end;
  //

  if (SLcmd[0] = SHUT_O_INFORMATION) then
  begin
    if SLcmd[1] = '0' then        //shutter disconnected
    begin
      UDP_transASCOM(Str);
      Shut_connected := false;
      btn_move_shut.Enabled := Shut_connected;
      btn_emerg_close.Enabled := Shut_connected;
      btn_shut_estp.Enabled := Shut_connected;

      exit;
    end;
    if SLcmd[1] = '1' then     //shutter connected
    begin
      UDP_transASCOM(Str);
      Shut_connected := true;
      btn_move_shut.Enabled := Shut_connected;
      btn_emerg_close.Enabled := Shut_connected;
      exit;
    end;
  end;


  //shut information
  if (SLcmd[0] = ROT_O_INFORMATION) then
  begin
    if SLcmd[1] = '0' then
    begin
      mm_info_add('Rotator emergency STOP!');
      exit;
    end;
    if SLcmd[1] = '1' then
    begin
      mm_info_add('Rotator STOP!');
      exit;
    end;
    if SLcmd[1] = '2' then
    begin
      mm_info_add('Rotator finding home');
      exit;
    end;
    if SLcmd[1] = '3' then
    begin
      mm_info_add('Rotator reached home nothing to do');
      exit;
    end;
    if SLcmd[1] = '4' then
    begin
      mm_info_add('Rotator at home, zerosearch finished, reset Rotator pos to 0');
      exit;
    end;
    if SLcmd[1] = '5' then
    begin
      mm_info_add('Rotator fliped due to ALT limit, ingoring AZIMUTH until ALT over the limit');
      exit;
    end;

    if SLcmd[1] = '6' then
    begin
      mm_info_add('Rotator is returning to AZIMUTH, ALT below the limit');
      exit;
    end;

  end;

  if (SLcmd[0] = ROT_O_DHT) then
  begin
    rot_DHT_recive(SLval[0], SLval[1]);
    exit;
  end;


  //just store dome params
  if (SLcmd[0] = ROT_O_PARAMS) then
  begin
    if (SLval[0] = '0') then
    begin
      Self.Caption := 'EzDome - fw:' + SLval[1];
      SLParamLst.Add('FW: ' + SLval[1]);
      exit;
    end;
    if (SLval[0] = '1') then
    begin
      SLParamLst.Add('Step/revolution: ' + SLval[1]);
      exit;
    end;
    if (SLval[0] = '2') then
    begin
      SLParamLst.Add('Full rotation/step: ' + SLval[1]);
      exit;
    end;
    if (SLval[0] = '3') then
    begin
      SLParamLst.Add('Step/DM: ' + SLval[1]);
      exit;
    end;

  end;

end;

procedure TMain_form.FormClose(Sender: TObject; var Action: TCloseAction);
begin

  SaveReg;
  RegINI.Free;
  SLParamLst.Free;
  SLcmd.Free;
  SLcmdHistory.Free;
  SLval.Free;
end;

procedure TMain_form.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
begin
  if (MountConnected = true) or (RotatorConnected = true) then
    CanClose := False
  else
    CanClose := true;
end;

procedure TMain_form.FormCreate(Sender: TObject);
begin

  RegINI := TRegistryIniFile.Create('SOFTWARE');
  cmdhistoryStepping := -1;
  LoadReg;
  idpsrvr1.DefaultPort := UDPPort;

  if (UDPPort > 0) then

  else
    UDPPort := 9999;

  idpsrvr1.Active := True;

  btn_rot_con.Text := LBL_ROT_CON;
  rot_enable_control(False);

  SLcmd := TStringlist.Create;
  SLval := TStringlist.Create;
  SLUDPcmd := TStringlist.Create;
  SLUDPval := TStringlist.Create;
  SLcmdHistory := TStringlist.Create;
  SLParamLst := TStringlist.Create;

  SLcmd.Delimiter := DELIMETER;
  SLcmd.StrictDelimiter := True;

  SLval.Delimiter := DMSEP;
  SLval.StrictDelimiter := True;

  SLUDPcmd.Delimiter := DELIMETER;
  SLUDPcmd.StrictDelimiter := True;
  SLUDPval := TStringlist.Create;
  SLUDPval.Delimiter := DMSEP;
  SLUDPval.StrictDelimiter := True;

  FormatSettings.DecimalSeparator := '.';
  RotatorConnected := False;
  MountConnected := False;

  cmdtpckt1.StopString := #13#10;

  btn_adv.OnClick(btn_adv);
  ALTorAZ := true;
  rot_DHT_recive('', '');

end;

procedure TMain_form.idpsrvr1Status(ASender: TObject; const AStatus: TIdStatus; const AStatusText: string);
begin
  mm_info_add('UDP:' + tab + AStatusText);
end;

procedure TMain_form.idpsrvr1UDPException(AThread: TIdUDPListenerThread; ABinding: TIdSocketHandle; const AMessage: string; const AExceptionClass: TClass);
begin
 // mm_info_add('UDPError:' + tab + AMessage);
end;

procedure TMain_form.idpsrvr1UDPRead(AThread: TIdUDPListenerThread; const AData: TIdBytes; ABinding: TIdSocketHandle);
var
  UdpRecString: string;
begin
  PeerIp := ABinding.PeerIP;
  PeerPort := ABinding.PeerPort;

  UdpRecString := TEncoding.UTF8.GetString(AData);
  if UdpRecString <> '' then
  try
    SLUDPcmd.DelimitedText := UdpRecString;

    if chk_alt_en.IsChecked or (not chk_alt_en.IsChecked and (ROT_I_ALT <> 'ALT')) then
    begin
      UDP_trns_Serial(UdpRecString);
      mm_log.Lines.Add(DateTimeToStr(Now) + ' UDP RX: ' + UdpRecString);
    end;

  except

  end;
end;

procedure TMain_form.UDP_transASCOM(s: string);
begin
  if PeerIp <> '' then
  try
    mm_log.Lines.Add(DateTimeToStr(Now) + ' UDP TX: ' + s);
    idpsrvr1.Send(PeerIp, PeerPort, s, IndyASCIIEncoding());
  except
  end;
end;

procedure TMain_form.UDP_trns_Serial(cmd: string);
var
  aTask: ITask;
begin

  aTask := TTask.Create(
    procedure
    begin
     // sleep(3000); // 3 seconds
      TThread.Synchronize(TThread.Current,
        procedure
        begin

          cmprt.WriteStr(cmd);

        end);

    end);
  aTask.ExecuteWork;

end;

end.

