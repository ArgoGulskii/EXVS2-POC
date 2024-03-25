using MediatR;
using Microsoft.EntityFrameworkCore;
using Newtonsoft.Json;
using nue.protocol.exvs;
using Server.Mappers;
using Server.Persistence;
using WebUI.Shared.Dto.Common;
using WebUI.Shared.Exception;

namespace Server.Handlers.Card.Controller;

public record GetControllerConfigCommand(string ChipId) : IRequest<ControllerConfig>;

public class GetControllerConfigCommandHandler : IRequestHandler<GetControllerConfigCommand, ControllerConfig>
{
    private readonly ServerDbContext context;
    
    public GetControllerConfigCommandHandler(ServerDbContext context)
    {
        this.context = context;
    }
    
    public Task<ControllerConfig> Handle(GetControllerConfigCommand request, CancellationToken cancellationToken)
    {
        var cardProfile = context.CardProfiles
            .Include(x => x.UserDomain)
            .FirstOrDefault(x => x.ChipId == request.ChipId);

        if (cardProfile is null)
        {
            throw new NullReferenceException("Card Profile is invalid");
        }
        
        var mobileUserGroup = JsonConvert.DeserializeObject<Response.LoadCard.MobileUserGroup>(cardProfile.UserDomain.MobileUserGroupJson);
        
        if (mobileUserGroup is null)
        {
            throw new InvalidCardDataException("Card Data is invalid");
        }

        if (mobileUserGroup.Controller is null)
        {
            return Task.FromResult(new ControllerConfig());
        }
        
        return Task.FromResult(mobileUserGroup.Controller.ToControllerConfig());
    }
}