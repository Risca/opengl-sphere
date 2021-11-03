#version 100

in lowp vec3 normalWorld;
in lowp vec3 vertexPositionWorld;
in lowp vec3 color;
in lowp vec2 textureCoordinate;

uniform lowp vec3 sunPositionWorld;
uniform lowp vec4 sunColor;
uniform lowp vec4 ambientLight;
uniform sampler2D textureHandle;

void main(void)
{
    // Diffuse light
    lowp vec3 lightVectorWorld = normalize(sunPositionWorld - vertexPositionWorld);
    lowp float brightness = dot(lightVectorWorld, normalize(normalWorld));
    lowp vec4 diffuseLight = texture2D(textureHandle, textureCoordinate) * brightness;

    // Specular light
    lowp vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
    lowp vec3 eyePositionWorld = vec3(0.0, 0.0, 0.0);
    lowp vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
    lowp float specularity = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0.0, 1.0);
    specularity = pow(specularity, 50.0);
    lowp vec4 specularLight = sunColor * specularity;

    lowp vec4 combinedLight = ambientLight + clamp(diffuseLight, 0.0, 1.0) + specularLight;
    gl_FragColor = combinedLight;
}
