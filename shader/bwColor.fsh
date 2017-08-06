varying vec4 v_fragmentColor;  
varying vec2 v_texCoord;  
   
void main()
{
    vec4 pixColor = texture2D(CC_Texture0, v_texCoord);  
    float sColor  = pixColor.r + pixColor.g + pixColor.b;
    sColor = sColor * 0.25;
    gl_FragColor  = vec4(sColor, sColor, sColor, 1.0);
}