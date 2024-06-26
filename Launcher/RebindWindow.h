#pragma once

namespace EXVSLauncher {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public
ref class RebindWindow : public System::Windows::Forms::Form {
 public:
  RebindWindow() { InitializeComponent(); }

 protected:
  ~RebindWindow() { delete components; }

 private:
  System::ComponentModel::Container ^ components;

#pragma region Windows Form Designer generated code
  /// <summary>
  /// Required method for Designer support - do not modify
  /// the contents of this method with the code editor.
  /// </summary>
  void InitializeComponent(void) {
    this->components = gcnew System::ComponentModel::Container();
    this->Size = System::Drawing::Size(300, 300);
    this->Text = L"RebindWindow";
    this->Padding = System::Windows::Forms::Padding(0);
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
  }
#pragma endregion
};
}  // namespace EXVSLauncher
