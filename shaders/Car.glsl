#shader vertex
#version 330 core

layout(location=0)in vec2 aPos;
layout(location=1)in vec2 aDir;
layout(location=2)in uint isCamera;
layout(location=3)in float inSpeed;

out vec2 direction;
out uint isCameraFollowing;
out float speed;

void main()
{
    speed=inSpeed;
    isCameraFollowing=isCamera;
    direction=aDir;
    gl_Position=vec4(aPos.x,aPos.y,0.,1.);
};

#shader geometry
#version 330 core
layout(points)in;
layout(triangle_strip,max_vertices=3)out;

out vec2 fVertexPos;// interpolated model-space position of vertices of triangle where circle is draw.
in vec2[]direction;
in uint[]isCameraFollowing;
in float[]speed;
out float fSpeed;
out vec2 triangleBottom;
out vec2 triangleTop;
uniform mat4 proj;
uniform mat4 mvp;
mat4 MVP;

void emitVertex(vec2 pos)
{
    fVertexPos=pos;
    gl_Position=MVP*vec4(pos.x,pos.y,0.,1.);
    EmitVertex();
}

void main()
{
    fSpeed=speed[0];
    if(isCameraFollowing[0]==uint(1))
    {
        MVP=proj;
    }
    else
    {
        MVP=mvp;
    }
    
    vec2 dir=normalize(direction[0]);
    vec2 dirNormal=vec2(dir.y,-dir.x);
    
    triangleBottom=gl_in[0].gl_Position.xy-10*dir;
    triangleTop=triangleBottom+40*(dir);
    
    emitVertex(triangleBottom+10*(dirNormal));
    emitVertex(triangleBottom-10*(dirNormal));
    emitVertex(triangleTop);
}

#shader fragment
#version 330 core

in float fSpeed;
in vec2 fVertexPos;
in vec2 triangleBottom;
in vec2 triangleTop;
layout(location=0)out vec4 FragColor;

void main()
{
    float fillFactor=fSpeed/3.;
    
    // Calculate the direction from the bottom to the top of the triangle
    vec2 bottomToTop=triangleTop-triangleBottom;
    
    // Calculate the vector from the bottom of the triangle to the current fragment
    vec2 bottomToFrag=fVertexPos-triangleBottom;
    
    // Project this vector onto the bottom-to-top vector
    float projection=dot(bottomToFrag,bottomToTop)/dot(bottomToTop,bottomToTop);
    
    // Check if the projection is within the fill level
    if(projection<=fillFactor){
        // Fill color
        FragColor=vec4(1.,.6,0.,1.);// Color representing speed
    }else{
        // Regular triangle color
        FragColor=vec4(.1608,.8667,.0667,1.);// Some color
    }
};