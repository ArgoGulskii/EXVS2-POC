﻿using MediatR;
using Microsoft.EntityFrameworkCore;
using Newtonsoft.Json;
using nue.protocol.exvs;
using Server.Models.Cards;
using Server.Persistence;
using WebUI.Shared.Dto.Response;

namespace Server.Handlers.Card;

public record GetAllBareboneCardCommand() : IRequest<List<BareboneCardProfile>>;

public class GetAllBareboneCardCommandHandler : IRequestHandler<GetAllBareboneCardCommand, List<BareboneCardProfile>>
{
    private readonly ILogger<GetAllBareboneCardCommandHandler> _logger;
    private readonly ServerDbContext context;

    public GetAllBareboneCardCommandHandler(ILogger<GetAllBareboneCardCommandHandler> logger, ServerDbContext context)
    {
        _logger = logger;
        this.context = context;
    }
    
    public Task<List<BareboneCardProfile>> Handle(GetAllBareboneCardCommand request, CancellationToken cancellationToken)
    {
        var cardProfiles = context.CardProfiles
            .Include(x => x.UserDomain)
            .ToList();

        var bareboneCardProfiles = cardProfiles
            .Where(cardProfile => cardProfile.IsNewCard == false)
            .Select(ToBareboneCardProfile())
            .ToList();
        
        return Task.FromResult(bareboneCardProfiles);
    }

    private Func<CardProfile, BareboneCardProfile> ToBareboneCardProfile()
    {
        return cardProfile =>
        {
            var user = JsonConvert.DeserializeObject<Response.PreLoadCard.MobileUserGroup>(cardProfile.UserDomain.UserJson);
                
            return new BareboneCardProfile
            {
                AccessCode = cardProfile.AccessCode,
                ChipId = cardProfile.ChipId,
                UserName = user?.PlayerName ?? string.Empty
            };
        };
    }
}