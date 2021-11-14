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
    lowp vec4 diffuseLight = vec4(0.0, 0.0, 0.0, 1.0);
    lowp vec4 specularLight = vec4(0.0, 0.0, 0.0, 1.0);

    // Diffuse light
    lowp vec3 lightVectorWorld = normalize(sunPositionWorld - vertexPositionWorld);
    lowp float brightness = dot(lightVectorWorld, normalize(normalWorld));

    lowp vec4 day = texture2D(dayTextureHandle, textureCoordinate);
    lowp vec4 night = texture2D(nightTextureHandle, textureCoordinate);
    lowp float alpha = clamp(atan(brightness), 0.0, 1.0);

#if 0
    // Specular light
    lowp vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
    lowp vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
    lowp float specularity = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0.0, 1.0);
    specularity = pow(specularity, 50.0);
    specularLight = sunColor * specularity;
#elif 0
    // Sun spot
    specularLight = sunColor * pow(clamp(brightness, 0.0, 1.0), 50.0);
#endif

    gl_FragColor = mix(night, day, alpha) + specularLight;
}
