#version 100

attribute vec4 vertexPositionModel;
attribute vec3 vertexColor;
attribute vec3 normalModel;
attribute vec2 textureCoordinateAttribute;

uniform mat4 modelToWorldMatrix;
uniform mat4 projectionMatrix;

varying vec3 normalWorld;
varying vec3 vertexPositionWorld;
varying vec3 color;
varying vec2 textureCoordinate;

void main(void)
{
    color = vertexColor;
    normalWorld = vec3(modelToWorldMatrix * vec4(normalModel, 0.0));
    vec4 worldPosition = modelToWorldMatrix * vertexPositionModel;
    vertexPositionWorld = vec3(worldPosition);
    gl_Position = projectionMatrix * worldPosition;
    textureCoordinate = textureCoordinateAttribute;
}
