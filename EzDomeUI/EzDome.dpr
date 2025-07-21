program EzDome;

uses
  System.StartUpCopy,
  FMX.Forms,
  main in 'main.pas' {Main_form};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TMain_form, Main_form);
  Application.Run;
end.
