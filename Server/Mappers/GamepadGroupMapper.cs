using nue.protocol.exvs;
using Riok.Mapperly.Abstractions;
using WebUI.Shared.Dto.Common;

namespace Server.Mappers;

[Mapper]
public static partial class ControllerGroupMapper
{
    public static partial ControllerConfig ToControllerConfig(this Response.LoadCard.MobileUserGroup.ControllerGroup controllerGroup);
    public static partial Response.LoadCard.MobileUserGroup.ControllerGroup ToControllerGroup(this ControllerConfig controllerConfig);
}