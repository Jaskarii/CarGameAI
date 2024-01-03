#shader vertex
#version 330 core

layout (location = 0) in vec2 aPos;

void main()
{
	gl_Position = vec4(aPos,0.0,1.0);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0,1.0,1.0,1.0);
};