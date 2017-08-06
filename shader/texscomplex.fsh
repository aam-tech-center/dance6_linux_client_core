varying vec4 v_fragmentColor;  
varying vec2 v_texCoord;  
  
uniform sampler2D texture0;
uniform sampler2D texture1;
 
void main()
{
    vec4 pixColor0 = texture2D(texture0, v_texCoord);  
    vec4 pixColor1 = texture2D(texture1, v_texCoord);  
        
    gl_FragColor = pixColor0 * 0.5 + pixColor1 * 0.5;    
}