using NAudio.CoreAudioApi;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Automation;

namespace Launcher.Output;

public class AudioOutput
{
    public AudioOutput(MMDevice device)
    {
        device_ = device;
    }

    private readonly MMDevice device_;

    public string Name => device_.FriendlyName;
    public string DevicePath => device_.DeviceTopology.DeviceId;

    static List<AudioOutput>? cachedOutputs_;
    public static List<AudioOutput> EnumerateOutputs()
    {
        lock (typeof(AudioOutput))
        {
            if (cachedOutputs_ != null) return cachedOutputs_;

            List<AudioOutput> result = new();
            MMDeviceEnumerator enumerator = new MMDeviceEnumerator();
            foreach (MMDevice device in enumerator.EnumerateAudioEndPoints(DataFlow.Render, DeviceState.Active))
            {
                result.Add(new AudioOutput(device));
            }
            cachedOutputs_ = result;
            return result;
        }
    }
}