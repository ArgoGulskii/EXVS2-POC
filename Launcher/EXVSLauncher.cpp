using namespace System;

#include "MainWindow.h"

using namespace System::Windows::Forms;

[STAThread]
int main()
{
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);
  Application::Run(gcnew EXVSLauncher::MainWindow());
  return 0;
}