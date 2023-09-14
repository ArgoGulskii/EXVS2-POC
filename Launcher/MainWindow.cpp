#include "MainWindow.h"

#include <utility>

#include "EXVSLauncher.h"

static std::string to_string(System::String^ str)
{
	using System::Runtime::InteropServices::Marshal;

	std::string result;
	char* p = reinterpret_cast<char*>(Marshal::StringToHGlobalAnsi(str).ToPointer());
	result = p;
	Marshal::FreeHGlobal(System::IntPtr(static_cast<void*>(p)));
	return result;
}

namespace EXVSLauncher
{
	void MainWindow::ReloadLeft()
	{
		bool previousIgnore = ignoreEvents_;
		ignoreEvents_ = true;

		listboxInstances->Items->Clear();
		for (auto& item : state_->config.clients)
		{
			listboxInstances->Items->Add(gcnew System::String(item.name.c_str()), item.enabled);
		}
		listboxInstances->SelectedIndex = state_->selected.value_or(-1);

		ignoreEvents_ = previousIgnore;
	}

	void MainWindow::ReloadRight()
	{
		bool previousIgnore = ignoreEvents_;
		ignoreEvents_ = true;

		buttonDelete->Enabled = state_->selected.has_value();

		if (!state_->selected)
		{

			textName->Enabled = false;
			textName->Text = "";

			textStoragePath->Enabled = false;
			textStoragePath->Text = "";

			comboMode->Enabled = false;
			comboMode->SelectedIndex = -1;

			checkboxHidden->Enabled = false;
			checkboxHidden->Checked = false;

			comboInterface->Enabled = false;
			comboInterface->SelectedIndex = -1;

			textInterface->Enabled = false;
			textInterface->Text = "";
		}
		else
		{
			size_t idx = *state_->selected;
			const auto& item = state_->config.clients[idx];

			textName->Enabled = true;
			textName->Text = gcnew System::String(item.name.c_str());

			textStoragePath->Enabled = true;
			textStoragePath->Text = gcnew System::String(item.storagePath.c_str());

			comboMode->Enabled = true;
			comboMode->SelectedIndex = size_t(item.mode);

			checkboxHidden->Enabled = true;
			checkboxHidden->Checked = item.hidden;

			comboInterface->Enabled = true;
			textInterface->Enabled = true;
			if (auto ip = std::get_if<IpAddress>(&item.networkInterface))
			{
				comboInterface->SelectedIndex = 0;
				textInterface->Text = gcnew System::String(ip->value.c_str());
			}
			else if (auto iface = std::get_if<InterfaceName>(&item.networkInterface))
			{
				comboInterface->SelectedIndex = 1;
				textInterface->Text = gcnew System::String(iface->value.c_str());
			}
		}

		ignoreEvents_ = previousIgnore;
	}

	void MainWindow::ReloadBottom()
	{
		bool previousIgnore = ignoreEvents_;
		ignoreEvents_ = true;

		textGamePath->Text = gcnew System::String(state_->config.gamePath.c_str());
		textServerPath->Text = gcnew System::String(state_->config.serverPath.c_str());

		ignoreEvents_ = previousIgnore;
	}

	void MainWindow::ReloadAll()
	{
		bool previousIgnore = ignoreEvents_;
		ignoreEvents_ = true;

		ReloadLeft();
		ReloadRight();
		ReloadBottom();

		ignoreEvents_ = previousIgnore;
	}

	ClientConfiguration* MainWindow::Current()
	{
		if (!state_->selected)
			return nullptr;
		return &state_->config.clients[*state_->selected];
	}

	void MainWindow::MainWindow_Load(System::Object^ sender, System::EventArgs^ e)
	{
		// TODO: Serialize/deserialize the config to/from disk.
		state_->config.rebindOnLaunch = true;
		state_->config.gamePath = "C:\\GXX10JPN27\\vsac25_Release.exe";
		state_->config.serverPath = "C:\\GXX10JPN27\\Server\\Server.exe";
		state_->config.clients.emplace_back(ClientConfiguration {
			.name = "LM",
			.storagePath = "C:\\GXX10JPN27\\storage\\lm",
			.mode = ClientMode::LiveMonitor,
			.hidden = true,
			.networkInterface = InterfaceName("vEthernet (LM)"),
		});
		state_->config.clients.emplace_back(ClientConfiguration{
			.name = "PCB1",
			.storagePath = "C:\\GXX10JPN27\\storage\\pcb1",
			.mode = ClientMode::Client,
			.hidden = false,
			.networkInterface = InterfaceName("vEthernet (PCB1)"),
		});
		state_->config.clients.emplace_back(ClientConfiguration{
			.name = "PCB2",
			.storagePath = "C:\\GXX10JPN27\\storage\\pcb2",
			.mode = ClientMode::Client,
			.hidden = false,
			.networkInterface = InterfaceName("vEthernet (PCB2)"),
		});
		state_->config.clients.emplace_back(ClientConfiguration{
			.name = "PCB3",
			.storagePath = "C:\\GXX10JPN27\\storage\\pcb3",
			.mode = ClientMode::Client,
			.hidden = false,
			.networkInterface = InterfaceName("vEthernet (PCB3)"),
			.enabled = false,
		});
		state_->config.clients.emplace_back(ClientConfiguration{
			.name = "PCB4",
			.storagePath = "C:\\GXX10JPN27\\storage\\pcb4",
			.mode = ClientMode::Client,
			.hidden = false,
			.networkInterface = InterfaceName("vEthernet (PCB4)"),
			.enabled = false,
		});

		ReloadAll();
	}

	void MainWindow::listboxInstances_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		size_t idx = listboxInstances->SelectedIndex;
		if (idx == -1)
		{
			state_->selected.reset();
		}
		else
		{
			state_->selected = idx;
		}
		ReloadRight();
	}
	
	void MainWindow::listboxInstances_ItemCheck(System::Object^ sender, System::Windows::Forms::ItemCheckEventArgs^ e)
	{
		if (ignoreEvents_) return;
		
		state_->config.clients[e->Index].enabled = e->NewValue == CheckState::Checked;
	}

	void MainWindow::buttonAdd_Click(System::Object^ sender, System::EventArgs^ e)
	{
		state_->config.clients.emplace_back();
		state_->selected = state_->config.clients.size() - 1;
		ReloadAll();
	}

	void MainWindow::buttonDelete_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (state_->config.clients.size() == 0)
			return;
		if (!state_->selected)
			return;
		state_->config.clients.erase(state_->config.clients.begin() + *state_->selected);
		state_->selected.reset();
		ReloadAll();
	}

	void MainWindow::buttonLaunch_Click(System::Object^ sender, System::EventArgs^ e)
	{
		// TODO: Validate the configuration and display errors to the user.
		Launch(state_->config);
	}

	void MainWindow::textName_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		auto current = Current();
		if (!current) return;

		current->name = to_string(textName->Text);
		ReloadLeft();
	}

	void MainWindow::textStoragePath_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		auto current = Current();
		if (!current) return;

		current->storagePath = to_string(textStoragePath->Text);
	}

	void MainWindow::comboMode_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		auto current = Current();
		if (!current) return;

		current->mode = ClientMode(comboMode->SelectedIndex);
	}

	void MainWindow::checkboxHidden_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		auto current = Current();
		if (!current) return;

		current->hidden = checkboxHidden->Checked;
	}

	void MainWindow::comboInterface_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		auto current = Current();
		if (!current) return;

		std::string str = to_string(textInterface->Text);
		switch (comboInterface->SelectedIndex)
		{
		case 0:
			textInterface->Enabled = true;
			current->networkInterface = IpAddress(str);
			break;

		case 1:
			textInterface->Enabled = true;
			current->networkInterface = InterfaceName(str);
			break;

		default:
			textInterface->Enabled = false;
			textInterface->Text = "";
			current->networkInterface = std::monostate();
			break;
		}
	}

	void MainWindow::textInterface_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		return comboInterface_Changed(sender, e);
	}

	void MainWindow::textGamePath_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;
		state_->config.gamePath = to_string(textGamePath->Text);
	}

	void MainWindow::textServerPath_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;
		state_->config.serverPath = to_string(textServerPath->Text);
	}

	void MainWindow::checkboxRebind_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		state_->config.rebindOnLaunch = checkboxRebind->Checked;
	}
} // namespace EXVSLauncher