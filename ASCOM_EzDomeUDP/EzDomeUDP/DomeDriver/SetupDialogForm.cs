using ASCOM.Utilities;
using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace ASCOM.EzDome.Dome
{
    [ComVisible(false)] // Form not registered for COM!
    public partial class SetupDialogForm : Form
    {
        const string NO_PORTS_MESSAGE = "No COM ports found";
        TraceLogger tl; // Holder for a reference to the driver's trace logger

        public SetupDialogForm(TraceLogger tlDriver)
        {
            InitializeComponent();

            // Save the provided trace logger for use within the setup dialogue
            tl = tlDriver;

            // Initialise current values of user settings from the ASCOM Profile
            InitUI();
        }

        private void CmdOK_Click(object sender, EventArgs e) // OK button event handler
        {
            // Place any validation constraint checks here and update the state variables with results from the dialogue

            tl.Enabled = chkTrace.Checked;

            // Update the COM port variable if one has been selected
            if (tb_ip.Text == "" && tb_port.Text == "" )   // No COM port selected
            {
                tl.LogMessage("Setup OK", $"New configuration values - IP and Port: Not selected");
            }

            else // A valid COM port has been selected
            {
                DomeHardware.UDP_IP = tb_ip.Text;
                DomeHardware.UDP_PORT = tb_port.Text;
                tl.LogMessage("Setup OK", $"New configuration values - Port: "+ DomeHardware.UDP_IP +" Port:"+ DomeHardware.UDP_PORT);
            }
        }

        private void CmdCancel_Click(object sender, EventArgs e) // Cancel button event handler
        {
            Close();
        }

        private void BrowseToAscom(object sender, EventArgs e) // Click on ASCOM logo event handler
        {
            try
            {
                System.Diagnostics.Process.Start("https://ascom-standards.org/");
            }
            catch (Win32Exception noBrowser)
            {
                if (noBrowser.ErrorCode == -2147467259)
                    MessageBox.Show(noBrowser.Message);
            }
            catch (Exception other)
            {
                MessageBox.Show(other.Message);
            }
        }

        private void InitUI()
        {

            // Set the trace checkbox
            chkTrace.Checked = tl.Enabled;

            // set the list of COM ports to those that are currently available

            tl.LogMessage("InitUI", $"Set UI controls to Trace: {chkTrace.Checked}, COM Port: "+DomeHardware.UDP_PORT);
        }

        private void SetupDialogForm_Load(object sender, EventArgs e)
        {
            // Bring the setup dialogue to the front of the screen
            if (WindowState == FormWindowState.Minimized)
                WindowState = FormWindowState.Normal;
            else
            {
                TopMost = true;
                Focus();
                BringToFront();
                TopMost = false;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}