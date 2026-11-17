using System.Text.RegularExpressions;
namespace PhongThan.Event.Runtime;

public sealed record BotChatAnswer(int Intent,string? Hex,int PartyAction){
    public bool Semantic {get;init;}
    public int Command {get;init;}
    public string? ArgumentHex {get;init;}
    public static implicit operator BotChatAnswer((int Intent,string? Hex,int PartyAction) value)=>new(value.Intent,value.Hex,value.PartyAction);
}
public static class BotCommandLanguage {
    // These codes are shared with PTBotCommand; they are proposals, never a
    // confirmation that movement, combat, purchase or trade has completed.
    public const string Instruction="""
        Interpret the latest Vietnamese message semantically in the conversation.
        Return command and argument in addition to intent, party_action and reply.
        command: 0=no request for your action; 1=follow the human now;
        2=confirm a previous invitation/question; 3=stand and wait;
        4=follow and help attack monsters; 5=assist the human's boss target;
        6=pick up nearby eligible loot; 7=buy an item; 8=cancel your current order;
        9=receive a gift through ordinary trade; 10=an open-ended invitation to go
        somewhere together, requiring a destination question before agreement.
        11=attack an explicitly identified player; 12=raid a supply cart;
        13=raid an escort. Distinguish bosses, ordinary monsters, players and carts.
        14=ask about buying your advertised item, its price, or a barter;
        15=ask to see the advertised item again;
        16=arrange a real trade meeting at an NPC+map or coordinates+map;
        17=accept your sale quote and meet to trade; 18=play rock paper scissors.
        19=invite this human to your party/team; 20=request to attend Van Tien Tran;
        21=go to a map/NPC/coordinate destination and WAIT there for the human.
        22=the HUMAN offers to sell you equipment/supplies, or asks what you need
        to buy. You are the BUYER in22, the seller in14. An item link will be
        checked by the game before you can quote or pay. 'xong roi, di train tiep',
        'di ve di', 'lam viec cua ong di' release the human's job with command8.
        23=make bag space by selling YOUR surplus items at a real NPC;
        24=open ordinary player trade, not necessarily a gift or sale;
        25=make space with explicit permission to drop only expendable junk.
        'ban bot do trong tui' means23, never14 (quote advertised sale).
        'vut bot do' means25. 'gd', 'giao dich di' without an agreed sale means24.
        Vietnamese game abbreviations: ott=oan tu ti (18), 'lam ti do den khong'
        also invites rock paper scissors; pt=party/to doi/lap nhom/tao nhom,
        even misspelled 'tao mom'. 'pt toi voi', 'pt di' request YOUR invitation=19.
        gd=giao dich: during a purchase ('ok thi gd') use sale acceptance17;
        a gift without sale context remains9. vtt=Van Tien Tran20, never ordinary
        combat or an unspecified invite. 'di vtt khong' requests eligibility check.
        'kiem kia ban khong', 'ngua ban dep day ban ko' ask to BUY your real item14,
        not an invented inventory claim. Preserve the item description.
        'toi Tay Ky cho tui', 'phu len Dai Phu Sung Thanh Doanh doi tui' are21.
        Preserve the full location in argument; don't invent map IDs or coordinates.
        A human buying YOUR advertised item is 14/17, never 7 (you buying at an NPC).
        A location supplied during sale negotiation is 16. Preserve the entire
        destination wording in argument, including NPC, coordinates and map.
        For 14/15/17/18 preserve the human's item/barter description in argument.
        Wait=3 means remain HERE waiting for the human. Cancel=8 means stop the
        assigned job and return to your own activities, not wait for the human.
        Gift=9 means the HUMAN gives YOU items/money. Buy=7 means YOU buy stock
        from an NPC. Receiving money is never a purchase of the item 'money'.
        Supply/lương=12 and escort/tiêu=13 are different actual game cargo types.
        Infer paraphrases and slang, not just exact keywords. 'cứ bám theo sau tui',
        'ông theo chân tôi nhé', 'tui đi trước, ông theo sau' request follow=1.
        'chờ tui ở đây', 'đừng chạy nữa' request wait=3. 'phụ tui xử lý mấy con này'
        requests assist=4. 'gom mấy món dưới đất giúp' requests loot=6.
        Distinguish who must act: 'tui nghỉ tí', 'tui theo ông', 'tui đi ăn' describe
        the HUMAN and do not command you. Greetings, status questions and casual
        observations have command=0. Do not convert fatigue into a command to rest.
        For command>0 reply MUST be empty and intent=2: the game will check hostility,
        identity, distance, path, resources and actual action state and reply itself.
        For buy, argument is the item description. For combat, argument is the target name
        from the human, without inventing a name; vague targets use "". For other actions use "".
        Never say you agreed, followed, attacked, waited, bought or received anything
        through free-form reply. Only the game can acknowledge its actions.
        """;
    public static int Validate(int command,string text){
        if(command is <0 or >25)return 0;
        string q=BotDialogueMemory.Normalize(text);
        if(ReleaseRequest(text))return 8;
        if(TravelSupplyStatement(text))return 0;
        if(Regex.IsMatch(q,@"\b(vut bot|bo bot do)\b"))return 25;
        if(Regex.IsMatch(q,@"\b(ban bot|don tui|don hanh trang)\b"))return 23;
        if(Regex.IsMatch(q,@"\bban\s+cho\s+(ong|ban|bot)\b|\b(ong|ban|bot)\s+(can|muon)\s+mua\b|\bmua\s+do\s+cua\s+(toi|tui)\b"))return 22;
        if(Regex.IsMatch(q,@"\b(ok|oke|dong y|chot)\b")&&Regex.IsMatch(q,@"\b(gd|giao dich)\b"))return 17;
        if(Regex.IsMatch(q,@"\A(xong roi|ve di|di ve di|di train tiep(?: di)?|lam viec cua ong(?: di)?)[.!?]*\z"))return 8;
        if(BotChatIntent.AskedLocation(text)&&!Regex.IsMatch(q,@"\b(gd|giao dich|hen|di den|toi cho|den cho|toi dai phu|den dai phu)\b"))return 0;
        if(command==20&&!Regex.IsMatch(q,@"\b(vtt|van tien tran)\b"))return 0;
        if(command is 0 or 4 or 5&&Regex.IsMatch(q,@"\b(dung|khong|thoi)\s+(danh|san|diet|xu ly)\b"))return 8;
        if(Regex.IsMatch(q,@"\Aott(?:\s|\z)")&&!Regex.IsMatch(q,@"\b(la gi|nghia la|viet tat)\b"))return 18;
        if(Regex.IsMatch(q,@"\blam (ti|ty|vai van) do den\b"))return 18;
        if(Regex.IsMatch(q,@"\A(gd|giao dich)( di| nhe| nha)?[.!?]*\z"))return command==17?17:24;
        if(Regex.IsMatch(q,@"\Avtt[.!?]*\z"))return 20;
        if(Regex.IsMatch(q,@"\A(pt\b|(?:lap|tao)\s+(?:nhom|mom)\b)"))return 19;
        if(command==19&&Regex.IsMatch(q,@"\b(di cung|di voi)\b")&&!Regex.IsMatch(q,@"\b(pt|doi|nhom|party)\b"))return 10;
        if(Regex.IsMatch(q,@"\A(tui|toi|minh|tao|to|em)\s+(se\s+|dang\s+)?(di\s+)?theo\s+(ong|ban|anh)\b"))return 0;
        if(command<14&&Regex.IsMatch(q,@"\A(tui|toi|minh|tao|to|em)\b")&&!Regex.IsMatch(q,@"\b(ong|ban|anh|giup|phu|muon|nho|dua|tang|cho)\b"))return 0;
        if(Regex.IsMatch(q,@"\b(cuop|chan|cuot|raid)\b")&&Regex.IsMatch(q,@"\b(tieu|bao tieu)\b"))return 13;
        if(Regex.IsMatch(q,@"\b(cuop|chan|cuot|raid)\b")&&Regex.IsMatch(q,@"\b(luong|van luong)\b"))return 12;
        if(Regex.IsMatch(q,@"\b(thoi khoi|khoi theo|dung bam|huy lenh|di lam viec cua|tu lo viec)\b"))return 8;
        if(Regex.IsMatch(q,@"\A(tui|toi|minh)\s+(se\s+)?(dua|tang|cho)\b")&&Regex.IsMatch(q,@"\b(giao dich|tien|xu|luong|nhan)\b"))return 9;
        if(Regex.IsMatch(q,@"\b(tai sao|sao ong|sao ban|cap bao nhieu|cap may|o dau)\b")&&command is not(3 or 8)&&command<14)return 0;
        // Validate the target category of an explicit combat request before a
        // generic follow proposal can reach the world controller.
        if(command is not(3 or 8)&&Regex.IsMatch(q,@"\b(danh|diet|xu ly|ha|san)\b")&&!Regex.IsMatch(q,@"\b(dang|tai sao|sao ong|sao ban)\b|\b(dung|khong|thoi)\s+(danh|san|diet|xu ly)\b")){
            if(Regex.IsMatch(q,@"\bboss\b"))return 5;
            if(Regex.IsMatch(q,@"\bquai\b"))return 4;
        }
        return command;
    }
    public static bool TravelSupplyStatement(string text)=>Regex.IsMatch(BotDialogueMemory.Normalize(text),@"\A(toi|tui|minh|em)\s+(da\s+)?(dua|gui|cho|giao)\b.*\bphu\b.*\broi( ma| nhe| nha| day)?[.!?]*\z");
    public static bool ReleaseRequest(string text)=>Regex.IsMatch(BotDialogueMemory.Normalize(text),@"\b((khong|ko) can (nua|ong|theo|cho)|khoi (cho|doi|theo)|dung cho nua|di train tiep|tu di train|tu lam nhiem vu|xong roi|di ve di|lam viec cua ong|lam gi thi lam|huy lenh)\b|\A(thoi( nhe| nha)?|khong can|ko can|xong|ve di)[.!?]*\z");
    public static bool BareTargetReply(string text){string q=BotDialogueMemory.Normalize(text);return q.Split(' ',StringSplitOptions.RemoveEmptyEntries).Length is >=1 and <=6&&!Regex.IsMatch(q,@"\b(khong|dung|thoi|sao|gi|dau|toi|tui|ong|ban|cho|dua|ban|mua|la|co)\b")&&!text.Contains('?');}
    public static string TargetArgument(string argument){string q=BotDialogueMemory.Normalize(argument);return Regex.IsMatch(q,@"\A(con |cai |nguoi |thang )?(boss|quai|nguoi|muc tieu|luong|tieu|xe luong|xe tieu)( kia| nay| do)?\z")?"":argument;}
    public static string HumanStatementReply(string text){string q=BotDialogueMemory.Normalize(text);if(q.Contains("boss")||q.Contains("danh"))return "Cẩn thận nha ông, cần phụ thì gọi tui.";if(q.Contains("theo"))return "Ừ, đi cùng cho vui ông.";return BotChatIntent.ConversationContext(text,null);}
    public static bool UnverifiedPromise(string text)=>Regex.IsMatch(BotDialogueMemory.Normalize(text),@"\b(tui|toi|minh|em)\s+((se|dang|da|vua)\s+)?(di theo|theo sau|theo ong|theo ban|di cung|dung cho|dung doi|nhat do|di danh|di mua|den giao dich)\b");
}
