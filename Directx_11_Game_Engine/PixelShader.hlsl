struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float3 inColour : COLOUR;
    
};
float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(input.inColour, 1.0f);
}