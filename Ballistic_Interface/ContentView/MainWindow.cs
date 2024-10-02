using BlueMystic;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using FastColoredTextBoxNS;
using RestSharp;
using System.Text.Json.Serialization;
using Newtonsoft.Json;
using System.Net.Http;
using System.Diagnostics;

namespace Ballistic_Interface
{
    public partial class MainWindow : Form
    {
        private DarkModeCS DarkMode = null;

        public MainWindow() => InitializeComponent();

        private void MainWindow_Load(object sender, EventArgs e)
        {
            DarkMode = new DarkModeCS(this);

            var Intellisense = new AutocompleteMenu(fastColoredTextBox1);
            Intellisense.MinFragmentLength = 2;
            Intellisense.Opacity = 0.5;
            Intellisense.AllowTabKey = true;

            var Keywords = new List<string>
            {
                "for", "do", "function", "end", "in", "pairs", "ipairs",

                "print", "warn", "error",

                "identity", "printidentity", "version", "getidentity",

                "loadstring",

                "wait",

                "readfile", "listfiles", "writefile",
                "makefolder", "appendfile", "isfile", "isfolder",
                "delfile", "delfolder", "dofile",

                "rconsoleclear", "rconsolecreate", "rconsoledestroy",
                "rconsoleprint", "rconsoleinput", "rconsoletitle",

                "time", "setclipboard", "getfps", "setfps", "isactive"
            };

            Intellisense.Items.SetAutocompleteItems(Keywords);
        }

        private async void ExecuteButton_Click(object sender, EventArgs e)
        {
            var Payload = new { Script = fastColoredTextBox1.Text };
            var Content = new StringContent(JsonConvert.SerializeObject(Payload), Encoding.UTF8, "application/json");

            try
            {
                using (var Client = new HttpClient())
                {
                    var Response = await Client.PostAsync("http://localhost:1337/Ballistic/Scheduled", Content);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Request failed: {ex.Message}");
            }

        }

        private void ClearButton_Click(object sender, EventArgs e)
            => fastColoredTextBox1.Clear();

        private void AttachButton_Click(object sender, EventArgs e)
            => Process.Start($"{Environment.CurrentDirectory}\\Ballistic_Injector.exe");
    }
}
