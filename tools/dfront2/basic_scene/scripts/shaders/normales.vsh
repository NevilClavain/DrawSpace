
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);
float4x4 matWorldNoTranslation: register(c12);

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 Normales : NORMAL0;
   float4 TexCoord0: TEXCOORD0;
      
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float4 Normales : NORMAL0;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;

   Output.Position = mul( Input.Position, matWorldViewProjection );
   Output.Normales = mul( Input.Normales, matWorldNoTranslation );

      
   return( Output );   
}
