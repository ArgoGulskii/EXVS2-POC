﻿using ReactiveUI;
using System.Reactive.Linq;
using System.Security.Cryptography.X509Certificates;

namespace Launcher.ViewModels;

public partial class ClientViewModel : ViewModelBase
{
    public ClientViewModel()
    {
        name_ = "";
        Mode = 0;
        NetworkMode = 0;
        NetworkValue = "";
        Path = "";
        Hidden = false;
        Enabled = true;
        AutoRebind = false;

        headerName = this
            .WhenAnyValue(x => x.Name)
            .Select(name => name.Length == 0 ? "?" : name)
            .ToProperty(this, x => x.HeaderName);
    }

    readonly ObservableAsPropertyHelper<string> headerName;
    public string HeaderName => headerName.Value;

    private string name_;
    public string Name
    {
        get => name_;
        set => this.RaiseAndSetIfChanged(ref name_, value);
    }

    public int Mode { get; set; }

    public int NetworkMode { get; set; }
    public string NetworkValue { get; set; }

    public string Path { get; set; }

    private bool hidden_;
    public bool Hidden
    {
        get => hidden_;
        set => this.RaiseAndSetIfChanged(ref hidden_, value);
    }

    public bool Enabled { get; set; }
    public bool AutoRebind { get; set; }
}