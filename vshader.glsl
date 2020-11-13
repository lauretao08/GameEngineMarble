#version 150

uniform mat4 mvp_matrix;

uniform sampler2D height_map;

in vec4 a_position;
in vec2 a_texcoord;

out vec2 v_texcoord;
out vec3 v_position;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    vec4 pos = a_position;
    //pos.z = texture2D(height_map, a_texcoord).r;
    gl_Position = mvp_matrix * pos;



    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
    v_position.z = pos.z;

}
//! [0]
