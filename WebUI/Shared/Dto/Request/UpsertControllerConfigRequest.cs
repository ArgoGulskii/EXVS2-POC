using WebUI.Shared.Dto.Common;

namespace WebUI.Shared.Dto.Request;

public class UpsertControllerConfigRequest : BasicCardRequest
{
    public ControllerConfig ControllerConfig { get; set; } = new();
}