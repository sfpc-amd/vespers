uniform mat4 modelViewProjectionMatrix;

attribute vec4 position;
attribute vec2 texcoord;

varying vec2 texCoordVarying;

void main()
{

	gl_Position = modelViewProjectionMatrix * position;

    texCoordVarying = texcoord;
}