// TODO fill in this information for your driver, then remove this line!
//
// ASCOM Dome hardware class for EzDome
//
// Description:	 <To be completed by driver developer>
//
// Implements:	ASCOM Dome interface version: <To be completed by driver developer>
// Author:		(XXX) Your N. Here <your@email.here>
//

using ASCOM.Astrometry.AstroUtils;
using ASCOM.DeviceInterface;
using ASCOM.LocalServer;
using ASCOM.Utilities;
using System;
using System.Collections;
using System.Data.Odbc;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;



namespace ASCOM.EzDome.Dome
{



    //
    // TODO Replace the not implemented exceptions with code to implement the function or throw the appropriate ASCOM exception.
    //

    /// <summary>
    /// ASCOM Dome hardware class for EzDome.
    /// </summary>
    [HardwareClass()] // Class attribute flag this as a device hardware class that needs to be disposed by the local server when it exits.
    internal static class DomeHardware
    {
        // Constants used for Profile persistence
        private static string DriverProgId = ""; // ASCOM DeviceID (COM ProgID) for this driver, the value is set by the driver's class initialiser.
        private static string DriverDescription = ""; // The value is set by the driver's class initialiser.
        internal static string UDP_IP; // COM port name (if required)
        internal static string UDP_PORT;
        private static bool connectedState; // Local server's connected state
        private static bool runOnce = false; // Flag to enable "one-off" activities only to run once.
        internal static Util utilities; // ASCOM Utilities object for use as required
        internal static AstroUtils astroUtilities; // ASCOM AstroUtilities object for use as required
        internal static TraceLogger tl; // Local server's trace logger object for diagnostic log with information that you specify

        //EzDome stuffs
        internal static string IPProfileName = "UDP IP";
        internal static string UDPPortProfileName = "UDP Port";// Constants used for Profile persistence
        internal static string IPDefault = "127.0.0.1";
        internal static string UDPPortDefault = "9999";
        internal static string traceStateProfileName = "Trace Level";
        internal static string traceStateDefault = "true";

        private static UdpClient _udpClient;
        // private static Thread _udpThread;
        private static Task _udpThread;
        private static bool _isRunning = true;


        internal static bool rot_AtHome = false;
        internal static bool rot_Slewing = false;
        internal static string DM_POS;


        static internal ArrayList param_lst = new ArrayList();
        static internal ASCOM.Utilities.Util AsUtil = new ASCOM.Utilities.Util();


        //COMMANDS ROTATOR IN
        internal const string
          ROT_I_REQUEST_INIT = "I",
          ROT_I_STOP = "P",            //Stop rotator
          ROT_I_EMERGENCY_STOP = "Y",  //Emergency stop rotator
          ROT_I_PARK = "K",            //PARK rotator
          ROT_I_SYNC = "S",            // Sync to AZ
          ROT_I_RESET = "R",           // Reset every non cons value
          ROT_I_QUERY_POS = "Q",
          ROT_I_ALT = "L",

        //COMMANDS ROTATOR IN/OUT
        ROT_IO_AZPOS = "A",  //GOTO AZ position
          ROT_IO_DMPOS = "D",  //GOTO AZ position
          ROT_IO_POS = "P",
          ROT_IO_HOME = "H",  // Rotator find home or send dome home

          //COMMANDS ROTATOR OUT

          ROT_O_PARAMS = "P",
          ROT_O_INFORMATION = "I",
            ROT_O_DHT = "T",
          ROT_TEST = "X", //used for testing


            ////SHUTTER COMMANDS
            //IN
            SHUT_I_EMERGENCY_STOP = "y",
            SHUT_I_RESET = "r",  //Emergency stop shutter

            //N/OUT
            //SHUT_IO_INIT = "i", //send inited/connected or request init value 1 connected 0 disconnected
            SHUT_IO_CLOSE = "c",
            SHUT_IO_OPEN = "o",
            //SHUT_IO_VOLTAGE = "w", //not developed yet

            //OUT
            SHUT_O_MOVING = "m",
            SHUT_O_VER = "v",
            SHUT_O_INFORMATION = "f",
            SHUT_O_INIT = "i",

            //TESTING
            C_TEST = "x",

        //Serial RX/TX seperators

        DELIMETER = "#",

        SlineEnding = "\r\n";

        //EzDome stuffs soros kapcsolat stb itt minden más ami GUI-val való kommunikáció az pedig a domedriver.cs-ben


        #region ez dome stuff



        //Client
        private static void StartUDPClient(string hostname, int port)
        {
              _udpClient = new UdpClient();
              _udpClient.Connect(hostname, port);
              _udpClient.Client.ReceiveTimeout = 5000;
              _udpClient.Client.ReceiveBufferSize = 65536;
              _udpThread = Task.Run(async () => await ReceiveUDPData());
              _isRunning = true;


        }

        private static void SendUDPData(string c_type, string msg)
        {
            String m = c_type + DELIMETER + msg;
            byte[] datagram = Encoding.UTF8.GetBytes(m);
            tl.LogMessage("UDP TX:", m);
            _udpClient.Send(datagram, datagram.Length);
        }

        private static async Task ReceiveUDPData()
        {
          /*  string[] RX;
            string RX_cmd;
            string RX_cmd_val;
            string RX_S;
            string[] RX_cmd_par;*/

            IPEndPoint remoteEndPoint = new IPEndPoint(IPAddress.Any, int.Parse(UDP_PORT));
            while (_isRunning)
            {
                try
                {
                    UdpReceiveResult receivedResults = await _udpClient.ReceiveAsync();
                    string RX_S = Encoding.UTF8.GetString(receivedResults.Buffer);

                    if (string.IsNullOrWhiteSpace(RX_S) || !RX_S.Contains("#"))
                        continue;

                    string[] RX = RX_S.Split('#');
                    if (RX.Length < 2) continue;

                    string RX_cmd = RX[0];
                    string RX_cmd_val = RX[1];
                    string[] RX_cmd_par = RX_cmd_val.Split(':');
                    if (RX.Length > 1)
                    {
                        if (RX_cmd == ROT_IO_DMPOS)
                        {
                            rot_Slewing = true;
                        }
                        else
                        {
                            rot_Slewing = false;
                        }
                        switch (RX_cmd)
                        {
                            case ROT_IO_DMPOS:
                                DM_POS = RX[1];
                                break;
                            case SHUT_O_INFORMATION:
                                {
                                    switch (RX[1])
                                    {
                                        case "0":
                                            {
                                                domeShutterStates = 4;
                                                break;
                                            }
                                    }
                                    break;
                                }
                            case SHUT_IO_CLOSE:
                                {
                                    switch (RX[1])
                                    {
                                        case "1":
                                            {
                                                domeShutterStates = 2;
                                                break;
                                            }
                                        case "0":
                                            {
                                                domeShutterStates = 0;
                                                break;
                                            }
                                        case "-1":
                                            {
                                                domeShutterStates = 4;
                                                break;
                                            }
                                    }
                                    break;
                                }
                            case SHUT_IO_OPEN:
                                {

                                    switch (RX[1])
                                    {
                                        case "1":
                                            {
                                                domeShutterStates = 3;
                                                break;
                                            }
                                        case "0":
                                            {
                                                domeShutterStates = 1;
                                                break;
                                            }
                                        case "-1":
                                            {
                                                domeShutterStates = 4;
                                                break;
                                            }
                                    }
                                    break;
                                }
                            //parameters storing
                            case ROT_O_PARAMS:
                                switch (RX_cmd_par[0])
                                {
                                    case "0":
                                        {
                                            param_lst.Add("FW: " + RX_cmd_par[1]);
                                            break;
                                        }
                                    case "1":
                                        {
                                            param_lst.Add("Step/revolution: " + RX_cmd_par[1]);
                                            break;
                                        }
                                    case "2":
                                        {
                                            param_lst.Add("Full rotation/step: " + RX_cmd_par[1]);
                                            break;
                                        }
                                    case "3":
                                        {
                                            param_lst.Add("Step/DM: " + RX_cmd_par[1]);
                                            break;
                                        }
                                }
                                break;
                            case ROT_IO_HOME:
                                rot_AtHome = true;
                                break;
                                //shutter controll checking, 0 closed, 1 opened, 2 closing, 3 opening, 4 error

                                // end of shutter controllg check
                              

                        }

                
                    }
                    tl.LogMessage("UDP RX:", RX_S);
                    RX_cmd = string.Empty;
                    RX_S = string.Empty;

                }     
                catch (SocketException ex)
                {
                    tl.LogMessage("UDP Error: ", ex.Message);
                }
                catch (Exception ex)
                {
                    tl.LogMessage("UDP General Error", ex.ToString());
                }
                // Thread.Sleep(1);
            }
        }
        #endregion ez dome stuff



        /// <summary>
        /// Initializes a new instance of the device Hardware class.
        /// </summary>
        static DomeHardware()
        {
            try
            {
                // Create the hardware trace logger in the static initialiser.
                // All other initialisation should go in the InitialiseHardware method.
                tl = new TraceLogger("", "EzDome.Hardware");

                // DriverProgId has to be set here because it used by ReadProfile to get the TraceState flag.
                DriverProgId = Dome.DriverProgId; // Get this device's ProgID so that it can be used to read the Profile configuration values

                // ReadProfile has to go here before anything is written to the log because it loads the TraceLogger enable / disable state.
                ReadProfile(); // Read device configuration from the ASCOM Profile store, including the trace state

                LogMessage("DomeHardware", $"Static initialiser completed.");
            }
            catch (Exception ex)
            {
                try { LogMessage("DomeHardware", $"Initialisation exception: {ex}"); } catch { }
                MessageBox.Show($"{ex.Message}", "Exception creating ASCOM.EzDome.Dome", MessageBoxButtons.OK, MessageBoxIcon.Error);
                throw;
            }
        }

        /// <summary>
        /// Place device initialisation code here that delivers the selected ASCOM <see cref="Devices."/>
        /// </summary>
        /// <remarks>Called every time a new instance of the driver is created.</remarks>
        internal static void InitialiseHardware()
        {
            // This method will be called every time a new ASCOM client loads your driver
            LogMessage("InitialiseHardware", $"Start.");

            // Make sure that "one off" activities are only undertaken once
            if (runOnce == false)
            {
                LogMessage("InitialiseHardware", $"Starting one-off initialisation.");

                DriverDescription = Dome.DriverDescription; // Get this device's Chooser description

                LogMessage("InitialiseHardware", $"ProgID: {DriverProgId}, Description: {DriverDescription}");

                connectedState = false; // Initialise connected to false
                utilities = new Util(); //Initialise ASCOM Utilities object
                astroUtilities = new AstroUtils(); // Initialise ASCOM Astronomy Utilities object

                LogMessage("InitialiseHardware", "Completed basic initialisation");

                // Add your own "one off" device initialisation here e.g. validating existence of hardware and setting up communications

                LogMessage("InitialiseHardware", $"One-off initialisation complete.");
                runOnce = true; // Set the flag to ensure that this code is not run again
            }
        }

        // PUBLIC COM INTERFACE IDomeV2 IMPLEMENTATION

        #region Common properties and methods.

        /// <summary>
        /// Displays the Setup Dialogue form.
        /// If the user clicks the OK button to dismiss the form, then
        /// the new settings are saved, otherwise the old values are reloaded.
        /// THIS IS THE ONLY PLACE WHERE SHOWING USER INTERFACE IS ALLOWED!
        /// </summary>
        public static void SetupDialog()
        {
            // Don't permit the setup dialogue if already connected
            if (IsConnected)
                MessageBox.Show("Already connected, just press OK");

            using (SetupDialogForm F = new SetupDialogForm(tl))
            {
                LogMessage("SetupDialog", "SetupDialog");
                var result = F.ShowDialog();
                if (result == DialogResult.OK)
                {
                    WriteProfile(); // Persist device configuration values to the ASCOM Profile store
                }
            }
        }

        /// <summary>Returns the list of custom action names supported by this driver.</summary>
        /// <value>An ArrayList of strings (SafeArray collection) containing the names of supported actions.</value>
        public static ArrayList SupportedActions
        {
            get
            {
                var actions = new ArrayList();
                actions = param_lst;

                LogMessage("SupportedActions Get", "Returning empty ArrayList");
                return new ArrayList();
            }
        }

        /// <summary>Invokes the specified device-specific custom action.</summary>
        /// <param name="ActionName">A well known name agreed by interested parties that represents the action to be carried out.</param>
        /// <param name="ActionParameters">List of required parameters or an <see cref="String.Empty">Empty String</see> if none are required.</param>
        /// <returns>A string response. The meaning of returned strings is set by the driver author.
        /// <para>Suppose filter wheels start to appear with automatic wheel changers; new actions could be <c>QueryWheels</c> and <c>SelectWheel</c>. The former returning a formatted list
        /// of wheel names and the second taking a wheel name and making the change, returning appropriate values to indicate success or failure.</para>
        /// </returns>
        public static string Action(string actionName, string actionParameters)
        {
            LogMessage("Action", $"Action {actionName}, parameters {actionParameters} is not implemented");
            throw new ActionNotImplementedException("Action " + actionName + " is not implemented by this driver");
        }

        /// <summary>
        /// Transmits an arbitrary string to the device and does not wait for a response.
        /// Optionally, protocol framing characters may be added to the string before transmission.
        /// </summary>
        /// <param name="Command">The literal command string to be transmitted.</param>
        /// <param name="Raw">
        /// if set to <c>true</c> the string is transmitted 'as-is'.
        /// If set to <c>false</c> then protocol framing characters may be added prior to transmission.
        /// </param>
        public static void CommandBlind(string command, bool raw)
        {
            CheckConnected("CommandBlind");
            // TODO The optional CommandBlind method should either be implemented OR throw a MethodNotImplementedException
            // If implemented, CommandBlind must send the supplied command to the mount and return immediately without waiting for a response

            throw new MethodNotImplementedException($"CommandBlind - Command:{command}, Raw: {raw}.");
        }

        /// <summary>
        /// Transmits an arbitrary string to the device and waits for a boolean response.
        /// Optionally, protocol framing characters may be added to the string before transmission.
        /// </summary>
        /// <param name="Command">The literal command string to be transmitted.</param>
        /// <param name="Raw">
        /// if set to <c>true</c> the string is transmitted 'as-is'.
        /// If set to <c>false</c> then protocol framing characters may be added prior to transmission.
        /// </param>
        /// <returns>
        /// Returns the interpreted boolean response received from the device.
        /// </returns>
        public static bool CommandBool(string command, bool raw)
        {
            CheckConnected("CommandBool");
            // TODO The optional CommandBool method should either be implemented OR throw a MethodNotImplementedException
            // If implemented, CommandBool must send the supplied command to the mount, wait for a response and parse this to return a True or False value

            throw new MethodNotImplementedException($"CommandBool - Command:{command}, Raw: {raw}.");
        }

        /// <summary>
        /// Transmits an arbitrary string to the device and waits for a string response.
        /// Optionally, protocol framing characters may be added to the string before transmission.
        /// </summary>
        /// <param name="Command">The literal command string to be transmitted.</param>
        /// <param name="Raw">
        /// if set to <c>true</c> the string is transmitted 'as-is'.
        /// If set to <c>false</c> then protocol framing characters may be added prior to transmission.
        /// </param>
        /// <returns>
        /// Returns the string response received from the device.
        /// </returns>
        public static string CommandString(string command, bool raw)
        {
            CheckConnected("CommandString");
            // TODO The optional CommandString method should either be implemented OR throw a MethodNotImplementedException
            // If implemented, CommandString must send the supplied command to the mount and wait for a response before returning this to the client

            throw new MethodNotImplementedException($"CommandString - Command:{command}, Raw: {raw}.");
        }

        /// <summary>
        /// Deterministically release both managed and unmanaged resources that are used by this class.
        /// </summary>
        /// <remarks>
        /// TODO: Release any managed or unmanaged resources that are used in this class.
        /// 
        /// Do not call this method from the Dispose method in your driver class.
        ///
        /// This is because this hardware class is decorated with the <see cref="HardwareClassAttribute"/> attribute and this Dispose() method will be called 
        /// automatically by the  local server executable when it is irretrievably shutting down. This gives you the opportunity to release managed and unmanaged 
        /// resources in a timely fashion and avoid any time delay between local server close down and garbage collection by the .NET runtime.
        ///
        /// For the same reason, do not call the SharedResources.Dispose() method from this method. Any resources used in the static shared resources class
        /// itself should be released in the SharedResources.Dispose() method as usual. The SharedResources.Dispose() method will be called automatically 
        /// by the local server just before it shuts down.
        /// 
        /// </remarks>
        public static void Dispose()
        {
            try { LogMessage("Dispose", $"Disposing of assets and closing down."); } catch { }

            try
            {
                // Clean up the trace logger and utility objects
                tl.Enabled = false;
                tl.Dispose();
                tl = null;
            }
            catch { }

            try
            {
                utilities.Dispose();
                utilities = null;
            }
            catch { }

            try
            {
                astroUtilities.Dispose();
                astroUtilities = null;
            }
            catch { }
        }

        /// <summary>
        /// Set True to connect to the device hardware. Set False to disconnect from the device hardware.
        /// You can also read the property to check whether it is connected. This reports the current hardware state.
        /// </summary>
        /// <value><c>true</c> if connected to the hardware; otherwise, <c>false</c>.</value>
        public static bool Connected
        {
            get
            {
                //  StartUDPClient(UDP_IP, int.Parse(UDP_PORT));
                //  LogMessage("Connected", $"Get {IsConnected}");


                return IsConnected;
            }
            set
            {
                LogMessage("Connected", $"Set {value}");
                if (value == IsConnected)
                    return;

                if (value)
                {
                    // TODO insert connect to the device code here
                    StartUDPClient(UDP_IP, int.Parse(UDP_PORT));

                    SendUDPData(ROT_I_REQUEST_INIT, "0");
                    SendUDPData(ROT_I_QUERY_POS, "0");
                    SendUDPData(SHUT_O_INIT, "0");
                    //   LogMessage("Connected Set", $"Connecting to port {comPort}");
                    connectedState = true;
                }
                else
                {
                    // TODO insert disconnect from the device code here
                    _isRunning = false;
                    _udpClient.Close();
                    //    _udpThread?.Join();
                    //  LogMessage("Connected Set", $"Disconnecting from port {comPort}");
                    connectedState = false;
                }
            }
        }

        /// <summary>
        /// Returns a description of the device, such as manufacturer and model number. Any ASCII characters may be used.
        /// </summary>
        /// <value>The description.</value>
        public static string Description
        {
            // TODO customise this device description if required
            // TODO customise this device description if required
            get
            {
                LogMessage("Description Get", DriverDescription);
                return DriverDescription;
            }
        }

        /// <summary>
        /// Descriptive and version information about this ASCOM driver.
        /// </summary>
        public static string DriverInfo
        {
            get
            {
                Version version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
                // TODO customise this driver description if required
                string driverInfo = $"Information about the driver itself. Version: {version.Major}.{version.Minor}";
                LogMessage("DriverInfo Get", driverInfo);
                return driverInfo;
            }
        }

        /// <summary>
        /// A string containing only the major and minor version of the driver formatted as 'm.n'.
        /// </summary>
        public static string DriverVersion
        {
            get
            {
                Version version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
                string driverVersion = $"{version.Major}.{version.Minor}";
                LogMessage("DriverVersion Get", driverVersion);
                return driverVersion;
            }
        }

        /// <summary>
        /// The interface version number that this device supports.
        /// </summary>
        public static short InterfaceVersion
        {
            // set by the driver wizard
            get
            {
                LogMessage("InterfaceVersion Get", "2");
                return Convert.ToInt16("2");
            }
        }

        /// <summary>
        /// The short name of the driver, for display purposes
        /// </summary>
        public static string Name
        {
            // TODO customise this device name as required
            get
            {
                string name = "EzDome";
                LogMessage("Name Get", name);
                return name;
            }
        }

        #endregion

        #region IDome Implementation

        private static int domeShutterStates; // 0 closed, 1 opened, 2 closing, 3 opening, 4 close error, 5 open error

        /// <summary>
        /// Immediately stops any and all movement of the dome.
        /// </summary>
        internal static void AbortSlew()
        {
            SendUDPData(ROT_I_STOP, "0");
            // This is a mandatory parameter but we have no action to take in this simple driver
            LogMessage("AbortSlew", "Completed");
        }

        /// <summary>
        /// The altitude (degrees, horizon zero and increasing positive to 90 zenith) of the part of the sky that the observer wishes to observe.
        /// </summary>
        internal static double Altitude
        {
            get
            {
                //   LogMessage("Altitude Get", "Not implemented");
                throw new PropertyNotImplementedException("Altitude", false);
            }
        }

        /// <summary>
        /// <para><see langword="true" /> when the dome is in the home position. Raises an error if not supported.</para>
        /// <para>
        /// This is normally used following a <see cref="FindHome" /> operation. The value is reset
        /// with any azimuth slew operation that moves the dome away from the home position.
        /// </para>
        /// <para>
        /// <see cref="AtHome" /> may optionally also become true during normal slew operations, if the
        /// dome passes through the home position and the dome controller hardware is capable of
        /// detecting that; or at the end of a slew operation if the dome comes to rest at the home
        /// position.
        /// </para>
        /// </summary>
        internal static bool AtHome
        {
            get
            {


                //LogMessage("AtHome Get", "Not implemented");
                //  throw new PropertyNotImplementedException("AtHome", true);
                return rot_AtHome;
            }

        }

        /// <summary>
        /// <see langword="true" /> if the dome is in the programmed park position.
        /// </summary>
        internal static bool AtPark
        {
            get
            {   //commented out
                // LogMessage("AtPark Get", "Not implemented");
                throw new PropertyNotImplementedException("AtPark", false);
            }
        }

        /// <summary>
        /// The dome azimuth (degrees, North zero and increasing clockwise, i.e., 90 East, 180 South, 270 West). North is true north and not magnetic north.
        /// </summary>
        internal static double Azimuth
        {

            get
            {
                return Convert.ToDouble(DM_POS, CultureInfo.InvariantCulture);
                
            }
        }



        /// <summary>
        /// Close the shutter or otherwise shield the telescope from the sky.
        /// </summary>
        internal static void CloseShutter()
        {
            SendUDPData(SHUT_IO_CLOSE, "0");
            LogMessage("CloseShutter", "Shutter has been closed");
        }

        /// <summary>
        /// Start operation to search for the dome home position.
        /// </summary>
        internal static void FindHome()
        {
            rot_Slewing = true;
            SendUDPData(ROT_IO_HOME, "0");
            // LogMessage("FindHome", "Not implemented");
            // throw new MethodNotImplementedException("FindHome");
        }

        /// <summary>
        /// Open shutter or otherwise expose telescope to the sky.
        /// </summary>
        internal static void OpenShutter()
        {

            SendUDPData(SHUT_IO_OPEN, "0");
            LogMessage("OpenShutter", "Shutter has been opened");

        }

        /// <summary>
        /// Rotate dome in azimuth to park position.
        /// </summary>
        internal static void Park()
        {
            // LogMessage("Park", "Not implemented");
            throw new MethodNotImplementedException("Park");
        }

        /// <summary>
        /// Set the current azimuth position of dome to the park position.
        /// </summary>
        internal static void SetPark()
        {
            // LogMessage("SetPark", "Not implemented");
            throw new MethodNotImplementedException("SetPark");
        }

        /// <summary>
        /// Gets the status of the dome shutter or roof structure.
        /// </summary>
        internal static ShutterState ShutterStatus
        {
            get
            {


                //  LogMessage("ShutterStatus Get", true.ToString());
                //switch (domeShutterStates)
                //{
                //    case 0:
                //        {
                //            LogMessage("ShutterStatus", ShutterState.shutterClosed.ToString());
                //            return ShutterState.shutterClosed;

                //        }
                //    case 1:
                //        {
                //            LogMessage("ShutterStatus", ShutterState.shutterOpen.ToString());
                //            return ShutterState.shutterOpen;

                //        }
                //    case 2:
                //        {
                //            LogMessage("ShutterStatus", ShutterState.shutterClosed.ToString());
                //            return ShutterState.shutterClosing;

                //        }
                //    case 3:
                //        {
                //            LogMessage("ShutterStatus", ShutterState.shutterOpening.ToString());
                //            return ShutterState.shutterOpening;

                //        }
                //    case -1:
                //        {
                //            LogMessage("ShutterStatus", ShutterState.shutterError.ToString());
                //            return ShutterState.shutterError;

                //        }
                //}

                if (domeShutterStates == 0)
                {
                    //  LogMessage("ShutterStatus", ShutterState.shutterClosed.ToString());
                    return ShutterState.shutterClosed;

                }
                if (domeShutterStates == 1)
                {
                    // LogMessage("ShutterStatus", ShutterState.shutterOpen.ToString());
                    return ShutterState.shutterOpen;

                }

                if (domeShutterStates == 2)
                {
                    //   LogMessage("ShutterStatus", ShutterState.shutterClosed.ToString());
                    return ShutterState.shutterClosing;

                }
                if (domeShutterStates == 3)
                {
                    // LogMessage("ShutterStatus", ShutterState.shutterOpening.ToString());
                    return ShutterState.shutterOpening;

                }

                else
                {

                    // LogMessage("ShutterStatus", ShutterState.shutterError.ToString());
                    return ShutterState.shutterError;

                }
            }

        }


        /// <summary>
        /// <see langword="true"/> if the dome is slaved to the telescope in its hardware, else <see langword="false"/>.
        /// </summary>
        internal static bool Slaved
        {
            get
            {
                LogMessage("Slaved Get", true.ToString());
               /* if (Slaved == true)
                    return true;
                else*/
                    return false;
            }
            set
            {
                LogMessage("Slaved Set", "not implemented");
                throw new PropertyNotImplementedException("Slaved", true);
            }
        }

        /// <summary>
        /// Ensure that the requested viewing altitude is available for observing.
        /// </summary>
        /// <param name="Altitude">
        /// The desired viewing altitude (degrees, horizon zero and increasing positive to 90 degrees at the zenith)
        /// </param>
        internal static void SlewToAltitude(double Altitude)
        {



            SendUDPData(ROT_I_ALT, (Math.Round(Altitude)).ToString());
            LogMessage("SlewToAltitude", "Alt limit checked ignoring Azimuth when limit reached");

            throw new MethodNotImplementedException("SlewToAltitude");

        }
        /// <summary>
        /// Ensure that the requested viewing azimuth is available for observing.
        /// The method should not block and the slew operation should complete asynchronously.
        /// </summary>
        /// <param name="Azimuth">
        /// Desired viewing azimuth (degrees, North zero and increasing clockwise. i.e., 90 East,
        /// 180 South, 270 West)
        /// </param>
        internal static void SlewToAzimuth(double Azimuth)
        {
            rot_Slewing = true;
            SendUDPData(ROT_IO_DMPOS, Azimuth.ToString(CultureInfo.InvariantCulture));

            // LogMessage("SlewToAzimuth", "Not implemented");
            // throw new MethodNotImplementedException("SlewToAzimuth");
        }

        /// <summary>
        /// <see langword="true" /> if any part of the dome is currently moving or a move command has been issued, 
        /// but the dome has not yet started to move. <see langword="false" /> if all dome components are stationary
        /// and no move command has been issued. /> 
        /// </summary>
        internal static bool Slewing
        {
            get
            {
                //commented out
                //  LogMessage("Slewing Get", rot_Slewing.ToString());
                return rot_Slewing;
            }
        }

        /// <summary>
        /// Synchronize the current position of the dome to the given azimuth.
        /// </summary>
        /// <param name="Azimuth">
        /// Target azimuth (degrees, North zero and increasing clockwise. i.e., 90 East,
        /// 180 South, 270 West)
        /// </param>
        /// 




        internal static void SyncToAzimuth(double Azimuth)
        {

            SendUDPData(ROT_I_SYNC, Azimuth.ToString(CultureInfo.InvariantCulture));
            //  LogMessage("SyncToAzimuth", "Not implemented");
            //   throw new MethodNotImplementedException("SyncToAzimuth");
        }

        #endregion

        #region Private properties and methods
        // Useful methods that can be used as required to help with driver development

        /// <summary>
        /// Returns true if there is a valid connection to the driver hardware
        /// </summary>
        private static bool IsConnected
        {
            get
            {
                // TODO check that the driver hardware connection exists and is connected to the hardware
                return connectedState;
            }
        }

        /// <summary>
        /// Use this function to throw an exception if we aren't connected to the hardware
        /// </summary>
        /// <param name="message"></param>
        private static void CheckConnected(string message)
        {
            if (!IsConnected)
            {
                throw new NotConnectedException(message);
            }
        }

        #region
        /// <summary>
        /// <see langword="true" /> if driver can perform a search for home position.
        /// </summary>
        internal static bool CanFindHome
        {
            get
            {
                LogMessage("CanFindHome Get", true.ToString());
                return true;
            }
        }

        /// <summary>
        /// <see langword="true" /> if the driver is capable of parking the dome.
        /// </summary>
        internal static bool CanPark
        {
            get
            {
                LogMessage("CanPark Get", false.ToString());
                return false;
            }
        }

        /// <summary>
        /// <see langword="true" /> if driver is capable of setting dome altitude.
        /// </summary>
        internal static bool CanSetAltitude
        {
            get
            {
                LogMessage("CanSetAltitude Get", false.ToString());
                return false;
            }
        }

        /// <summary>
        /// <see langword="true" /> if driver is capable of rotating the dome. Muste be <see "langword="false" /> for a 
        /// roll-off roof or clamshell.
        /// </summary>
        internal static bool CanSetAzimuth
        {
            get
            {
                LogMessage("CanSetAzimuth Get", true.ToString());
                return true;
            }
        }

        /// <summary>
        /// <see langword="true" /> if the driver can set the dome park position.
        /// </summary>
        internal static bool CanSetPark
        {
            get
            {
                LogMessage("CanSetPark Get", false.ToString());
                return false;
            }
        }

        /// <summary>
        /// <see langword="true" /> if the driver is capable of opening and closing the shutter or roof
        /// mechanism.
        /// </summary>
        internal static bool CanSetShutter
        {
            get
            {
                LogMessage("CanSetShutter Get", true.ToString());
                return true;
            }
        }

        /// <summary>
        /// <see langword="true" /> if the dome hardware supports slaving to a telescope.
        /// </summary>
        internal static bool CanSlave
        {
            get
            {
                LogMessage("CanSlave Get", true.ToString());
                return true;
            }
        }

        /// <summary>
        /// <see langword="true" /> if the driver is capable of synchronizing the dome azimuth position
        /// using the <see cref="SyncToAzimuth" /> method.
        /// </summary>
        internal static bool CanSyncAzimuth
        {
            get
            {
                LogMessage("CanSyncAzimuth Get", false.ToString());
                return true;
            }
        }

        #endregion
        /// <summary>
        /// Read the device configuration from the ASCOM Profile store
        /// </summary>
        internal static void ReadProfile()
        {
            using (Profile driverProfile = new Profile())
            {
                driverProfile.DeviceType = "Dome";
                tl.Enabled = Convert.ToBoolean(driverProfile.GetValue(DriverProgId, traceStateProfileName, string.Empty, traceStateDefault));
                UDP_IP = driverProfile.GetValue(DriverProgId, IPProfileName, string.Empty, IPDefault);
                UDP_PORT = driverProfile.GetValue(DriverProgId, UDPPortProfileName, string.Empty, UDPPortDefault);

                LogMessage("Profile", "ReadProfile");
            }
        }

        /// <summary>
        /// Write the device configuration to the  ASCOM  Profile store
        /// </summary>
        internal static void WriteProfile()
        {
            using (Profile driverProfile = new Profile())
            {
                driverProfile.DeviceType = "Dome";
                driverProfile.WriteValue(DriverProgId, traceStateProfileName, tl.Enabled.ToString());
                driverProfile.WriteValue(DriverProgId, IPProfileName, UDP_IP);
                driverProfile.WriteValue(DriverProgId, UDPPortProfileName, UDP_PORT);
                LogMessage("Profile", "WriteProfile");
            }
        }

        /// <summary>
        /// Log helper function that takes identifier and message strings
        /// </summary>
        /// <param name="identifier"></param>
        /// <param name="message"></param>
        internal static void LogMessage(string identifier, string message)
        {
            tl.LogMessageCrLf(identifier, message);
        }

        /// <summary>
        /// Log helper function that takes formatted strings and arguments
        /// </summary>
        /// <param name="identifier"></param>
        /// <param name="message"></param>
        /// <param name="args"></param>
        internal static void LogMessage(string identifier, string message, params object[] args)
        {
            var msg = string.Format(message, args);
            LogMessage(identifier, msg);
        }
        #endregion
    }
}

