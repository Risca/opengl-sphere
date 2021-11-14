#version 100

varying lowp vec3 normalWorld;
varying lowp vec3 vertexPositionWorld;
varying lowp vec3 color;
varying mediump vec2 textureCoordinate;

uniform lowp vec3 eyePositionWorld;
uniform lowp vec3 sunPositionWorld;
uniform lowp vec4 sunColor;
uniform lowp vec4 ambientLight;
uniform sampler2D dayTextureHandle;
uniform sampler2D nightTextureHandle;

void main(void)
{
    // Diffuse light
    lowp vec3 lightVectorWorld = normalize(sunPositionWorld - vertexPositionWorld);
    lowp float brightness = dot(lightVectorWorld, normalize(normalWorld));

    lowp vec4 diffuseLight = vec4(0.0, 0.0, 0.0, 1.0);
    diffuseLight += texture2D(dayTextureHandle, textureCoordinate) * clamp(atan(brightness) * 10.0, 0.0, 1.0);
    diffuseLight += texture2D(nightTextureHandle, textureCoordinate) * (1.0 - clamp(atan(brightness) * 10.0, 0.0, 1.0));
    diffuseLight.a = 1.0;

    // Specular light
#if 0
    lowp vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
    lowp vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
    lowp float specularity = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0.0, 1.0);
    specularity = pow(specularity, 50.0);
    lowp vec4 specularLight = sunColor * specularity;
#else
    lowp vec4 specularLight = sunColor * pow(clamp(brightness, 0.0, 1.0), 50.0);
#endif

    lowp vec4 combinedLight = clamp(diffuseLight, 0.0, 1.0) + specularLight;
    gl_FragColor = combinedLight;
}
