using MediatR;
using Microsoft.EntityFrameworkCore;
using Newtonsoft.Json;
using nue.protocol.exvs;
using Server.Mappers;
using Server.Persistence;
using System.Collections.Generic;
using WebUI.Shared.Dto.Request;
using WebUI.Shared.Dto.Response;
using WebUI.Shared.Exception;

using static System.Text.Json.JsonSerializer;

namespace Server.Handlers.Card.Controller;

public record UpsertControllerConfigCommand(UpsertControllerConfigRequest Request) : IRequest<BasicResponse>;

public class UpsertControllerConfigCommandHandler : IRequestHandler<UpsertControllerConfigCommand, BasicResponse>
{
    private readonly ServerDbContext context;
    
    public UpsertControllerConfigCommandHandler(ServerDbContext context)
    {
        this.context = context;
    }
    
    public Task<BasicResponse> Handle(UpsertControllerConfigCommand request, CancellationToken cancellationToken)
    {
        var updateRequest = request.Request;

        Console.WriteLine(updateRequest);

        Console.WriteLine(Serialize(request));


        var cardProfile = context.CardProfiles
            .Include(x => x.UserDomain)
            .FirstOrDefault(x => x.ChipId == updateRequest.ChipId);

        if (cardProfile is null)
        {
            throw new NullReferenceException("Card Profile is invalid");
        }
        
        var mobileUserGroup = JsonConvert.DeserializeObject<Response.LoadCard.MobileUserGroup>(cardProfile.UserDomain.MobileUserGroupJson);
        
        if (mobileUserGroup is null)
        {
            throw new InvalidCardDataException("Card Data is invalid");
        }

        mobileUserGroup.Controller = updateRequest.ControllerConfig.ToControllerGroup();
        
        cardProfile.UserDomain.MobileUserGroupJson = JsonConvert.SerializeObject(mobileUserGroup);
        
        context.SaveChanges();

        return Task.FromResult(new BasicResponse
        {
            Success = true
        });
    }
}