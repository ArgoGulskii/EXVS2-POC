#pragma once

#include "Configuration.h"

namespace EXVSLauncher {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MainWindow : public System::Windows::Forms::Form
	{
	public:
		MainWindow() : state_(new LauncherState())
		{
			InitializeComponent();
		}

	protected:
		~MainWindow()
		{
			delete state_;
		}

	private:
		System::Windows::Forms::CheckedListBox^ listboxInstances;
		System::Windows::Forms::Button^ buttonAdd;
		System::Windows::Forms::Button^ buttonDelete;
		System::Windows::Forms::Button^ buttonLaunch;

		System::Windows::Forms::Label^ labelName;
		System::Windows::Forms::TextBox^ textName;

		System::Windows::Forms::Label^ labelStoragePath;
		System::Windows::Forms::TextBox^ textStoragePath;

		System::Windows::Forms::Label^ labelMode;
		System::Windows::Forms::ComboBox^ comboMode;

		System::Windows::Forms::CheckBox^ checkboxHidden;

		System::Windows::Forms::Label^ labelInterface;
		System::Windows::Forms::ComboBox^ comboInterface;
		System::Windows::Forms::TextBox^ textInterface;

		System::Windows::Forms::Label^ labelDivider;

		System::Windows::Forms::Label^ labelGamePath;
		System::Windows::Forms::TextBox^ textGamePath;

		System::Windows::Forms::Label^ labelServerPath;
		System::Windows::Forms::TextBox^ textServerPath;

		System::Windows::Forms::CheckBox^ checkboxRebind;

		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->listboxInstances = (gcnew System::Windows::Forms::CheckedListBox());
			this->buttonLaunch = (gcnew System::Windows::Forms::Button());
			this->buttonAdd = (gcnew System::Windows::Forms::Button());
			this->labelName = (gcnew System::Windows::Forms::Label());
			this->labelStoragePath = (gcnew System::Windows::Forms::Label());
			this->textName = (gcnew System::Windows::Forms::TextBox());
			this->textStoragePath = (gcnew System::Windows::Forms::TextBox());
			this->comboMode = (gcnew System::Windows::Forms::ComboBox());
			this->labelMode = (gcnew System::Windows::Forms::Label());
			this->checkboxHidden = (gcnew System::Windows::Forms::CheckBox());
			this->textInterface = (gcnew System::Windows::Forms::TextBox());
			this->labelInterface = (gcnew System::Windows::Forms::Label());
			this->buttonDelete = (gcnew System::Windows::Forms::Button());
			this->labelDivider = (gcnew System::Windows::Forms::Label());
			this->textGamePath = (gcnew System::Windows::Forms::TextBox());
			this->labelGamePath = (gcnew System::Windows::Forms::Label());
			this->textServerPath = (gcnew System::Windows::Forms::TextBox());
			this->labelServerPath = (gcnew System::Windows::Forms::Label());
			this->checkboxRebind = (gcnew System::Windows::Forms::CheckBox());
			this->comboInterface = (gcnew System::Windows::Forms::ComboBox());
			this->SuspendLayout();
			// 
			// listboxInstances
			// 
			this->listboxInstances->FormattingEnabled = true;
			this->listboxInstances->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Foo", L"Bar", L"Baz" });
			this->listboxInstances->Location = System::Drawing::Point(12, 12);
			this->listboxInstances->Name = L"listboxInstances";
			this->listboxInstances->Size = System::Drawing::Size(141, 109);
			this->listboxInstances->TabIndex = 0;
			this->listboxInstances->ItemCheck += gcnew System::Windows::Forms::ItemCheckEventHandler(this, &MainWindow::listboxInstances_ItemCheck);
			this->listboxInstances->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::listboxInstances_SelectedIndexChanged);
			// 
			// buttonLaunch
			// 
			this->buttonLaunch->Location = System::Drawing::Point(12, 162);
			this->buttonLaunch->Name = L"buttonLaunch";
			this->buttonLaunch->Size = System::Drawing::Size(141, 35);
			this->buttonLaunch->TabIndex = 0;
			this->buttonLaunch->Text = L"Launch";
			this->buttonLaunch->UseVisualStyleBackColor = true;
			this->buttonLaunch->Click += gcnew System::EventHandler(this, &MainWindow::buttonLaunch_Click);
			// 
			// buttonAdd
			// 
			this->buttonAdd->Location = System::Drawing::Point(12, 130);
			this->buttonAdd->Name = L"buttonAdd";
			this->buttonAdd->Size = System::Drawing::Size(68, 26);
			this->buttonAdd->TabIndex = 1;
			this->buttonAdd->Text = L"Add";
			this->buttonAdd->UseVisualStyleBackColor = true;
			this->buttonAdd->Click += gcnew System::EventHandler(this, &MainWindow::buttonAdd_Click);
			// 
			// labelName
			// 
			this->labelName->AutoSize = true;
			this->labelName->Location = System::Drawing::Point(166, 15);
			this->labelName->Name = L"labelName";
			this->labelName->Size = System::Drawing::Size(35, 13);
			this->labelName->TabIndex = 5;
			this->labelName->Text = L"Name";
			// 
			// labelStoragePath
			// 
			this->labelStoragePath->AutoSize = true;
			this->labelStoragePath->Location = System::Drawing::Point(167, 41);
			this->labelStoragePath->Name = L"labelStoragePath";
			this->labelStoragePath->Size = System::Drawing::Size(69, 13);
			this->labelStoragePath->TabIndex = 6;
			this->labelStoragePath->Text = L"Storage Path";
			// 
			// textName
			// 
			this->textName->Location = System::Drawing::Point(246, 12);
			this->textName->Name = L"textName";
			this->textName->Size = System::Drawing::Size(241, 20);
			this->textName->TabIndex = 3;
			this->textName->TextChanged += gcnew System::EventHandler(this, &MainWindow::textName_Changed);
			// 
			// textStoragePath
			// 
			this->textStoragePath->Location = System::Drawing::Point(246, 38);
			this->textStoragePath->Name = L"textStoragePath";
			this->textStoragePath->Size = System::Drawing::Size(241, 20);
			this->textStoragePath->TabIndex = 4;
			this->textStoragePath->TextChanged += gcnew System::EventHandler(this, &MainWindow::textStoragePath_Changed);
			// 
			// comboMode
			// 
			this->comboMode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboMode->FormattingEnabled = true;
			this->comboMode->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Client", L"Live Monitor" });
			this->comboMode->Location = System::Drawing::Point(246, 64);
			this->comboMode->Name = L"comboMode";
			this->comboMode->Size = System::Drawing::Size(100, 21);
			this->comboMode->TabIndex = 5;
			this->comboMode->SelectionChangeCommitted += gcnew System::EventHandler(this, &MainWindow::comboMode_Changed);
			// 
			// labelMode
			// 
			this->labelMode->AutoSize = true;
			this->labelMode->Location = System::Drawing::Point(167, 67);
			this->labelMode->Name = L"labelMode";
			this->labelMode->Size = System::Drawing::Size(34, 13);
			this->labelMode->TabIndex = 10;
			this->labelMode->Text = L"Mode";
			// 
			// checkboxHidden
			// 
			this->checkboxHidden->AutoSize = true;
			this->checkboxHidden->Location = System::Drawing::Point(427, 66);
			this->checkboxHidden->Name = L"checkboxHidden";
			this->checkboxHidden->Size = System::Drawing::Size(60, 17);
			this->checkboxHidden->TabIndex = 6;
			this->checkboxHidden->Text = L"Hidden";
			this->checkboxHidden->UseVisualStyleBackColor = true;
			this->checkboxHidden->CheckStateChanged += gcnew System::EventHandler(this, &MainWindow::checkboxHidden_Changed);
			// 
			// textInterface
			// 
			this->textInterface->Location = System::Drawing::Point(352, 89);
			this->textInterface->Name = L"textInterface";
			this->textInterface->Size = System::Drawing::Size(138, 20);
			this->textInterface->TabIndex = 8;
			this->textInterface->TextChanged += gcnew System::EventHandler(this, &MainWindow::textInterface_Changed);
			// 
			// labelInterface
			// 
			this->labelInterface->AutoSize = true;
			this->labelInterface->Location = System::Drawing::Point(167, 92);
			this->labelInterface->Name = L"labelInterface";
			this->labelInterface->Size = System::Drawing::Size(49, 13);
			this->labelInterface->TabIndex = 12;
			this->labelInterface->Text = L"Interface";
			// 
			// buttonDelete
			// 
			this->buttonDelete->Location = System::Drawing::Point(85, 130);
			this->buttonDelete->Name = L"buttonDelete";
			this->buttonDelete->Size = System::Drawing::Size(68, 26);
			this->buttonDelete->TabIndex = 2;
			this->buttonDelete->Text = L"Delete";
			this->buttonDelete->UseVisualStyleBackColor = true;
			this->buttonDelete->Click += gcnew System::EventHandler(this, &MainWindow::buttonDelete_Click);
			// 
			// labelDivider
			// 
			this->labelDivider->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->labelDivider->Location = System::Drawing::Point(170, 119);
			this->labelDivider->Name = L"labelDivider";
			this->labelDivider->Size = System::Drawing::Size(320, 2);
			this->labelDivider->TabIndex = 15;
			// 
			// textGamePath
			// 
			this->textGamePath->Location = System::Drawing::Point(246, 130);
			this->textGamePath->Name = L"textGamePath";
			this->textGamePath->Size = System::Drawing::Size(244, 20);
			this->textGamePath->TabIndex = 9;
			this->textGamePath->TextChanged += gcnew System::EventHandler(this, &MainWindow::textGamePath_Changed);
			// 
			// labelGamePath
			// 
			this->labelGamePath->AutoSize = true;
			this->labelGamePath->Location = System::Drawing::Point(167, 133);
			this->labelGamePath->Name = L"labelGamePath";
			this->labelGamePath->Size = System::Drawing::Size(60, 13);
			this->labelGamePath->TabIndex = 16;
			this->labelGamePath->Text = L"Game Path";
			// 
			// textServerPath
			// 
			this->textServerPath->Location = System::Drawing::Point(246, 156);
			this->textServerPath->Name = L"textServerPath";
			this->textServerPath->Size = System::Drawing::Size(244, 20);
			this->textServerPath->TabIndex = 10;
			this->textServerPath->TextChanged += gcnew System::EventHandler(this, &MainWindow::textServerPath_Changed);
			// 
			// labelServerPath
			// 
			this->labelServerPath->AutoSize = true;
			this->labelServerPath->Location = System::Drawing::Point(167, 159);
			this->labelServerPath->Name = L"labelServerPath";
			this->labelServerPath->Size = System::Drawing::Size(63, 13);
			this->labelServerPath->TabIndex = 18;
			this->labelServerPath->Text = L"Server Path";
			// 
			// checkboxRebind
			// 
			this->checkboxRebind->AutoSize = true;
			this->checkboxRebind->Location = System::Drawing::Point(246, 185);
			this->checkboxRebind->Name = L"checkboxRebind";
			this->checkboxRebind->Size = System::Drawing::Size(141, 17);
			this->checkboxRebind->TabIndex = 11;
			this->checkboxRebind->Text = L"Rebind inputs on launch";
			this->checkboxRebind->UseVisualStyleBackColor = true;
			this->checkboxRebind->CheckStateChanged += gcnew System::EventHandler(this, &MainWindow::checkboxRebind_Changed);
			// 
			// comboInterface
			// 
			this->comboInterface->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboInterface->FormattingEnabled = true;
			this->comboInterface->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"IP address", L"Name" });
			this->comboInterface->Location = System::Drawing::Point(246, 89);
			this->comboInterface->Name = L"comboInterface";
			this->comboInterface->Size = System::Drawing::Size(100, 21);
			this->comboInterface->TabIndex = 7;
			this->comboInterface->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::comboInterface_Changed);
			// 
			// MainWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(502, 207);
			this->Controls->Add(this->comboInterface);
			this->Controls->Add(this->checkboxRebind);
			this->Controls->Add(this->textServerPath);
			this->Controls->Add(this->labelServerPath);
			this->Controls->Add(this->textGamePath);
			this->Controls->Add(this->labelGamePath);
			this->Controls->Add(this->labelDivider);
			this->Controls->Add(this->buttonDelete);
			this->Controls->Add(this->textInterface);
			this->Controls->Add(this->labelInterface);
			this->Controls->Add(this->checkboxHidden);
			this->Controls->Add(this->labelMode);
			this->Controls->Add(this->comboMode);
			this->Controls->Add(this->textStoragePath);
			this->Controls->Add(this->textName);
			this->Controls->Add(this->labelStoragePath);
			this->Controls->Add(this->labelName);
			this->Controls->Add(this->buttonAdd);
			this->Controls->Add(this->buttonLaunch);
			this->Controls->Add(this->listboxInstances);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MainWindow";
			this->Text = L"EXVS2 Launcher";
			this->Load += gcnew System::EventHandler(this, &MainWindow::MainWindow_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private:
	bool ignoreEvents_ = false;
	LauncherState* state_;

	void ReloadLeft();
	void ReloadRight();
	void ReloadBottom();
	void ReloadAll();

	ClientConfiguration* Current();

	void MainWindow_Load(System::Object^ sender, System::EventArgs^ e);

	void listboxInstances_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
	void listboxInstances_ItemCheck(System::Object^ sender, System::Windows::Forms::ItemCheckEventArgs^ e);

	void buttonAdd_Click(System::Object^ sender, System::EventArgs^ e);
	void buttonDelete_Click(System::Object^ sender, System::EventArgs^ e);
	void buttonLaunch_Click(System::Object^ sender, System::EventArgs^ e);

	void textName_Changed(System::Object^ sender, System::EventArgs^ e);
	void textStoragePath_Changed(System::Object^ sender, System::EventArgs^ e);
	void comboMode_Changed(System::Object^ sender, System::EventArgs^ e);
	void checkboxHidden_Changed(System::Object^ sender, System::EventArgs^ e);

	void comboInterface_Changed(System::Object^ sender, System::EventArgs^ e);
	void textInterface_Changed(System::Object^ sender, System::EventArgs^ e);

	void textGamePath_Changed(System::Object^ sender, System::EventArgs^ e);
	void textServerPath_Changed(System::Object^ sender, System::EventArgs^ e);
	void checkboxRebind_Changed(System::Object^ sender, System::EventArgs^ e);
};
}
