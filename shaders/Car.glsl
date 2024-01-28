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
out vec2 triangleBottomLeft;
out vec2 triangleBottomRight;
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
    
    vec2 triangleBottom=gl_in[0].gl_Position.xy-10*dir;
    triangleBottomLeft=triangleBottom-10*(dirNormal);
    triangleBottomRight=triangleBottom+10*(dirNormal);
    triangleTop=triangleBottom+40*(dir);
    
    emitVertex(triangleBottomRight);
    emitVertex(triangleBottomLeft);
    emitVertex(triangleTop);
}

#shader fragment
#version 330 core

in float fSpeed;
in vec2 fVertexPos;
in vec2 triangleBottomLeft;
in vec2 triangleBottomRight;
in vec2 triangleTop;
layout(location=0)out vec4 FragColor;

// Function to calculate the distance from a point to a line segment
float edgeDistance(vec2 point,vec2 lineStart,vec2 lineEnd){
    vec2 line=lineEnd-lineStart;
    vec2 toPoint=point-lineStart;
    float t=max(0,min(1,dot(toPoint,line)/dot(line,line)));
    vec2 projection=lineStart+t*line;
    return distance(point,projection);
}

void main(){
    vec4 borderColor=vec4(0.,0.,1.,1.);// Blue for border
    vec4 triangleColor=vec4(0.,1.,0.,1.);// Green for triangle
    vec4 fillColor=vec4(1.,0.,0.,1.);// Red for fill
    
    float fillFactor=fSpeed/4.;// Assuming max speed is 3.0
    float edgeSoftness=2;// Adjust for softer/harder edges
    
    // Calculate distances to each edge
    float d1=edgeDistance(fVertexPos,triangleBottomLeft,triangleBottomRight);
    float d2=edgeDistance(fVertexPos,triangleBottomRight,triangleTop);
    float d3=edgeDistance(fVertexPos,triangleTop,triangleBottomLeft);
    
    // Find the minimum distance to an edge
    float minDistance=min(min(d1,d2),d3);
    
    // Calculate alpha based on edge distance
    float alpha=min(minDistance/edgeSoftness,1.);
    
    // Calculate the direction from the bottom to the top of the triangle
    vec2 bottomToTop=triangleTop-triangleBottomLeft;
    
    // Calculate the vector from the bottom of the triangle to the current fragment
    vec2 bottomToFrag=fVertexPos-triangleBottomLeft;
    
    // Project this vector onto the bottom-to-top vector
    float projection=dot(bottomToFrag,bottomToTop)/dot(bottomToTop,bottomToTop);
    
    if(projection<=fillFactor){
        // Fill color
        FragColor=vec4(fillColor.rgb,alpha);// Use fill color with calculated alpha
    }else{
        // Triangle color
        FragColor=vec4(triangleColor.rgb,alpha);// Use triangle color with calculated alpha
    }
    
    // Apply border color with alpha for edge softness
    if(minDistance<edgeSoftness){
        FragColor=mix(borderColor,FragColor,minDistance/edgeSoftness);
    }
}