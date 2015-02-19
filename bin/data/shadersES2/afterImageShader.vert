uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
attribute vec4 position;
attribute vec2 texcoord;

varying vec2 texCoordVarying;

void main()
{

    vec4 pos = projectionMatrix * modelViewMatrix * position;
	gl_Position = pos;

    texCoordVarying = texcoord;
}