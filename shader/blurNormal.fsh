      
varying vec4 v_fragmentColor;  
varying vec2 v_texCoord;  

vec4 blurValue()
{
    vec4  _result = vec4(0.0,0.0,0.0,0.0);
    
    float _widthFlag = 0.012;
    float _widthWFlag = 0.002;    
    float _count = 0.0;
    
       for(float w=-_widthFlag; w<=_widthFlag; w+= _widthWFlag)
    {
        for(float h=-_widthFlag; h<=_widthFlag; h+= _widthWFlag)
        {
            vec2 _tex = v_texCoord;
            _tex.x += w;
            _tex.y += h;            
            _result = _result + texture2D(CC_Texture0, _tex);            
            _count += 1.0;
        }
    }
    
    if( _count > 0.0 )
    {
        _result = _result / _count;    
    }
    
    return _result;
}
   
void main()
{
    vec4 pixColor = 
    blurValue();

    gl_FragColor.a = pixColor.a;            
    gl_FragColor.rgb = v_fragmentColor.rgb;
}