#include "MainWindow.h"

#include <utility>


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

	void MainWindow::ReloadAll()
	{
		bool previousIgnore = ignoreEvents_;
		ignoreEvents_ = true;

		ReloadLeft();
		ReloadRight();

		ignoreEvents_ = previousIgnore;
	}

	ClientConfiguration* MainWindow::Current()
	{
		if (!state_->selected)
			return nullptr;
		return &state_->config.clients[*state_->selected];
	}

	System::Void MainWindow::MainWindow_Load(System::Object^ sender, System::EventArgs^ e)
	{
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
		});
		state_->config.clients.emplace_back(ClientConfiguration{
			.name = "PCB4",
			.storagePath = "C:\\GXX10JPN27\\storage\\pcb4",
			.mode = ClientMode::Client,
			.hidden = false,
			.networkInterface = InterfaceName("vEthernet (PCB4)"),
		});

		ReloadAll();
	}

	System::Void MainWindow::listboxInstances_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
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

	System::Void MainWindow::buttonAdd_Click(System::Object^ sender, System::EventArgs^ e)
	{
		state_->config.clients.emplace_back();
		state_->selected = state_->config.clients.size() - 1;
		ReloadAll();
	}

	System::Void MainWindow::buttonDelete_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (state_->config.clients.size() == 0)
			return;
		if (!state_->selected)
			return;
		state_->config.clients.erase(state_->config.clients.begin() + *state_->selected);
		state_->selected.reset();
		ReloadAll();
	}

	System::Void MainWindow::buttonLaunch_Click(System::Object^ sender, System::EventArgs^ e)
	{
	}

	System::Void MainWindow::textName_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		auto current = Current();
		if (!current) return;

		current->name = to_string(textName->Text);
		ReloadLeft();
	}

	System::Void MainWindow::textStoragePath_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		auto current = Current();
		if (!current) return;

		current->storagePath = to_string(textStoragePath->Text);
	}

	System::Void MainWindow::comboMode_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		auto current = Current();
		if (!current) return;

		current->mode = ClientMode(comboMode->SelectedIndex);
	}

	System::Void MainWindow::checkboxHidden_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		auto current = Current();
		if (!current) return;

		current->hidden = checkboxHidden->Checked;
	}

	System::Void MainWindow::comboInterface_Changed(System::Object^ sender, System::EventArgs^ e)
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

	System::Void MainWindow::textInterface_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		return comboInterface_Changed(sender, e);
	}

	System::Void MainWindow::textGamePath_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;
		state_->config.gamePath = to_string(textGamePath->Text);
	}

	System::Void MainWindow::textServerPath_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;
		state_->config.serverPath = to_string(textServerPath->Text);
	}

	System::Void MainWindow::checkboxRebind_Changed(System::Object^ sender, System::EventArgs^ e)
	{
		if (ignoreEvents_) return;

		state_->config.rebindOnLaunch = checkboxRebind->Checked;
	}
} // namespace EXVSLauncher