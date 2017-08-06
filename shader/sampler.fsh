varying vec4 v_fragmentColor;  
varying vec2 v_texCoord;  
 
void main()
{
    vec4 pixColor = texture2D(CC_Texture0, v_texCoord);  
        
    gl_FragColor = pixColor + vec4(1.0,0.0,0.0,1.0);
}