
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);

struct VS_INPUT 
{
   float4 Position : POSITION0;      
};

struct VS_OUTPUT 
{
   float4 Position  : POSITION0;
   float  Fog      : FOG;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;

   Output.Position = mul( Input.Position, matWorldViewProjection );   
   float4 Position = mul( Input.Position, matWorldView );
   
   Output.Fog = -Position.z;
      
   return( Output );   
}
