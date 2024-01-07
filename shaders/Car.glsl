#shader vertex
#version 330 core

layout(location=0)in vec2 aPos;

void main()
{
    gl_Position=vec4(aPos.x,aPos.y,0.,1.);
};

#shader geometry
#version 330 core
layout(points)in;
layout(triangle_strip,max_vertices=3)out;

out vec2 fVertexPos;// interpolated model-space position of vertices of triangle where circle is draw.
uniform vec2 direction;
uniform mat4 MVP;

void emitVertex(vec2 pos)
{
    fVertexPos=gl_Position.xy;
    gl_Position=MVP*vec4(pos.x,pos.y,0.,1.);
    EmitVertex();
}

void main()
{
    vec2 dir=normalize(direction);
    vec2 dirNormal=vec2(dir.y,-dir.x);
    
    emitVertex(gl_in[0].gl_Position.xy+5*(-dir+dirNormal));
    emitVertex(gl_in[0].gl_Position.xy+5*(-dir-dirNormal));
    emitVertex(gl_in[0].gl_Position.xy+10*(dir));
}

#shader fragment
#version 330 core

in vec2 fVertexPos;
layout(location=0)out vec4 FragColor;

void main()
{
    FragColor=vec4(1.,0,1.,1.);
};