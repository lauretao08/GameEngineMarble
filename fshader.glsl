#version 150

uniform sampler2D texture;

uniform sampler2D grassTex;
uniform sampler2D rockTex;
uniform sampler2D snowTex;

in vec2 v_texcoord;
in vec3 v_position;
//! [0]
void main()
{
    // Set fragment color from texture
    gl_FragColor = texture2D(texture, v_texcoord);
    /*if(v_position.z<0.2){
        gl_FragColor = texture2D(grassTex, v_texcoord);
    }else if(v_position.z>= 0.2 && v_position.z<0.4){
        gl_FragColor = texture2D(rockTex, v_texcoord);
    }else{
        gl_FragColor = texture2D(snowTex, v_texcoord);
    }*/
}
//! [0]

