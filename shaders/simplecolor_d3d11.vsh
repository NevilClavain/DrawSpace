
float4x4 matWorldViewProjection: register(c0);

struct VS_INPUT 
{
   float4 Position : POSITION;
   float2 TexCoord0: TEXCOORD0;

      
};

struct VS_OUTPUT 
{
   float4 Position : SV_POSITION;
   float2 TexCoord0: TEXCOORD0;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
    VS_OUTPUT Output;

    //Output.Position = mul( Input.Position, matWorldViewProjection );
    
    matrix mat = 0.0;

    mat[0][0] = 2.0;
    mat[1][1] = 3.2;
    mat[2][2] = -1.111111;
    mat[2][3] = -1.111111;
    mat[3][2] = -1.0;

    float4 tpoint;

    tpoint.x = Input.TexCoord0.x;
    tpoint.y = Input.TexCoord0.y;
    tpoint.z = -5.0;
    tpoint.w = 1.0;

    Output.Position = mul(tpoint, mat);

    Output.TexCoord0 = Input.TexCoord0;
      
   return( Output );   
}
