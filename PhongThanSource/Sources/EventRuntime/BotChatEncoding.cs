// Canonical original VNG glyph map; generated from Export-StarterBagLookup.ps1.
namespace PhongThan.Event.Runtime;
public static class BotChatEncoding {
 public static string EncodeSpeech(string text){
  text=text.Replace('“','"').Replace('”','"').Replace('‘','\'').Replace('’','\'').Replace("…","...").Replace('–','-').Replace('—','-').Replace('\u00a0',' ');
  var speech=new System.Text.StringBuilder();foreach(char c in text.Normalize(System.Text.NormalizationForm.FormC)){
   if(char.IsSurrogate(c)||c is >= '\u2600' and <= '\u27bf')continue;
   char lower=char.ToLowerInvariant(c);speech.Append(c>=128&&Array.IndexOf(To,(int)c)<0&&Array.IndexOf(To,(int)lower)>=0?lower:c);
  }
  return Encode(speech.ToString());
 }
 public static string Encode(string text){var result=new List<byte>();foreach(char c in text.Normalize(System.Text.NormalizationForm.FormC)){if(c<128)result.Add((byte)c);else{int index=Array.IndexOf(To,(int)c);if(index<0)throw new InvalidDataException("Ký tự chưa có trong font game.");result.Add((byte)From[index]);}}return Convert.ToHexStringLower(result.ToArray());}
 static readonly int[] From=[181,184,182,183,185,168,187,190,188,189,198,169,199,202,200,201,203,174,204,208,206,207,209,170,210,213,211,212,214,215,221,216,220,222,223,227,225,226,228,171,229,232,230,231,233,172,234,237,235,236,238,239,243,241,242,244,173,245,248,246,247,249,250,253,251,252,254,161,162,167,163,164,165,166],To=[224,225,7843,227,7841,259,7857,7855,7859,7861,7863,226,7847,7845,7849,7851,7853,273,232,233,7867,7869,7865,234,7873,7871,7875,7877,7879,236,237,7881,297,7883,242,243,7887,245,7885,244,7891,7889,7893,7895,7897,417,7901,7899,7903,7905,7907,249,250,7911,361,7909,432,7915,7913,7917,7919,7921,7923,253,7927,7929,7925,258,194,272,202,212,416,431];
 public static string Decode(string hex){var bytes=Convert.FromHexString(hex);return new string(bytes.Select(b=>{int i=Array.IndexOf(From,(int)b);return (char)(i<0?b:To[i]);}).ToArray());}
}
