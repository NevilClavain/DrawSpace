
float4x4 matWorldViewProjection : register(c0);
float4x4 matWorldView : register(c4);
float4x4 matWorld : register(c8);
float4x4 matView : register(c12);
float4x4 matProj : register(c20);

struct VS_INPUT
{
    float4 Position : POSITION0;
    float4 TexCoord0 : TEXCOORD0;
      
};

struct VS_OUTPUT
{
    float4 Position : POSITION0;
    float4 TexCoord0 : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
    VS_OUTPUT Output;

    float4x4 id = 0;
    id[0][0] = 1.0;
    id[1][1] = 1.0;
    id[2][2] = -1.0;
    id[3][3] = 1.0;

    float4x4 matView2 = mul(matView, id);
    
    float4 worldPos = mul(Input.Position, matWorld);
    
     
    float4 worldPosRefl;


    worldPosRefl.xyz = reflect(worldPos.xyz, normalize(float3( 0.0, 1.0, 0.0)));
    worldPosRefl.w = worldPos.w;

    float4 viewPos = mul(worldPosRefl, matView2);
    Output.Position = mul(viewPos, matProj);

    Output.TexCoord0 = Input.TexCoord0;
      
    return (Output);
}
