using Avalonia.Controls;
using Avalonia.Input;
using Launcher.ViewModels;
using System;

namespace Launcher.Views;


public partial class SettingsView : UserControl
{
    public SettingsView()
    {
        InitializeComponent();
    }

    SettingsViewModel? Context()
    {
        return (SettingsViewModel?)DataContext;
    }

    private void RowUp(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        var button = (Button)sender!;
        var rowInfo = (RowInfo)button.DataContext!;

        var rows = Context()!.Rows;
        int idx = rows.IndexOf(rowInfo);
        if (idx - 1 < 0) {
            return;
        }
        rows.Move(idx, idx - 1);
        Context()!.UpdateRows();
    }

    private void RowDown(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        var button = (Button)sender!;
        var rowInfo = (RowInfo)button.DataContext!;

        var rows = Context()!.Rows;
        int idx = rows.IndexOf(rowInfo);
        if (idx + 1 >= rows.Count)
        {
            return;
        }
        rows.Move(idx, idx + 1);
        Context()!.UpdateRows();
    }

    private void RowPreview(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        var button = (Button)sender!;
        var rowInfo = (RowInfo)button.DataContext!;
    }

    private void RowDelete(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        var button = (Button)sender!;
        var rowInfo = (RowInfo)button.DataContext!;

        var rows = Context()!.Rows;
        rows.Remove(rowInfo);
        Context()!.UpdateRows();
    }

    private void RowAdd(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        Context()!.RowAdd();
    }
}