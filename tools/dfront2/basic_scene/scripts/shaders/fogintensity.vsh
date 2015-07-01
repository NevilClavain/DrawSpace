
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView:           register(c4);

float    fogDensity:             register(c12);


struct VS_INPUT 
{
   float4 Position : POSITION0;      
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float  Fog      : FOG;
};

float ComputeExp2Fog( float4 worldViewPos, float density )
{
   float4 org;
   
   org.x = 0.0;
   org.y = 0.0;
   org.z = 0.0;
   org.w = 1.0;
   
   //float4 d = distance( org, worldViewPos );
   float4 d = - worldViewPos[2];
   return 1 / exp2( d * density );
   
   // retour a 0.0 -> brouillard au maximum
}


VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;
   Output.Position = mul( Input.Position, matWorldViewProjection );  
      
   float4 Position = mul( Input.Position, matWorldView );   
   Output.Fog = clamp( 0.0, 1.0, ComputeExp2Fog( Position, fogDensity ) );       
   return( Output );   
}
